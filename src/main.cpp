// #include "graphics.hpp"
// int main() {
//     gui::Window window("Tetrisand", 800, 800);
//
//     while (window.is_open()) {
//         window.poll_events();
//     }
// }

#include <SDL_error.h>
#include <SDL_events.h>
#include <SDL_render.h>
#include <stdlib.h>

#include <iostream>

#include "kiss_sdl.h"

int main() {
    kiss_array objects;
    char title[] = "Tetrisand";
    auto renderer = kiss_init(title, &objects, 800, 800);
    if (renderer == nullptr) {
        std::cout << SDL_GetError() << std::endl;
        return EXIT_FAILURE;
    }

    kiss_window window;
    kiss_window_new(&window, nullptr, 1, 0, 0, kiss_screen_width,
                    kiss_screen_height);

    kiss_label label;
    char label_text[] = "Please don't crash my system";
    kiss_label_new(
        &label, &window, label_text,
        kiss_screen_width / 2 - (150 + kiss_up.w - kiss_edge) / 2 + kiss_edge,
        6 * kiss_textfont.lineheight);

    window.visible = 1;

    SDL_Event e;
    int draw = 1, quit = 0;
    while (!quit) {
        SDL_Delay(10);
        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT) quit = 1;

            kiss_window_event(&window, &e, &draw);
        }

        if (!draw) continue;

        SDL_RenderClear(renderer);
        kiss_window_draw(&window, renderer);
        kiss_label_draw(&label, renderer);
        SDL_RenderPresent(renderer);
        draw = 0;
    }

    kiss_clean(&objects);
}
