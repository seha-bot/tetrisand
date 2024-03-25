#include "config.hpp"
#include "gui.hpp"
#include "layout.hpp"
#include "simulation.hpp"
#include "texture.hpp"
#include <SDL2/SDL_timer.h>
#include <array>
#include <cstddef>
#include <cstdint>
#include <ctime>
#include <string>
#include <sys/types.h>
#include <vector>

class ScoreBoard final : public gui::Drawable
{
  public:
    int score = 0;
    std::array<gui::Texture, 10> digits = {
        gui::P3Parser::parse("assets/0.ppm"),
        gui::P3Parser::parse("assets/1.ppm"),
        gui::P3Parser::parse("assets/2.ppm"),
        gui::P3Parser::parse("assets/3.ppm"),
        gui::P3Parser::parse("assets/4.ppm"),
        gui::P3Parser::parse("assets/5.ppm"),
        gui::P3Parser::parse("assets/6.ppm"),
        gui::P3Parser::parse("assets/7.ppm"),
        gui::P3Parser::parse("assets/8.ppm"),
        gui::P3Parser::parse("assets/9.ppm")
    };

    void render()
    {
        const auto s = std::to_string(score);

        gui::DrawCleaner cleaner(get());

        for (size_t i = 0; i < s.size(); i++) {
            const auto d = digits[s[i] - '0'];
            for (uint32_t y = 0; y < d.height(); y++) {
                for (uint32_t x = 0; x < d.width(); x++) {
                    auto [r, g, b] = gui::Color(d.at(x, y)).asDouble();
                    cleaner.setPixel(x + 8 * i, y, r * 255, g * 255, b * 255);
                }
            }
        }
    }

    ScoreBoard(gui::Window& window, uint32_t width, uint32_t height)
      : Drawable(window, width, height)
    {
    }
};

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

class PlayView final : public gui::View
{
    double sandTick = 0.0;
    double solidTick = 0.0;
    sim::SandGrid grid =
      sim::SandGrid(m_window, cfg::gridWidth, cfg::gridHeight);
    ScoreBoard score =
      ScoreBoard(m_window, 8 * 5, 8);

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
            score.score++;
            solidTick -= 0.03;
            grid.moveCurrentSolid(sim::Direction::down);
            replaceCheck(grid, solidTick);
        }

        grid.render();
        score.render();
        const int32_t s = std::min(m_window.height(), m_window.width());

        const int32_t gw =
          s * static_cast<double>(grid.width()) / grid.height();
        const int32_t gh = s;

        const int32_t nextBoxSize = 0.2 * s;

        // Background
        m_window.drawRect(0, 0, m_window.width(), m_window.height(), 0xFFFFFF);

        // Next shape
        m_window.drawRect(m_window.width() - nextBoxSize - 0.03 * s,
                          m_window.height() - nextBoxSize - 0.7 * s,
                          nextBoxSize,
                          nextBoxSize,
                          0x00FF00);

        // Next color
        m_window.drawRect(m_window.width() - nextBoxSize - 0.265 * s,
                          m_window.height() - nextBoxSize - 0.7 * s,
                          nextBoxSize,
                          nextBoxSize,
                          0x00FF00);

        // Game
        m_window.drawTexture((m_window.width() - gw) * 0.1,
                             (m_window.height() - gh) / 2,
                             gw,
                             gh,
                             grid);

        // Score
        m_window.drawTexture(gw + (m_window.width() - gw) * 0.1,
                          m_window.height() - 0.4 * s,
                          m_window.width() - gw - (m_window.width() - gw) * 0.1,
                          nextBoxSize,
                          score);
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
