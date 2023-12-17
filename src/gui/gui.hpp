#ifndef GUIHPP
#define GUIHPP

#include <SDL2/SDL_scancode.h>
#include <cstdint>
#include <memory>

namespace gui {
    class SDLCleaner final {
    public:
        SDLCleaner();
        SDLCleaner(const SDLCleaner&) = delete;
        SDLCleaner& operator=(const SDLCleaner&) = delete;
        SDLCleaner(SDLCleaner&&) = delete;
        SDLCleaner& operator=(SDLCleaner&&) = delete;
        ~SDLCleaner();
    };

    class Window final {
        std::shared_ptr<struct WindowHandle> wH;
        bool isClosed_;
    public:
        void drawPixel(int32_t x, int32_t y, uint32_t color);
        void drawRect(int32_t x, int32_t y, int32_t width, int32_t height, uint32_t color);

        void flush() noexcept;
        void pollEvents() noexcept;

        bool isKeyDown(SDL_Scancode key) const noexcept;
        bool isKeyDownOnce(SDL_Scancode key) const noexcept;

        bool isClosed() const noexcept { return isClosed_; }

        Window(const char *title, uint32_t width, uint32_t height);

        Window(const Window&) = default;
        Window& operator=(const Window&) = default;
        ~Window();
    };
}

#endif
