/* Copyright (c) 2023 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at https://mozilla.org/MPL/2.0/. */

use std::collections::{HashMap, HashSet};
use std::str::Utf8Error;
use std::sync::Arc;

use adblock::cosmetic_filter_cache::{ProceduralOrActionFilter, UrlSpecificResources};
use adblock::filters::cosmetic::{CosmeticFilterAction, CosmeticFilterOperator};
use adblock::lists::FilterSet as InnerFilterSet;
use adblock::resources::{
    InMemoryResourceStorage, MimeType, Resource, ResourceImpl, ResourceStorageBackend,
    ResourceType,
};
use adblock::url_parser::ResolvesDomain;
use adblock::Engine as InnerEngine;
use cxx::{let_cxx_string, CxxString, CxxVector};

use crate::ffi::{
    resolve_domain_position, BlockerResult, BoxEngineResult, ContentBlockingRulesResult,
    FilterListMetadata, RegexDebugInfo, RegexManagerDiscardPolicy, UnitResult, VecStringResult,
};
use crate::filter_set::FilterSet;
use crate::result::InternalError;

#[cfg(feature = "ios")]
use crate::ffi::ContentBlockingRules;

pub struct Engine {
    engine: InnerEngine,
    /// adblock-rust 0.13 keeps resources behind a storage backend and no longer
    /// lets an `Engine` add a single resource, so the wrapper keeps the
    /// in-memory set it hands to the engine (shared, not copied) to implement
    /// `add_resource` on top of it.
    resources: Arc<InMemoryResourceStorage>,
}

/// Shares one `InMemoryResourceStorage` between this wrapper and its engine.
#[derive(Clone)]
struct SharedResourceStorage(Arc<InMemoryResourceStorage>);

impl ResourceStorageBackend for SharedResourceStorage {
    fn get_resource(&self, resource_ident: &str) -> Option<ResourceImpl> {
        self.0.get_resource(resource_ident)
    }
}

impl Engine {
    fn from_inner(engine: InnerEngine) -> Box<Engine> {
        Box::new(Engine { engine, resources: Arc::new(InMemoryResourceStorage::default()) })
    }

    fn set_resources(&mut self, resources: InMemoryResourceStorage) {
        self.resources = Arc::new(resources);
        self.engine.use_resource_storage(SharedResourceStorage(self.resources.clone()));
    }
}

impl Default for Box<Engine> {
    fn default() -> Self {
        new_engine()
    }
}

pub fn new_engine() -> Box<Engine> {
    Engine::from_inner(InnerEngine::default())
}

pub fn engine_with_rules(rules: &CxxVector<u8>) -> BoxEngineResult {
    || -> Result<Box<Engine>, InternalError> {
        let mut filter_set = InnerFilterSet::new(false);
        filter_set.add_filter_list(
            std::str::from_utf8(rules.as_slice())?.to_string(),
            Default::default(),
        );
        let engine = InnerEngine::new_with_filter_set(filter_set);
        Ok(Engine::from_inner(engine))
    }()
    .into()
}

/// Creates a new engine with rules from a given filter set.
pub fn engine_from_filter_set(filter_set: Box<FilterSet>) -> BoxEngineResult {
    || -> Result<Box<Engine>, InternalError> {
        let engine = InnerEngine::new_with_filter_set(filter_set.0);
        Ok(Engine::from_inner(engine))
    }()
    .into()
}

struct DomainResolver;

impl ResolvesDomain for DomainResolver {
    fn get_host_domain(&self, host: &str) -> (usize, usize) {
        let_cxx_string!(host_cxx_string = host);
        let position = resolve_domain_position(&host_cxx_string);
        (position.start as usize, position.end as usize)
    }
}

pub fn set_domain_resolver() -> bool {
    adblock::url_parser::set_domain_resolver(Box::new(DomainResolver)).is_ok()
}

pub fn read_list_metadata(list: &CxxVector<u8>) -> FilterListMetadata {
    std::str::from_utf8(list.as_slice())
        .map(|list| adblock::lists::read_list_metadata(list).into())
        .unwrap_or_default()
}

