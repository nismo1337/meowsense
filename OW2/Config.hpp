#pragma once

namespace OW {
	namespace Config {
		/* Aimbot */
		float Flick_smooth = 3.f;
		float Tracking_smooth = 3.f;
		float Fov = 200.f;
		
		bool Flick = false;
		bool Tracking = false;

		/* Draw */
		bool draw_fov = true;
		bool draw_box = false;
		bool draw_edge = true;

		/* Draw Menu */
		bool Menu = true;

		/* Team */
		bool is_team = false;

		/* Color */
		ImVec4 EdgeESPColor = ImVec4(1, 1, 1, 1);
	}
}