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
  bool suspended;
} leds_t;

typedef struct leds_flash_t {
  const uint8_t* pattern;
  bool on;
  deferred_token token;
} leds_flash_t;

leds_t leds = (const leds_t){
  .left = false,
  .right = false,
  .suspended = false
};

const uint8_t cancel_flash[] = {0};

leds_flash_t leds_flash = (const leds_flash_t){
  .pattern = cancel_flash,
  .on = false,
  .token = INVALID_DEFERRED_TOKEN
};

void update_leds(void) {
  const bool left_on = (leds.left || leds_flash.on) && !leds.suspended;
  const bool right_on = (leds.right || leds_flash.on) && !leds.suspended;

  if (left_on)
    planck_ez_left_led_on();
  else
    planck_ez_left_led_off();

  if (right_on)
    planck_ez_right_led_on();
  else
    planck_ez_right_led_off();
}

uint8_t next_flash_leds(void) {
  const uint8_t duration = *leds_flash.pattern * FLASH_LED_TICK;

  if (duration > 0) {
    leds_flash.on = !leds_flash.on;
    ++leds_flash.pattern;
  } else {
    // End of array
    leds_flash.on = false;
    leds_flash.token = INVALID_DEFERRED_TOKEN;
  }

  update_leds();
  return duration;
}

uint32_t flash_led_callback(uint32_t trigger_time, void *arg) {
  // Next state
  const uint8_t duration = next_flash_leds();
  return duration;
}

void flash_led(const uint8_t* pattern) {
  // Cancel any current flash
  if (leds_flash.token != INVALID_DEFERRED_TOKEN) {
    cancel_deferred_exec(leds_flash.token);
    leds_flash.on = false;
    leds_flash.token = INVALID_DEFERRED_TOKEN;
    update_leds();
  }

  // Start new flash
  leds_flash.pattern = pattern;
  leds_flash.on = false;
  const uint8_t duration = next_flash_leds();
  if (duration > 0)
    leds_flash.token = defer_exec(duration, flash_led_callback, NULL);
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
  deferred_token token;
} slider_t;

slider_t slider = (const slider_t){
  .value = 0,
  .token = INVALID_DEFERRED_TOKEN,
};

uint32_t slider_cancel_callback(uint32_t trigger_time, void *arg) {
  slider.value = 0;
  slider.token = INVALID_DEFERRED_TOKEN;
  return 0;
}

void set_slider(uint8_t value) {
  // Cancel any current slider
  if (slider.token != INVALID_DEFERRED_TOKEN) {
    cancel_deferred_exec(slider.token);
    slider.value = 0;
    slider.token = INVALID_DEFERRED_TOKEN;
  }

  // Start new slider
  slider.value = value;
  slider.token = defer_exec(FEEDBACK_TIMEOUT, slider_cancel_callback, NULL);
}


// Layer lock display

// Planck mit led pattern is
//  0  1  2  3  4  5   6  7  8  9 10 11
// 12 13 14 15 16 17  18 19 20 21 22 23
// 24 25 26 27 28 29  30 31 32 33 34 35
// 36 37 38 39 40   41   42 43 44 45 46   

uint8_t default_layer = U_BASE;

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
      rgb_matrix_set_color(20, rgb.r, rgb.g, rgb.b);
      rgb_matrix_set_color(21, rgb.r, rgb.g, rgb.b);
      rgb_matrix_set_color(22, rgb.r, rgb.g, rgb.b);
      rgb_matrix_set_color(23, rgb.r, rgb.g, rgb.b);
      break;
    case U_NUM:
    case U_SYM:
    case U_FUN:
      // Accent the "numpad" keys
      rgb_matrix_set_color_all(0, 0, 0);
      rgb_matrix_set_color(1, rgb.r, rgb.g, rgb.b);
      rgb_matrix_set_color(2, rgb.r, rgb.g, rgb.b);
      rgb_matrix_set_color(3, rgb.r, rgb.g, rgb.b);
      rgb_matrix_set_color(13, rgb.r, rgb.g, rgb.b);
      rgb_matrix_set_color(14, rgb.r, rgb.g, rgb.b);
      rgb_matrix_set_color(15, rgb.r, rgb.g, rgb.b);
      rgb_matrix_set_color(25, rgb.r, rgb.g, rgb.b);
      rgb_matrix_set_color(26, rgb.r, rgb.g, rgb.b);
      rgb_matrix_set_color(27, rgb.r, rgb.g, rgb.b);
      rgb_matrix_set_color(39, rgb.r, rgb.g, rgb.b);
      break;
    default:
      // Standard animation
      break;
  }

  // Accent the thumb key
  switch (layer) {
    case U_NAV:
      rgb_matrix_set_color(39, rgb.r, rgb.g, rgb.b);
      break;
    case U_MOUSE:
      rgb_matrix_set_color(40, rgb.r, rgb.g, rgb.b);
      break;
    case U_MEDIA:
      rgb_matrix_set_color(38, rgb.r, rgb.g, rgb.b);
      break;
      break;
    case U_NUM:
      rgb_matrix_set_color(43, rgb.r, rgb.g, rgb.b);
      break;
    case U_SYM:
      rgb_matrix_set_color(42, rgb.r, rgb.g, rgb.b);
      break;
    case U_FUN:
      rgb_matrix_set_color(44, rgb.r, rgb.g, rgb.b);
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
      rgb_matrix_set_color(32, on.r, on.g, on.b);
      break;
    case OS_MODE_MAC:
      rgb_matrix_set_color(33, on.r, on.g, on.b);
      break;
    case OS_MODE_LNX:
      rgb_matrix_set_color(34, on.r, on.g, on.b);
      break;
    default:
      break;
  }

  if (rgb_matrix_is_enabled())
    rgb_matrix_set_color(7, on.r, on.g, on.b);
  else
    rgb_matrix_set_color(7, off.r, off.g, off.b);

  if (is_audio_on())
    rgb_matrix_set_color(19, on.r, on.g, on.b);
  else
    rgb_matrix_set_color(19, off.r, off.g, off.b);
}

