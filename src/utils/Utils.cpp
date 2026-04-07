#include "Utils.h"

const std::wstring& Utils::getLocalAppdataFolder() {
    static std::wstring path;

    if (path.empty()) {
        wchar_t buf[MAX_PATH]{};
        const auto res = GetEnvironmentVariableW(L"LOCALAPPDATA", buf, MAX_PATH);

        if (res > 0)
            path = std::wstring(buf, res) + L"\\";
    }

    return path;
}

const std::wstring& Utils::getLauncherFolder() {
    static auto path = getLocalAppdataFolder() + L"BorionLauncher\\";

    return path;
}

bool Utils::doesFileExist(const std::wstring& path) {
    if (!std::filesystem::exists(path))
        return false;

    return std::filesystem::is_regular_file(path);
}

bool Utils::doesFolderExist(const std::wstring& path) {
    if (!std::filesystem::exists(path))
        return false;

    return std::filesystem::is_directory(path);
}

void Utils::createFolder(const std::wstring& path) {
    if (doesFolderExist(path))
        return;

    std::filesystem::create_directory(path);
}

std::wstring Utils::convertToWString(const std::string& str) {
    const int reqSize = MultiByteToWideChar(CP_UTF8, 0, str.c_str(), static_cast<int>(str.size()), nullptr, 0);
    std::wstring wstr(reqSize, 0);
    MultiByteToWideChar(CP_UTF8, 0, str.c_str(), static_cast<int>(str.size()), &wstr[0], reqSize);
    return wstr;
}

std::string Utils::readBin(const std::wstring& path) {
    if (!std::filesystem::exists(path))
        return "";

    std::ifstream file;
    file.open(path, std::ios::binary);

    if (!file.is_open())
        return "";

    std::ostringstream result;

    result << file.rdbuf();

    return result.str();
}

void Utils::createFile(const std::wstring& path) {
    FILE* f;
    _wfopen_s(&f, path.c_str(), L"w");
    fclose(f);
}

std::string Utils::requestString(const std::string& url) {
    const auto handle = InternetOpenA("Mozilla/4.0 (compatible)", INTERNET_OPEN_TYPE_DIRECT, nullptr, nullptr, 0);

    if (handle) {
        std::string str;

        const auto urlHandle = InternetOpenUrlA(handle, url.c_str(), nullptr, 0, 0, 0);

        if (urlHandle) {
            char buf[1024]{};
            DWORD dataRead;

            do {
                InternetReadFile(urlHandle, buf, sizeof(buf), &dataRead);
                str.append(buf, dataRead);
                memset(buf, 0, sizeof(buf));
            }
            while (dataRead > 0);

            InternetCloseHandle(urlHandle);
        }

        InternetCloseHandle(handle);
        return str;
    }

    return "";
}

std::vector<std::string> Utils::splitStringAndDiscardEmpty(const std::string& str, char delimiter) {
    return str
      | std::views::split(delimiter)
      | std::views::filter([](auto&& token) { return !token.empty(); })
      | std::views::transform([](auto&& token) { return std::string{token.begin(), token.end()}; })
      | std::ranges::to<std::vector>();
}

HICON Utils::convertToHICON(const void* data, const size_t size) {
    const Gdiplus::GdiplusStartupInput input;
    ULONG_PTR token;

    if (GdiplusStartup(&token, &input, nullptr) != Gdiplus::Ok)
        return nullptr;

    const auto stream = SHCreateMemStream(static_cast<const BYTE*>(data), size);

    HICON icon = nullptr;
    const auto bmp = Gdiplus::Bitmap::FromStream(stream, FALSE);

    if (bmp != nullptr && bmp->GetLastStatus() == Gdiplus::Ok)
        bmp->GetHICON(&icon);

    delete bmp;
    (void)stream->Release();

    Gdiplus::GdiplusShutdown(token);
    return icon;
}
