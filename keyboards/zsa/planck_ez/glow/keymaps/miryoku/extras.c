// Copyright 2023 Scan Line
// https://github.com/scan-line

// This program is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 2 of the License, or (at your option) any later version. This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details. You should have received a copy of the GNU General Public License along with this program. If not, see <http://www.gnu.org/licenses/>.

#include QMK_KEYBOARD_H

#include "miryoku.h"


// "Tooth" led management

#define FLASH_LED_TICK 100

typedef struct leds_t {
  bool left;
  bool right;
  bool flash;
  const uint8_t* pattern;
  deferred_token token;
  bool suspended;
} leds_t;

const uint8_t no_flash[] = {0};

leds_t leds = (const leds_t){
  .left = false,
  .right = false,
  .flash = false,
  .pattern = no_flash,
  .token = INVALID_DEFERRED_TOKEN,
  .suspended = false,
};

void update_leds(void) {
  const bool left_on = (leds.left || leds.flash) && !leds.suspended;
  const bool right_on = (leds.right || leds.flash) && !leds.suspended;

  if (left_on)
    planck_ez_left_led_on();
  else
    planck_ez_left_led_off();

  if (right_on)
    planck_ez_right_led_on();
  else
    planck_ez_right_led_off();
}

uint8_t flash_leds_next(void) {
  const uint8_t duration = *leds.pattern * FLASH_LED_TICK;

  if (duration > 0) {
    leds.flash = !leds.flash;
    ++leds.pattern;
  } else {
    // End of array
    leds.flash = false;
    leds.token = INVALID_DEFERRED_TOKEN;
  }

  update_leds();
  return duration;
}

uint32_t flash_leds_callback(uint32_t trigger_time, void *arg) {
  // Next state
  const uint8_t duration = flash_leds_next();
  return duration;
}

void flash_leds(const uint8_t* pattern) {
  // Cancel any current flash
  if (leds.token != INVALID_DEFERRED_TOKEN) {
    cancel_deferred_exec(leds.token);
    leds.flash = false;
    leds.token = INVALID_DEFERRED_TOKEN;
    update_leds();
  }

  // Start new flash
  leds.flash = false;
  leds.pattern = pattern;
  const uint8_t duration = flash_leds_next();
  if (duration > 0)
    leds.token = defer_exec(duration, flash_leds_callback, NULL);
}

void suspend_power_down_extra(void) {
  // May be run multiple times on suspend
  leds.suspended = true;
  update_leds();
}

void suspend_wakeup_init_extra(void) {
  leds.suspended = false;
  update_leds();
}


// Slider display

typedef struct slider_t {
  uint8_t value;
  bool active;
  deferred_token token;
} slider_t;

slider_t slider = (const slider_t){
  .value = 0,
  .active = false,
  .token = INVALID_DEFERRED_TOKEN,
};

uint32_t slider_end_callback(uint32_t trigger_time, void *arg) {
  slider.active = false;
  slider.token = INVALID_DEFERRED_TOKEN;
  return 0;
}

void set_slider(uint8_t value) {
  // Cancel any current slider
  if (slider.token != INVALID_DEFERRED_TOKEN) {
    cancel_deferred_exec(slider.token);
    slider.active = false;
    slider.token = INVALID_DEFERRED_TOKEN;
  }

  // Start new slider
  slider.value = value;
  slider.active = true;
  slider.token = defer_exec(FEEDBACK_TIMEOUT, slider_end_callback, NULL);
}


// Layer lock display

uint8_t default_layer = U_BASE;

// Planck mit led pattern is
//  0  1  2  3  4  5   6  7  8  9 10 11
// 12 13 14 15 16 17  18 19 20 21 22 23
// 24 25 26 27 28 29  30 31 32 33 34 35
// 36 37 38 39 40   41   42 43 44 45 46

const uint8_t led_grid[][10] = {
  { 0,  1,  2,  3,  4,    7,  8,  9, 10, 11},
  {12, 13, 14, 15, 16,   19, 20, 21, 22, 23},
  {24, 25, 26, 27, 28,   31, 32, 33, 34, 35},
};
const uint8_t led_thumb[] = {
           38, 39, 40,   42, 43, 44
};

RGB scaled_hsv_to_rgb(uint8_t scale, uint8_t h, uint8_t s, uint8_t v) {
  const HSV hsv = (HSV){
    .h = h,
    .s = s,
    .v = scale,
  };
  return hsv_to_rgb(hsv);
}

RGB layer_accent_color(uint8_t layer) {
  const uint8_t val = rgb_matrix_get_val();
  switch (layer) {
    case U_NAV:
      return scaled_hsv_to_rgb(val, HSV_CYAN);
    case U_MOUSE:
      return scaled_hsv_to_rgb(val, HSV_YELLOW);
    case U_MEDIA:
      return scaled_hsv_to_rgb(val, HSV_MAGENTA);
    case U_NUM:
      return scaled_hsv_to_rgb(val, HSV_BLUE);
    case U_SYM:
      return scaled_hsv_to_rgb(val, HSV_GREEN);
    case U_FUN:
      return scaled_hsv_to_rgb(val, HSV_RED);
    default:
      return scaled_hsv_to_rgb(val, HSV_OFF);
  }
}

