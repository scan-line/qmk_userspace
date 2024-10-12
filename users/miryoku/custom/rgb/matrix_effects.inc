// Copyright 2023 Scan Line
// https://github.com/scan-line

// This program is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 2 of the License, or (at your option) any later version. This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details. You should have received a copy of the GNU General Public License along with this program. If not, see <http://www.gnu.org/licenses/>.

// Replace the shipped rgb_matrix_effects.inc to control matrix mode order
// Matrix mode follows include order, starting with enum 1
#include "custom/rgb/feedback_anim.h"
#include "solid_color_anim.h"
#include "alpha_mods_anim.h"
#include "gradient_up_down_anim.h"
#include "gradient_left_right_anim.h"
#include "hue_breathing_anim.h"
#include "riverflow_anim.h"
#include "pixel_flow_anim.h"
#include "custom/rgb/slowdrops_anim.h"
#include "solid_reactive_wide.h"
#include "solid_reactive_nexus.h"
#include "typing_heatmap_anim.h"
