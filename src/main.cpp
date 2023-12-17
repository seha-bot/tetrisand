#include "gui.hpp"
#include "config.hpp"
#include "simulation.hpp"
#include <SDL2/SDL_timer.h>
#include <ctime>

static sim::Solid generateRandomSolid(uint32_t x, uint32_t y) {
    return { cfg::masks[rand() % cfg::masks.size()], cfg::maskColors[rand() % cfg::maskColors.size()], x, y };
}

// TODO this name is unclear
static void replaceCheck(sim::SandGrid& grid, double& solidTick) {
    if (grid.doesCurrentSolidTouchSandOrBottom()) {
        grid.convertCurrentSolidToSand();
        solidTick = -1.0;
        grid.placeSolid(generateRandomSolid(0, 0));
    }
}

int main() {
    std::srand(std::time(nullptr));
    gui::SDLCleaner cleaner;

    gui::Window window("tetrisand", cfg::gridWidth * cfg::cellSize, cfg::gridHeight * cfg::cellSize);
    sim::SandGrid grid(cfg::gridWidth, cfg::gridHeight, cfg::cellSize);

    grid.placeSolid(generateRandomSolid(0, 0));

    double dt = 0.0;
    auto start = SDL_GetTicks();
    double sandTick = 0.0;
    double solidTick = 0.0;

    while (!window.isClosed()) {
        auto now = SDL_GetTicks();
        dt = (now - start) / 1000.0;
        start = now;

        window.pollEvents();
        if (window.isKeyDown(SDL_SCANCODE_LEFT)) {
            grid.moveCurrentSolid(sim::Direction::left);
            replaceCheck(grid, solidTick);
        }
        if (window.isKeyDown(SDL_SCANCODE_RIGHT)) {
            grid.moveCurrentSolid(sim::Direction::right);
            replaceCheck(grid, solidTick);
        }
        if (window.isKeyDown(SDL_SCANCODE_DOWN)) {
            grid.moveCurrentSolid(sim::Direction::down);
            replaceCheck(grid, solidTick);
        }
        if (window.isKeyDown(SDL_SCANCODE_SPACE)) {
            solidTick = sandTick = -10000;
        }
        if (window.isKeyDownOnce(SDL_SCANCODE_UP)) {
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

        grid.draw(window);
        window.flush();
    }
}
