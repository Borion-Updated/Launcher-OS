#pragma once

#ifndef formatF
#define formatF(fmt, ...) Utils::format(fmt, ##__VA_ARGS__)
#endif

class Utils {
public:
    static const std::wstring& getLocalAppdataFolder();
    static const std::wstring& getLauncherFolder();

    static bool doesFileExist(const std::wstring& path);
    static bool doesFolderExist(const std::wstring& path);

    static void createFolder(const std::wstring& path);

    static std::wstring convertToWString(const std::string& str);

    static std::string readBin(const std::wstring& path);

    static void createFile(const std::wstring& path);

    static std::string requestString(const std::string& url);

    static std::vector<std::string> splitStringAndDiscardEmpty(const std::string& str, char delimiter);

    template <typename... Args>
    static std::string format(const std::string& fmt, Args... args) {
        return fmt::vformat(fmt, fmt::make_format_args(args...));
    }

    static HICON convertToHICON(const void* data, size_t size);
};
