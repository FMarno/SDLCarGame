#include "SDL.h"
#include "SDL_image.h"
#include "iostream"
#include <memory>
#include <array>

typedef std::unique_ptr<SDL_Texture, decltype(&SDL_DestroyTexture)> TexturePtr;

const unsigned int framerate = 20;

template <unsigned int frames,unsigned int rows, unsigned int columns>
constexpr std::array<SDL_Rect, frames> generate_spritesheet_boxes(
	unsigned int width, unsigned int height
) noexcept {
	std::array<SDL_Rect, frames> boxes;
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

template <unsigned int frames, unsigned int rows, unsigned int columns>
class SpriteSheet {
	private:
		TexturePtr texture;
		const std::array<SDL_Rect,frames> boxes;

	public:
	    SpriteSheet(
			TexturePtr texture, unsigned int width, unsigned int height
		): texture(std::move(texture)), boxes(generate_spritesheet_boxes<frames, rows, columns>(width, height)) { }

		void Render(SDL_Renderer* renderer, unsigned int frame, SDL_Rect* dstrect){
			SDL_RenderCopy(renderer, texture.get(), &boxes[frame % frames], dstrect);
		}
};

template<unsigned int frames, unsigned int rows, unsigned int columns>
SpriteSheet<frames, rows, columns> load_spritesheet(SDL_Renderer* renderer, const char* path){
	//load png
	SDL_Surface* image = IMG_Load(path);
	if (image == nullptr){
		std::cout << "failed to load image at" << path << '\n';
		exit(1);
	}
	SpriteSheet<frames, rows, columns> sheet(
		TexturePtr(SDL_CreateTextureFromSurface(renderer, image),&SDL_DestroyTexture),
		image->w,
	   	image->h
	);
	SDL_FreeSurface(image);
	return sheet;
}

template <unsigned int frames, unsigned int rows, unsigned int columns>
void game_loop(SDL_Renderer * renderer, SpriteSheet<frames, rows, columns> runner){
	SDL_Event event;
	for (unsigned int frame =0;true;++frame){
		while (SDL_PollEvent(&event) != 0){
			if (event.type == SDL_QUIT) return;

			if (event.type == SDL_KEYDOWN) {
				switch (event.key.keysym.sym){
					case SDLK_RIGHT:
						std::cout << "right\n";
						break;
					case SDLK_LEFT:
						std::cout << "left\n";
						break;
					case SDLK_q:
						return;
				}
			}
		}
		SDL_RenderClear(renderer);
		runner.Render(renderer, frame, nullptr);
		SDL_RenderPresent(renderer);
		SDL_Delay(1000/framerate);
	}
}

int main(int argc, char *argv[])
{
	SDL_Init(SDL_INIT_VIDEO);
	IMG_Init(IMG_INIT_PNG);

	std::unique_ptr<SDL_Window, decltype(&SDL_DestroyWindow)> window(
		SDL_CreateWindow(
			"SDL2Test",
			SDL_WINDOWPOS_UNDEFINED,
			SDL_WINDOWPOS_UNDEFINED,
			640,
			480,
			0
		),
		&SDL_DestroyWindow
	);

	std::unique_ptr<SDL_Renderer, decltype(&SDL_DestroyRenderer)> renderer(
		SDL_CreateRenderer(window.get(), -1, SDL_RENDERER_SOFTWARE),
		&SDL_DestroyRenderer
	);

	SDL_SetRenderDrawColor(renderer.get(), 0xFF, 0xFF, 0xFF, 0xFF);

	auto runner = load_spritesheet<7,3,3>(renderer.get(), "runner.png");
	auto car = load_spritesheet<1,1,1>(renderer.get(), "car.png");

	game_loop(renderer.get(), std::move(runner));

	IMG_Quit();
	SDL_Quit();

	return 0;
}
