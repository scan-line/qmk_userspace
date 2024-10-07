// Copyright 2023 Scan Line
// https://github.com/scan-line

// This program is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 2 of the License, or (at your option) any later version. This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details. You should have received a copy of the GNU General Public License along with this program. If not, see <http://www.gnu.org/licenses/>.

#include QMK_KEYBOARD_H

#include "miryoku.h"
#include "custom.h"
#include "custom_rgb.h"
#include "process_magic.h"


// Qmk definitions

// Oneshot no-ops
#ifdef NO_ACTION_ONESHOT
#  define get_oneshot_mods() 0
#  define del_oneshot_mods(mask) (void)0
#endif


// Miryoku definitions

// Duplicated from miryoku.c
enum {
    U_TD_BOOT,
#define MIRYOKU_X(LAYER, STRING) U_TD_U_##LAYER,
MIRYOKU_LAYER_LIST
#undef MIRYOKU_X
};


// Persistent user state

typedef union {
  uint32_t raw;
  struct {
    // Set if in linux mode
    // See also ctl-gui-swap for mac mode / windows mode
    bool os_mode_linux :1;
  };
} user_config_t;

user_config_t user_config;


// Sounds

#ifdef AUDIO_ENABLE

float os_mode_set_song[][2] = OS_MODE_SET_SONG;
float layer_set_song[][2] = LAYER_SET_SONG;
float toggle_on_song[][2] = TOGGLE_ON_SONG;
float toggle_off_song[][2] = TOGGLE_OFF_SONG;
float detent_song[][2] = DETENT_SONG;

#else

#define PLAY_SONG(song)

#endif


// Feedback

__attribute__((weak)) void show_os_mode_extra(uint16_t keycode) {
}

void show_os_mode(uint16_t keycode) {
  PLAY_SONG(os_mode_set_song);
  show_os_mode_extra(keycode);
}

__attribute__((weak)) void show_layer_extra(uint8_t layer, uint8_t default_layer) {
}

void show_layer(uint8_t layer, uint8_t default_layer) {
  clear_slider();
  show_layer_extra(layer, default_layer);
}

__attribute__((weak)) void show_default_layer_extra(uint8_t layer) {
}

void show_default_layer(uint8_t layer) {
  PLAY_SONG(layer_set_song);
  clear_overlay();
  show_default_layer_extra(layer);
}

__attribute__((weak)) void show_toggle_extra(uint16_t keycode, bool value) {
}

void show_toggle(uint16_t keycode, bool value) {
  if (value)
    PLAY_SONG(toggle_on_song);
  else
    PLAY_SONG(toggle_off_song);
  show_toggle_extra(keycode, value);
}

__attribute__((weak)) void show_value_extra(uint16_t keycode, uint8_t value, bool detent) {
}

void show_value(uint16_t keycode, uint8_t value, bool detent) {
  if (detent)
    PLAY_SONG(detent_song);
  set_slider(value, detent);
  show_value_extra(keycode, value, detent);
}


// Layers

layer_state_t layer_state_set_user(layer_state_t state) {
  const uint8_t layer = get_highest_layer(state|default_layer_state);
  const uint8_t default_layer = get_highest_layer(default_layer_state);
  show_layer(layer, default_layer);
  return state;
}

layer_state_t default_layer_state_set_user(layer_state_t state) {
  const uint8_t default_layer = get_highest_layer(state);
  show_default_layer(default_layer);
  const uint8_t layer = get_highest_layer(state|layer_state);
  show_layer(layer, default_layer);
  return state;
}


// OS mode

os_mode_t os_mode_get(void) {
  if (keymap_config.swap_lctl_lgui)
    return OS_MODE_MAC;
  else if (user_config.os_mode_linux)
    return OS_MODE_LNX;
  else
    return OS_MODE_WIN;
}

bool process_os_mode(uint16_t keycode, keyrecord_t *record) {
  if (!record->event.pressed)
    return false;

  if (keycode == U_MAC)
    process_magic(QK_MAGIC_SWAP_CTL_GUI, record);
  else
    process_magic(QK_MAGIC_UNSWAP_CTL_GUI, record);

  user_config.os_mode_linux = (keycode == U_LNX);
  eeconfig_update_user(user_config.raw);

  show_os_mode(keycode);
  return false;
}


// User key

