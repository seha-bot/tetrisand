#ifndef SIMULATIONHPP
#define SIMULATIONHPP

#include "grid.hpp"
#include "gui.hpp"
#include "texture.hpp"
#include <cstdint>
#include <optional>

namespace sim {

enum class GrainState
{
    empty,
    solid,
    sand
};

struct Grain
{
    GrainState state;
    uint8_t mask;
    uint32_t color;

    static Grain empty() { return { GrainState::empty, 0, 0 }; }
};

struct Solid
{
    gui::PostProcessedTexture texture;
    uint32_t color;
    uint32_t x;
    uint32_t y;
};

enum class Direction
{
    left,
    right,
    up,
    down
};

class SandGrid
  : public gui::Grid<Grain>
  , public gui::Drawable
{
    std::optional<Solid> currentSolid;

  public:
    // TODO this function is inefficient.
    // The texture doesn't change every frame, so this
    // function doesn't need to be constantly called.
    // Instead, update the texture only when the grid changes.
    void render() noexcept;

    void updateSand() noexcept;

    void placeSolid(const Solid& solid);
    void removeCurrentSolid();
    void moveCurrentSolid(Direction direction);
    void rotateCurrentSolid();

    bool doesCurrentSolidTouchSandOrBottom() const;
    void convertCurrentSolidToSand();

    SandGrid(gui::Window& window, uint32_t width, uint32_t height)
      : Grid(width, height, Grain::empty())
      , gui::Drawable(window, width, height)
    {
    }
};

std::optional<uint32_t>
getAnyAreaId(const SandGrid& grid) noexcept;

void
removeArea(SandGrid& grid, uint32_t id);

} // namespace sim

#endif
