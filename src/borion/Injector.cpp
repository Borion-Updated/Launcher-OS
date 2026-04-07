#include "Injector.h"

#include "../utils/Logger.h"
#include "../utils/Utils.h"

bool Injector::downloadDLL() {
    const auto versionData = Utils::requestString("https://raw.githubusercontent.com/Borion-Updated/Releases/refs/heads/master/data.txt");

    if (versionData.empty()) {
        (void)MessageBoxA(nullptr, "Failed to retrieve version list", "Connection error", MB_OK);
        return false;
    }

    const auto split = Utils::splitStringAndDiscardEmpty(versionData, '\n');

    if (split.empty()) {
        (void)MessageBoxA(nullptr, "Received invalid version list", "Connection error", MB_OK);
        return false;
    }

    std::string relVersion{};
    std::string betaVersion{};
    std::string betaURL{};

    for (const auto& str : split) {
        const auto strs = Utils::splitStringAndDiscardEmpty(str, ' ');

        if (strs.size() < 2)
            continue;

        if (strs[0] == "rel")
            relVersion = strs[1];
        else if (strs[0] == "beta") {
            if (strs.size() != 3)
                continue;

            betaVersion = strs[1];
            betaURL = strs[2];
        }
    }

    const auto file = Utils::getLauncherFolder() + L"Borion.dll";

    if (!relVersion.empty() && !betaVersion.empty() && !betaURL.empty() && relVersion != betaVersion) {
        const auto ret = MessageBoxA(nullptr, formatF("Beta DLL for {} available.\nRelease supports {}.\nUse Beta DLL?", betaVersion, relVersion).c_str(), "Beta DLL available", MB_YESNO);

        if (ret == IDYES) {
            const auto dlRet = URLDownloadToFileW(nullptr, Utils::convertToWString(betaURL).c_str(), file.c_str(), 0, nullptr) == S_OK;

            if (!dlRet)
                (void)MessageBoxA(nullptr, "Failed to download DLL", "Connection error", MB_OK);

            return dlRet;
        }
    }

    constexpr auto latestURL = "https://github.com/Borion-Updated/Releases/releases/latest/download/Borion.dll";

    const auto dlRet = URLDownloadToFileW(nullptr, Utils::convertToWString(latestURL).c_str(), file.c_str(), 0, nullptr) == S_OK;

    if (!dlRet)
        (void)MessageBoxA(nullptr, "Failed to download DLL", "Connection error", MB_OK);

    return dlRet;
}

void Injector::inject(const std::wstring& path) {
    if (!std::filesystem::exists(path)) {
        logF("Path doesn't exist");
        (void)MessageBoxA(nullptr, "DLL file too small (has it been downloaded correctly?)", "Invalid DLL", MB_OK);
        return;
    }

    if (std::filesystem::file_size(path) < 100) {
        logF("Invalid DLL");
        (void)MessageBoxA(nullptr, "DLL file too small (has it been downloaded correctly?)", "Invalid DLL", MB_OK);
        return;
    }

    DWORD procId = 0;
    const auto hSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);

    if (hSnap != INVALID_HANDLE_VALUE) {
        PROCESSENTRY32W entry;
        entry.dwSize = sizeof(PROCESSENTRY32W);

        if (Process32FirstW(hSnap, &entry)) {
            do {
                if (!_wcsicmp(entry.szExeFile, L"Minecraft.Windows.exe")) {
                    procId = entry.th32ProcessID;
                    break;
                }
            }
            while (Process32NextW(hSnap, &entry));
        }
    }

    (void)CloseHandle(hSnap);

    if (procId == 0) {
        logF("Minecraft not running");
        (void)MessageBoxA(nullptr, "Please start Minecraft (couldn't bother to start it myself)", "Minecraft not running", MB_OK);
        return;
    }

    const auto hProc = OpenProcess(PROCESS_ALL_ACCESS, 0, procId);

    if (hProc && hProc != INVALID_HANDLE_VALUE) {
        const auto loc = VirtualAllocEx(hProc, nullptr, MAX_PATH, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);

        (void)WriteProcessMemory(hProc, loc, path.data(), path.size() * 2 + 2, nullptr);

        const auto hThread = CreateRemoteThread(hProc, nullptr, 0, reinterpret_cast<LPTHREAD_START_ROUTINE>(LoadLibraryW), loc, 0, nullptr);

        if (hThread)
            (void)CloseHandle(hThread);
        else {
            logF("Failed to create remote thread: {}", GetLastError());
            (void)MessageBoxA(nullptr, "Some random error occurred while trying to load DLL (see logs for more information)", "Failed to create remote thread", MB_OK);
        }
    }
    else {
        logF("Failed to open process: {}", GetLastError());
        (void)MessageBoxA(nullptr, "Some random error occurred while trying to access Minecraft (see logs for more information)", "Failed to open process", MB_OK);
    }

    if (hProc)
        (void)CloseHandle(hProc);
}