#[cfg(feature = "ios")]
pub fn convert_rules_to_content_blocking(rules: &CxxString) -> ContentBlockingRulesResult {
    || -> Result<ContentBlockingRules, InternalError> {
        use adblock::lists::{ParseOptions, RuleTypes};

        /// This value corresponds to `maxRuleCount` here:
        /// https://github.com/WebKit/WebKit/blob/4a2df13be2253f64d8da58b794d74347a3742652/Source/WebCore/contentextensions/ContentExtensionParser.cpp#L299
        const MAX_CB_LIST_SIZE: usize = 150000;

        let mut filter_set = InnerFilterSet::new(true);
        filter_set.add_filter_list(
            rules.to_str()?.to_string(),
            ParseOptions { rule_types: RuleTypes::NetworkOnly, ..Default::default() },
        );

        // `unwrap` is safe here because `into_content_blocking` only panics if the
        // `FilterSet` was not created in debug mode
        let (mut cb_rules, _) = filter_set.into_content_blocking().unwrap();
        let rules_len = cb_rules.len();
        let truncated = if rules_len > MAX_CB_LIST_SIZE {
            // Note that the last rule is always the first-party document exception rule,
            // which we want to keep. Otherwise, we can arbitrarily truncate rules
            // before that to ensure that the list can actually compile.
            cb_rules.swap(rules_len - 1, MAX_CB_LIST_SIZE - 1);
            cb_rules.truncate(MAX_CB_LIST_SIZE);
            true
        } else {
            false
        };
        Ok(ContentBlockingRules { rules_json: serde_json::to_string(&cb_rules)?, truncated })
    }()
    .into()
}

#[cfg(not(feature = "ios"))]
pub fn convert_rules_to_content_blocking(_rules: &CxxString) -> ContentBlockingRulesResult {
    panic!("convert_rules_to_content_blocking can only be called on iOS");
}

fn convert_cxx_string_vector_to_string_collection<C>(
    value: &CxxVector<CxxString>,
) -> Result<C, Utf8Error>
where
    C: FromIterator<String>,
{
    value.iter().map(|s| s.to_str().map(|t| t.to_string())).collect()
}

/// adblock-rust 0.13 reports `:style()`, `:remove()`, `:remove-attr()` and
/// `:remove-class()` rules as JSON entries of `procedural_actions`, where 0.8
/// had `style_selectors`, `remove_selectors`, `remove_attrs` and
/// `remove_classes`. The cosmetic filters renderer still reads the 0.8 fields,
/// so the entries that are a plain CSS selector with one of those actions are
/// moved back into them; the rest (procedural operators such as `:has-text()`,
/// which 0.8 could not parse) stay in `procedural_actions`.
struct LegacyUrlSpecificResources {
    hide_selectors: HashSet<String>,
    style_selectors: HashMap<String, Vec<String>>,
    remove_selectors: HashSet<String>,
    remove_attrs: HashMap<String, Vec<String>>,
    remove_classes: HashMap<String, Vec<String>>,
    procedural_actions: HashSet<String>,
    exceptions: HashSet<String>,
    injected_script: String,
    generichide: bool,
}

impl From<UrlSpecificResources> for LegacyUrlSpecificResources {
    fn from(resources: UrlSpecificResources) -> Self {
        let mut legacy = Self {
            hide_selectors: resources.hide_selectors,
            style_selectors: HashMap::new(),
            remove_selectors: HashSet::new(),
            remove_attrs: HashMap::new(),
            remove_classes: HashMap::new(),
            procedural_actions: HashSet::new(),
            exceptions: resources.exceptions,
            injected_script: resources.injected_script,
            generichide: resources.generichide,
        };
        for json in resources.procedural_actions {
            let Ok(filter) = serde_json::from_str::<ProceduralOrActionFilter>(&json) else {
                legacy.procedural_actions.insert(json);
                continue;
            };
            let selector = match &filter.selector[..] {
                [CosmeticFilterOperator::CssSelector(selector)] => selector.clone(),
                _ => {
                    legacy.procedural_actions.insert(json);
                    continue;
                }
            };
            match filter.action {
                None => {
                    legacy.hide_selectors.insert(selector);
                }
                Some(CosmeticFilterAction::Style(style)) => {
                    legacy.style_selectors.entry(selector).or_default().push(style)
                }
                Some(CosmeticFilterAction::Remove) => {
                    legacy.remove_selectors.insert(selector);
                }
                Some(CosmeticFilterAction::RemoveAttr(attr)) => {
                    legacy.remove_attrs.entry(selector).or_default().push(attr)
                }
                Some(CosmeticFilterAction::RemoveClass(class)) => {
                    legacy.remove_classes.entry(selector).or_default().push(class)
                }
                #[allow(unreachable_patterns)]
                Some(_) => {
                    legacy.procedural_actions.insert(json);
                }
            }
        }
        legacy
    }
}

impl LegacyUrlSpecificResources {
    fn to_json(&self) -> serde_json::Value {
        serde_json::json!({
            "hide_selectors": self.hide_selectors,
            "style_selectors": self.style_selectors,
            "remove_selectors": self.remove_selectors,
            "remove_attrs": self.remove_attrs,
            "remove_classes": self.remove_classes,
            "procedural_actions": self.procedural_actions,
            "exceptions": self.exceptions,
            "injected_script": self.injected_script,
            "generichide": self.generichide,
        })
    }
}

impl Engine {
    // Tags are deprecated in adblock-rust 0.13 (Brave 1.98 rebuilds engines
    // instead), but still work; FlyWeb keeps them for its tag-based lists.
    #[allow(deprecated)]
    pub fn enable_tag(&mut self, tag: &CxxString) {
        self.engine.enable_tags(&[tag.to_str().unwrap()])
    }

