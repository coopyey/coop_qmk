// Copyright 2023 Massdrop, Inc.
// SPDX-License-Identifier: GPL-2.0-or-later

// qmk compile -kb drop/ctrl/v2 -km default
// qmk flash -kb drop/ctrl/v2 -km default

#include QMK_KEYBOARD_H
#include "rgb_matrix.h"
#include "rgb_matrix_map.h"

#define _DEFAULT 0
#define _FUNC 1

const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
    [_DEFAULT] = LAYOUT_tkl_ansi(
        KC_ESC,           KC_F1,   KC_F2,   KC_F3,   KC_F4,   KC_F5,   KC_F6,   KC_F7,   KC_F8,   KC_F9,   KC_F10,  KC_F11,  KC_F12,     KC_PSCR, KC_SCRL, KC_PAUS,
        KC_GRV,  KC_1,    KC_2,    KC_3,    KC_4,    KC_5,    KC_6,    KC_7,    KC_8,    KC_9,    KC_0,    KC_MINS, KC_EQL,  KC_BSPC,    KC_INS,  KC_HOME, KC_PGUP,
        KC_TAB,  KC_Q,    KC_W,    KC_E,    KC_R,    KC_T,    KC_Y,    KC_U,    KC_I,    KC_O,    KC_P,    KC_LBRC, KC_RBRC, KC_BSLS,    KC_DEL,  KC_END,  KC_PGDN,
        KC_CAPS, KC_A,    KC_S,    KC_D,    KC_F,    KC_G,    KC_H,    KC_J,    KC_K,    KC_L,    KC_SCLN, KC_QUOT,          KC_ENT,
        KC_LSFT,          KC_Z,    KC_X,    KC_C,    KC_V,    KC_B,    KC_N,    KC_M,    KC_COMM, KC_DOT,  KC_SLSH,          KC_RSFT,             KC_UP,
        KC_LCTL, KC_LGUI, KC_LALT,                            KC_SPC,                             KC_RALT, MO(1),   KC_APP,  KC_RCTL,    KC_LEFT, KC_DOWN, KC_RGHT
    ),
    [_FUNC] = LAYOUT_tkl_ansi(
        _______,          _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______,    KC_MUTE, _______, _______,
        _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______,    KC_MPLY, KC_MSTP, KC_VOLU,
        _______, _______, _______, _______, RGB_HUI, RGB_SAI, _______, _______, _______, RGB_SPI, RGB_VAI, RGB_TOG, _______, _______,    KC_MPRV, KC_MNXT, KC_VOLD,
        _______, _______, _______, _______, RGB_HUD, RGB_SAD, _______, _______, _______, RGB_SPD, RGB_VAD, RGB_MOD,          _______,
        _______,          RGB_M_P, RGB_M_B, RGB_M_R, RGB_M_SW,QK_BOOT, NK_TOGG, _______, _______, _______, _______,          _______,             _______,
        _______, _______, _______,                            EE_CLR,                             _______, _______, _______, _______,    _______, _______, _______
    )
};

void keyboard_post_init_user(void) {
    #ifdef RGB_MATRIX_ENABLE
    rgb_matrix_mode_noeeprom(RGB_MATRIX_SOLID_COLOR);
    rgb_matrix_sethsv_noeeprom(HSV_RED);
    #endif
}

#ifdef RGB_MATRIX_ENABLE
bool rgb_matrix_indicators_advanced_user(uint8_t led_min, uint8_t led_max) {
    uint8_t layer = get_highest_layer(layer_state);

    switch(layer) {
        // Lights all assigned keys on function layer to red
        case _FUNC:
            for (uint8_t row = 0; row < MATRIX_ROWS; ++row) {
                for (uint8_t col = 0; col < MATRIX_COLS; ++col) {
                    uint8_t index = g_led_config.matrix_co[row][col];

                    if (index >= led_min && index < led_max && index != NO_LED && keymap_key_to_keycode(layer, (keypos_t){col,row}) > KC_TRNS) {
                        rgb_matrix_set_color(index, RGB_RED);
                    }
                }
            }

            //Set bootloader key to white
            rgb_matrix_set_color(LED_B, RGB_WHITE);
            break;
        // Controls caps lock indicator & default underglow only lighting
        case _DEFAULT:
            // Caps lock toggled on
            if (host_keyboard_led_state().caps_lock) {
                for (uint8_t i = led_min; i < led_max; i++) {
                    // Set underglow to yellow
                    if (g_led_config.flags[i] & LED_FLAG_UNDERGLOW) { rgb_matrix_set_color(i, RGB_CyberYELLOW); }
                    // Set modifier keys to blue
                    else if (g_led_config.flags[i] & LED_FLAG_MODIFIER) { rgb_matrix_set_color(i, RGB_CyberBLUE); }
                }

                // Set caps lock key to blue
                rgb_matrix_set_color(LED_CAP, RGB_CyberBLUE);
            }
            // Caps lock toggled off
            else if (!host_keyboard_led_state().caps_lock) {
                for (uint8_t i = led_min; i < led_max; i++) {
                    // Set underglow back to red
                    if (g_led_config.flags[i] & LED_FLAG_UNDERGLOW) { rgb_matrix_set_color(i, RGB_RED); }
                    // Turn off non underglow keys
                    else { rgb_matrix_set_color(i, RGB_OFF); }
                }
            }

            break;
    }

    return false;
}
#endif
