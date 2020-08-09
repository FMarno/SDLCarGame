#pragma once

#include "SDL.h"
#include <vector>
#include <memory>

typedef std::unique_ptr<SDL_Texture, decltype(&SDL_DestroyTexture)> TexturePtr;
TexturePtr make_unique_texture(SDL_Texture* texture);

class SpriteSheet {
	private:
		TexturePtr texture;
		const std::vector<SDL_Rect> boxes;
		const unsigned int frames;

	public:
		SpriteSheet(TexturePtr texture, unsigned int frames, unsigned int rows, unsigned int columns, unsigned int width, unsigned int height);

		void render(SDL_Renderer* renderer, unsigned int frame, SDL_Rect* dstrect);
};

SpriteSheet load_spritesheet(SDL_Renderer* renderer,unsigned int frames, unsigned int rows, unsigned int columns, const char* path);
