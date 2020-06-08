#include "SDL.h"
#include "SDL_image.h"
#include <memory>
#include <vector>
#include <cmath>
#include <stdio.h>


typedef std::unique_ptr<SDL_Texture, decltype(&SDL_DestroyTexture)> TexturePtr;

const unsigned int framerate = 60;
const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;
const double PI_4 = atan2(1,1);
const Uint8 sand[4] = {0xFB,0xB6,0x52,0xFF};
const Uint8 peach[4] = {0xFF,0xCE,0x9A,0xFF};
const Uint8 light_blue[4] = {0x01,0xAC,0xD2,0xFF};
const Uint8 dark_blue[4] = {0x00,0x90,0xC4,0xFF};

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
	return std::unique_ptr<SDL_Texture, decltype(&SDL_DestroyTexture)>(
			texture,
			&SDL_DestroyTexture
			);
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
	SDL_Rect rect;
	Velocity velocity;
	unsigned int current_frame = 0;
	unsigned int frame_denominator;

	Character(SpriteSheet&& sprite_sheet, Point p, Velocity v, int w, int h, unsigned int fps) :
		sprite_sheet(std::move(sprite_sheet)), rect(SDL_Rect{p.x,p.y,w,h}), velocity(v), frame_denominator(framerate/fps){}

	void update(){
		rect.x += velocity.x;
		rect.y += velocity.y;
	}

	void render(SDL_Renderer* renderer){
		sprite_sheet.render(renderer, current_frame/frame_denominator, &rect);
		++current_frame;
	}
};

SpriteSheet load_spritesheet(SDL_Renderer* renderer,unsigned int frames, unsigned int rows, unsigned int columns, const char* path){
	//load png
	SDL_Surface* image = IMG_Load(path);
	if (image == nullptr){
		printf("failed to load image at %s\n", path);
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

struct ButtonPresses {
	bool quit = false;
	bool right_pressed = false;
	bool left_pressed = false;
	bool up_pressed = false;
	bool down_pressed = false;
};

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

void game_loop(SDL_Renderer * renderer, TexturePtr background, Character runner, Character car){
	SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
	ButtonPresses buttons;
	std::vector<SDL_Rect> collision_boxes;
	collision_boxes.emplace_back(SDL_Rect{10,10,100,100});
	collision_boxes.emplace_back(SDL_Rect{250,250,100,200});
	for (unsigned int frame =0;true;++frame){
		read_buttons(buttons);
		if (buttons.quit){
			return;
		}
		// update
		runner.velocity = {0,0};
		if (buttons.right_pressed){
			runner.velocity.x +=10;
		}
		if (buttons.left_pressed){
			runner.velocity.x -=10;
		}
		if (buttons.up_pressed){
			runner.velocity.y -=10;
		}
		if (buttons.down_pressed){
			runner.velocity.y +=10;
		}
		runner.update();
		if (runner.rect.y + runner.rect.h > SCREEN_HEIGHT){
			runner.rect.y = SCREEN_HEIGHT - runner.rect.h;
		}

		if (car.rect.x + car.rect.w < 0){
			car.rect.x = SCREEN_WIDTH;
		}
		car.update();

		// colllision check
		if (SDL_HasIntersection(&runner.rect, &car.rect) == SDL_TRUE){
			auto angle = atan2(car.rect.y - runner.rect.y, car.rect.x - runner.rect.x);
			printf("%lf", angle);
			if (angle < PI_4){
				printf(" DEAD");
			} else {
				runner.rect.y = car.rect.y - runner.rect.h;
			}
			printf("\n");
		}

		// render
		SDL_SetRenderDrawColor(renderer, 0xFF, 0xFF, 0xFF, 0xFF);
		SDL_RenderCopy(renderer, background.get(), nullptr, nullptr);
		runner.render(renderer);
		car.render(renderer);

		// collision boxes
		SDL_SetRenderDrawColor(renderer, 0xFF, 0x00, 0x00, 0xFF);
		for (auto box : collision_boxes){
			SDL_RenderDrawRect(renderer, &box);
		}
		SDL_SetRenderDrawColor(renderer, 0xFF, 0x00, 0x00, 0x20);
		for (auto box : collision_boxes){
			SDL_RenderFillRect(renderer, &box);
		}

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


	auto runner = load_spritesheet(renderer.get(),7,3,3, "runner.png");
	auto car = load_spritesheet(renderer.get(),1,1,1, "car.png");

	game_loop(
			renderer.get(),
			generate_backgroud(renderer.get(), SCREEN_WIDTH, SCREEN_HEIGHT),
			Character(std::move(runner),Point{0,0},Velocity{0,0},50,50,15),
			Character(std::move(car), Point{SCREEN_WIDTH - 100, SCREEN_HEIGHT - 100}, Velocity{-5, 0}, 200, 80, 1)
			);

	IMG_Quit();
	SDL_Quit();

	return 0;
}
