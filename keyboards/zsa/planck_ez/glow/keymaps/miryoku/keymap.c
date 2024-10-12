// Copyright 2023 Scan Line
// https://github.com/scan-line

// This program is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 2 of the License, or (at your option) any later version. This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details. You should have received a copy of the GNU General Public License along with this program. If not, see <http://www.gnu.org/licenses/>.

#include QMK_KEYBOARD_H

#include "miryoku.h"


// "Tooth" led management

typedef struct leds_t {
  bool left;
  bool right;
  bool flash;
  const uint16_t* pattern;
  deferred_token token;
  bool suspended;
} leds_t;

const uint16_t flash_off_pattern[] = {0};

leds_t leds = (const leds_t){
  .left = false,
  .right = false,
  .flash = false,
  .pattern = flash_off_pattern,
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

uint16_t flash_leds_next(void) {
  const uint16_t duration = *leds.pattern;

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

void flash_leds_cancel(void) {
  if (leds.token == INVALID_DEFERRED_TOKEN)
    return;

  cancel_deferred_exec(leds.token);
  leds.flash = false;
  leds.token = INVALID_DEFERRED_TOKEN;
  update_leds();
}

uint32_t flash_leds_callback(uint32_t trigger_time, void *arg) {
  // Next state
  return flash_leds_next();
}

void flash_leds(const uint16_t* pattern) {
  // Cancel any current flash
  flash_leds_cancel();
  // Start new flash
  leds.flash = false;
  leds.pattern = pattern;
  const uint16_t duration = flash_leds_next();
  if (duration > 0)
    leds.token = defer_exec(duration, flash_leds_callback, NULL);
}

void suspend_power_down_keymap(void) {
  // May be run multiple times on suspend
  leds.suspended = true;
  update_leds();
}

void suspend_wakeup_init_keymap(void) {
  leds.suspended = false;
  update_leds();
}


// Layer rgb

// Planck mit led pattern is
//  0  1  2  3  4  5     6  7  8  9 10 11
// 12 13 14 15 16 17    18 19 20 21 22 23
// 24 25 26 27 28 29    30 31 32 33 34 35
// 36 37 38 39 40    41    42 43 44 45 46

const uint8_t led_grid[3][10] = {
  { 0,  1,  2,  3,  4,    7,  8,  9, 10, 11},
  {12, 13, 14, 15, 16,   19, 20, 21, 22, 23},
  {24, 25, 26, 27, 28,   31, 32, 33, 34, 35},
};
const uint8_t led_thumb[6] = {
           38, 39, 40,   42, 43, 44
};


// Feedback

// Flash on/off timings in milliseconds
// Must end in 0
const uint16_t flash_value_on_pattern[] = {150, 150, 150, 0};
const uint16_t flash_value_off_pattern[] = {450, 0};
const uint16_t flash_value_detent_pattern[] = {150, 0};

void show_os_mode_keymap(uint16_t keycode) {
  flash_leds(flash_value_on_pattern);
}

void show_layer_keymap(uint8_t layer, uint8_t default_layer) {
  // No layer leds on default layer (layer lock)
  if (layer == default_layer) {
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

void show_default_layer_keymap(uint8_t layer) {
  flash_leds(flash_value_on_pattern);
}

void show_toggle_keymap(uint16_t keycode, bool value) {
  if (value)
    flash_leds(flash_value_on_pattern);
  else
    flash_leds(flash_value_off_pattern);
}

void show_value_keymap(uint16_t keycode, uint8_t value, bool detent) {
  if (detent)
    flash_leds(flash_value_detent_pattern);
  else
    flash_leds(flash_off_pattern);
}


// Initialization

void eeconfig_init_keymap(void) {
  // Turn the initial led level down from 4 to 1
  keyboard_config.led_level = 1;
  eeconfig_update_kb(keyboard_config.raw);
}