const char* const userkey_win =
  SS_LALT(
    SS_TAP(X_KP_0) SS_TAP(X_KP_1) SS_TAP(X_KP_6) SS_TAP(X_KP_3)
  );
const char* const userkey_mac =
  SS_LALT("3");
const char* const userkey_lnx =
  SS_LALT("U")
  "00a3"
  " ";

void tap_userkey(void) {
  // Save and cancel mods
  const uint8_t saved_mods = get_mods();
  del_mods(MOD_MASK_CSAG);
  del_weak_mods(MOD_MASK_CSAG);
  del_oneshot_mods(MOD_MASK_CSAG);

  // Toggle lock keys?
  const led_t saved_led_state = host_keyboard_led_state();
  // Linux unicode entry requires an uppercase U
  // Caps lock interferes
  const bool toggle_caps_lock = saved_led_state.caps_lock;
  // Windows numpad cp1252 entry requires num-lock
  const bool toggle_num_lock = !saved_led_state.num_lock;

  // Send key presses
  const os_mode_t os_mode = os_mode_get();
  switch (os_mode) {
    case OS_MODE_WIN:
      if (toggle_num_lock)
        tap_code(KC_NUM_LOCK);
      send_string_with_delay(userkey_win, TAP_CODE_DELAY);
      if (toggle_num_lock)
        tap_code(KC_NUM_LOCK);
      break;
    case OS_MODE_MAC:
      send_string_with_delay(userkey_mac, TAP_CODE_DELAY);
      break;
    case OS_MODE_LNX:
      if (toggle_caps_lock)
        tap_code(KC_CAPS_LOCK);
      send_string_with_delay(userkey_lnx, TAP_CODE_DELAY);
      if (toggle_caps_lock)
        tap_code(KC_CAPS_LOCK);
      break;
    default:
      break;
  }

  // Restore mods
  set_mods(saved_mods);
  send_keyboard_report();
}

bool process_userkey(uint16_t keycode, keyrecord_t *record) {
  if (record->event.pressed)
    tap_userkey();
  return false;
}


// Clipboard and undo/redo

typedef enum {
  CLIP_CUT,
  CLIP_CPY,
  CLIP_PST,
  CLIP_UND,
  CLIP_RDO,
  CLIP_END,
} clip_t;

const uint16_t clipcodes[][CLIP_END] = {
  [OS_MODE_WIN] = { LCTL(KC_X), LCTL(KC_C), LCTL(KC_V), LCTL(KC_Z), LCTL(KC_Y), },
  [OS_MODE_MAC] = { LCMD(KC_X), LCMD(KC_C), LCMD(KC_V), LCMD(KC_Z), SCMD(KC_Z), },
  [OS_MODE_LNX] = { U_CUT, U_CPY, U_PST, U_UND, U_RDO, },
};

bool process_clipcode(clip_t clip, keyrecord_t *record) {
  const os_mode_t os_mode = os_mode_get();
  const uint16_t keycode = clipcodes[os_mode][clip];
  if (record->event.pressed)
    register_code16(keycode);
  else
    unregister_code16(keycode);
  return false;
}


// Audio

#ifdef AUDIO_ENABLE

bool process_audio_toggle(keyrecord_t *record) {
  if (!record->event.pressed)
    return false;

  const uint8_t mods = get_mods() | get_weak_mods() | get_oneshot_mods();
  const uint8_t shifted = mods & MOD_MASK_SHIFT;
  if (shifted) {
    // Audio off
    // Temporarily enable audio for feedback
    audio_on();
  }

  // Show toggle in the on state
  if (audio_is_on()) {
      show_toggle(QK_AUDIO_TOGGLE, false);
      // Wait for the show_toggle sound to finish
      // (the wait inside audio_off is not enough)
      wait_ms(100);
      audio_off();
  } else {
      audio_on();
      show_toggle(QK_AUDIO_TOGGLE, true);
  }
  return false;
}

#endif


// Rgb

#ifdef RGB_MATRIX_ENABLE

// Configuration checks

_Static_assert((RGB_MATRIX_EFFECT_MAX == (RGB_MATRIX_EFFECTS_EXPECTED + 1)), "RGB_MATRIX_EFFECT_MAX is the wtong value");
_Static_assert((RGB_MATRIX_DEFAULT_VAL <= RGB_MATRIX_MAXIMUM_BRIGHTNESS), "RGB_MATRIX_DEFAULT_VAL is too large");


