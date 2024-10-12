// Copyright 2024 Scan Line
// https://github.com/scan-line

// This program is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 2 of the License, or (at your option) any later version. This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details. You should have received a copy of the GNU General Public License along with this program. If not, see <http://www.gnu.org/licenses/>.

#include QMK_KEYBOARD_H

#include "custom/rgb/rgb.h"
#include "miryoku.h"


#ifdef RGB_MATRIX_ENABLE


// Slider display

typedef struct slider_t {
  uint8_t value;
  bool detent;
  bool active;
  deferred_token token;
} slider_t;

slider_t slider = (const slider_t){
  .value = 0,
  .detent = false,
  .active = false,
  .token = INVALID_DEFERRED_TOKEN,
};

uint32_t slider_end_callback(uint32_t trigger_time, void *arg) {
  slider.active = false;
  slider.token = INVALID_DEFERRED_TOKEN;
  return 0;
}

void set_slider(uint8_t value, bool detent) {
  // Clear any current slider
  clear_slider();
  // Start new slider
  slider.value = value;
  slider.detent = detent;
  slider.active = true;
  slider.token = defer_exec(FEEDBACK_TIMEOUT, slider_end_callback, NULL);
}

void clear_slider(void) {
  if (slider.token == INVALID_DEFERRED_TOKEN)
    return;

  cancel_deferred_exec(slider.token);
  slider.active = false;
  slider.token = INVALID_DEFERRED_TOKEN;
}


// Rgb

// Rgb layer

extern const uint8_t led_grid[3][10];
extern const uint8_t led_thumb[6];

RGB scaled_hsv_to_rgb(uint8_t scale, uint8_t h, uint8_t s, uint8_t v) {
  const HSV hsv = (HSV){
    .h = h,
    .s = s,
    .v = scale,
  };
  return hsv_to_rgb(hsv);
}

RGB layer_accent_color(uint8_t scale, uint8_t layer) {
  switch (layer) {
    case U_NAV:
      return scaled_hsv_to_rgb(scale, HSV_CYAN);
    case U_MOUSE:
      return scaled_hsv_to_rgb(scale, HSV_YELLOW);
    case U_MEDIA:
      return scaled_hsv_to_rgb(scale, HSV_MAGENTA);
    case U_NUM:
      return scaled_hsv_to_rgb(scale, HSV_BLUE);
    case U_SYM:
      return scaled_hsv_to_rgb(scale, HSV_GREEN);
    case U_FUN:
      return scaled_hsv_to_rgb(scale, HSV_RED);
    default:
      return scaled_hsv_to_rgb(scale, HSV_YELLOW);
  }
}

void overlay_media_settings(void) {
  const uint8_t scale = rgb_matrix_get_val();
  const RGB on = scaled_hsv_to_rgb(scale, HSV_GREEN);
  const RGB off = scaled_hsv_to_rgb(scale, HSV_RED);

  switch (os_mode_get())
  {
    case OS_MODE_WIN:
      rgb_matrix_set_color(led_grid[2][6], on.r, on.g, on.b);
      break;
    case OS_MODE_MAC:
      rgb_matrix_set_color(led_grid[2][7], on.r, on.g, on.b);
      break;
    case OS_MODE_LNX:
      rgb_matrix_set_color(led_grid[2][8], on.r, on.g, on.b);
      break;
    default:
      break;
  }

  if (rgb_matrix_is_enabled())
    rgb_matrix_set_color(led_grid[0][5], on.r, on.g, on.b);
  else
    rgb_matrix_set_color(led_grid[0][5], off.r, off.g, off.b);

#ifdef AUDIO_ENABLE
  if (is_audio_on())
    rgb_matrix_set_color(led_grid[1][5], on.r, on.g, on.b);
  else
    rgb_matrix_set_color(led_grid[1][5], off.r, off.g, off.b);
#endif

  const HSV hsv = rgb_matrix_get_hsv();
  const RGB rgb = hsv_to_rgb(hsv);
  rgb_matrix_set_color(led_grid[0][7], rgb.r, rgb.g, rgb.b);
  rgb_matrix_set_color(led_grid[0][8], rgb.r, rgb.g, rgb.b);
  rgb_matrix_set_color(led_grid[0][9], rgb.r, rgb.g, rgb.b);
}

