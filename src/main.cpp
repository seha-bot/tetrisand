#include "config.hpp"
#include "gui.hpp"
#include "simulation.hpp"
#include <SDL2/SDL_timer.h>
#include <cstdint>
#include <ctime>
#include <initializer_list>
#include <memory>
#include <vector>

static sim::Solid
generateRandomSolid(uint32_t x, uint32_t y)
{
    return { cfg::masks[rand() % cfg::masks.size()],
             cfg::maskColors[rand() % cfg::maskColors.size()],
             x,
             y };
}

// TODO this name is unclear
static void
replaceCheck(sim::SandGrid& grid, double& solidTick)
{
    if (grid.doesCurrentSolidTouchSandOrBottom()) {
        grid.convertCurrentSolidToSand();
        solidTick = -1.0;
        grid.placeSolid(generateRandomSolid(0, 0));
    }
}

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

class PlayView final : public gui::View
{
    double sandTick = 0.0;
    double solidTick = 0.0;
    sim::SandGrid grid =
      sim::SandGrid(m_window, cfg::gridWidth, cfg::gridHeight);

  public:
    void draw(double dt) noexcept override
    {
        if (m_window.isKeyDown(SDL_SCANCODE_LEFT)) {
            grid.moveCurrentSolid(sim::Direction::left);
            replaceCheck(grid, solidTick);
        }
        if (m_window.isKeyDown(SDL_SCANCODE_RIGHT)) {
            grid.moveCurrentSolid(sim::Direction::right);
            replaceCheck(grid, solidTick);
        }
        if (m_window.isKeyDown(SDL_SCANCODE_DOWN)) {
            grid.moveCurrentSolid(sim::Direction::down);
            replaceCheck(grid, solidTick);
        }
        if (m_window.isKeyDown(SDL_SCANCODE_SPACE)) {
            solidTick = sandTick = -10000;
        }
        if (m_window.isKeyDownOnce(SDL_SCANCODE_UP)) {
            grid.rotateCurrentSolid();
        }

        sandTick += dt;
        if (sandTick > 0.01) {
            sandTick -= 0.01;
            grid.updateSand();
            replaceCheck(grid, solidTick);

            const auto id = sim::getAnyAreaId(grid);
            if (id.has_value()) {
                sim::removeArea(grid, id.value());
            }
        }

        solidTick += dt;
        if (solidTick > 0.03) {
            solidTick -= 0.03;
            grid.moveCurrentSolid(sim::Direction::down);
            replaceCheck(grid, solidTick);
        }

        m_window.drawRect(0, 0, m_window.width(), m_window.height(), 0xFFFFFF);

        grid.render();
        const int32_t s = std::min(m_window.height(), m_window.width());

        const double gw = static_cast<double>(grid.width()) / grid.height();

        m_window.drawRect((m_window.width() - s) / 2,
                          (m_window.height() - s) / 2,
                          s,
                          s,
                          0xFF0000);

        m_window.drawTexture((m_window.width() - gw * s) / 2,
                             (m_window.height() - s) / 2,
                             gw * s,
                             s,
                             grid);
    }

    PlayView(gui::Window& window)
      : gui::View(window)
    {
        grid.placeSolid(generateRandomSolid(0, 0));
    }
};

class IntroView final : public gui::View
{
  public:
    void draw(double dt) noexcept override
    {
        std::cout << m_window.width() << dt;
    }

    IntroView(gui::Window& window)
      : gui::View(window)
    {
    }
};

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

int
main()
{
    std::srand(std::time(nullptr));

    gui::SDLCleaner cleaner;
    gui::Window window("tetrisand", cfg::gridWidth, cfg::gridHeight);

    Router router = { new IntroView(window), new PlayView(window) };
    router.currentRoute = Route::play;

    Runner runner(window, router);
    runner.run();
}
