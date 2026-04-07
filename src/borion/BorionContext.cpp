#include "BorionContext.h"

#include "Injector.h"
#include "../engine/renderer/Renderer.h"
#include "../utils/Logger.h"
#include "../utils/Utils.h"

void BorionContext::render(const double deltaTime) {
    /*{
        const auto speed = 150.f * static_cast<float>(deltaTime);
        const auto width = static_cast<float>(windowSize.x) / 2.f;
        const auto scale = width / static_cast<float>(this->banner.getWidth());
        const auto height = static_cast<float>(this->banner.getHeight()) * scale;

        this->x += speed * static_cast<float>(this->horizontal * 2 - 1);
        this->y += speed * static_cast<float>(this->vertical * 2 - 1);

        if (!this->horizontal && this->x < 0.f) {
            this->horizontal = true;
            this->x = 0.f;
        }

        if (!this->vertical && this->y < 0.f) {
            this->vertical = true;
            this->y = 0.f;
        }

        if (this->horizontal && this->x >= static_cast<float>(windowSize.x) - width) {
            this->horizontal = false;
            this->x = static_cast<float>(windowSize.x) - width;
        }

        if (this->vertical && this->y >= static_cast<float>(windowSize.y) - height) {
            this->vertical = false;
            this->y = static_cast<float>(windowSize.y) - height;
        }
    }*/

    {
        constexpr auto range = 5.f;
        constexpr auto rawSpeed = range * 3.f;

        const auto speed = rawSpeed * static_cast<float>(deltaTime);

        if (this->down) {
            this->rot += speed;

            if (this->rot >= range) {
                this->rot = range;
                this->down = false;
            }
        }
        else {
            this->rot -= speed;

            if (this->rot <= -range) {
                this->rot = -range;
                this->down = true;
            }
        }
    }

    auto& stack = this->renderer->getModelMatrix();

    {
        this->renderer->setShaderColor(0.f, 0.f, 0.f, 0.75f);

        auto& mat = stack.push();

        mat = translate(mat, {0.f, 0.f, -250.f});

        this->background.draw();

        stack.pop();

        this->renderer->setShaderColor();
    }

    const auto windowSize = this->window->getWindowSize();
    const auto mousePos = this->window->getMousePos();

    {
        const float logoWidth = static_cast<float>(windowSize.x) / 2.f;
        const float scale = logoWidth / static_cast<float>(this->banner.getWidth());
        //const float logoHeight = static_cast<float>(this->banner.getHeight()) * scale;

        auto& mat = stack.push();

        mat = translate(mat, {static_cast<float>(windowSize.x) / 2.f - logoWidth / 2.f, 16.f, 0.f});
        //mat = translate(mat, {x, y, 0.f});
        mat = glm::scale(mat, {scale, scale, 1.f});

        this->banner.draw();

        stack.pop();
    }

    {
        const auto dim = this->getDownloadButtonDimensions();
        auto dlx = dim.x;
        auto dly = dim.y;
        const auto dlw = dim.z;
        const auto dlh = dim.w;

        const auto w = static_cast<float>(this->download.getWidth());
        const auto h = static_cast<float>(this->download.getHeight());
        const auto scale = dlw / w;

        constexpr auto range = 1.1f;
        constexpr auto rawSpeed = (range - 1.f) * 5.f;
        const auto speed = rawSpeed * static_cast<float>(deltaTime);

        if (this->isDownloadButtonHovered(mousePos)) {
            if (this->leftDown || this->rightDown) {
                this->dlScale -= speed * 2;

                if (this->dlScale <= 0.9f)
                    this->dlScale = 0.9f;
            }
            else {
                this->dlScale += speed;

                if (this->dlScale >= range)
                    this->dlScale = range;
            }
        }
        else {
            if (this->dlScale < 1.f) {
                this->dlScale += speed;

                if (this->dlScale >= 1.f)
                    this->dlScale = 1.f;
            }
            else {
                this->dlScale -= speed;

                if (this->dlScale <= 1.f)
                    this->dlScale = 1.f;
            }
        }

        dlx -= dlw * (this->dlScale - 1.f) / 2.f;
        dly -= dlh * (this->dlScale - 1.f) / 2.f;

        auto& mat = stack.push();

        mat = translate(mat, {dlx, dly, 0.f});
        mat = glm::scale(mat, {scale * this->dlScale, scale * this->dlScale, 1.f});

        mat = translate(mat, {w / 2.f, h / 2.f, 0.f});
        mat = rotate(mat, glm::radians(this->rot), {0.f, 0.f, 1.f});
        mat = translate(mat, {-w / 2.f, -h / 2.f, 0.f});

        this->download.draw();

        stack.pop();
    }

    {
        const auto dim = this->getCloseButtonDimensions();
        const auto dlx = dim.x;
        const auto dly = dim.y;

        constexpr auto range = 0.5f;
        const auto speed = range * deltaTime * 8.f;

        if (this->isCloseButtonHovered(mousePos)) {
            this->closeCol -= speed;

            if (this->closeCol <= 1.f - range)
                this->closeCol = 1.f - range;
        }
        else {
            this->closeCol += speed;

            if (this->closeCol >= 1.f)
                this->closeCol = 1.f;
        }

        this->renderer->setShaderColor(this->closeCol, this->closeCol, this->closeCol);

        auto& mat = stack.push();

        mat = translate(mat, {dlx, dly, 0.f});
        mat = scale(mat, {0.5f, 0.5f, 1.f});

        this->close.draw();

        stack.pop();

        this->renderer->setShaderColor();
    }

    {
        constexpr auto off = 4.f;
        const auto w = static_cast<float>(this->kirby.getWidth());
        const auto h = static_cast<float>(this->kirby.getHeight());
        const auto x = static_cast<float>(windowSize.x) - w - off * 2.f;
        const auto y = static_cast<float>(windowSize.y) - h - off;

        auto& mat = stack.push();

        mat = translate(mat, {off, y, 0.f});

        this->kirby.draw();

        mat = translate(mat, {x, 0.f, 0.f});

        this->kirby.draw();

        stack.pop();
    }
}

