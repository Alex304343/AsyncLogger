#pragma once

#include <cstdint>
#include <string_view>

namespace async_logger{

enum class LogLevel : uint8_t{
    kDebug = 0,
    kInfo = 1,
    kWarning = 2,
    kError = 3,
    kFatal = 4
};

constexpr std::string_view ToString(LogLevel level) noexcept {
    switch (level) {
        case LogLevel::kDebug:   return "DEBUG";
        case LogLevel::kInfo:    return "INFO";
        case LogLevel::kWarning: return "WARNING";
        case LogLevel::kError:   return "ERROR";
        case LogLevel::kFatal:   return "FATAL";
    }
    return "UNKNOWN";
}

} // namespace async_logger