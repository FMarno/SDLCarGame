#include "Character.hpp"

Character::Character(SpriteSheet&& sprite_sheet, Point p, Velocity v, int w, int h, unsigned int framerate, unsigned int fps) :
	sprite_sheet(std::move(sprite_sheet)), rect(SDL_Rect{p.x,p.y,w,h}), velocity(v), frame_denominator(framerate/fps){}

	void Character::render(SDL_Renderer* renderer){
		sprite_sheet.render(renderer, current_frame/frame_denominator, &rect);
		++current_frame;
	}
