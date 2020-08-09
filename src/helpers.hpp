#pragma once
#include "SpriteSheet.hpp"

typedef std::unique_ptr<SDL_Window, decltype(&SDL_DestroyWindow)> WindowPtr;
typedef std::unique_ptr<SDL_Renderer, decltype(&SDL_DestroyRenderer)> RendererPtr;

WindowPtr make_unique_window(SDL_Window* window);
RendererPtr make_unique_renderer(SDL_Renderer* renderer);

struct ButtonPresses {
	bool quit = false;
	bool jump_pressed = false;
};

void read_buttons(ButtonPresses& buttons);
TexturePtr generate_backgroud(SDL_Renderer* renderer, int width, int height);