void overlay_layer(uint8_t layer) {
  const uint8_t scale = rgb_matrix_get_val();
  const RGB rgb = layer_accent_color(scale, layer);
  const RGB on = scaled_hsv_to_rgb(scale, HSV_GREEN);

  switch (layer) {
    case U_BASE:
    case U_EXTRA:
    case U_TAP:
      // Accent the home position
      rgb_matrix_set_color(led_grid[1][0], rgb.r, rgb.g, rgb.b);
      rgb_matrix_set_color(led_grid[1][1], rgb.r, rgb.g, rgb.b);
      rgb_matrix_set_color(led_grid[1][2], rgb.r, rgb.g, rgb.b);
      rgb_matrix_set_color(led_grid[1][3], rgb.r, rgb.g, rgb.b);
      rgb_matrix_set_color(led_grid[1][6], rgb.r, rgb.g, rgb.b);
      rgb_matrix_set_color(led_grid[1][7], rgb.r, rgb.g, rgb.b);
      rgb_matrix_set_color(led_grid[1][8], rgb.r, rgb.g, rgb.b);
      rgb_matrix_set_color(led_grid[1][9], rgb.r, rgb.g, rgb.b);
      rgb_matrix_set_color(led_thumb[1], rgb.r, rgb.g, rgb.b);
      rgb_matrix_set_color(led_thumb[4], rgb.r, rgb.g, rgb.b);
      break;
    case U_BUTTON:
       // Accent cut/copy/paste
      rgb_matrix_set_color(led_grid[0][1], rgb.r, rgb.g, rgb.b);
      rgb_matrix_set_color(led_grid[0][2], rgb.r, rgb.g, rgb.b);
      rgb_matrix_set_color(led_grid[0][3], rgb.r, rgb.g, rgb.b);
      rgb_matrix_set_color(led_grid[0][6], rgb.r, rgb.g, rgb.b);
      rgb_matrix_set_color(led_grid[0][7], rgb.r, rgb.g, rgb.b);
      rgb_matrix_set_color(led_grid[0][8], rgb.r, rgb.g, rgb.b);
      rgb_matrix_set_color(led_thumb[1], rgb.r, rgb.g, rgb.b);
      rgb_matrix_set_color(led_thumb[4], rgb.r, rgb.g, rgb.b);
     break;
    case U_NAV:
    case U_MOUSE:
    case U_MEDIA:
      // Accent the cursor keys
      rgb_matrix_set_color(led_grid[1][6], rgb.r, rgb.g, rgb.b);
      rgb_matrix_set_color(led_grid[1][7], rgb.r, rgb.g, rgb.b);
      rgb_matrix_set_color(led_grid[1][8], rgb.r, rgb.g, rgb.b);
      rgb_matrix_set_color(led_grid[1][9], rgb.r, rgb.g, rgb.b);
      break;
    case U_NUM:
    case U_SYM:
    case U_FUN:
      // Accent the numpad keys
      rgb_matrix_set_color(led_grid[0][1], rgb.r, rgb.g, rgb.b);
      rgb_matrix_set_color(led_grid[0][2], rgb.r, rgb.g, rgb.b);
      rgb_matrix_set_color(led_grid[0][3], rgb.r, rgb.g, rgb.b);
      rgb_matrix_set_color(led_grid[1][1], rgb.r, rgb.g, rgb.b);
      rgb_matrix_set_color(led_grid[1][2], rgb.r, rgb.g, rgb.b);
      rgb_matrix_set_color(led_grid[1][3], rgb.r, rgb.g, rgb.b);
      rgb_matrix_set_color(led_grid[2][1], rgb.r, rgb.g, rgb.b);
      rgb_matrix_set_color(led_grid[2][2], rgb.r, rgb.g, rgb.b);
      rgb_matrix_set_color(led_grid[2][3], rgb.r, rgb.g, rgb.b);
      rgb_matrix_set_color(led_thumb[1], rgb.r, rgb.g, rgb.b);
      break;
    default:
      break;
  }

  // For thumb layers, accent the thumb pressed
  // For base layers, color layer-lock finger
  // For media layer, color settings state
  switch (layer) {
    case U_BASE:
      rgb_matrix_set_color(led_grid[1][3], on.r, on.g, on.b);
      rgb_matrix_set_color(led_grid[1][6], on.r, on.g, on.b);
      break;
    case U_EXTRA:
      rgb_matrix_set_color(led_grid[1][2], on.r, on.g, on.b);
      rgb_matrix_set_color(led_grid[1][7], on.r, on.g, on.b);
      break;
    case U_TAP:
      rgb_matrix_set_color(led_grid[1][1], on.r, on.g, on.b);
      rgb_matrix_set_color(led_grid[1][8], on.r, on.g, on.b);
      break;
    case U_BUTTON:
      break;
    case U_NAV:
      rgb_matrix_set_color(led_thumb[1], rgb.r, rgb.g, rgb.b);
      break;
    case U_MOUSE:
      rgb_matrix_set_color(led_thumb[2], rgb.r, rgb.g, rgb.b);
      break;
    case U_MEDIA:
      rgb_matrix_set_color(led_thumb[0], rgb.r, rgb.g, rgb.b);
      overlay_media_settings();
      break;
    case U_NUM:
      rgb_matrix_set_color(led_thumb[4], rgb.r, rgb.g, rgb.b);
      break;
    case U_SYM:
      rgb_matrix_set_color(led_thumb[3], rgb.r, rgb.g, rgb.b);
      break;
    case U_FUN:
      rgb_matrix_set_color(led_thumb[5], rgb.r, rgb.g, rgb.b);
      break;
    default:
      break;
  }
}

