// Copyright 2023 Scan Line
// https://github.com/scan-line

// This program is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 2 of the License, or (at your option) any later version. This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details. You should have received a copy of the GNU General Public License along with this program. If not, see <http://www.gnu.org/licenses/>.

#pragma once

#include "keycodes.h"


// Keycode changes

#define RGB_TOG QK_RGB_MATRIX_TOGGLE
#define RGB_MOD QK_RGB_MATRIX_MODE_NEXT
#define RGB_HUI QK_RGB_MATRIX_HUE_UP
#define RGB_SAI QK_RGB_MATRIX_SATURATION_UP
#define RGB_VAI QK_RGB_MATRIX_VALUE_UP

#define KC_MS_U QK_MOUSE_CURSOR_UP
#define KC_MS_D QK_MOUSE_CURSOR_DOWN
#define KC_MS_L QK_MOUSE_CURSOR_LEFT
#define KC_MS_R QK_MOUSE_CURSOR_RIGHT
#define KC_BTN1 QK_MOUSE_BUTTON_1
#define KC_BTN2 QK_MOUSE_BUTTON_2
#define KC_BTN3 QK_MOUSE_BUTTON_3
#define KC_WH_U	QK_MOUSE_WHEEL_UP
#define KC_WH_D	QK_MOUSE_WHEEL_DOWN
#define KC_WH_L	QK_MOUSE_WHEEL_LEFT
#define KC_WH_R	QK_MOUSE_WHEEL_RIGHT


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