// Override keys to allow feedback on keydown
// (A post_process_record_user implementation is simpler, but fails)
// (The user function is called on keyup but not on keydown following a return-false)

// Return number of steps (qadd8, qsub8) from zero
int8_t step_value(uint8_t i, int8_t step) {
  const uint8_t n = i / step;
  const uint8_t over = i % step;
  const uint8_t under = step - over;
  // Round to the nearest step
  if (over <= under)
    return n;
  else
    return n + 1;
}

// Return true if slider value (qadd8, qsub8) is on detent
bool slider_on_detent(uint8_t value, uint8_t detent, int8_t step) {
  uint8_t lower = detent - (step / 2);
  uint8_t upper = lower + step - 1;
  // Wrap around? floor at 0
  if (lower > detent)
    lower = 0;
  // Wrap around? cap at 0xFF
  if (upper < detent)
    upper = 255;
  return (lower <= value && value <= upper);
}

// Return true if rotator value (qadd8, qsub8) is on detent
bool rotator_on_detent(uint8_t value, uint8_t detent, int8_t step) {
  // Transform into a middle-of-the-range test.
  const uint8_t offset = 128 - detent;
  return slider_on_detent(value+offset, detent+offset, step);
}

bool process_rgb_toggle(uint16_t keycode, keyrecord_t *record) {
  if (!record->event.pressed)
    return false;

  const uint8_t mods = get_mods() | get_weak_mods() | get_oneshot_mods();
  const uint8_t shifted = mods & MOD_MASK_SHIFT;
  if (shifted) {
    // Rgb off
    // Temporarily enable rgb matrix for feedback
    rgb_matrix_enable();
  }

  // Show toggle in the on state
  if (rgb_matrix_is_enabled()) {
    show_toggle(RGB_TOG, false);
    rgb_matrix_disable();
  } else {
    rgb_matrix_enable();
    show_toggle(RGB_TOG, true);
  }
  return false;
}

bool process_rgb_mode(uint16_t keycode, keyrecord_t *record) {
  if (!record->event.pressed)
    return false;

  const uint8_t mods = get_mods() | get_weak_mods() | get_oneshot_mods();
  const uint8_t shifted = mods & MOD_MASK_SHIFT;
  if (!shifted)
    rgb_matrix_step();
  else
    rgb_matrix_step_reverse();

  const uint8_t mode = rgb_matrix_get_mode();
  const bool detent = (mode == RGB_MATRIX_DEFAULT_MODE);
  show_value(keycode, mode, detent);
  return false;
}

bool process_rgb_hue(uint16_t keycode, keyrecord_t *record) {
  if (!record->event.pressed)
    return false;

  const uint8_t mods = get_mods() | get_weak_mods() | get_oneshot_mods();
  const uint8_t shifted = mods & MOD_MASK_SHIFT;
  if (!shifted)
    rgb_matrix_increase_hue();
  else
    rgb_matrix_decrease_hue();

  const uint8_t hue = rgb_matrix_get_hue();
  const uint8_t value = step_value(hue, RGB_MATRIX_HUE_STEP);
  const bool detent = rotator_on_detent(hue, RGB_MATRIX_DEFAULT_HUE, RGB_MATRIX_HUE_STEP);
  show_value(keycode, value, detent);
  return false;
}

bool process_rgb_sat(uint16_t keycode, keyrecord_t *record) {
  if (!record->event.pressed)
    return false;

  const uint8_t mods = get_mods() | get_weak_mods() | get_oneshot_mods();
  const uint8_t shifted = mods & MOD_MASK_SHIFT;
  if (!shifted)
    rgb_matrix_increase_sat();
  else
    rgb_matrix_decrease_sat();

  const uint8_t sat = rgb_matrix_get_sat();
  const uint8_t value = step_value(sat, RGB_MATRIX_SAT_STEP);
  const bool detent = slider_on_detent(sat, RGB_MATRIX_DEFAULT_SAT, RGB_MATRIX_SAT_STEP);
  show_value(keycode, value, detent);
  return false;
}

