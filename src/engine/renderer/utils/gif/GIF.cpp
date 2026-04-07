#include "GIF.h"

#include "../../../../utils/Utils.h"
#include "EasyGifReader.h"

GIF::GIF(const std::string& file) {
    if (!Utils::doesFileExist(Utils::convertToWString(file)))
        return;

    if (file.contains('/'))
        this->name = file.substr(file.find_last_of('/') + 1);
    else if (file.contains('\\'))
        this->name = file.substr(file.find_last_of('\\') + 1);
    else
        this->name = file;

    try {
        const auto gif = EasyGifReader::openFile(file.c_str());
        this->frames = gif.frameCount();
        this->width = gif.width();
        this->height = gif.height();

        this->frameData.resize(this->frames);

        int f = 0;
        for (const auto& frame : gif) {
            const auto framePixels = frame.pixels();

            auto data = std::make_unique<uint8_t[]>(this->width * this->height * 4);
            memcpy(data.get(), framePixels, this->width * this->height * 4);

            this->frameData[f] = {PixelBlob{std::move(data), static_cast<size_t>(this->width * this->height * 4)}, frame.duration().milliseconds()};

            f++;
        }

        this->valid = true;
        this->loaded = true;
    }
    catch (EasyGifReader::Error) {
        this->loaded = true;
    }
}

GIF::GIF(const std::string& name, const b::EmbedInternal::EmbeddedFile& file) {
    this->name = name;

    try {
        const auto gif = EasyGifReader::openMemory(file.data(), file.size());
        this->frames = gif.frameCount();
        this->width = gif.width();
        this->height = gif.height();

        this->frameData.resize(this->frames);

        int f = 0;
        for (const auto& frame : gif) {
            const auto framePixels = frame.pixels();

            auto data = std::make_unique<uint8_t[]>(this->width * this->height * 4);
            memcpy(data.get(), framePixels, this->width * this->height * 4);

            this->frameData[f] = {PixelBlob{std::move(data), static_cast<size_t>(this->width * this->height * 4)}, frame.duration().milliseconds()};

            f++;
        }

        this->valid = true;
        this->loaded = true;
    }
    catch (EasyGifReader::Error) {
        this->loaded = true;
    }
}

int GIF::getCurrentFrame() {
    const auto currTime = TimeUtils::currentTimeMillis();

    if (this->currentTime == 0)
        this->currentTime = currTime;

    if (this->currentTime <= currTime - this->frameData[this->currentFrame].second) {
        this->currentTime = currTime;

        this->currentFrame++;

        if (this->currentFrame >= this->frames)
            this->currentFrame = 0;
    }

    return this->currentFrame;
}

int GIF::getWidth() const {
    return this->width;
}

int GIF::getHeight() const {
    return this->height;
}

bool GIF::isValid() const {
    return this->valid;
}

bool GIF::isLoaded() const {
    return this->loaded;
}
