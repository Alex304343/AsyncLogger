#pragma once

#include <cstdint>
#include <future>
#include <memory>
#include <string>
#include <type_traits>

#include "async_logger/log_level.hpp"

namespace async_logger {

struct Message {
    // Готовая строка. Пустое тело означает сбой форматирования: Message всё равно
    // обязан дойти до Buffer, иначе в нумерации образуется дыра.
    std::string body;

    // Копируется из Log без изменений.
    std::uint64_t queue_number{0};

    // Нужен sink-у (цвет, сброс на kError и выше).
    LogLevel level{LogLevel::kInfo};

    // Переносится из Log всегда, даже при сбое форматирования.
    std::shared_ptr<std::promise<void>> done;
};

// Message хранится в vector<optional<Message>> внутри Buffer.
static_assert(std::is_nothrow_move_constructible_v<Message>);
static_assert(std::is_nothrow_move_assignable_v<Message>);

}  // namespace async_logger