#pragma once

#include <chrono>
#include <cstdint>
#include <future>
#include <memory>
#include <source_location>
#include <string>
#include <type_traits>

#include "async_logger/log_level.hpp"

namespace async_logger {

struct Log {
    LogLevel level{LogLevel::kInfo};

    std::chrono::system_clock::time_point time{};

    std::uint32_t thread_id{0};

    std::source_location location{};

    std::string message;

    // Порядковый номер, по нему писатель восстанавливает порядок вывода.
    // Назначается непосредственно перед push.
    std::uint64_t queue_number{0};

    // Непустой только у kFatal. Писатель вызывает set_value() после write + flush.
    // Если Log уничтожен без set_value, future получает broken_promise.
    std::shared_ptr<std::promise<void>> done;
};

// Log перемещается через deque очереди, исключения при перемещении недопустимы.
static_assert(std::is_nothrow_move_constructible_v<Log>);
static_assert(std::is_nothrow_move_assignable_v<Log>);

}  // namespace async_logger