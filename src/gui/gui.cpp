#include "gui.hpp"
#include <cstdint>
#include <raylib.h>
#include <stdexcept>

namespace gui {
    static Color colorFromUint32(uint32_t color) {
        const uint8_t r = color >> 16 & 0xFF;
        const uint8_t g = color >> 8 & 0xFF;
        const uint8_t b = color & 0xFF;
        return { r, g, b, 255 };
    }

    void Window::drawPixel(int32_t x, int32_t y, uint32_t color) noexcept {
        DrawPixel(x, y, colorFromUint32(color));
    }

    void Window::drawRect(int32_t x, int32_t y, int32_t width, int32_t height, uint32_t color) noexcept {
        DrawRectangle(x, y, width, height, colorFromUint32(color));
    }

    void Window::flush() noexcept {
        EndDrawing();
        BeginDrawing();
    }

    void Window::pollEvents() noexcept {
        PollInputEvents();
    }

    bool Window::isClosed() const noexcept {
        return WindowShouldClose();
    }

    Window::Window(const char *title, uint32_t width, uint32_t height) {
        // TODO this should really be a mutex
        static bool windowAlreadyCreated = false;

        if (windowAlreadyCreated) {
            throw std::runtime_error("can't have multiple windows");
        }

        InitWindow(width, height, title);
        if (!IsWindowReady()) {
            throw std::runtime_error("can't create a window");
        }
        windowAlreadyCreated = true;

        // Window config
        SetTargetFPS(60);
        ClearBackground({0, 0, 0, 255});
        BeginDrawing();
    }

    Window::~Window() {
        CloseWindow();
    }
}
