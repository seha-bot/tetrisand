#ifndef MASKHPP
#define MASKHPP

#include <cstdint>
#include <fstream>
#include <string>
#include <vector>

namespace gui {
    class Mask {
        std::vector<uint8_t> mask_;
        uint32_t width_;
        uint32_t height_;
    public:
        uint32_t width() const noexcept { return width_; }
        uint32_t height() const noexcept { return height_; }

        uint8_t at(uint32_t x, uint32_t y) const {
            if (x >= width_ || y >= height_) {
                throw std::runtime_error("Mask position out of bounds");
            }
            return mask_[x + y * width_];
        }

        void ror() noexcept {
            const auto maskRot(mask_);
            for (uint32_t y = 0; y < height_; y++) {
                for (uint32_t x = 0; x < width_; x++) {
                    mask_[y + (width_ - x - 1) * height_] = maskRot[x + y * width_];
                }
            }
            std::swap(width_, height_);
        }

        void rol() noexcept {
            const auto maskRot(mask_);
            for (uint32_t y = 0; y < height_; y++) {
                for (uint32_t x = 0; x < width_; x++) {
                    mask_[(height_ - y - 1) + x * height_] = maskRot[x + y * width_];
                }
            }
            std::swap(width_, height_);
        }

        Mask(const std::vector<uint8_t>& mask, uint32_t width, uint32_t height) : mask_(mask), width_(width), height_(height) {}

        Mask(const std::string& pgmFilePath) {
            std::ifstream file(pgmFilePath);

            std::string buf;
            file >> buf;
            if (buf != "P5") {
                throw std::runtime_error("bad image file format");
            }

            file >> width_;
            file >> height_;
            file >> buf;

            auto begin = std::istreambuf_iterator<char>(file);
            std::advance(begin, 1);
            mask_.insert(mask_.begin(), begin, std::istreambuf_iterator<char>());
        }
    };
}

#endif