void overlay_slider(void) {
  if (!slider.active)
    return;

  // Display bits in binary
  const uint8_t val = rgb_matrix_get_val();
  const RGB bit = scaled_hsv_to_rgb(val, HSV_YELLOW);
  const RGB on = scaled_hsv_to_rgb(val, HSV_GREEN);

  uint8_t bits = slider.value;
  const uint8_t mask = 1;
  // Five bits is enough
  for (int i = 0; i < 5; i++) {
    if (bits & mask)
      rgb_matrix_set_color(led_grid[0][i], bit.r, bit.g, bit.b);
    else
      rgb_matrix_set_color(led_grid[0][i], 0, 0, 0);
    bits >>= 1;
  }

  // Display a block of hsv color below the slider
  const HSV hsv = rgb_matrix_get_hsv();
  const RGB rgb = hsv_to_rgb(hsv);
  for (int i = 0; i < 5; i++) {
    rgb_matrix_set_color(led_grid[1][i], rgb.r, rgb.g, rgb.b);
  }

  // Display detent match
  if (slider.detent)
    rgb_matrix_set_color(led_grid[2][4], on.r, on.g, on.b);
  else
    rgb_matrix_set_color(led_grid[2][4], 0, 0, 0);
}

bool rgb_matrix_indicators_user(void) {
  const uint8_t default_layer = get_highest_layer(default_layer_state);
  switch (default_layer) {
    case U_BUTTON:
    case U_NAV:
    case U_MOUSE:
    case U_NUM:
    case U_SYM:
    case U_FUN:
      rgb_matrix_set_color_all(0, 0, 0);
      overlay_layer(default_layer);
      return false;
    case U_MEDIA:
      rgb_matrix_set_color_all(0, 0, 0);
      overlay_layer(default_layer);
      overlay_slider();
      return false;
    default:
      return true;
  }
}

void clear_overlay(void) {
  // Restart matrix to remove overlay
  if (!rgb_matrix_get_suspend_state()) {
    rgb_matrix_set_suspend_state(true);
    rgb_matrix_set_suspend_state(false);
  }
}

bool rgb_matrix_effect_feedback(effect_params_t* params) {
  RGB_MATRIX_USE_LIMITS(led_min, led_max);

  if (params->init) {
    const uint8_t default_layer = get_highest_layer(default_layer_state);
    switch (default_layer) {
      case U_BUTTON:
      case U_NAV:
      case U_MOUSE:
      case U_NUM:
      case U_SYM:
      case U_FUN:
      case U_MEDIA:
        // Start with overlay
        // Leave any overlay slider
        break;
      default:
        // Start without overlay.
        // Start clean
        clear_slider();
        break;
    }
  }

  rgb_matrix_set_color_all(0, 0, 0);
  const uint8_t layer = get_highest_layer(layer_state|default_layer_state);
  overlay_layer(layer);
  overlay_slider();

  return rgb_matrix_check_finished_leds(led_max);
}

#else


// No RGB_MATRIX_ENABLE

void set_slider(uint8_t value, bool detent) {
}

void clear_slider(void) {
}

void clear_overlay(void) {
}

#endif
