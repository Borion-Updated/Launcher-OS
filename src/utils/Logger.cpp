#include "Logger.h"

#include "Utils.h"

void Logger::initializeLogger() {
    logger = spdlog::basic_logger_mt("launcher_logger", Utils::getLauncherFolder() + L"logs.txt");
    logger->set_pattern("[%Y-%m-%d %H:%M:%S.%e] %v");
    logger->flush_on(spdlog::level::info);
}

void Logger::clearLogs() {
    std::ofstream logfile(Utils::getLauncherFolder() + L"logs.txt", std::ios::trunc);
    logfile.close();
}

std::shared_ptr<spdlog::logger> Logger::logger = nullptr;
