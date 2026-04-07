#pragma once

#include "../../../../utils/TimeUtils.h"

class GIF {
public:
    struct PixelBlob {
        std::unique_ptr<uint8_t[]> data{};
        size_t size{};
    };

    GIF() = default;
    explicit GIF(const std::string& file);
    GIF(const std::string& name, const b::EmbedInternal::EmbeddedFile& file);

    [[nodiscard]] int getCurrentFrame();
    [[nodiscard]] int getWidth() const;
    [[nodiscard]] int getHeight() const;
    [[nodiscard]] bool isValid() const;
    [[nodiscard]] bool isLoaded() const;

protected:
    bool valid = false;
    bool loaded = false;
    std::string name{};
    millis currentTime = 0;
    int frames = 0;
    int width = 0;
    int height = 0;
    int currentFrame = 0;
    std::vector<std::pair<PixelBlob, millis>> frameData{};
};
