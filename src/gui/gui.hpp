#ifndef GUIHPP
#define GUIHPP

#include <cstdint>
#include <raylib.h>

namespace gui {
    class Window final {
    public:
        void drawPixel(int32_t x, int32_t y, uint32_t color) noexcept;
        void drawRect(int32_t x, int32_t y, int32_t width, int32_t height, uint32_t color) noexcept;

        void flush() noexcept;
        void pollEvents() noexcept;

        // TODO map all keys to a Key struct and remove raylib.h
        // from this header
        bool isKeyDown(int key) const noexcept { return ::IsKeyDown(key); }
        bool isKeyDownOnce(int key) const noexcept { return IsKeyPressed(key); }

        bool isClosed() const noexcept;

        Window(const char *title, uint32_t width, uint32_t height);

        // No copying and moving because Window is a singleton under raylib
        Window(const Window&) = delete;
        Window& operator=(const Window&) = delete;
        Window(Window&&) = delete;
        Window& operator=(Window&&) = delete;

        ~Window();
    };
}

#endif
