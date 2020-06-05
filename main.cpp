#include "SDL.h"
#include "SDL_image.h"
#include "iostream"
#include <memory>
#include <vector>

typedef std::unique_ptr<SDL_Texture, decltype(&SDL_DestroyTexture)> TexturePtr;

const unsigned int framerate = 60;
const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;

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

class SpriteSheet {
	private:
		TexturePtr texture;
		const std::vector<SDL_Rect> boxes;
		const unsigned int frames;

	public:
		SpriteSheet(
				TexturePtr texture, unsigned int frames, unsigned int rows, unsigned int columns, unsigned int width, unsigned int height
				): texture(std::move(texture)), frames(frames), boxes(generate_spritesheet_boxes(frames, rows, columns, width, height)) { }

		void render(SDL_Renderer* renderer, unsigned int frame, SDL_Rect* dstrect){
			SDL_RenderCopy(renderer, texture.get(), &boxes[frame % frames], dstrect);
		}
};

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
	Point position;
	Velocity velocity;
	unsigned int width;
	unsigned int height;
	unsigned int current_frame = 0;
	unsigned int frame_denominator;

	Character(SpriteSheet&& sprite_sheet, Point p, Velocity v, unsigned int w, unsigned int h, unsigned int fps) : sprite_sheet(std::move(sprite_sheet)), position(p), velocity(v), width(w), height(h), frame_denominator(framerate/fps){}

	void update(){
		position.x += velocity.x;
		position.y += velocity.y;
	}

	void render(SDL_Renderer* renderer){
		SDL_Rect dstrect;
		dstrect.x = position.x;
		dstrect.y = position.y;
		dstrect.w = width;
		dstrect.h = height;
		sprite_sheet.render(renderer, current_frame/frame_denominator, &dstrect);
		++current_frame;
	}
};

SpriteSheet load_spritesheet(SDL_Renderer* renderer,unsigned int frames, unsigned int rows, unsigned int columns, const char* path){
	//load png
	SDL_Surface* image = IMG_Load(path);
	if (image == nullptr){
		std::cout << "failed to load image at" << path << '\n';
		exit(1);
	}
	SpriteSheet sheet(
			TexturePtr(SDL_CreateTextureFromSurface(renderer, image),&SDL_DestroyTexture),
			frames,
			rows,
			columns,
			image->w,
			image->h
			);
	SDL_FreeSurface(image);
	return sheet;
}

void game_loop(SDL_Renderer * renderer, Character runner, Character car){
	SDL_Event event;
	bool right_pressed = false;
	bool left_pressed = false;
	bool up_pressed = false;
	bool down_pressed = false;
	for (unsigned int frame =0;true;++frame){
		while (SDL_PollEvent(&event) != 0){
			if (event.type == SDL_QUIT) return;

			if (event.type == SDL_KEYDOWN) {
				switch (event.key.keysym.sym){
					case SDLK_RIGHT:
						right_pressed = true;
						break;
					case SDLK_LEFT:
						left_pressed = true;
						break;
					case SDLK_UP:
						up_pressed = true;
						break;
					case SDLK_DOWN:
						down_pressed = true;
						break;
				}
			} else if (event.type == SDL_KEYUP) {
				switch (event.key.keysym.sym){
					case SDLK_RIGHT:
						right_pressed = false;
						break;
					case SDLK_LEFT:
						left_pressed = false;
						break;
					case SDLK_UP:
						up_pressed = false;
						break;
					case SDLK_DOWN:
						down_pressed = false;
						break;
					case SDLK_q:
						return;
				}
			}
		}

		// update
		runner.velocity = {0,0};
		if (right_pressed){
			runner.velocity.x +=10;
		}
		if (left_pressed){
			runner.velocity.x -=10;
		}
		if (up_pressed){
			runner.velocity.y -=10;
		}
		if (down_pressed){
			runner.velocity.y +=10;
		}
		runner.update();
		if (car.position.x + (int)car.width < 0){
			car.position.x = SCREEN_WIDTH;
		}
		car.update();

		// render
		SDL_RenderClear(renderer);
		runner.render(renderer);
		car.render(renderer);
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
				SCREEN_WIDTH,
				SCREEN_HEIGHT,
				0
				),
			&SDL_DestroyWindow
			);

	std::unique_ptr<SDL_Renderer, decltype(&SDL_DestroyRenderer)> renderer(
			SDL_CreateRenderer(window.get(), -1, SDL_RENDERER_SOFTWARE),
			&SDL_DestroyRenderer
			);

	SDL_SetRenderDrawColor(renderer.get(), 0xFF, 0x00, 0xFF, 0xFF);

	auto runner = load_spritesheet(renderer.get(),7,3,3, "runner.png");
	auto car = load_spritesheet(renderer.get(),1,1,1, "car.png");

	game_loop(
			renderer.get(),
			Character(std::move(runner),Point{0,0},Velocity{0,0},50,50,15),
			Character(std::move(car), Point{SCREEN_WIDTH - 100, SCREEN_HEIGHT - 100}, Velocity{-10, 0}, 80, 80, 1)
			);

	IMG_Quit();
	SDL_Quit();

	return 0;
}
