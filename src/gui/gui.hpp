#ifndef GUIHPP
#define GUIHPP

#include <SDL2/SDL_scancode.h>

#include <cstdint>
#include <memory>

namespace gui {
class SDLCleaner final
{
  public:
    SDLCleaner();
    SDLCleaner(const SDLCleaner&) = delete;
    SDLCleaner& operator=(const SDLCleaner&) = delete;
    SDLCleaner(SDLCleaner&&) = delete;
    SDLCleaner& operator=(SDLCleaner&&) = delete;
    ~SDLCleaner();
};

struct TextureHandle;
class DrawCleaner final
{
    TextureHandle& tH;
    uint32_t* pixels;

  public:
    void setPixel(int32_t x, int32_t y, uint32_t color);

    DrawCleaner(TextureHandle& texture);
    ~DrawCleaner();
};

class Drawable
{
    std::unique_ptr<TextureHandle, void (*)(TextureHandle*)> tH;

  public:
    TextureHandle& get() { return *tH.get(); }
    Drawable(class Window& window, uint32_t width, uint32_t height);
    virtual ~Drawable() = 0;
};

struct WindowHandle;

class Window final
{
    std::unique_ptr<WindowHandle> wH;
    bool m_isClosed = false;
    uint32_t m_width;
    uint32_t m_height;

  public:
    WindowHandle& get() { return *wH.get(); }
    void drawPixel(int32_t x, int32_t y, uint32_t color);
    void drawRect(int32_t x,
                  int32_t y,
                  int32_t width,
                  int32_t height,
                  uint32_t color);
    void drawTexture(int32_t x,
                     int32_t y,
                     int32_t width,
                     int32_t height,
                     Drawable& drawable);

    void flush() noexcept;
    void pollEvents() noexcept;

    bool isKeyDown(SDL_Scancode key) const noexcept;
    bool isKeyDownOnce(SDL_Scancode key) const noexcept;

    bool isClosed() const noexcept { return m_isClosed; }
    uint32_t width() const noexcept { return m_width; }
    uint32_t height() const noexcept { return m_height; }

    Window(const char* title, uint32_t width, uint32_t height);

    Window(const Window&) = delete;
    Window& operator=(const Window&) = delete;
    ~Window();
};
} // namespace gui

#endif
