#include "gui.hpp"
#include <SDL2/SDL.h>
#include <stdexcept>
#include <vector>

namespace gui {
    SDLCleaner::SDLCleaner() {
        if (SDL_InitSubSystem(SDL_INIT_VIDEO) < 0) {
            throw std::runtime_error(SDL_GetError());
        }
    }

    SDLCleaner::~SDLCleaner() {
        SDL_Quit();
    }

    enum KeyState { released, pressed, held };

    struct WindowHandle {
        SDL_Window *window;
        SDL_Renderer *renderer;
        int keyboardSize;
        const uint8_t *keyboard;
        std::vector<KeyState> keyStates;
    };

    static void setColor(const WindowHandle& wH, uint32_t color) {
        const uint8_t r = color >> 16 & 0xFF;
        const uint8_t g = color >> 8 & 0xFF;
        const uint8_t b = color & 0xFF;
        if (SDL_SetRenderDrawColor(wH.renderer, r, g, b, 255) < 0) {
            throw std::runtime_error(SDL_GetError());
        }
    }

    void Window::drawPixel(int32_t x, int32_t y, uint32_t color) {
        setColor(*wH, color);
        if (SDL_RenderDrawPoint(wH->renderer, x, y) < 0) {
            throw std::runtime_error(SDL_GetError());
        }
    }

    void Window::drawRect(int32_t x, int32_t y, int32_t width, int32_t height, uint32_t color) {
        setColor(*wH, color);
        SDL_Rect rect{x, y, width, height};
        if (SDL_RenderFillRect(wH->renderer, &rect) < 0) {
            throw std::runtime_error(SDL_GetError());
        }
    }

    void Window::flush() noexcept {
        SDL_RenderPresent(wH->renderer);
    }

    void Window::pollEvents() noexcept {
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                isClosed_ = true;
            }
        }

        for (int i = 0; i < wH->keyboardSize; i++) {
            if (wH->keyboard[i] && wH->keyStates[i] == KeyState::released) {
                wH->keyStates[i] = KeyState::pressed;
            } else if (!wH->keyboard[i]) {
                wH->keyStates[i] = KeyState::released;
            }
        }
    }

    bool Window::isKeyDown(SDL_Scancode key) const noexcept {
        return wH->keyboard[key];
    }

    bool Window::isKeyDownOnce(SDL_Scancode key) const noexcept {
        if (wH->keyStates[key] == KeyState::pressed) {
            wH->keyStates[key] = KeyState::held;
            return true;
        }
        return false;
    }

    Window::Window(const char *title, uint32_t width, uint32_t height)
        : wH(new WindowHandle{nullptr, nullptr, 0, nullptr, {}}), isClosed_(false) {
        wH->window = SDL_CreateWindow(title, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, width, height, 0);
        if (wH->window == nullptr) {
            throw std::runtime_error(SDL_GetError());
        }

        wH->renderer = SDL_CreateRenderer(wH->window, -1, SDL_RENDERER_SOFTWARE | SDL_RENDERER_PRESENTVSYNC);
        if (wH->renderer == nullptr) {
            throw std::runtime_error(SDL_GetError());
        }

        wH->keyboard = SDL_GetKeyboardState(&wH->keyboardSize);
        wH->keyStates.resize(wH->keyboardSize, KeyState::released);
    }

    Window::~Window() {
        SDL_DestroyRenderer(wH->renderer);
        SDL_DestroyWindow(wH->window);
    }
}
