#include "SpriteSheet.hpp"
#include "SDL_image.h"

std::vector<SDL_Rect> generate_spritesheet_boxes(
		unsigned int frames, unsigned int rows, unsigned int columns, unsigned int width, unsigned int height
		) noexcept;

SpriteSheet::SpriteSheet(
		TexturePtr texture, unsigned int frames, unsigned int rows, unsigned int columns, unsigned int width, unsigned int height
		): texture(std::move(texture)), boxes(generate_spritesheet_boxes(frames, rows, columns, width, height)), frames(frames) { }

void SpriteSheet::render(SDL_Renderer* renderer, unsigned int frame, SDL_Rect* dstrect){
	SDL_RenderCopy(renderer, texture.get(), &boxes[frame % frames], dstrect);
}

TexturePtr make_unique_texture(SDL_Texture* texture){
	return TexturePtr(texture, &SDL_DestroyTexture);
}

SpriteSheet load_spritesheet(SDL_Renderer* renderer,unsigned int frames, unsigned int rows, unsigned int columns, const char* path){
	//load png
	SDL_Surface* image = IMG_Load(path);
	if (image == nullptr){
		printf("failed to load image at %s\n", path);
		exit(1);
	}
	SpriteSheet sheet(
			make_unique_texture(SDL_CreateTextureFromSurface(renderer, image)),
			frames,
			rows,
			columns,
			image->w,
			image->h
			);
	SDL_FreeSurface(image);
	return sheet;
}

std::vector<SDL_Rect> generate_spritesheet_boxes(
		unsigned int frames, unsigned int rows, unsigned int columns, unsigned int width, unsigned int height
		) noexcept {
	std::vector<SDL_Rect> boxes(frames);
	for (unsigned int i = 0; i < frames; ++i){
		boxes[i].w = width/columns;
		boxes[i].h = height/rows;
		auto frame_idx_x = i % columns;
		auto frame_idx_y = i / columns;
		boxes[i].x = frame_idx_x * boxes[i].w;
		boxes[i].y = frame_idx_y * boxes[i].h;
	}
	return boxes;
}
