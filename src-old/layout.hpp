#include "gui.hpp"
#include <SDL2/SDL_timer.h>
#include <cstdint>
#include <memory>
#include <vector>

namespace gui {
class View
{
  protected:
    gui::Window& m_window;

  public:
    virtual void draw(double dt) noexcept = 0;
    View(gui::Window& window)
      : m_window(window)
    {
    }
    virtual ~View() {}
};
} // namespace gui

class Router final
{
    std::vector<std::unique_ptr<gui::View>> m_routes;

  public:
    uint8_t currentRoute = 0;

    gui::View& currentView() { return *m_routes.at(currentRoute).get(); }

    Router(std::initializer_list<gui::View *> routes) noexcept
    {
        for (auto route : routes) {
            m_routes.push_back(std::unique_ptr<gui::View>(route));
        }
    }
};

class Runner final
{
    gui::Window& m_window;
    Router& m_router;
    double dt = 0.0;
    uint32_t start = SDL_GetTicks();

  public:
    void run()
    {
        while (!m_window.isClosed()) {
            auto now = SDL_GetTicks();
            dt = (now - start) / 1000.0;
            start = now;

            m_window.pollEvents();
            m_router.currentView().draw(dt);
            m_window.flush();
        }
    }

    Runner(gui::Window& window, Router& router)
      : m_window(window)
      , m_router(router)
    {
    }
};

namespace Route {
enum Route : uint8_t
{
    intro,
    play,
    gameover
};
}
