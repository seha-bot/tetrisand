#ifndef TEXTUREHPP
#define TEXTUREHPP

#include "grid.hpp"
#include <cstdint>
#include <fstream>
#include <functional>
#include <iostream>
#include <string>
#include <tuple>
#include <vector>

namespace gui {
    class Texture : public Grid<uint32_t> {
    public:
        Texture ror() noexcept {
            std::vector<uint32_t> textureRot(width() * height());
            for (uint32_t y = 0; y < height(); y++) {
                for (uint32_t x = 0; x < width(); x++) {
                    textureRot[y + (width() - x - 1) * height()] = at(x, y);
                }
            }
            return Texture(textureRot, height(), width());
        }

        Texture(const std::vector<uint32_t>& texture, uint32_t width, uint32_t height) noexcept
            : Grid(texture, width, height) {}
    };

    struct P3Parser final {
        static Texture parse(const std::string& pgmFilePath) {
            std::ifstream file(pgmFilePath);

            std::string buf;
            file >> buf;
            if (buf != "P6") {
                throw std::runtime_error("bad image file format");
            }

            uint32_t width, height, max;
            file >> width >> height >> max;

            auto it = std::istreambuf_iterator<char>(file >> std::ws);
            std::vector<uint32_t> pixels;

            for (size_t y = 0; y < height; y++) {
                for (size_t x = 0; x < width; x++) {
                    uint32_t r = *it++;
                    uint32_t g = *it++;
                    uint32_t b = *it++;
                    pixels.push_back(
                        (static_cast<double>(r << 16 | g << 8 | b) / max) * 255.0
                    );
                }
            }

            return Texture(pixels, width, height);
        }

        P3Parser() = delete;
    };

    class Color final {
        uint32_t color_;
    public:
        uint32_t asInt() const noexcept { return color_; }
        std::tuple<double, double, double> asDouble() const noexcept {
            return std::make_tuple(
                (color_ << 16 & 0xFF) / 255.0,
                (color_ << 8 & 0xFF) / 255.0,
                (color_ & 0xFF) / 255.0
            );
        }

        Color(uint32_t color) noexcept : color_(color) {}

        Color(std::tuple<double, double, double> color) noexcept
            : color_(static_cast<uint32_t>(std::clamp(std::get<0>(color), 0.0, 1.0) * 255.0) << 16
                   | static_cast<uint32_t>(std::clamp(std::get<1>(color), 0.0, 1.0) * 255.0) << 8
                   | static_cast<uint32_t>(std::clamp(std::get<2>(color), 0.0, 1.0) * 255.0)) {}
    };

    class PostProcessedTexture : public Texture {
        typedef std::function<Color(int32_t, int32_t, const PostProcessedTexture&)> shader;

        shader processor_;
    public:
        uint32_t at(uint32_t x, uint32_t y) const {
            if (x >= width() || y >= height()) {
                throw std::runtime_error("Texture position out of bounds");
            }
            return processor_(x, y, *this).asInt();
        }

        PostProcessedTexture ror() noexcept { return { Texture::ror(), processor_ }; }

        PostProcessedTexture(const Texture& texture, shader processor) noexcept
            : Texture(texture), processor_(processor) {}
    };
}

#endif
