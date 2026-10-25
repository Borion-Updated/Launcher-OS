#include "borion/BorionContext.h"
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

    const auto window = std::make_shared<Win32Window>(L"Borion");
    const auto context = std::make_shared<BorionContext>(hInstance);

    if (!window->execute(hInstance, context))
        logF("Failed to create Vulkan, OpenGL and DirectX 9 contexts");

    CoUninitialize();

    return 0;
}
