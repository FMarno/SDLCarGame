#pragma once

#include "SpriteSheet.hpp"

struct Point {
	int x;
	int y;
};

struct Velocity {
	int x;
	int y;
};

struct Character {
	SpriteSheet sprite_sheet;
	SDL_Rect rect;
	Velocity velocity;
	unsigned int current_frame = 0;
	unsigned int frame_denominator;

	Character(SpriteSheet&& sprite_sheet, Point p, Velocity v, int w, int h, unsigned int framerate, unsigned int fps);

	void render(SDL_Renderer* renderer);
};