    #[allow(deprecated)]
    pub fn disable_tag(&mut self, tag: &CxxString) {
        self.engine.disable_tags(&[tag.to_str().unwrap()])
    }

    #[allow(deprecated)]
    pub fn tag_exists(&self, key: &CxxString) -> bool {
        self.engine.tag_exists(key.to_str().unwrap())
    }

    pub fn matches(
        &self,
        url: &CxxString,
        hostname: &CxxString,
        source_hostname: &CxxString,
        request_type: &CxxString,
        third_party_request: bool,
        previously_matched_rule: bool,
        force_check_exceptions: bool,
    ) -> BlockerResult {
        // The following strings are guaranteed to be
        // UTF-8, so unwrapping directly should be okay.
        self.engine
            .check_network_request_subset(
                &adblock::request::Request::preparsed(
                    url.to_str().unwrap(),
                    hostname.to_str().unwrap(),
                    source_hostname.to_str().unwrap(),
                    request_type.to_str().unwrap(),
                    third_party_request,
                    // The C++ side does not pass the request method (Brave
                    // 1.98 added it to this call), so `$method` rules, which
                    // adblock-rust 0.8 could not parse, never match.
                    "",
                ),
                previously_matched_rule,
                force_check_exceptions,
            )
            .into()
    }

    pub fn get_csp_directives(
        &self,
        url: &CxxString,
        hostname: &CxxString,
        source_hostname: &CxxString,
        request_type: &CxxString,
        third_party_request: bool,
    ) -> String {
        // The following strings are also UTF-8.
        self.engine
            .get_csp_directives(&adblock::request::Request::preparsed(
                url.to_str().unwrap(),
                hostname.to_str().unwrap(),
                source_hostname.to_str().unwrap(),
                request_type.to_str().unwrap(),
                third_party_request,
                "",
            ))
            .unwrap_or_default()
    }

    /// A 0-length vector will be returned if there was any issue during serialization. Be sure to
    /// handle that case.
    pub fn serialize(&self) -> Vec<u8> {
        self.engine.serialize()
    }

    pub fn deserialize(&mut self, serialized: &CxxVector<u8>) -> bool {
        self.engine.deserialize(serialized.as_slice()).is_ok()
    }

    pub fn add_resource(
        &mut self,
        name: &CxxString,
        content_type: &CxxString,
        data: &CxxString,
    ) -> UnitResult {
        || -> Result<(), InternalError> {
            let resource = Resource {
                name: name.to_str()?.to_string(),
                aliases: vec![],
                kind: ResourceType::Mime(MimeType::from(content_type.to_str()?)),
                content: data.to_string(),
                dependencies: vec![],
                // user-added resources require full permissions
                permission: adblock::resources::PermissionMask::from_bits(0b11111111),
            };
            let mut resources = self.resources.as_ref().clone();
            resources.add_resource(resource)?;
            self.set_resources(resources);
            Ok(())
        }()
        .into()
    }

    pub fn use_resources(&mut self, resources_json: &CxxString) -> bool {
        resources_json
            .to_str()
            .ok()
            .and_then(|resources_json| serde_json::from_str::<Vec<Resource>>(resources_json).ok())
            .map(|resources| {
                self.set_resources(InMemoryResourceStorage::from_resources(resources));
            })
            .is_some()
    }

    pub fn url_cosmetic_resources(&self, url: &CxxString) -> String {
        let resources = self.engine.url_cosmetic_resources(url.to_str().unwrap());
        LegacyUrlSpecificResources::from(resources).to_json().to_string()
    }

    pub fn hidden_class_id_selectors(
        &self,
        classes: &CxxVector<CxxString>,
        ids: &CxxVector<CxxString>,
        exceptions: &CxxVector<CxxString>,
    ) -> VecStringResult {
        || -> Result<Vec<String>, InternalError> {
            let classes: Vec<String> = convert_cxx_string_vector_to_string_collection(classes)?;
            let ids: Vec<String> = convert_cxx_string_vector_to_string_collection(ids)?;
            let exceptions: HashSet<String> =
                convert_cxx_string_vector_to_string_collection(exceptions)?;
            Ok(self.engine.hidden_class_id_selectors(&classes, &ids, &exceptions))
        }()
        .into()
    }

    pub fn get_regex_debug_info(&self) -> RegexDebugInfo {
        self.engine.get_debug_info().regex_debug_info.into()
    }

    pub fn discard_regex(&mut self, regex_id: u64) {
        self.engine.discard_regex(regex_id)
    }

    pub fn set_regex_discard_policy(&mut self, new_discard_policy: &RegexManagerDiscardPolicy) {
        self.engine.set_regex_discard_policy(new_discard_policy.into())
    }
}
