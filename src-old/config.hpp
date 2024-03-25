#ifndef CONFIGHPP
#define CONFIGHPP

#include "gui/texture.hpp"
#include <cstdint>
#include <tuple>
#include <vector>

namespace cfg {
static const uint32_t gridWidth = 80;
static const uint32_t gridHeight = 180;

static const std::vector<uint32_t> maskColors(
  { 0x89FC00, 0xF5B700, 0xDC0073, 0x008BF8 });

static gui::Color
shader(int32_t x, int32_t y, const gui::PostProcessedTexture& texture)
{
    gui::Color c(texture.gui::Texture::at(x, y));
    const auto [r, g, b] = c.asDouble();
    static const double brightnessDark = 0.6;

    try {
        if (texture.gui::Texture::at(x + 1, y) == 0 ||
            texture.gui::Texture::at(x, y + 1) == 0 ||
            texture.gui::Texture::at(x + 1, y + 1) == 0) {
            c = std::make_tuple(
              r * brightnessDark, g * brightnessDark, b * brightnessDark);
        }
    } catch (...) {
        c = std::make_tuple(
          r * brightnessDark, g * brightnessDark, b * brightnessDark);
    }

    return c;
}

static const std::vector<gui::PostProcessedTexture> masks({
  { gui::P3Parser::parse("assets/mask1.ppm"), shader },
  { gui::P3Parser::parse("assets/mask2.ppm"), shader },
  { gui::P3Parser::parse("assets/mask3.ppm"), shader },
  { gui::P3Parser::parse("assets/mask4.ppm"), shader },
  { gui::P3Parser::parse("assets/mask5.ppm"), shader },
});
} // namespace cfg

#endif
