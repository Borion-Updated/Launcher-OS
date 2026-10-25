#pragma once

#include "../../../utils/Logger.h"

#ifndef DX_CHECK
#define DX_CHECK(func, ...) ([&] { \
        const auto res = func(__VA_ARGS__); \
        if (FAILED(res)) \
            logF("{}: {}", #func, res); \
    })()
#endif
