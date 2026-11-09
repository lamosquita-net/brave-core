// Copyright (c) 2022 The Brave Authors. All rights reserved.
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this file,
// you can obtain one at https://mozilla.org/MPL/2.0/.
import * as React from 'react'
import styled from 'styled-components'
import { getLocale } from '$web-common/locale'
import moscaSvg from '../flyweb/moscaSvg'
import svgEnReact from '../flyweb/svgEnReact'

const Box = styled.div`
  display: flex;
  justify-content: center;
  flex-direction: column;
  align-items: center;
  gap: 40px;
  width: 100%;
`

const Form = styled.form`
  --bg-color: rgba(255, 255, 255, 0.22);
  --box-shadow: 0px 2px 70px rgba(0, 0, 0, 0.3);

  display: grid;
  grid-template-columns: 1fr 50px;
  align-items: center;
  width: 100%;
  height: 52px;
  font-family: ${p => p.theme.fontFamily.heading};
  color: white;
  font-size: 14px;
  font-weight: 400;
  background: var(--bg-color);
  border-radius: 8px;
  transition: box-shadow 0.3s ease-in-out;
  overflow: hidden;

  &:focus-within,
  &:hover {
    box-shadow: var(--box-shadow);
  }

  input[type="text"] {
    width: 100%;
    height: 36px;
    border: 0;
    background-color: transparent;
    padding: 5px 16px;

    &:focus {
      outline: 0;
    }

    &::placeholder {
      color: rgba(255,255,255,0.7);
    }
  }
`

const IconButton = styled.button`
  background: transparent;
  padding: 0;
  margin: 0;
  border: 0;
  width: 100%;
  height: 100%;
  cursor: pointer;

  &:hover {
    background: linear-gradient(304.74deg, #6F4CD2 15.81%, #BF14A2 63.17%, #F73A1C 100%);

    path {
      fill: white;
    }
  }
`

// FlyWeb: the fly instead of the Brave Search logo (the search now goes to the user's default
// search engine, see BravePrivateNewTabPageHandler::GoToSearch). Built once: React elements,
// not an HTML string (Trusted Types, see flyweb/svgEnReact.tsx).
const moscaDibujo = svgEnReact(moscaSvg)

const Mosca = styled.div`
  width: 72px;
  height: 72px;
  margin: 0 auto 24px;

  svg {
    width: 100%;
    height: 100%;
  }
`

interface Props {
  onSubmit?: (value: string, openNewTab: boolean) => unknown
}

function Search (props: Props) {
  const [value, setValue] = React.useState('')
  const inputRef = React.useRef<HTMLInputElement>(null)

  const onInputChange = (e: React.ChangeEvent<HTMLInputElement>) => {
    setValue(e.currentTarget.value)
  }

  const handleFormBoxClick = () => {
    inputRef.current && inputRef.current.focus()
  }

  const handleSubmit = (e: React.ChangeEvent<HTMLFormElement>) => {
    e.preventDefault()
    props.onSubmit?.(value, false)
  }

  const handleKeyDown = (e: React.KeyboardEvent<HTMLInputElement>) => {
    if (value === '') return

    if ((e.metaKey || e.ctrlKey) && (e.key === 'Enter')) {
      props.onSubmit?.(value, true)
    }
  }

  return (
    <Box>
      <Mosca aria-hidden="true">{moscaDibujo}</Mosca>
      <Form onSubmit={handleSubmit} onClick={handleFormBoxClick} role="search" aria-label={getLocale('searchPlaceholderLabel')}>
        <input ref={inputRef} onChange={onInputChange} onKeyDown={handleKeyDown} type="text" placeholder={getLocale('searchPlaceholderLabel')} value={value} autoCapitalize="off" autoComplete="off" autoCorrect="off" spellCheck="false" aria-label="Search" title="Search" aria-autocomplete="none" aria-haspopup="false" maxLength={2048} autoFocus />
        <IconButton data-test-id="submit_button" aria-label="Submit">
          <svg width="20" height="20" fill="none" xmlns="http://www.w3.org/2000/svg"><path fillRule="evenodd" clipRule="evenodd" d="M8 16a8 8 0 1 1 5.965-2.67l5.775 5.28a.8.8 0 1 1-1.08 1.18l-5.88-5.375A7.965 7.965 0 0 1 8 16Zm4.374-3.328a.802.802 0 0 0-.201.18 6.4 6.4 0 1 1 .202-.181Z" fill="url(#search_icon_gr)"/><defs><linearGradient id="search_icon_gr" x1="20" y1="20" x2="-2.294" y2="3.834" gradientUnits="userSpaceOnUse"><stop stopColor="#BF14A2"/><stop offset="1" stopColor="#F73A1C"/></linearGradient></defs></svg>
        </IconButton>
      </Form>
    </Box>
  )
}

export default Search
