//render.hpp

#pragma once

#include "game.hpp"


namespace meteor {

	const int WINDOW_WIDTH = 1200, 
			  WINDOW_HEIGHT = 1200;


	void draw() {
		ClearBackground(RAYWHITE);
		BeginDrawing();
		DrawFPS(2, 2);


		
		EndDrawing();
	}
}
