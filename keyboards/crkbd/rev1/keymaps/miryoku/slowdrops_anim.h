// Copyright 2023 Scan Line
// https://github.com/scan-line

// This program is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 2 of the License, or (at your option) any later version. This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details. You should have received a copy of the GNU General Public License along with this program. If not, see <http://www.gnu.org/licenses/>.

// Lightly adapted from raindrops_anim.h

#ifdef ENABLE_RGB_MATRIX_SLOWDROPS
RGB_MATRIX_EFFECT(SLOWDROPS)
#ifdef RGB_MATRIX_CUSTOM_EFFECT_IMPLS

static void slowdrops_set_color(int i, effect_params_t* params) {
  if (!HAS_ANY_FLAGS(g_led_config.flags[i], params->flags)) return;
  HSV hsv = {0, rgb_matrix_config.hsv.s, rgb_matrix_config.hsv.v};

  // Take the shortest path between hues
  int16_t deltaH = ((rgb_matrix_config.hsv.h + 180) % 360 - rgb_matrix_config.hsv.h) / 4;
  if (deltaH > 127)
    deltaH -= 256;
  else if (deltaH < -127)
    deltaH += 256;

  hsv.h   = rgb_matrix_config.hsv.h + (deltaH * (random8() & 0x03));
  RGB rgb = rgb_matrix_hsv_to_rgb(hsv);
  rgb_matrix_set_color(i, rgb.r, rgb.g, rgb.b);
}

bool SLOWDROPS(effect_params_t* params) {
  RGB_MATRIX_USE_LIMITS(led_min, led_max);
  
  static uint32_t wait_timer = 0;
  inline uint32_t interval(void) {
    return 3000 / scale16by8(qadd8(rgb_matrix_config.speed, 16), 16);
  }

  if (!params->init) {
    if (timer_expired32(g_rgb_timer, wait_timer)) {
      wait_timer = g_rgb_timer + interval();
      slowdrops_set_color(random8() % RGB_MATRIX_LED_COUNT, params);
    }
  } else {
    wait_timer = g_rgb_timer + interval();
    for (int i = led_min; i < led_max; i++)
      slowdrops_set_color(i, params);
  }
  return rgb_matrix_check_finished_leds(led_max);
}

#endif // RGB_MATRIX_CUSTOM_EFFECT_IMPLS
#endif // ENABLE_RGB_MATRIX_SLOWDROPS
