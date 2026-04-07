#include "borion/BorionContext.h"
#include "engine/renderer/Renderer.h"
#include "engine/window/Win32Window.h"
#include "utils/Logger.h"
#include "utils/Utils.h"

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR pCmdLine, int nCmdShow) {
    Utils::createFolder(Utils::getLauncherFolder());

    Logger::clearLogs();
    Logger::initializeLogger();

    if (FAILED(CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE))) {
        logF("Failed to initialize COM library");
        return 0;
    }

    const auto window = std::make_shared<Win32Window>("Borion");
    const auto context = std::make_shared<BorionContext>(window);

    // This architecture isn't nice tbh, but it gets the job done
    if (Utils::doesFileExist(Utils::getLauncherFolder() + L"opengl.txt")) {
        const auto renderer = Renderer::create(Renderer::Type::OpenGL, window, context);

        window->setRenderer(renderer);
        context->setRenderer(renderer);

        window->create(hInstance);

        window->show();

        window->run();

        window->destroy();
    }
    else {
        auto renderer = Renderer::create(Renderer::Type::Vulkan, window, context);

        window->setRenderer(renderer);
        context->setRenderer(renderer);

        if (!window->create(hInstance)) {
            window->destroy();

            logF("Failed to create Vulkan context, falling back to OpenGL");

            Utils::createFile(Utils::getLauncherFolder() + L"opengl.txt");

            renderer = Renderer::create(Renderer::Type::OpenGL, window, context);

            window->setRenderer(renderer);
            context->setRenderer(renderer);

            window->create(hInstance);
        }

        window->show();

        window->run();

        window->destroy();
    }

    CoUninitialize();

    return 0;
}
