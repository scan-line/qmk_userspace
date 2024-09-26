// Copyright 2023 Scan Line
// https://github.com/scan-line

// This program is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 2 of the License, or (at your option) any later version. This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details. You should have received a copy of the GNU General Public License along with this program. If not, see <http://www.gnu.org/licenses/>.

#ifdef ENABLE_RGB_MATRIX_FEEDBACK
RGB_MATRIX_EFFECT(FEEDBACK)
#ifdef RGB_MATRIX_CUSTOM_EFFECT_IMPLS

bool rgb_matrix_effect_feedback(effect_params_t* params);

bool FEEDBACK(effect_params_t* params) {
  return rgb_matrix_effect_feedback(params);
}

#endif // RGB_MATRIX_CUSTOM_EFFECT_IMPLS
#endif // ENABLE_RGB_MATRIX_FEEDBACK
