// Copyright 2023 Scan Line
// https://github.com/scan-line

// This program is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 2 of the License, or (at your option) any later version. This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details. You should have received a copy of the GNU General Public License along with this program. If not, see <http://www.gnu.org/licenses/>.

#include QMK_KEYBOARD_H

#include "miryoku.h"


// "Tooth" led management

#define FLASH_LED_TICK 100

struct {
  bool left;
  bool right;
  bool suspended;
} led;

struct {
  const uint8_t* pattern;
  bool on;
  bool done;
} led_flash;

void left_led_on(void) {
  led.left = true;
  if (!led.suspended)
    planck_ez_left_led_on();
}

void left_led_off(void) {
  led.left = false;
  planck_ez_left_led_off();
}

void right_led_on(void) {
  led.right = true;
  if (!led.suspended)
    planck_ez_right_led_on();
}

void right_led_off(void) {
  led.right = false;
  planck_ez_right_led_off();
}

uint32_t flash_led_callback(uint32_t trigger_time, void *arg) {
  // Flash unused leds
  bool flash_left = !led.left;
  bool flash_right = !led.right;
  
  uint8_t duration = *led_flash.pattern * FLASH_LED_TICK;
  if (duration == 0) {
    // End of array
    // Turn off and stop
    if (flash_left || led.suspended)
      planck_ez_left_led_off();
    if (flash_right || led.suspended)
      planck_ez_right_led_off();          
    led_flash.done = true;
    return duration;
  }

  if (led_flash.on) {
    // Turn on
    if (flash_left)
      planck_ez_left_led_on();
    if (flash_right)
      planck_ez_right_led_on();          
  } else {
    // Turn off
    if (flash_left)
      planck_ez_left_led_off();
    if (flash_right)
      planck_ez_right_led_off();          
  }
  
  // Next state
  led_flash.on = !led_flash.on;
  ++led_flash.pattern;
  return duration;
}

void flash_led(const uint8_t* pattern) {
  static deferred_token token = INVALID_DEFERRED_TOKEN;

  // Halt any flash in progress
  if (!led_flash.done) {
    cancel_deferred_exec(token);
    token = INVALID_DEFERRED_TOKEN;
  }

  // Start new flash
  led_flash.pattern = pattern;
  led_flash.on = true;
  led_flash.done = false;
  token = defer_exec(FLASH_LED_TICK, flash_led_callback, NULL);
}

void suspend_power_down_extra(void) {
  // May be run multiple times on suspend
  led.suspended = true;
  planck_ez_left_led_off();
  planck_ez_right_led_off();
}

void suspend_wakeup_init_extra(void) {
  led.suspended = false;
  if (led.left)
    planck_ez_left_led_on();
  if (led.right)
    planck_ez_right_led_on();
}


// Feedback

// On/off timings - must end in 0
const uint8_t os_mode_flash[] = {10, 0};
const uint8_t default_layer_flash[] = {1, 1, 1, 0};
const uint8_t toggle_on_flash[] = {10, 0};
const uint8_t detent_flash[] = {1, 0};
const uint8_t cancel_flash[] = {0};

void show_os_mode_extra(uint16_t keycode) {
  flash_led(os_mode_flash);
}

void show_layer_extra(uint8_t layer) {
  switch (layer) {
    case U_BASE:
    case U_EXTRA:
    case U_TAP:
      left_led_off();
      right_led_off();
      break;
    case U_NAV:
    case U_MOUSE:
    case U_MEDIA:
      left_led_on();
      right_led_off();
      break;
    case U_NUM:
    case U_SYM:
    case U_FUN:
      left_led_off();
      right_led_on();
      break;
    case U_BUTTON:
      left_led_on();
      right_led_on();
      break;
    default:
      left_led_off();
      right_led_off();
      break;
    }
}

void show_default_layer_extra(uint8_t layer) {
  flash_led(default_layer_flash);
}

void show_toggle_extra(uint16_t keycode, bool value) {
  if (value)
    flash_led(toggle_on_flash);
  else
    flash_led(cancel_flash);
}

void show_value_extra(uint16_t keycode, uint16_t value, bool detent) {
  if (detent)
    flash_led(detent_flash);
  else
    flash_led(cancel_flash);
}


// Initialization

void eeconfig_init_extra(void) {
  // Turn the initial led level down from 4 to 1
  keyboard_config.led_level = 1;
  eeconfig_update_kb(keyboard_config.raw);
}
