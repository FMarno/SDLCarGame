#include "helpers.hpp"
#include <stdio.h>

const Uint8 sand[4] = {0xFB,0xB6,0x52,0xFF};
// const Uint8 peach[4] = {0xFF,0xCE,0x9A,0xFF};
// const Uint8 light_blue[4] = {0x01,0xAC,0xD2,0xFF};
const Uint8 dark_blue[4] = {0x00,0x90,0xC4,0xFF};


WindowPtr make_unique_window(SDL_Window* window){
	return WindowPtr(window, &SDL_DestroyWindow);
}

RendererPtr make_unique_renderer(SDL_Renderer* renderer){
	return RendererPtr(renderer, &SDL_DestroyRenderer);
}

void read_buttons(ButtonPresses& buttons){
	SDL_Event event;
	while (SDL_PollEvent(&event) != 0){
		if (event.type == SDL_QUIT) {
			buttons.quit = true;
			return;
		}
		if (event.type == SDL_KEYDOWN) {
			switch (event.key.keysym.sym){
				case SDLK_RIGHT:
					buttons.right_pressed = true;
					break;
				case SDLK_LEFT:
					buttons.left_pressed = true;
					break;
				case SDLK_UP:
					buttons.up_pressed = true;
					break;
				case SDLK_DOWN:
					buttons.down_pressed = true;
					break;
			}
		} else if (event.type == SDL_KEYUP) {
			switch (event.key.keysym.sym){
				case SDLK_RIGHT:
					buttons.right_pressed = false;
					break;
				case SDLK_LEFT:
					buttons.left_pressed = false;
					break;
				case SDLK_UP:
					buttons.up_pressed = false;
					break;
				case SDLK_DOWN:
					buttons.down_pressed = false;
					break;
				case SDLK_q:
					buttons.quit = true;
					return;
			}
		}
	}
}

TexturePtr generate_backgroud(SDL_Renderer* renderer, int width, int height){
	SDL_Texture* texture = SDL_CreateTexture(
			renderer,
			SDL_PIXELFORMAT_RGBA32,
			SDL_TEXTUREACCESS_STATIC,
			width,
			height
			);
	if (texture == nullptr){
		printf("Error creating texture: %s\n", SDL_GetError());
	}
	Uint32* pixels = new Uint32[width*height];

	for (int h=0; h < height; ++h){
		Uint8 colour[4];
		colour[0] = ((sand[0] * h)/height) + ((dark_blue[0] * (height - h))/height);
		colour[1] = ((sand[1] * h)/height) + ((dark_blue[1] * (height - h))/height);
		colour[2] = ((sand[2] * h)/height) + ((dark_blue[2] * (height - h))/height);
		for (int w = 0; w < width; ++w){
			pixels[h * width + w] = *(Uint32*)colour;
		}
	}

	if (SDL_UpdateTexture(texture, nullptr, pixels, width * sizeof(Uint32)) != 0){
		printf("Error updating texture: %s\n", SDL_GetError());
	}
	delete[] pixels;
	return make_unique_texture(texture);
}