bool BorionContext::onClick(const glm::ivec2& pos, const int button) {
    if (button == 0 || button == 1) {
        if (this->isDownloadButtonHovered(pos)) {
            if (button == 0)
                this->leftDown = true;
            else
                this->rightDown = true;
            return true;
        }

        if (this->isCloseButtonHovered(pos)) {
            (void)PostMessageW(this->window->getHWND(), WM_CLOSE, 0, 0);
            return true;
        }
    }

    return false;
}

bool BorionContext::onRelease(const glm::ivec2& pos, const int button) {
    if (button == 0 || button == 1) {
        if (this->leftDown || this->rightDown) {
            if (this->isDownloadButtonHovered(pos) && !this->injecting) {
                if (button == 0) {
                    this->injecting = true;

                    std::thread t{[&] {
                        if (Injector::downloadDLL())
                            Injector::inject(Utils::getLauncherFolder() + L"Borion.dll");

                        this->injecting = false;
                    }};
                    t.detach();
                }
                else {
                    Microsoft::WRL::ComPtr<IFileOpenDialog> dialog{};

                    if (FAILED(CoCreateInstance(CLSID_FileOpenDialog, nullptr, CLSCTX_ALL, IID_PPV_ARGS(&dialog))))
                        logF("Failed to create file open dialog");
                    else {
                        COMDLG_FILTERSPEC spec[] = {
                            {L"dll", L"*.dll"}
                        };

                        (void)dialog->SetFileTypes(_countof(spec), spec);

                        if (SUCCEEDED(dialog->Show(this->window->getHWND()))) {
                            Microsoft::WRL::ComPtr<IShellItem> item{};

                            if (FAILED(dialog->GetResult(&item)))
                                logF("Failed to get result");
                            else {
                                PWSTR path{};
                                if (FAILED(item->GetDisplayName(SIGDN_FILESYSPATH, &path)))
                                    logF("Failed to get file path");
                                else {
                                    this->injecting = true;

                                    this->tmpPath = std::wstring{path};

                                    std::thread t{[&] {
                                        Injector::inject(this->tmpPath);
                                        this->injecting = false;
                                    }};
                                    t.detach();

                                    CoTaskMemFree(path);
                                }
                            }
                        }
                    }
                }

                this->leftDown = false;
                this->rightDown = false;
            }
        }

        if (button == 0)
            this->leftDown = false;
        else
            this->rightDown = false;
    }

    return false;
}

void BorionContext::init() {
    if (this->renderer == nullptr)
        return;

    this->background.load(b::embed<"assets/background.png">(), this->renderer);
    this->banner.load(b::embed<"assets/banner.png">(), this->renderer);
    this->download.load(b::embed<"assets/download.png">(), this->renderer);
    this->close.load(b::embed<"assets/close.png">(), this->renderer);

    this->kirby = AnimatedTexture{"kirby", b::embed<"assets/kirby.gif">()};
    this->kirby.generate(this->renderer);
}

void BorionContext::exit() {
    this->background.cleanup();
    this->banner.cleanup();
    this->download.cleanup();
    this->close.cleanup();

    this->kirby.cleanup();
}

glm::vec4 BorionContext::getDownloadButtonDimensions() const {
    const auto windowSize = this->window->getWindowSize();

    const auto w = static_cast<float>(this->download.getWidth());
    const auto h = static_cast<float>(this->download.getHeight());
    const auto downloadWidth = static_cast<float>(windowSize.x) / 3.f;
    const auto scale = downloadWidth / w;
    const auto downloadHeight = h * scale;
    const auto dlx = static_cast<float>(windowSize.x) / 2.f - downloadWidth / 2.f;
    const auto dly = static_cast<float>(windowSize.y) - downloadHeight - 35.f;

    return {dlx, dly, downloadWidth, downloadHeight};
}

bool BorionContext::isDownloadButtonHovered(const glm::ivec2& mousePos) const {
    return isHovered(this->getDownloadButtonDimensions(), mousePos);
}

glm::vec4 BorionContext::getCloseButtonDimensions() const {
    const auto windowSize = this->window->getWindowSize();

    constexpr auto off = 12.f;
    const auto w = static_cast<float>(this->close.getWidth());
    const auto h = static_cast<float>(this->close.getHeight());
    const auto closeWidth = w / 2.f;
    const auto closeHeight = h / 2.f;
    const auto dlx = static_cast<float>(windowSize.x) - closeWidth - off;
    constexpr auto dly = off;

    return {dlx, dly, closeWidth, closeHeight};
}

bool BorionContext::isCloseButtonHovered(const glm::ivec2& mousePos) const {
    return isHovered(this->getCloseButtonDimensions(), mousePos);
}

bool BorionContext::isHovered(const glm::vec4& dim, const glm::ivec2& mousePos) {
    const auto dlx = dim.x;
    const auto dly = dim.y;
    const auto dlw = dim.z;
    const auto dlh = dim.w;

    return static_cast<float>(mousePos.x) >= dlx && static_cast<float>(mousePos.x) < dlx + dlw && static_cast<float>(mousePos.y) >= dly && static_cast<float>(mousePos.y) < dly + dlh;
}
