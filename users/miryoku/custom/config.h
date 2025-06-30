// Copyright 2023 Scan Line
// https://github.com/scan-line

// This program is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 2 of the License, or (at your option) any later version. This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details. You should have received a copy of the GNU General Public License along with this program. If not, see <http://www.gnu.org/licenses/>.

#pragma once

#include "custom/rgb/rgb_undef.h"

#ifndef __ASSEMBLER__
#include "custom/custom.h"
#endif


// Layers

#define MIRYOKU_ALPHAS_QWERTY
#define MIRYOKU_EXTRA_COLEMAKDH
#define MIRYOKU_TAP_QWERTY

#define MIRYOKU_LAYER_MEDIA \
TD(U_TD_BOOT),     TD(U_TD_U_TAP),    TD(U_TD_U_EXTRA),  TD(U_TD_U_BASE),   U_NA,              U_RGB_TOG,         U_RGB_MOD,         U_RGB_HUI,         U_RGB_SAI,         U_RGB_VAI,         \
KC_LGUI,           KC_LALT,           KC_LCTL,           KC_LSFT,           U_NA,              U_AUD_TOG,         KC_MPRV,           KC_VOLD,           KC_VOLU,           KC_MNXT,           \
U_NA,              KC_ALGR,           TD(U_TD_U_FUN),    TD(U_TD_U_MEDIA),  U_NA,              OU_AUTO,           U_WIN,             U_MAC,             U_LNX,             U_NU,              \
U_NP,              U_NP,              U_NA,              U_NA,              U_NA,              KC_MSTP,           KC_MPLY,           KC_MUTE,           U_NP,              U_NP

#define MIRYOKU_LAYER_SYM \
KC_LCBR,           KC_AMPR,           KC_ASTR,           U_USER,            KC_RCBR,           U_NA,              TD(U_TD_U_BASE),   TD(U_TD_U_EXTRA),  TD(U_TD_U_TAP),    TD(U_TD_BOOT),     \
KC_COLN,           KC_DLR,            KC_PERC,           KC_CIRC,           KC_PLUS,           U_NA,              KC_LSFT,           KC_LCTL,           KC_LALT,           KC_LGUI,           \
KC_TILD,           KC_EXLM,           KC_AT,             KC_HASH,           KC_PIPE,           U_NA,              TD(U_TD_U_SYM),    TD(U_TD_U_MOUSE),  KC_ALGR,           U_NA,              \
U_NP,              U_NP,              KC_LPRN,           KC_RPRN,           KC_UNDS,           U_NA,              U_NA,              U_NA,              U_NP,              U_NP


// Sound design

#ifdef AUDIO_ENABLE

#define OS_MODE_SET_SONG SONG(AG_NORM_SOUND)
#define LAYER_SET_SONG SONG(AG_SWAP_SOUND)
#define TOGGLE_ON_SONG SONG(AUDIO_ON_SOUND)
#define TOGGLE_OFF_SONG SONG(AUDIO_OFF_SOUND)
#define DETENT_SONG SONG(Q__NOTE(_A4),)

#define AUDIO_INIT_DELAY

#define AUDIO_ON_SONG SONG(NO_SOUND)
#define AUDIO_OFF_SONG SONG(NO_SOUND)
#define CG_NORM_SONG SONG(NO_SOUND)
#define CG_SWAP_SONG SONG(NO_SOUND)

#endif


// Lighting design

#ifdef RGBLIGHT_ENABLE

#define RGBLIGHT_DISABLE_KEYCODES

#endif

#ifdef RGB_MATRIX_ENABLE

#define RGB_MATRIX_DISABLE_KEYCODES

#define RGB_MATRIX_DEFAULT_MODE RGB_MATRIX_FEEDBACK
#define RGB_MATRIX_DEFAULT_HUE 0
#define RGB_MATRIX_DEFAULT_SAT 255
#define RGB_MATRIX_DEFAULT_VAL (RGB_MATRIX_MAXIMUM_BRIGHTNESS / 2)
#define RGB_MATRIX_DEFAULT_SPD 60
#define RGB_MATRIX_HUE_STEP 8
#define RGB_MATRIX_SAT_STEP 16
#define RGB_MATRIX_VAL_STEP 16
#define RGB_MATRIX_SPD_STEP 16
#define RGB_MATRIX_TIMEOUT 900000

#define RGB_MATRIX_SLEEP

#define ENABLE_RGB_MATRIX_FEEDBACK
#define ENABLE_RGB_MATRIX_SOLID_COLOR
#define ENABLE_RGB_MATRIX_ALPHAS_MODS
#define ENABLE_RGB_MATRIX_GRADIENT_UP_DOWN
#define ENABLE_RGB_MATRIX_GRADIENT_LEFT_RIGHT
#define ENABLE_RGB_MATRIX_HUE_BREATHING
#define ENABLE_RGB_MATRIX_RIVERFLOW
#define ENABLE_RGB_MATRIX_PIXEL_FLOW
#define ENABLE_RGB_MATRIX_SLOWDROPS
#define ENABLE_RGB_MATRIX_SOLID_REACTIVE_MULTIWIDE
#define ENABLE_RGB_MATRIX_SOLID_REACTIVE_MULTINEXUS
#define ENABLE_RGB_MATRIX_TYPING_HEATMAP
#define RGB_MATRIX_EFFECTS_EXPECTED 12

#endif


// Feedback

#define FEEDBACK_TIMEOUT 3000


// Configure QMK

// Tapping terms
// Beware - Miryoku config.h unconditionally resets TAPPING_TERM to 200
#define TAPPING_TERM_PER_KEY
#define FAST_TAPPING_TERM (TAPPING_TERM)
#define SLOW_TAPPING_TERM (TAPPING_TERM + 100)
#define DOUBLE_TAPPING_TERM (TAPPING_TERM + 200)

// Caps Word
#define CAPS_WORD_INVERT_ON_SHIFT
