// Copyright 2023 Scan Line
// https://github.com/scan-line

// This program is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 2 of the License, or (at your option) any later version. This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details. You should have received a copy of the GNU General Public License along with this program. If not, see <http://www.gnu.org/licenses/>.

#include QMK_KEYBOARD_H

#include "miryoku.h"


// Qmk definitions

// Oneshot no-ops
#ifdef NO_ACTION_ONESHOT
#  define get_oneshot_mods() 0
#endif


// Layer rgb


// Corrne 3x6_3 led pattern is
// split left
// 24 23 18 17 10  9    36 37 44 45 50 51
// 25 22 19 16 11  8    35 38 43 46 49 52
// 26 21 20 15 12  7    34 39 42 47 48 53
//          14 13  6    33 40 41
          
 
// TODO
const uint8_t led_grid[3][10] = {
  {23, 18, 17, 10,  9,   36, 37, 44, 45, 50},
  {22, 19, 16, 11,  8,   35, 38, 43, 46, 49},
  {21, 20, 15, 12,  7,   34, 39, 42, 47, 48},
};
const uint8_t led_thumb[6] = {
           14, 13,  6,   33, 40, 41
};
