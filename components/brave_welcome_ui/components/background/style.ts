// Copyright (c) 2022 The Brave Authors. All rights reserved.
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this file,
// you can obtain one at https://mozilla.org/MPL/2.0/.

import styled from 'styled-components'

export const Box = styled.div`
  .content-box {
    position: fixed;
    width: 100%;
    height: 100%;
    z-index: 999;

    display: flex;
    align-items: center;
    justify-content: center;
  }

  .background-img {
    position: fixed;
    width: 100%;
    height: 100%;
    z-index: 1;
    object-fit: cover;
    opacity: 0;
    transition: opacity .2s ease-in;

    &.is-visible {
      opacity: 1;
    }
  }
`