void display_slider(void) {
  // Display bits in binary
  const uint8_t val = rgb_matrix_get_val();
  const RGB on = scaled_hsv_to_rgb(val, HSV_YELLOW);

  uint8_t bits = slider.value;
  const uint8_t mask = 1;
  // Five bits is enough
  for (int i = 0; i < 5; i++) {
    if (bits & mask)
      rgb_matrix_set_color(i, on.r, on.g, on.b);
    else
      rgb_matrix_set_color(i, 0, 0, 0);
    bits >>= 1;
  }
}

void display_hsv_color(void) {
  // Display a block of hsv color below the slider
  const HSV hsv = rgb_matrix_get_hsv();
  const RGB rgb = hsv_to_rgb(hsv);
  for (int i = 12; i < 17; i++) {
    rgb_matrix_set_color(i, rgb.r, rgb.g, rgb.b);
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
      display_hsv_color();
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

// "User" matrix mode
uint8_t user_matrix_mode = RGB_MATRIX_SOLID_COLOR;

void show_os_mode_extra(uint16_t keycode) {
  flash_led(os_mode_flash);
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

uint8_t base_matrix_mode(void) {
  switch (default_layer) {
    case U_NAV:
    case U_MOUSE:
    case U_MEDIA:
    case U_NUM:
    case U_SYM:
    case U_FUN:
      // Use a single-color matrix
      // so that animations restart correctly
      // on return to "user" matrix mode
      return RGB_MATRIX_SOLID_COLOR;
    default:
      // "User" matrix mode
      return user_matrix_mode;
  }
}

void show_default_layer_extra(uint8_t layer) {
  flash_led(default_layer_flash);
  default_layer = layer;
  // Layer-lock display?
  const uint8_t matrix_mode = base_matrix_mode();
  rgb_matrix_mode_noeeprom(matrix_mode);
}

void show_toggle_extra(uint16_t keycode, bool value) {
  if (value)
    flash_led(toggle_on_flash);
  else
    flash_led(cancel_flash);
}

void show_value_extra(uint16_t keycode, uint8_t value, bool detent) {
  if (detent)
    flash_led(detent_flash);
  else
    flash_led(cancel_flash);

  set_slider(value);

  if (keycode == U_RGB_MOD) {
    // "User" matrix mode has changed
    user_matrix_mode = rgb_matrix_get_mode();
    // Return to layer-lock display?
    const uint8_t matrix_mode = base_matrix_mode();
    rgb_matrix_mode_noeeprom(matrix_mode);
  }
}

bool process_record_extra(uint16_t keycode, keyrecord_t *record) {
  if (keycode == U_RGB_MOD && record->event.pressed) {
    // Prepare to change the "user" matrix mode
    rgb_matrix_mode_noeeprom(user_matrix_mode);
  }
  return true;
}


// Initialization

void eeconfig_init_extra(void) {
  // Turn the initial led level down from 4 to 1
  keyboard_config.led_level = 1;
  eeconfig_update_kb(keyboard_config.raw);
}

void keyboard_post_init_extra(void) {
  // Track matrix mode
  user_matrix_mode = rgb_matrix_get_mode();
}
