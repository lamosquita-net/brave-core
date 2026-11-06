/* Copyright (c) 2019 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "brave/browser/brave_content_browser_client.h"

#include <algorithm>
#include <optional>
#include <string>
#include <utility>
#include <vector>

#include "base/command_line.h"
#include "base/functional/bind.h"
#include "base/json/json_reader.h"
#include "base/strings/strcat.h"
#include "base/system/sys_info.h"
#include "brave/browser/bluetooth/brave_bluetooth_delegate.h"
#include "brave/browser/brave_browser_features.h"
#include "brave/browser/brave_browser_main_extra_parts.h"
#include "brave/browser/brave_browser_process.h"
#include "brave/browser/brave_shields/brave_shields_web_contents_observer.h"
#include "brave/browser/debounce/debounce_service_factory.h"
#include "brave/browser/ephemeral_storage/ephemeral_storage_service_factory.h"
#include "brave/browser/flyweb/declared_version.h"
#include "brave/browser/ephemeral_storage/ephemeral_storage_tab_helper.h"
#include "brave/browser/net/brave_proxying_url_loader_factory.h"
#include "brave/browser/net/brave_proxying_web_socket.h"
#include "brave/browser/profiles/brave_renderer_updater.h"
#include "brave/browser/profiles/brave_renderer_updater_factory.h"
#include "brave/browser/ui/brave_ui_features.h"
#include "brave/browser/url_sanitizer/url_sanitizer_service_factory.h"
#include "brave/components/body_sniffer/body_sniffer_throttle.h"
#include "brave/components/brave_search/browser/brave_search_default_host.h"
#include "brave/components/brave_search/browser/brave_search_default_host_private.h"
#include "brave/components/brave_search/browser/brave_search_fallback_host.h"
#include "brave/components/brave_search/common/brave_search_default.mojom.h"
#include "brave/components/brave_search/common/brave_search_fallback.mojom.h"
#include "brave/components/brave_search/common/brave_search_utils.h"
#include "brave/components/brave_shields/content/browser/ad_block_service.h"
#include "brave/components/brave_shields/content/browser/brave_farbling_service.h"
#include "brave/components/brave_shields/content/browser/brave_shields_util.h"
#include "brave/components/brave_shields/content/browser/domain_block_navigation_throttle.h"
#include "brave/components/brave_shields/core/common/brave_shield_constants.h"
#include "brave/components/brave_shields/core/common/features.h"
#include "brave/components/constants/pref_names.h"
#include "brave/components/constants/webui_url_constants.h"
#include "brave/components/cosmetic_filters/browser/cosmetic_filters_resources.h"
#include "brave/components/cosmetic_filters/common/cosmetic_filters.mojom.h"
#include "brave/components/de_amp/browser/de_amp_body_handler.h"
#include "brave/components/debounce/content/browser/debounce_navigation_throttle.h"
#include "brave/components/google_sign_in_permission/google_sign_in_permission_throttle.h"
#include "brave/components/google_sign_in_permission/google_sign_in_permission_util.h"
#include "brave/components/request_otr/common/buildflags/buildflags.h"
#include "brave/components/translate/core/common/brave_translate_switches.h"
#include "brave/components/url_sanitizer/browser/url_sanitizer_service.h"
#include "brave/grit/brave_generated_resources.h"
#include "brave/third_party/blink/renderer/brave_farbling_constants.h"
#include "build/build_config.h"
#include "chrome/browser/bluetooth/chrome_bluetooth_delegate_impl_client.h"
#include "chrome/browser/browser_process.h"
#include "chrome/browser/chrome_browser_interface_binders.h"
#include "chrome/browser/chrome_content_browser_client.h"
#include "chrome/browser/content_settings/host_content_settings_map_factory.h"
#include "chrome/browser/profiles/profile.h"
#include "chrome/browser/profiles/profile_io_data.h"
#include "chrome/browser/search_engines/template_url_service_factory.h"
#include "chrome/common/url_constants.h"
#include "chrome/grit/branded_strings.h"
#include "components/content_settings/browser/page_specific_content_settings.h"
#include "components/content_settings/core/browser/host_content_settings_map.h"
#include "components/embedder_support/switches.h"
#include "components/prefs/pref_service.h"
#include "components/services/heap_profiling/public/mojom/heap_profiling_client.mojom.h"
#include "components/user_prefs/user_prefs.h"
#include "components/version_info/version_info.h"
#include "content/public/browser/browser_context.h"
#include "content/public/browser/browser_task_traits.h"
#include "content/public/browser/browser_thread.h"
#include "content/public/browser/browser_url_handler.h"
#include "content/public/browser/navigation_handle.h"
#include "content/public/browser/navigation_throttle_registry.h"
#include "content/public/browser/render_frame_host.h"
#include "content/public/browser/render_process_host.h"
#include "content/public/browser/site_instance.h"
#include "content/public/browser/storage_partition.h"
#include "content/public/browser/weak_document_ptr.h"
#include "content/public/browser/web_ui_browser_interface_broker_registry.h"
#include "content/public/browser/web_ui_controller_interface_binder.h"
#include "content/public/common/content_client.h"
#include "content/public/common/content_switches.h"
#include "content/public/common/url_constants.h"
#include "extensions/buildflags/buildflags.h"
#include "mojo/public/cpp/bindings/pending_receiver.h"
#include "mojo/public/cpp/bindings/self_owned_receiver.h"
#include "net/base/registry_controlled_domains/registry_controlled_domain.h"
#include "net/cookies/site_for_cookies.h"
#include "services/service_manager/public/cpp/binder_registry.h"
#include "third_party/blink/public/common/associated_interfaces/associated_interface_registry.h"
#include "third_party/blink/public/common/loader/url_loader_throttle.h"
#include "third_party/blink/public/mojom/webpreferences/web_preferences.mojom.h"
#include "third_party/widevine/cdm/buildflags.h"
#include "ui/base/l10n/l10n_util.h"

#if BUILDFLAG(ENABLE_REQUEST_OTR)
#include "brave/browser/request_otr/request_otr_service_factory.h"
#include "brave/components/request_otr/browser/request_otr_navigation_throttle.h"
#include "brave/components/request_otr/browser/request_otr_storage_tab_helper.h"
#endif

using blink::web_pref::WebPreferences;
using brave_shields::BraveShieldsWebContentsObserver;
using brave_shields::ControlType;
using brave_shields::GetBraveShieldsEnabled;
using brave_shields::GetFingerprintingControlType;
using content::BrowserThread;
using content::ContentBrowserClient;
using content::RenderFrameHost;
using content::WebContents;

#if BUILDFLAG(ENABLE_EXTENSIONS)
#include "chrome/browser/extensions/chrome_content_browser_client_extensions_part.h"
#include "extensions/browser/extension_registry.h"
#include "extensions/common/constants.h"

using extensions::ChromeContentBrowserClientExtensionsPart;
#endif

#if BUILDFLAG(ENABLE_WIDEVINE)
#include "brave/browser/brave_drm_tab_helper.h"
#endif

#if !BUILDFLAG(IS_ANDROID)
#include "brave/browser/new_tab/new_tab_shows_navigation_throttle.h"
#include "brave/browser/ui/geolocation/brave_geolocation_permission_tab_helper.h"
#include "brave/browser/ui/webui/brave_settings_ui.h"
#include "brave/browser/ui/webui/brave_shields/shields_panel_ui.h"
#include "brave/browser/ui/webui/new_tab_page/brave_new_tab_ui.h"
#include "brave/browser/ui/webui/private_new_tab_page/brave_private_new_tab_ui.h"
#include "brave/components/brave_new_tab_ui/brave_new_tab_page.mojom.h"
#include "brave/components/brave_private_new_tab_ui/common/brave_private_new_tab.mojom.h"
#include "brave/components/brave_shields/core/common/brave_shields_panel.mojom.h"
#include "brave/components/commands/common/commands.mojom.h"
#include "brave/components/commands/common/features.h"
#include "components/omnibox/browser/searchbox.mojom.h"
#endif

namespace {

bool HandleURLReverseOverrideRewrite(GURL* url,
                                     content::BrowserContext* browser_context) {
  return BraveContentBrowserClient::HandleURLOverrideRewrite(url,
                                                             browser_context);
}

bool HandleURLRewrite(GURL* url, content::BrowserContext* browser_context) {
  return BraveContentBrowserClient::HandleURLOverrideRewrite(url,
                                                             browser_context);
}

void BindCosmeticFiltersResourcesOnTaskRunner(
    mojo::PendingReceiver<cosmetic_filters::mojom::CosmeticFiltersResources>
        receiver) {
  mojo::MakeSelfOwnedReceiver(
      std::make_unique<cosmetic_filters::CosmeticFiltersResources>(
          g_brave_browser_process->ad_block_service()),
      std::move(receiver));
}

void BindCosmeticFiltersResources(
    content::RenderFrameHost* const frame_host,
    mojo::PendingReceiver<cosmetic_filters::mojom::CosmeticFiltersResources>
        receiver) {
  g_brave_browser_process->ad_block_service()->GetTaskRunner()->PostTask(
      FROM_HERE, base::BindOnce(&BindCosmeticFiltersResourcesOnTaskRunner,
                                std::move(receiver)));
}

void BindBraveSearchFallbackHost(
    int process_id,
    mojo::PendingReceiver<brave_search::mojom::BraveSearchFallback> receiver) {
  content::RenderProcessHost* render_process_host =
      content::RenderProcessHost::FromID(process_id);
  if (!render_process_host) {
    return;
  }

  content::BrowserContext* context = render_process_host->GetBrowserContext();
  mojo::MakeSelfOwnedReceiver(
      std::make_unique<brave_search::BraveSearchFallbackHost>(
          context->GetDefaultStoragePartition()
              ->GetURLLoaderFactoryForBrowserProcess()),
      std::move(receiver));
}

void BindBraveSearchDefaultHost(
    content::RenderFrameHost* const frame_host,
    mojo::PendingReceiver<brave_search::mojom::BraveSearchDefault> receiver) {
  auto* context = frame_host->GetBrowserContext();
  auto* profile = Profile::FromBrowserContext(context);
  if (profile->IsRegularProfile()) {
    auto* template_url_service =
        TemplateURLServiceFactory::GetForProfile(profile);
    const std::string host = frame_host->GetLastCommittedURL().GetHost();
    mojo::MakeSelfOwnedReceiver(
        std::make_unique<brave_search::BraveSearchDefaultHost>(
            host, template_url_service, profile->GetPrefs()),
        std::move(receiver));
  } else {
    // Dummy API which always returns false for private contexts.
    mojo::MakeSelfOwnedReceiver(
        std::make_unique<brave_search::BraveSearchDefaultHostPrivate>(),
        std::move(receiver));
  }
}

}  // namespace

BraveContentBrowserClient::BraveContentBrowserClient() = default;

BraveContentBrowserClient::~BraveContentBrowserClient() = default;

std::unique_ptr<content::BrowserMainParts>
BraveContentBrowserClient::CreateBrowserMainParts(bool is_integration_test) {
  std::unique_ptr<content::BrowserMainParts> main_parts =
      ChromeContentBrowserClient::CreateBrowserMainParts(is_integration_test);
  ChromeBrowserMainParts* chrome_main_parts =
      static_cast<ChromeBrowserMainParts*>(main_parts.get());
  chrome_main_parts->AddParts(std::make_unique<BraveBrowserMainExtraParts>());
  return main_parts;
}

bool BraveContentBrowserClient::AreIsolatedWebAppsEnabled(
    content::BrowserContext* browser_context) {
  return false;
}

void BraveContentBrowserClient::BrowserURLHandlerCreated(
    content::BrowserURLHandler* handler) {
  handler->AddHandlerPair(&HandleURLRewrite, &HandleURLReverseOverrideRewrite);
  ChromeContentBrowserClient::BrowserURLHandlerCreated(handler);
}

void BraveContentBrowserClient::RenderProcessWillLaunch(
    content::RenderProcessHost* host) {
  Profile* profile = Profile::FromBrowserContext(host->GetBrowserContext());
  // The BraveRendererUpdater might be null for some irregular profiles, e.g.
  // the System Profile.
  if (BraveRendererUpdater* service =
          BraveRendererUpdaterFactory::GetForProfile(profile)) {
    service->InitializeRenderer(host);
  }

  ChromeContentBrowserClient::RenderProcessWillLaunch(host);
}

void BraveContentBrowserClient::
    RegisterAssociatedInterfaceBindersForRenderFrameHost(
        content::RenderFrameHost& render_frame_host,                // NOLINT
        blink::AssociatedInterfaceRegistry& associated_registry) {  // NOLINT
#if BUILDFLAG(ENABLE_WIDEVINE)
  associated_registry.AddInterface<
      brave_drm::mojom::BraveDRM>(base::BindRepeating(
      [](content::RenderFrameHost* render_frame_host,
         mojo::PendingAssociatedReceiver<brave_drm::mojom::BraveDRM> receiver) {
        BraveDrmTabHelper::BindBraveDRM(std::move(receiver), render_frame_host);
      },
      &render_frame_host));
#endif  // BUILDFLAG(ENABLE_WIDEVINE)

#if !BUILDFLAG(IS_ANDROID)
  associated_registry.AddInterface<
      geolocation::mojom::BraveGeolocationPermission>(base::BindRepeating(
      [](content::RenderFrameHost* render_frame_host,
         mojo::PendingAssociatedReceiver<
             geolocation::mojom::BraveGeolocationPermission> receiver) {
        BraveGeolocationPermissionTabHelper::BindBraveGeolocationPermission(
            std::move(receiver), render_frame_host);
      },
      &render_frame_host));
#endif  // !BUILDFLAG(IS_ANDROID)

  associated_registry.AddInterface<
      brave_shields::mojom::BraveShieldsHost>(base::BindRepeating(
      [](content::RenderFrameHost* render_frame_host,
         mojo::PendingAssociatedReceiver<brave_shields::mojom::BraveShieldsHost>
             receiver) {
        brave_shields::BraveShieldsWebContentsObserver::BindBraveShieldsHost(
            std::move(receiver), render_frame_host);
      },
      &render_frame_host));

  ChromeContentBrowserClient::
      RegisterAssociatedInterfaceBindersForRenderFrameHost(render_frame_host,
                                                           associated_registry);
}

void BraveContentBrowserClient::RegisterTrustedWebUIInterfaceBrokers(
    content::WebUIBrowserInterfaceBrokerRegistry& registry) {
  ChromeContentBrowserClient::RegisterTrustedWebUIInterfaceBrokers(registry);

#if !BUILDFLAG(IS_ANDROID)
  auto ntp_registration =
      registry.ForWebUI<BraveNewTabUI>()
          .Add<brave_new_tab_page::mojom::PageHandlerFactory>();

  if (base::FeatureList::IsEnabled(features::kBraveNtpSearchWidget)) {
    ntp_registration.Add<searchbox::mojom::PageHandlerFactory>();
  }
#endif
}

std::optional<base::UnguessableToken>
BraveContentBrowserClient::GetEphemeralStorageToken(
    content::RenderFrameHost* render_frame_host,
    const url::Origin& origin) {
  DCHECK(render_frame_host);
  auto* wc = content::WebContents::FromRenderFrameHost(render_frame_host);
  if (!wc) {
    return std::nullopt;
  }

  auto* es_tab_helper =
      ephemeral_storage::EphemeralStorageTabHelper::FromWebContents(wc);
  if (!es_tab_helper) {
    return std::nullopt;
  }

  return es_tab_helper->GetEphemeralStorageToken(origin);
}

bool BraveContentBrowserClient::AllowWorkerFingerprinting(
    const GURL& url,
    content::BrowserContext* browser_context) {
  return WorkerGetBraveFarblingLevel(url, browser_context) !=
         BraveFarblingLevel::MAXIMUM;
}

uint8_t BraveContentBrowserClient::WorkerGetBraveFarblingLevel(
    const GURL& url,
    content::BrowserContext* browser_context) {
  HostContentSettingsMap* host_content_settings_map =
      HostContentSettingsMapFactory::GetForProfile(browser_context);
  const bool shields_up =
      brave_shields::GetBraveShieldsEnabled(host_content_settings_map, url);
  if (!shields_up) {
    return BraveFarblingLevel::OFF;
  }
  auto fingerprinting_type = brave_shields::GetFingerprintingControlType(
      host_content_settings_map, url);
  if (fingerprinting_type == ControlType::BLOCK) {
    return BraveFarblingLevel::MAXIMUM;
  }
  if (fingerprinting_type == ControlType::ALLOW) {
    return BraveFarblingLevel::OFF;
  }
  return BraveFarblingLevel::BALANCED;
}

content::BluetoothDelegate* BraveContentBrowserClient::GetBluetoothDelegate() {
  if (!bluetooth_delegate_) {
    bluetooth_delegate_ = std::make_unique<BraveBluetoothDelegate>(
        std::make_unique<ChromeBluetoothDelegateImplClient>());
  }
  return bluetooth_delegate_.get();
}

bool BraveContentBrowserClient::CanCreateWindow(
    content::RenderFrameHost* opener,
    const GURL& opener_url,
    const GURL& opener_top_level_frame_url,
    const url::Origin& source_origin,
    content::mojom::WindowContainerType container_type,
    const GURL& target_url,
    const content::Referrer& referrer,
    const std::string& frame_name,
    WindowOpenDisposition disposition,
    const blink::mojom::WindowFeatures& features,
    bool user_gesture,
    bool opener_suppressed,
    bool* no_javascript_access) {
  // Check base implementation first
  bool can_create_window = ChromeContentBrowserClient::CanCreateWindow(
      opener, opener_url, opener_top_level_frame_url, source_origin,
      container_type, target_url, referrer, frame_name, disposition, features,
      user_gesture, opener_suppressed, no_javascript_access);

#if BUILDFLAG(ENABLE_REQUEST_OTR)
  // If the user has requested going off-the-record in this tab, don't allow
  // opening new windows via script
  if (content::WebContents* web_contents =
          content::WebContents::FromRenderFrameHost(opener)) {
    if (request_otr::
            RequestOTRStorageTabHelper* request_otr_storage_tab_helper =
                request_otr::RequestOTRStorageTabHelper::FromWebContents(
                    web_contents)) {
      if (request_otr_storage_tab_helper->has_requested_otr()) {
        *no_javascript_access = true;
      }
    }
  }
#endif

  return can_create_window && google_sign_in_permission::CanCreateWindow(
                                  opener, opener_url, target_url);
}

void BraveContentBrowserClient::ExposeInterfacesToRenderer(
    service_manager::BinderRegistry* registry,
    blink::AssociatedInterfaceRegistry* associated_registry,
    content::RenderProcessHost* render_process_host) {
  ChromeContentBrowserClient::ExposeInterfacesToRenderer(
      registry, associated_registry, render_process_host);
  registry->AddInterface(
      base::BindRepeating(&BindBraveSearchFallbackHost,
                          render_process_host->GetDeprecatedID()),
      content::GetUIThreadTaskRunner({}));
}

void BraveContentBrowserClient::RegisterBrowserInterfaceBindersForFrame(
    content::RenderFrameHost* render_frame_host,
    mojo::BinderMapWithContext<content::RenderFrameHost*>* map) {
  ChromeContentBrowserClient::RegisterBrowserInterfaceBindersForFrame(
      render_frame_host, map);
  map->Add<cosmetic_filters::mojom::CosmeticFiltersResources>(
      base::BindRepeating(&BindCosmeticFiltersResources));
  if (brave_search::IsDefaultAPIEnabled()) {
    map->Add<brave_search::mojom::BraveSearchDefault>(
        base::BindRepeating(&BindBraveSearchDefaultHost));
  }

#if !BUILDFLAG(IS_ANDROID)
  content::RegisterWebUIControllerInterfaceBinder<
      brave_private_new_tab::mojom::PageHandler, BravePrivateNewTabUI>(map);
  content::RegisterWebUIControllerInterfaceBinder<
      brave_shields::mojom::PanelHandlerFactory, ShieldsPanelUI>(map);
  if (base::FeatureList::IsEnabled(commands::features::kBraveCommands)) {
    content::RegisterWebUIControllerInterfaceBinder<
        commands::mojom::CommandsService, BraveSettingsUI>(map);
  }
#endif
}

bool BraveContentBrowserClient::HandleExternalProtocol(
    const GURL& url,
    content::WebContents::Getter web_contents_getter,
    content::FrameTreeNodeId frame_tree_node_id,
    content::NavigationUIData* navigation_data,
    bool is_primary_main_frame,
    bool is_in_fenced_frame_tree,
    network::mojom::WebSandboxFlags sandbox_flags,
    ui::PageTransition page_transition,
    bool has_user_gesture,
    const std::optional<url::Origin>& initiating_origin,
    content::RenderFrameHost* initiator_document,
    const net::IsolationInfo& isolation_info,
    mojo::PendingRemote<network::mojom::URLLoaderFactory>* out_factory) {
  return ChromeContentBrowserClient::HandleExternalProtocol(
      url, web_contents_getter, frame_tree_node_id, navigation_data,
      is_primary_main_frame, is_in_fenced_frame_tree, sandbox_flags,
      page_transition, has_user_gesture, initiating_origin, initiator_document,
      isolation_info, out_factory);
}

void BraveContentBrowserClient::AppendExtraCommandLineSwitches(
    base::CommandLine* command_line,
    int child_process_id) {
  ChromeContentBrowserClient::AppendExtraCommandLineSwitches(command_line,
                                                             child_process_id);
  std::string process_type =
      command_line->GetSwitchValueASCII(switches::kProcessType);
  if (process_type == switches::kRendererProcess) {
    uint64_t session_token =
        12345;  // the kinda thing an idiot would have on his luggage

    // Command line parameters from the browser process are propagated to the
    // renderers *after* ContentBrowserClient::AppendExtraCommandLineSwitches()
    // is called from RenderProcessHostImpl::AppendRendererCommandLine(). This
    // means we have to inspect the main browser process' parameters for the
    // |switches::kTestType| as it will be too soon to find it on command_line.
    const base::CommandLine& browser_command_line =
        *base::CommandLine::ForCurrentProcess();
    if (!browser_command_line.HasSwitch(switches::kTestType)) {
      content::RenderProcessHost* process =
          content::RenderProcessHost::FromID(child_process_id);
      Profile* profile =
          process ? Profile::FromBrowserContext(process->GetBrowserContext())
                  : nullptr;
      session_token =
          g_brave_browser_process->brave_farbling_service()->session_token(
              profile && !profile->IsOffTheRecord());

      if (command_line->HasSwitch(switches::kEnableIsolatedWebAppsInRenderer)) {
        command_line->RemoveSwitch(switches::kEnableIsolatedWebAppsInRenderer);
      }
    }
    command_line->AppendSwitchASCII("brave_session_token",
                                    base::NumberToString(session_token));

    // Switches to pass to render processes.
    static const char* const kSwitchNames[] = {
        translate::switches::kBraveTranslateUseGoogleEndpoint,
    };
    command_line->CopySwitchesFrom(browser_command_line, kSwitchNames);
  }
}

std::vector<std::unique_ptr<blink::URLLoaderThrottle>>
BraveContentBrowserClient::CreateURLLoaderThrottles(
    const network::ResourceRequest& request,
    content::BrowserContext* browser_context,
    const content::WebContents::Getter& wc_getter,
    content::NavigationUIData* navigation_ui_data,
    content::FrameTreeNodeId frame_tree_node_id,
    std::optional<int64_t> navigation_id) {
  auto result = ChromeContentBrowserClient::CreateURLLoaderThrottles(
      request, browser_context, wc_getter, navigation_ui_data,
      frame_tree_node_id, navigation_id);
  content::WebContents* contents = wc_getter.Run();

  if (contents) {
    const bool isMainFrame =
        request.resource_type ==
        static_cast<int>(blink::mojom::ResourceType::kMainFrame);

    auto body_sniffer_throttle =
        std::make_unique<body_sniffer::BodySnifferThrottle>(
            base::SingleThreadTaskRunner::GetCurrentDefault());

    if (isMainFrame) {
      // De-AMP
      auto handler = de_amp::DeAmpBodyHandler::Create(request, wc_getter);
      if (handler) {
        body_sniffer_throttle->AddHandler(std::move(handler));
      }
    }

    result.push_back(std::move(body_sniffer_throttle));

    if (auto google_sign_in_permission_throttle =
            google_sign_in_permission::GoogleSignInPermissionThrottle::
                MaybeCreateThrottleFor(request, wc_getter)) {
      result.push_back(std::move(google_sign_in_permission_throttle));
    }
  }

  return result;
}

void BraveContentBrowserClient::WillCreateURLLoaderFactory(
    content::BrowserContext* browser_context,
    content::RenderFrameHost* frame,
    int render_process_id,
    URLLoaderFactoryType type,
    const url::Origin& request_initiator,
    const net::IsolationInfo& isolation_info,
    std::optional<int64_t> navigation_id,
    ukm::SourceIdObj ukm_source_id,
    network::URLLoaderFactoryBuilder& factory_builder,
    mojo::PendingRemote<network::mojom::TrustedURLLoaderHeaderClient>*
        header_client,
    bool* bypass_redirect_checks,
    bool* disable_secure_dns,
    network::mojom::URLLoaderFactoryOverridePtr* factory_override,
    scoped_refptr<base::SequencedTaskRunner> navigation_response_task_runner,
    bool is_for_network_service) {
  // TODO(iefremov): Skip proxying for certain requests?
  BraveProxyingURLLoaderFactory::MaybeProxyRequest(
      browser_context, frame,
      type == URLLoaderFactoryType::kNavigation ? -1 : render_process_id,
      factory_builder, type, navigation_id, navigation_response_task_runner);

  ChromeContentBrowserClient::WillCreateURLLoaderFactory(
      browser_context, frame, render_process_id, type, request_initiator,
      isolation_info, std::move(navigation_id), ukm_source_id, factory_builder,
      header_client, bypass_redirect_checks, disable_secure_dns,
      factory_override, navigation_response_task_runner,
      is_for_network_service);
}

bool BraveContentBrowserClient::WillInterceptWebSocket(
    content::RenderFrameHost* frame) {
  return (frame != nullptr);
}

void BraveContentBrowserClient::CreateWebSocket(
    content::RenderFrameHost* frame,
    content::ContentBrowserClient::WebSocketFactory factory,
    const GURL& url,
    const net::SiteForCookies& site_for_cookies,
    const std::optional<std::string>& user_agent,
    mojo::PendingRemote<network::mojom::WebSocketHandshakeClient>
        handshake_client,
    content::ContentBrowserClient::WebSocketOptions options) {
  auto* proxy = BraveProxyingWebSocket::ProxyWebSocket(
      frame, std::move(factory), url, site_for_cookies, user_agent,
      std::move(handshake_client));

  if (ChromeContentBrowserClient::WillInterceptWebSocket(frame)) {
    // Chrome's proxy (extensions WebRequest) takes the options' header
    // client and calls Brave's factory with its own.
    ChromeContentBrowserClient::CreateWebSocket(
        frame, proxy->web_socket_factory(), url, site_for_cookies, user_agent,
        proxy->handshake_client().Unbind(), std::move(options));
  } else {
    // Nobody else proxies: the options' header client (156 enterprise custom
    // headers) is the downstream trusted-header client, as in 1.98.
    proxy->Start(std::move(options.header_client));
  }
}

void BraveContentBrowserClient::MaybeHideReferrer(
    content::BrowserContext* browser_context,
    const GURL& request_url,
    const GURL& document_url,
    blink::mojom::ReferrerPtr* referrer) {
  DCHECK(referrer && !referrer->is_null());
#if BUILDFLAG(ENABLE_EXTENSIONS)
  if (document_url.SchemeIs(extensions::kExtensionScheme) ||
      request_url.SchemeIs(extensions::kExtensionScheme)) {
    return;
  }
#endif
  if (document_url.SchemeIs(content::kChromeUIScheme) ||
      request_url.SchemeIs(content::kChromeUIScheme)) {
    return;
  }

  Profile* profile = Profile::FromBrowserContext(browser_context);
  const bool allow_referrers = brave_shields::AreReferrersAllowed(
      HostContentSettingsMapFactory::GetForProfile(profile), document_url);
  const bool shields_up = brave_shields::GetBraveShieldsEnabled(
      HostContentSettingsMapFactory::GetForProfile(profile), document_url);

  content::Referrer new_referrer;
  if (brave_shields::MaybeChangeReferrer(allow_referrers, shields_up,
                                         (*referrer)->url, request_url,
                                         &new_referrer)) {
    (*referrer)->url = new_referrer.url;
    (*referrer)->policy = new_referrer.policy;
  }
}

std::optional<GURL> BraveContentBrowserClient::GetEffectiveURL(
    content::BrowserContext* browser_context,
    const GURL& url) {
  Profile* profile = Profile::FromBrowserContext(browser_context);
  if (!profile) {
    return url;
  }

#if BUILDFLAG(ENABLE_EXTENSIONS)
  if (extensions::ChromeContentBrowserClientExtensionsPart::
          AreExtensionsDisabledForProfile(profile)) {
    return url;
  }
  return ChromeContentBrowserClientExtensionsPart::GetEffectiveURL(profile,
                                                                   url);
#else
  return url;
#endif
}

// [static]
bool BraveContentBrowserClient::HandleURLOverrideRewrite(
    GURL* url,
    content::BrowserContext* browser_context) {
  // Some of these rewrites are for WebUI pages with URL that has moved.
  // After rewrite happens, GetWebUIFactoryFunction() will work as expected.
  // (see browser\ui\webui\brave_web_ui_controller_factory.cc for more info)
  //
  // Scope of schema is intentionally narrower than content::HasWebUIScheme(url)
  // which also allows both `chrome-untrusted` and `chrome-devtools`.
  if (!url->SchemeIs(content::kBraveUIScheme) &&
      !url->SchemeIs(content::kChromeUIScheme)) {
    return false;
  }

  // brave://sync => brave://settings/braveSync
  if (url->host() == chrome::kBraveUISyncHost) {
    GURL::Replacements replacements;
    replacements.SetSchemeStr(content::kChromeUIScheme);
    replacements.SetHostStr(chrome::kChromeUISettingsHost);
    replacements.SetPathStr(kBraveSyncPath);
    *url = url->ReplaceComponents(replacements);
    return true;
  }

#if !BUILDFLAG(IS_ANDROID)
  // brave://adblock => brave://settings/shields/filters
  if (url->host() == kAdblockHost) {
    GURL::Replacements replacements;
    replacements.SetSchemeStr(content::kChromeUIScheme);
    replacements.SetHostStr(chrome::kChromeUISettingsHost);
    replacements.SetPathStr(kContentFiltersPath);
    *url = url->ReplaceComponents(replacements);
    return false;
  }
#endif

  // no special win10 welcome page
  if (url->host() == kWelcomeHost) {
    *url = GURL(kWelcomeURL);
    return true;
  }

  return false;
}

void BraveContentBrowserClient::CreateThrottlesForNavigation(
    content::NavigationThrottleRegistry& registry) {
  ChromeContentBrowserClient::CreateThrottlesForNavigation(registry);

  content::NavigationHandle& navigation_handle = registry.GetNavigationHandle();

#if !BUILDFLAG(IS_ANDROID)
  NewTabShowsNavigationThrottle::MaybeCreateAndAdd(registry);
#endif

  content::BrowserContext* context =
      navigation_handle.GetWebContents()->GetBrowserContext();

  // Debounce
  debounce::DebounceNavigationThrottle::MaybeCreateAndAdd(
      registry,
      debounce::DebounceServiceFactory::GetForBrowserContext(context));

  // The HostContentSettingsMap might be null for some irregular profiles, e.g.
  // the System Profile.
  auto* host_content_settings_map =
      HostContentSettingsMapFactory::GetForProfile(context);
  if (host_content_settings_map) {
    brave_shields::DomainBlockNavigationThrottle::MaybeCreateAndAdd(
        registry, g_brave_browser_process->ad_block_service(),
        g_brave_browser_process->ad_block_service()->custom_filters_provider(),
        EphemeralStorageServiceFactory::GetForContext(context),
        host_content_settings_map, g_browser_process->GetApplicationLocale());
  }

#if BUILDFLAG(ENABLE_REQUEST_OTR)
  // Request Off-The-Record
  request_otr::RequestOTRNavigationThrottle::MaybeCreateAndAdd(
      registry,
      request_otr::RequestOTRServiceFactory::GetForBrowserContext(context),
      EphemeralStorageServiceFactory::GetForContext(context),
      Profile::FromBrowserContext(context)->GetPrefs(),
      g_browser_process->GetApplicationLocale());
#endif
}

bool PreventDarkModeFingerprinting(WebContents* web_contents,
                                   WebPreferences* prefs) {
  Profile* profile =
      Profile::FromBrowserContext(web_contents->GetBrowserContext());
  // The HostContentSettingsMap might be null for some irregular profiles, e.g.
  // the System Profile.
  auto* host_content_settings_map =
      HostContentSettingsMapFactory::GetForProfile(profile);
  if (!host_content_settings_map) {
    return false;
  }
  const GURL url = web_contents->GetLastCommittedURL();
  const bool shields_up =
      brave_shields::GetBraveShieldsEnabled(host_content_settings_map, url);
  auto fingerprinting_type = brave_shields::GetFingerprintingControlType(
      host_content_settings_map, url);
  // https://github.com/brave/brave-browser/issues/15265
  // Always use color scheme Light if fingerprinting mode strict
  if (base::FeatureList::IsEnabled(
          brave_shields::features::kBraveDarkModeBlock) &&
      shields_up && fingerprinting_type == ControlType::BLOCK &&
      prefs->preferred_color_scheme !=
          blink::mojom::PreferredColorScheme::kLight) {
    prefs->preferred_color_scheme = blink::mojom::PreferredColorScheme::kLight;
    return true;
  }
  return false;
}

bool BraveContentBrowserClient::OverrideWebPreferencesAfterNavigation(
    WebContents* web_contents,
    content::SiteInstance& main_frame_site,
    WebPreferences* prefs) {
  bool changed =
      ChromeContentBrowserClient::OverrideWebPreferencesAfterNavigation(
          web_contents, main_frame_site, prefs);
  return PreventDarkModeFingerprinting(web_contents, prefs) || changed;
}

void BraveContentBrowserClient::OverrideWebPreferences(
    WebContents* web_contents,
    content::SiteInstance& main_frame_site,
    WebPreferences* web_prefs) {
  ChromeContentBrowserClient::OverrideWebPreferences(
      web_contents, main_frame_site, web_prefs);
  PreventDarkModeFingerprinting(web_contents, web_prefs);
  // 156 dropped allow_non_empty_navigator_plugins: DOMPluginArray always
  // builds the fixed PDF list, which the plugins farbling wrapper farbles.
}

std::string BraveContentBrowserClient::GetUserAgent() {
  const std::string user_agent = ChromeContentBrowserClient::GetUserAgent();
  if (base::CommandLine::ForCurrentProcess()->HasSwitch(
          embedder_support::kUserAgent)) {
    return user_agent;
  }
  return flyweb::DeclareUserAgent(user_agent);
}

blink::UserAgentMetadata BraveContentBrowserClient::GetUserAgentMetadata() {
  blink::UserAgentMetadata metadata =
      ChromeContentBrowserClient::GetUserAgentMetadata();
  // Expect the brand version lists to have brand version, chromium_version, and
  // greased version.
  base::CommandLine* command_line = base::CommandLine::ForCurrentProcess();
  if (command_line->HasSwitch(embedder_support::kUserAgent)) {
    return metadata;
  }
  // FlyWeb: embedder_support::GetUserAgentMetadata() already declares the
  // engine level and a real full version of it (chromium_src override, F4.8),
  // for the request headers too; no zeroed "131.0.0.0" full versions, which
  // no Chrome 131 sends. Two brands, without "Brave".
  DCHECK_EQ(2UL, metadata.brand_version_list.size());
  DCHECK_EQ(2UL, metadata.brand_full_version_list.size());
  return metadata;
}

GURL BraveContentBrowserClient::SanitizeURL(
    content::RenderFrameHost* render_frame_host,
    const GURL& url) {
  if (!base::FeatureList::IsEnabled(features::kBraveCopyCleanLinkFromJs)) {
    return url;
  }
  CHECK(render_frame_host);
  CHECK(render_frame_host->GetBrowserContext());
  auto* url_sanitizer_service =
      brave::URLSanitizerServiceFactory::GetForBrowserContext(
          render_frame_host->GetBrowserContext());
  CHECK(url_sanitizer_service);
  if (!url_sanitizer_service->CheckJsPermission(
          render_frame_host->GetLastCommittedURL())) {
    return url;
  }
  return url_sanitizer_service->SanitizeURL(url);
}
