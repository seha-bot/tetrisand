#ifndef GRAPHICSHPP
#define GRAPHICSHPP

#include <SDL2/SDL_scancode.h>

#include <cstdint>
#include <memory>

namespace gui {

struct SDLCleaner final {
    SDLCleaner();
    SDLCleaner(const SDLCleaner&) = delete;
    SDLCleaner& operator=(const SDLCleaner&) = delete;
    SDLCleaner(SDLCleaner&&) = delete;
    SDLCleaner& operator=(SDLCleaner&&) = delete;
    ~SDLCleaner();
};

class Window final {
    std::unique_ptr<struct WindowHandle> m_handle;
    bool m_is_open = true;
    uint32_t m_width;
    uint32_t m_height;

public:
    Window(const char *title, uint32_t width, uint32_t height);

    Window(const Window&) = delete;
    Window operator=(const Window&) = delete;
    ~Window();

    void draw_pixel(int32_t x, int32_t y, uint32_t color);
    void draw_rect(int32_t x, int32_t y, int32_t width, int32_t height,
                   uint32_t color);

    void flush() noexcept;
    void poll_events() noexcept;

    bool is_key_down(SDL_Scancode key) const noexcept;
    bool is_key_down_once(SDL_Scancode key) const noexcept;

    bool is_open() const noexcept { return m_is_open; }
    uint32_t width() const noexcept { return m_width; }
    uint32_t height() const noexcept { return m_height; }
};

}  // namespace gui

#endif
