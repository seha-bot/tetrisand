#include "simulation.hpp"
#include <set>
#include <stdexcept>

namespace sim {
    void SandGrid::draw(gui::Window& window) const noexcept {
        for (uint32_t y = 0; y < height(); y++) {
            for (uint32_t x = 0; x < width(); x++) {
                const auto& cell = at(x, y);
                const auto color = cell.state == GrainState::empty ? 0 : cell.color;

                const auto r = (color >> 16 & 0xFF) * cell.mask / 255;
                const auto g = (color >> 8 & 0xFF) * cell.mask / 255;
                const auto b = (color & 0xFF) * cell.mask / 255;

                window.drawRect(x * cellSize_, y * cellSize_, cellSize_, cellSize_, r << 16 | g << 8 | b);
            }
        }
    }

    void SandGrid::updateSand() noexcept {
        for (uint32_t y = height() - 1; y != static_cast<uint32_t>(-1); y--) {
            for (uint32_t x = 0; x < width(); x++) {
                auto& cell = at(x, y);
                if (cell.state != GrainState::sand) {
                    continue;
                }

                gui::CellNeighbours others(*this, x, y);
                if (others.bottom == nullptr) {
                    continue;
                }

                const bool goDown = std::rand() & 1;
                if (!goDown) {
                    continue;
                }

                if (others.bottom->state == GrainState::empty) {
                    *others.bottom = cell;
                    cell.state = GrainState::empty;
                } else if (others.left != nullptr && others.left->state == GrainState::empty && others.bottomLeft->state == GrainState::empty) {
                    *others.bottomLeft = cell;
                    cell.state = GrainState::empty;
                } else if (others.right != nullptr && others.right->state == GrainState::empty && others.bottomRight->state == GrainState::empty) {
                    *others.bottomRight = cell;
                    cell.state = GrainState::empty;
                    x++;
                }
            }
        }
    }

    void SandGrid::placeSolid(const Solid& solid) {
        currentSolid_.emplace(solid);

        for (uint32_t y = 0; y < solid.texture.height(); y++) {
            for (uint32_t x = 0; x < solid.texture.width(); x++) {
                const uint8_t pixel = solid.texture.at(x, y) & 0xFF;
                if (pixel == 0) {
                    continue;
                }

                auto& grain = at(solid.x + x, solid.y + y);
                if (grain.state != GrainState::empty) {
                    removeCurrentSolid();
                    throw std::runtime_error("trying to draw over a non-empty grain");
                }

                grain = {GrainState::solid, pixel, solid.color};
            }
        }
    }

    void SandGrid::removeCurrentSolid() {
        if (!currentSolid_.has_value()) {
            throw std::runtime_error("trying to remove a solid when there are none");
        }

        for (uint32_t y = 0; y < currentSolid_->texture.height(); y++) {
            for (uint32_t x = 0; x < currentSolid_->texture.width(); x++) {
                if (currentSolid_->texture.at(x, y) == 0) {
                    continue;
                }
                at(currentSolid_->x + x, currentSolid_->y + y) = Grain::empty();
            }
        }
        currentSolid_.reset();
    }

    void SandGrid::moveCurrentSolid(Direction direction) {
        if (!currentSolid_.has_value()) {
            throw std::runtime_error("trying to move a solid when there are none");
        }

        auto newX = currentSolid_->x;
        auto newY = currentSolid_->y;

        switch (direction) {
            case Direction::left:
                if (currentSolid_->x != 0) {
                    newX--;
                }
            break;
            case Direction::right:
                if (currentSolid_->x + currentSolid_->texture.width() != width()) {
                    newX++;
                }
            break;
            case Direction::up:
                if (currentSolid_->y != 0) {
                    newY--;
                }
            break;
            case Direction::down:
                if (currentSolid_->y + currentSolid_->texture.height() != height()) {
                    newY++;
                }
            break;
        }

        auto solid = currentSolid_.value();
        removeCurrentSolid();
        solid.x = newX;
        solid.y = newY;
        placeSolid(solid);
    }

    static bool doesSolidFit(const SandGrid& grid, const Solid& solid) noexcept {
        for (uint32_t y = 0; y < solid.texture.height(); y++) {
            for (uint32_t x = 0; x < solid.texture.width(); x++) {
                if (solid.texture.at(x, y) == 0) {
                    continue;
                }

                if (solid.x + x >= grid.width() || solid.y + y >= grid.height()) {
                    return false;
                }

                if (grid.at(solid.x + x, solid.y + y).state == GrainState::sand) {
                    return false;
                }
            }
        }

        return true;
    }