bool process_rgb_val(uint16_t keycode, keyrecord_t *record) {
  if (!record->event.pressed)
    return false;

  const uint8_t mods = get_mods() | get_weak_mods() | get_oneshot_mods();
  const uint8_t shifted = mods & MOD_MASK_SHIFT;
  if (!shifted)
    rgb_matrix_increase_val();
  else
    rgb_matrix_decrease_val();

  const uint8_t val = rgb_matrix_get_val();
  const uint8_t value = step_value(val, RGB_MATRIX_VAL_STEP);
  const bool detent = slider_on_detent(val, RGB_MATRIX_DEFAULT_VAL, RGB_MATRIX_VAL_STEP);
  show_value(keycode, value, detent);
  return false;
}

#endif


// Caps word

bool caps_word_press_user(uint16_t keycode) {
  switch (keycode) {
    // Continue Caps Word, with shift
    case KC_A ... KC_Z:
        add_weak_mods(MOD_BIT(KC_LSFT));
        return true;

    // Continue Caps Word, no shift
    case KC_1 ... KC_0:
    case KC_BSPC:
    case KC_DEL:
    case KC_MINS:
    case KC_UNDS:
        return true;

    // End Caps Word
    default:
        return false;
  }
}


// Shift and Auto Shift overrides

#define ko_make_with_action_and_layers(trigger_mods_, trigger_key, action, layer_mask)    \
  ((const key_override_t){                                                                \
      .trigger_mods                           = (trigger_mods_),                          \
      .layers                                 = (layer_mask),                             \
      .suppressed_mods                        = 0,                                        \
      .options                                = ko_options_default,                       \
      .negative_mod_mask                      = 0,                                        \
      .custom_action                          = action,                                   \
      .context                                = NULL,                                     \
      .trigger                                = (trigger_key),                            \
      .replacement                            = KC_NO,                                    \
      .enabled                                = NULL                                      \
  })

bool nine_shift_action(bool key_down, void *context) {
  if (key_down)
    tap_userkey();
  return false;
}

#define LAYER_MASK_NUM (1 << U_NUM)

const key_override_t capsword_shift_override = ko_make_basic(MOD_MASK_SHIFT, CW_TOGG, KC_CAPS);
const key_override_t nine_shift_override = ko_make_with_action_and_layers(MOD_MASK_SHIFT, KC_9, nine_shift_action, LAYER_MASK_NUM);
const key_override_t dot_shift_override = ko_make_with_layers(MOD_MASK_SHIFT, KC_DOT, KC_LEFT_PAREN, LAYER_MASK_NUM);
// The following key overrides give auto-repeat consistency to the left-hand thumb keys
// Without the key override these shifted keys tap (because of auto-shift)
// With the key override these shifted keys press/release (like the dot-key override)
const key_override_t zero_shift_override = ko_make_with_layers(MOD_MASK_SHIFT, KC_0, KC_RIGHT_PAREN, LAYER_MASK_NUM);
const key_override_t minus_shift_override = ko_make_with_layers(MOD_MASK_SHIFT, KC_MINUS, KC_UNDERSCORE, LAYER_MASK_NUM);
const key_override_t* const custom_overrides[] = {
  &capsword_shift_override,
  &nine_shift_override,
  &dot_shift_override,
  &zero_shift_override,
  &minus_shift_override,
};

uint16_t key_override_count(void) {
  return ARRAY_SIZE(custom_overrides);
}

const key_override_t* key_override_get(uint16_t key_override_idx) {
  if (key_override_idx >= key_override_count()) {
    return NULL;
  }
  return custom_overrides[key_override_idx];
}

uint16_t autoshift_override(uint16_t keycode, keyrecord_t *record) {
  // Look for matching key override
  const uint8_t layer = 1 << read_source_layers_cache(record->event.key);
  for (uint8_t i = 0; i < key_override_count(); i++) {
    const key_override_t *const override = key_override_get(i);
    if (!override)
      break;

    // Trigger match?
    if (keycode != override->trigger)
      continue;
    // Layer match?
    if ((layer & override->layers) == 0)
      continue;
    // Found replacement
    return override->replacement;
  }

  // No override
  return KC_TRANSPARENT;
}

void autoshift_press_user(uint16_t keycode, bool shifted, keyrecord_t *record) {
  const uint16_t replacement = autoshift_override(keycode, record);
  if (replacement != KC_TRANSPARENT) {
    // Tap to be consistent with no-override behaviour
    register_code16(shifted ? replacement : keycode);
    return;
  }

  if (shifted)
    register_weak_mods(MOD_BIT(KC_LSFT));
  // & 0xFF gets the Tap key for Tap Holds, required when using Retro Shift
  register_code16((IS_RETRO(keycode)) ? keycode & 0xFF : keycode);
}

