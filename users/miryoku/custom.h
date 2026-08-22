// Copyright 2023 Scan Line
// https://github.com/scan-line

// This program is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 2 of the License, or (at your option) any later version. This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details. You should have received a copy of the GNU General Public License along with this program. If not, see <http://www.gnu.org/licenses/>.

#pragma once

#include <stdint.h>

#include "action_layer.h"
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


// Custom double-tap implementation

#define QK_DT_GET_INDEX(CODE) QK_TAP_DANCE_GET_INDEX(CODE)
#define IS_QK_DOUBLE_TAP IS_QK_TAP_DANCE

typedef struct {
  uint8_t count;
} double_tap_state_t;

typedef void (*double_tap_function_t)(double_tap_state_t *state, void *reserved);

extern double_tap_function_t double_taps[];


#define ACTION_TAP_DANCE_FN(FUNCTION) FUNCTION
#define tap_dance_state_t double_tap_state_t
#define tap_dance_action_t double_tap_function_t
#define tap_dance_actions double_taps


// Custom key-override implementation

typedef struct {
  uint16_t trigger;
  uint16_t replacement;
  layer_state_t layers;
} shift_override_t;

#define make_shift_override(TRIGGER, REPLACEMENT, LAYERS) \
  ((const shift_override_t){        \
    .trigger = (TRIGGER),           \
    .replacement = (REPLACEMENT),   \
    .layers = (LAYERS)              \
  })

extern const shift_override_t* const shift_overrides[];


#define key_override_t shift_override_t
#define ko_make_basic(MASK, TRIGGER, REPLACEMENT) \
  make_shift_override(TRIGGER, REPLACEMENT, ~0)
