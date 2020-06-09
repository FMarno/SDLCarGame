#include "Character.hpp"
#include "SDL_image.h"
#include "helpers.hpp"
#include <cmath>
#include <stdio.h>

const unsigned int framerate = 60;
const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;
const double PI_4 = atan2(1,1);

void update_runner(Character& runner, ButtonPresses& buttons){
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
	runner.rect.x += runner.velocity.x;
	runner.rect.y += runner.velocity.y;
	if (runner.rect.y + runner.rect.h > SCREEN_HEIGHT){
		runner.rect.y = SCREEN_HEIGHT - runner.rect.h;
	}
}

void update_car(Character& car){
	if (car.rect.x + car.rect.w < 0){
		car.rect.x = SCREEN_WIDTH;
	}
	car.rect.x += car.velocity.x;
	car.rect.y += car.velocity.y;
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
		update_runner(runner, buttons);
		update_car(car);

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

		SDL_RenderPresent(renderer);
		SDL_Delay(1000/framerate);
	}
}

int main(int, char*[])
{
	SDL_Init(SDL_INIT_VIDEO);
	IMG_Init(IMG_INIT_PNG);

	auto window = make_unique_window(SDL_CreateWindow(
				"SDL2Test",
				SDL_WINDOWPOS_UNDEFINED,
				SDL_WINDOWPOS_UNDEFINED,
				SCREEN_WIDTH,
				SCREEN_HEIGHT,
				0
				));
	auto renderer = make_unique_renderer(SDL_CreateRenderer(window.get(), -1, SDL_RENDERER_SOFTWARE));
	auto runner = load_spritesheet(renderer.get(),7,3,3, "runner.png");
	auto car = load_spritesheet(renderer.get(),1,1,1, "car.png");

	game_loop(
			renderer.get(),
			generate_backgroud(renderer.get(), SCREEN_WIDTH, SCREEN_HEIGHT),
			Character(std::move(runner),Point{0,0},Velocity{0,0},50,50,framerate, 15),
			Character(std::move(car), Point{SCREEN_WIDTH - 100, SCREEN_HEIGHT - 100}, Velocity{-5, 0}, 200, 80,framerate, 1)
			);

	IMG_Quit();
	SDL_Quit();
	return 0;
}
