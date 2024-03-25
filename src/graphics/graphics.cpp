#include "graphics.hpp"

#include <SDL2/SDL.h>
#include <SDL2/SDL_pixels.h>
#include <SDL2/SDL_rect.h>
#include <SDL2/SDL_render.h>

#include <cstdint>
#include <memory>
#include <stdexcept>
#include <vector>

namespace gui {

static inline void sdl_error(int result) {
    if (result < 0) {
        throw std::runtime_error(SDL_GetError());
    }
}

SDLCleaner::SDLCleaner() { sdl_error(SDL_InitSubSystem(SDL_INIT_VIDEO)); }
SDLCleaner::~SDLCleaner() { SDL_Quit(); }

enum class KeyState { released, pressed, held };

struct WindowHandle {
    SDL_Window *window = nullptr;
    SDL_Renderer *renderer = nullptr;
    int keyboard_size = 0;
    const uint8_t *keyboard = nullptr;
    std::vector<KeyState> key_states;

    ~WindowHandle() {
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
    }
};

Window::Window(const char *title, uint32_t width, uint32_t height)
    : m_handle(std::make_unique<WindowHandle>()),
      m_width(width),
      m_height(height) {
    if ((m_handle->window = SDL_CreateWindow(
             title, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, width,
             height, SDL_WINDOW_RESIZABLE)) == nullptr) {
        sdl_error(-1);
    }

    if ((m_handle->renderer = SDL_CreateRenderer(
             m_handle->window, -1,
             SDL_RENDERER_SOFTWARE | SDL_RENDERER_PRESENTVSYNC)) == nullptr) {
        sdl_error(-1);
    }

    m_handle->keyboard = SDL_GetKeyboardState(&m_handle->keyboard_size);
    m_handle->key_states.resize(m_handle->keyboard_size, KeyState::released);
}

Window::~Window() {}

static inline void set_color(const WindowHandle& handle, uint32_t color) {
    const uint8_t r = color >> 16 & 0xFF;
    const uint8_t g = color >> 8 & 0xFF;
    const uint8_t b = color & 0xFF;
    sdl_error(SDL_SetRenderDrawColor(handle.renderer, r, g, b, 255));
}

void Window::draw_pixel(int32_t x, int32_t y, uint32_t color) {
    set_color(*m_handle, color);
    sdl_error(SDL_RenderDrawPoint(m_handle->renderer, x, y));
}

void Window::draw_rect(int32_t x, int32_t y, int32_t width, int32_t height,
                       uint32_t color) {
    set_color(*m_handle, color);
    SDL_Rect rect{x, y, width, height};
    sdl_error(SDL_RenderFillRect(m_handle->renderer, &rect));
}

void Window::flush() noexcept { SDL_RenderPresent(m_handle->renderer); }

void Window::poll_events() noexcept {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_QUIT) {
            m_is_open = false;
        } else if (event.type == SDL_WINDOWEVENT &&
                   event.window.event == SDL_WINDOWEVENT_RESIZED) {
            m_width = event.window.data1;
            m_height = event.window.data2;
        }
    }

    for (int i = 0; i < m_handle->keyboard_size; i++) {
        if (m_handle->keyboard[i] &&
            m_handle->key_states[i] == KeyState::released) {
            m_handle->key_states[i] = KeyState::pressed;
        } else if (!m_handle->keyboard[i]) {
            m_handle->key_states[i] = KeyState::released;
        }
    }
}

bool Window::is_key_down(SDL_Scancode key) const noexcept {
    return m_handle->keyboard[key];
}

bool Window::is_key_down_once(SDL_Scancode key) const noexcept {
    if (m_handle->key_states[key] == KeyState::pressed) {
        m_handle->key_states[key] = KeyState::held;
        return true;
    }
    return false;
}

}  // namespace gui