void display_layer(uint8_t layer) {
  RGB rgb = layer_accent_color(layer);

  switch (layer) {
    case U_NAV:
    case U_MOUSE:
    case U_MEDIA:
      // Accent the "cursor" keys
      rgb_matrix_set_color_all(0, 0, 0);
      rgb_matrix_set_color(led_grid[1][6], rgb.r, rgb.g, rgb.b);
      rgb_matrix_set_color(led_grid[1][7], rgb.r, rgb.g, rgb.b);
      rgb_matrix_set_color(led_grid[1][8], rgb.r, rgb.g, rgb.b);
      rgb_matrix_set_color(led_grid[1][9], rgb.r, rgb.g, rgb.b);
      break;
    case U_NUM:
    case U_SYM:
    case U_FUN:
      // Accent the "numpad" keys
      rgb_matrix_set_color_all(0, 0, 0);
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
      // Standard animation
      break;
  }

  // Accent the thumb key
  switch (layer) {
    case U_NAV:
      rgb_matrix_set_color(led_thumb[1], rgb.r, rgb.g, rgb.b);
      break;
    case U_MOUSE:
      rgb_matrix_set_color(led_thumb[2], rgb.r, rgb.g, rgb.b);
      break;
    case U_MEDIA:
      rgb_matrix_set_color(led_thumb[0], rgb.r, rgb.g, rgb.b);
      break;
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

void display_settings(void) {
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

  if (is_audio_on())
    rgb_matrix_set_color(led_grid[1][5], on.r, on.g, on.b);
  else
    rgb_matrix_set_color(led_grid[1][5], off.r, off.g, off.b);

  const HSV hsv = rgb_matrix_get_hsv();
  const RGB rgb = hsv_to_rgb(hsv);
  rgb_matrix_set_color(led_grid[0][7], rgb.r, rgb.g, rgb.b);
  rgb_matrix_set_color(led_grid[0][8], rgb.r, rgb.g, rgb.b);
  rgb_matrix_set_color(led_grid[0][9], rgb.r, rgb.g, rgb.b);

}

void display_slider(void) {
  if (!slider.active)
    return;

  // Display bits in binary
  const uint8_t val = rgb_matrix_get_val();
  const RGB on = scaled_hsv_to_rgb(val, HSV_YELLOW);

  uint8_t bits = slider.value;
  const uint8_t mask = 1;
  // Five bits is enough
  for (int i = 0; i < 5; i++) {
    if (bits & mask)
      rgb_matrix_set_color(led_grid[0][i], on.r, on.g, on.b);
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
}

bool rgb_matrix_indicators_user(void) {
  switch (default_layer) {
    case U_NAV:
    case U_MOUSE:
    case U_NUM:
    case U_SYM:
    case U_FUN:
      display_layer(default_layer);
      return false;
    case U_MEDIA:
      display_layer(default_layer);
      display_settings();
      display_slider();
      return false;
    default:
      return true;
  }
}


// Feedback

// On/off timings - must end in 0
const uint8_t os_mode_flash[] = {10, 0};
const uint8_t default_layer_flash[] = {1, 1, 1, 0};
const uint8_t toggle_on_flash[] = {10, 0};
const uint8_t detent_flash[] = {1, 0};

void show_os_mode_extra(uint16_t keycode) {
  flash_leds(os_mode_flash);
}

void show_layer_extra(uint8_t layer) {
  // No layer leds during layer lock
  switch (default_layer) {
    case U_BASE:
    case U_EXTRA:
    case U_TAP:
      break;
    default:
      leds.left = false;
      leds.right = false;
      update_leds();
      return;
  }

  // Match led to thumb hold
  switch (layer) {
    case U_BASE:
    case U_EXTRA:
    case U_TAP:
      leds.left = false;
      leds.right = false;
      update_leds();
      break;
    case U_NAV:
    case U_MOUSE:
    case U_MEDIA:
      leds.left = true;
      leds.right = false;
      update_leds();
      break;
    case U_NUM:
    case U_SYM:
    case U_FUN:
      leds.left = false;
      leds.right = true;
      update_leds();
      break;
    case U_BUTTON:
      leds.left = true;
      leds.right = true;
      update_leds();
      break;
    default:
      leds.left = false;
      leds.right = false;
      update_leds();
      break;
  }
}

void show_default_layer_extra(uint8_t layer) {
  flash_leds(default_layer_flash);
  default_layer = layer;

  switch (layer) {
    case U_NAV:
    case U_MOUSE:
    case U_NUM:
    case U_SYM:
    case U_FUN:
    case U_MEDIA:
      // Overlay matrix
      break;
    default:
      // Restart matrix to remove overlay
      if (!rgb_matrix_get_suspend_state()) {
        rgb_matrix_set_suspend_state(true);
        rgb_matrix_set_suspend_state(false);
      }
      break;
  }
}

void show_toggle_extra(uint16_t keycode, bool value) {
  if (value)
    flash_leds(toggle_on_flash);
  else
    flash_leds(no_flash);
}

void show_value_extra(uint16_t keycode, uint8_t value, bool detent) {
  if (detent)
    flash_leds(detent_flash);
  else
    flash_leds(no_flash);

  set_slider(value);
}


// Initialization

void eeconfig_init_extra(void) {
  // Turn the initial led level down from 4 to 1
  keyboard_config.led_level = 1;
  eeconfig_update_kb(keyboard_config.raw);
}
