#pragma once

#define NOMINMAX
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <filesystem>
#include <cstdint>
#include <chrono>
#include <span>
#include <ranges>
#include <stack>
#include <bitset>
#include <dwmapi.h>
#include <windowsx.h>
#include <tlhelp32.h>
#include <shobjidl.h>
#include <wrl.h>
#include <fcntl.h>
#include <wininet.h>
#include <gdiplus.h>
#include <shlwapi.h>

#include <battery/embed.hpp>

#include <magic_enum_all.hpp>

#include <spdlog/spdlog.h>
#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/fmt/bundled/chrono.h>

#include <gif_lib.h>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <glm/ext.hpp>

#define VK_USE_PLATFORM_WIN32_KHR
#define VK_NO_PROTOTYPES
#include <vulkan/vulkan.h>

#include "include/KHR/khrplatform.h"
#include "include/glad/glad.h"
