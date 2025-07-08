// Copyright 2025 Scan Line
// https://github.com/scan-line

// This program is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 2 of the License, or (at your option) any later version. This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details. You should have received a copy of the GNU General Public License along with this program. If not, see <http://www.gnu.org/licenses/>.

#include "oled.h"

#include "qp.h"
#include "qp_comms.h"
#include "qp_st77xx_opcodes.h"
#include "qp_lvgl.h"
#include "printf.h"


void oled_draw_left(void) {
}

void oled_draw_right(void) {
}

void oled_init_styles(void) {
}

void oled_init(void) {
    master = is_keyboard_master();
    // transaction_register_rpc(SCR_SLV_SYNC, screen_sync_slave_handler);
    // timer = // _read();
    uprintf("Post Init\n");
    wait_ms(LCD_WAIT_TIME);
    // Initialise the LCD
    lcd = qp_st7735_make_spi_device(LCD_WIDTH, LCD_HEIGHT, LCD_CS_PIN, LCD_DC_PIN, LCD_RST_PIN, LCD_SPI_DIVISOR, LCD_SPI_MODE);
    qp_init(lcd, LCD_ROTATION);

    // Apply Offset
    qp_set_viewport_offsets(lcd, LCD_OFFSET_X, LCD_OFFSET_Y);

// Invert Colour
#ifdef LCD_INVERT_COLOUR
    qp_comms_start(lcd);
    qp_comms_command(lcd, ST77XX_CMD_INVERT_ON);
    qp_comms_stop(lcd);
#endif

    if (!qp_lvgl_attach(lcd)) return;

    oled_init_styles();

    if (is_keyboard_left())
        oled_draw_left();
    else
        oled_draw_right();
}

void oled_housekeeping_task(void) {
}
