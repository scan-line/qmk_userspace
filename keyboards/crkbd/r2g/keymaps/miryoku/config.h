// Copyright 2023 Scan Line
// https://github.com/scan-line

// This program is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 2 of the License, or (at your option) any later version. This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details. You should have received a copy of the GNU General Public License along with this program. If not, see <http://www.gnu.org/licenses/>.

#pragma once

#include "layouts/split_3x6_3/miryoku/config.h"


// Configure for limited AVR space.
#define NO_ACTION_ONESHOT
#undef LOCKING_SUPPORT_ENABLE
#undef LOCKING_RESYNC_ENABLE


// Lighting design

#define RGB_MATRIX_DEFAULT_MODE RGB_MATRIX_CUSTOM_SLOWDROPS
#define RGB_MATRIX_DEFAULT_HUE 0
#define RGB_MATRIX_DEFAULT_SAT 255
#define RGB_MATRIX_DEFAULT_VAL (RGB_MATRIX_MAXIMUM_BRIGHTNESS / 2)
#define RGB_MATRIX_DEFAULT_SPD 60
#define RGB_MATRIX_HUE_STEP 8
#define RGB_MATRIX_SAT_STEP 16
#define RGB_MATRIX_VAL_STEP 16
#define RGB_MATRIX_SPD_STEP 16
// Disable rgb matrix timeout for firmware size.
// #define RGB_MATRIX_TIMEOUT 900000

// Enable rgb_matrix_kb.inc
#define RGB_MATRIX_CUSTOM_KB

// Disable all quantum effects
#undef RGBLIGHT_EFFECT_BREATHING
#undef RGBLIGHT_EFFECT_RAINBOW_MOOD
#undef RGBLIGHT_EFFECT_RAINBOW_SWIRL
#undef RGBLIGHT_EFFECT_SNAKE
#undef RGBLIGHT_EFFECT_KNIGHT
#undef RGBLIGHT_EFFECT_CHRISTMAS
#undef RGBLIGHT_EFFECT_STATIC_GRADIENT
#undef RGBLIGHT_EFFECT_RGB_TEST
#undef RGBLIGHT_EFFECT_ALTERNATING
#undef RGBLIGHT_EFFECT_TWINKLE

// Disable all quantum effects
#undef ENABLE_RGB_MATRIX_ALPHAS_MODS
#undef ENABLE_RGB_MATRIX_GRADIENT_UP_DOWN
#undef ENABLE_RGB_MATRIX_GRADIENT_LEFT_RIGHT
#undef ENABLE_RGB_MATRIX_BREATHING
#undef ENABLE_RGB_MATRIX_BAND_SAT
#undef ENABLE_RGB_MATRIX_BAND_VAL
#undef ENABLE_RGB_MATRIX_BAND_SPIRAL_SAT
#undef ENABLE_RGB_MATRIX_BAND_SPIRAL_VAL
#undef ENABLE_RGB_MATRIX_CYCLE_ALL
#undef ENABLE_RGB_MATRIX_RAINBOW_MOVING_CHEVRON
#undef ENABLE_RGB_MATRIX_CYCLE_SPIRAL
#undef ENABLE_RGB_MATRIX_RAINDROPS
#undef ENABLE_RGB_MATRIX_HUE_BREATHING
#undef ENABLE_RGB_MATRIX_PIXEL_FRACTAL
#undef ENABLE_RGB_MATRIX_TYPING_HEATMAP
#undef ENABLE_RGB_MATRIX_SOLID_REACTIVE_SIMPLE
#undef ENABLE_RGB_MATRIX_SOLID_REACTIVE
#undef ENABLE_RGB_MATRIX_SOLID_REACTIVE_MULTINEXUS
#undef ENABLE_RGB_MATRIX_SPLASH
#undef ENABLE_RGB_MATRIX_SOLID_SPLASH


// Configure Luna
#define WPM_UNFILTERED
