// Copyright 2023 Scan Line
// https://github.com/scan-line

// This program is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 2 of the License, or (at your option) any later version. This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details. You should have received a copy of the GNU General Public License along with this program. If not, see <http://www.gnu.org/licenses/>.

#pragma once

#include "keycodes.h"


// Keycodes

enum my_keycodes {
  U_USER = QK_USER,
  U_WIN,
  U_MAC,
  U_LNX,
  U_RGB_TOG,
  U_RGB_MOD,
  U_RGB_HUI,
  U_RGB_SAI,
  U_RGB_VAI,
  U_AUD_TOG,
};


// OS mode

typedef enum {
  OS_MODE_WIN,
  OS_MODE_MAC,
  OS_MODE_LNX,
} os_mode_t;

os_mode_t os_mode_get(void);