    void SandGrid::rotateCurrentSolid() {
        if (!currentSolid_.has_value()) {
            throw std::runtime_error("Trying to check a solid when there are none");
        }

        auto solid = currentSolid_.value();
        solid.texture = solid.texture.ror();

        if (doesSolidFit(*this, solid)) {
            removeCurrentSolid();
            placeSolid(solid);
        }
    }

    bool SandGrid::doesCurrentSolidTouchSandOrBottom() const {
        if (!currentSolid_.has_value()) {
            throw std::runtime_error("trying to check a solid when there are none");
        }

        if (currentSolid_->y + currentSolid_->texture.height() == height()) {
            return true;
        }

        for (uint32_t y = 0; y < currentSolid_->texture.height(); y++) {
            for (uint32_t x = 0; x < currentSolid_->texture.width(); x++) {
                if (currentSolid_->texture.at(x, y) == 0) {
                    continue;
                }
                // const_cast is safe here because data is never modified
                const gui::CellNeighbours others(const_cast<SandGrid&>(*this), currentSolid_->x + x, currentSolid_->y + y);

                if ((others.left != nullptr && others.left->state == GrainState::sand)
                    || (others.right != nullptr && others.right->state == GrainState::sand)
                    || (others.top != nullptr && others.top->state == GrainState::sand)
                    || (others.bottom != nullptr && others.bottom->state == GrainState::sand)) {
                    return true;
                }
            }
        }
        return false;
    }

    void SandGrid::convertCurrentSolidToSand() {
        if (!currentSolid_.has_value()) {
            throw std::runtime_error("trying to convert a solid when there are none");
        }

        for (uint32_t y = 0; y < currentSolid_->texture.height(); y++) {
            for (uint32_t x = 0; x < currentSolid_->texture.width(); x++) {
                if (currentSolid_->texture.at(x, y) == 0) {
                    continue;
                }
                at(currentSolid_->x + x, currentSolid_->y + y).state = GrainState::sand;
            }
        }
    }

    static bool doesAreaHitRightBorder(const SandGrid& grid, uint32_t color, uint32_t x, uint32_t y, std::set<std::pair<uint32_t, uint32_t>>& checked) noexcept {
        if (x >= grid.width() || y >= grid.height()) {
            return false;
        }

        const auto& cell = grid.at(x, y);
        if (checked.insert({x, y}).second == false || cell.state != GrainState::sand || cell.color != color) {
            return false;
        }

        return x == grid.width() - 1
            || doesAreaHitRightBorder(grid, color, x + 1, y, checked)
            || doesAreaHitRightBorder(grid, color, x, y + 1, checked)
            || doesAreaHitRightBorder(grid, color, x, y - 1, checked)
            || doesAreaHitRightBorder(grid, color, x - 1, y - 1, checked)
            || doesAreaHitRightBorder(grid, color, x + 1, y - 1, checked)
            || doesAreaHitRightBorder(grid, color, x - 1, y + 1, checked)
            || doesAreaHitRightBorder(grid, color, x + 1, y + 1, checked);
    }

    std::optional<uint32_t> getAnyAreaId(const SandGrid& grid) noexcept {
        std::optional<uint32_t> currentColor;

        for (uint32_t y = 0; y < grid.height(); y++) {
            const auto& cell = grid.at(0, y);
            if (cell.state != GrainState::sand || currentColor == cell.color) {
                continue;
            }

            std::set<std::pair<uint32_t, uint32_t>> checked;
            if (doesAreaHitRightBorder(grid, cell.color, 0, y, checked)) {
                return y;
            }

            currentColor.emplace(cell.color);
        }

        return std::nullopt;
    }

    static void removeAreaRecursive(SandGrid& grid, uint32_t color, uint32_t x, uint32_t y) noexcept {
        if (x >= grid.width() || y >= grid.height()) {
            return;
        }

        auto& cell = grid.at(x, y);
        if (cell.state == GrainState::empty || cell.color != color) {
            return;
        }
        cell.state = GrainState::empty;

        removeAreaRecursive(grid, color, x + 1, y);
        removeAreaRecursive(grid, color, x, y + 1);
        removeAreaRecursive(grid, color, x, y - 1);
        removeAreaRecursive(grid, color, x - 1, y - 1);
        removeAreaRecursive(grid, color, x + 1, y - 1);
        removeAreaRecursive(grid, color, x - 1, y + 1);
        removeAreaRecursive(grid, color, x + 1, y + 1);
    }

    void removeArea(SandGrid& grid, uint32_t id) {
        const auto& cell = grid.at(0, id);
        if (cell.state != GrainState::sand) {
            throw std::runtime_error("Trying to remove a non-sand area");
        }

        removeAreaRecursive(grid, cell.color, 0, id);
    }
}
