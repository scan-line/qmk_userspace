// Copyright 2025 Scan Line
// https://github.com/scan-line

// This program is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 2 of the License, or (at your option) any later version. This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details. You should have received a copy of the GNU General Public License along with this program. If not, see <http://www.gnu.org/licenses/>.

#include "lcd.h"

#include "qp.h"
#include "qp_comms.h"
#include "qp_st77xx_opcodes.h"
#include "qp_lvgl.h"
#include "printf.h"


void lcd_init_left(void) {
}

void lcd_init_right(void) {
}

void lcd_init(void) {
    // Initialise the lcd
    wait_ms(LCD_WAIT_TIME);
    painter_device_t lcd = qp_st7735_make_spi_device(LCD_WIDTH, LCD_HEIGHT, LCD_CS_PIN, LCD_DC_PIN, LCD_RST_PIN, LCD_SPI_DIVISOR, LCD_SPI_MODE);
    qp_init(lcd, LCD_ROTATION);
    qp_set_viewport_offsets(lcd, LCD_OFFSET_X, LCD_OFFSET_Y);

#ifdef LCD_INVERT_COLOUR
    qp_comms_start(lcd);
    qp_comms_command(lcd, ST77XX_CMD_INVERT_ON);
    qp_comms_stop(lcd);
#endif

    if (!qp_lvgl_attach(lcd)) return;

    if (is_keyboard_left())
        lcd_init_left();
    else
        lcd_init_right();
}

void lcd_housekeeping_task(void) {
}