void autoshift_release_user(uint16_t keycode, bool shifted, keyrecord_t *record) {
  const uint16_t replacement = autoshift_override(keycode, record);
  if (replacement != KC_TRANSPARENT) {
    // Tap to be consistent with no-override behaviour
    unregister_code16(shifted ? replacement : keycode);
    return;
  }

  // & 0xFF gets the Tap key for Tap Holds, required when using Retro Shift
  // The IS_RETRO check isn't really necessary here, always using
  // keycode & 0xFF would be fine
  unregister_code16((IS_RETRO(keycode)) ? keycode & 0xFF : keycode);
}


// Key processing

__attribute__((weak)) bool process_record_extra(uint16_t keycode, keyrecord_t *record) {
  return true;
}

bool process_record_user(uint16_t keycode, keyrecord_t *record) {
  if (!process_record_extra(keycode, record))
    return false;

  switch (keycode) {
    case U_USER:
      return process_userkey(keycode, record);
    case U_WIN:
      return process_os_mode(keycode, record);
    case U_MAC:
      return process_os_mode(keycode, record);
    case U_LNX:
      return process_os_mode(keycode, record);
    case U_CUT:
      return process_clipcode(CLIP_CUT, record);
    case U_CPY:
      return process_clipcode(CLIP_CPY, record);
    case U_PST:
      return process_clipcode(CLIP_PST, record);
    case U_UND:
      return process_clipcode(CLIP_UND, record);
    case U_RDO:
      return process_clipcode(CLIP_RDO, record);
#ifdef RGB_MATRIX_ENABLE
    case U_RGB_TOG:
      return process_rgb_toggle(keycode, record);
    case U_RGB_MOD:
      return process_rgb_mode(keycode, record);
    case U_RGB_HUI:
      return process_rgb_hue(keycode, record);
    case U_RGB_SAI:
      return process_rgb_sat(keycode, record);
    case U_RGB_VAI:
      return process_rgb_val(keycode, record);
#endif
#ifdef AUDIO_ENABLE
    case U_AUD_TOG:
      return process_audio_toggle(record);
#endif
    default:
      return true;
  }
}


// Tapping term

uint16_t get_tapping_term(uint16_t keycode, keyrecord_t *record) {
  // Extra delay on weaker ring and pinky fingers
  switch (keycode) {
    default:
      return FAST_TAPPING_TERM;
    // Colemak DH, Qwerty
    case LGUI_T(KC_A):
    case LGUI_T(KC_O):
    case LGUI_T(KC_QUOT):
    case LALT_T(KC_R):
    case LALT_T(KC_I):
    case LALT_T(KC_S):
    case LALT_T(KC_L):
      return SLOW_TAPPING_TERM;
    // Double taps
    case TD(U_TD_BOOT):
    case TD(U_TD_U_BASE):
    case TD(U_TD_U_EXTRA):
    case TD(U_TD_U_TAP):
    case TD(U_TD_U_BUTTON):
    case TD(U_TD_U_NAV):
    case TD(U_TD_U_MOUSE):
    case TD(U_TD_U_MEDIA):
    case TD(U_TD_U_NUM):
    case TD(U_TD_U_SYM):
    case TD(U_TD_U_FUN):
      return DOUBLE_TAPPING_TERM;
  }
}


// Power

__attribute__((weak)) void suspend_power_down_extra(void) {
}

void suspend_power_down_user(void) {
  // May be run multiple times on suspend
  suspend_power_down_extra();
}

__attribute__((weak)) void suspend_wakeup_init_extra(void) {
}

void suspend_wakeup_init_user(void) {
  suspend_wakeup_init_extra();
}


// Initialization

__attribute__((weak)) void eeconfig_init_extra(void) {
}

void eeconfig_init_user(void) {
  user_config.raw = 0;
  // Windows mode by default
  // keymap_config.swap_lctl_lgui defaults to false
  user_config.os_mode_linux = false;

  eeconfig_update_user(user_config.raw);
  eeconfig_init_extra();
}

__attribute__((weak)) void keyboard_post_init_extra(void) {
}

void keyboard_post_init_user(void) {
  // Restore user state
  user_config.raw = eeconfig_read_user();
  keyboard_post_init_extra();
}
