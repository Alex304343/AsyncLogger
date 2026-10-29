#pragma once

#include <condition_variable>
#include <cstddef>
#include <deque>
#include <mutex>
#include <type_traits>
#include <utility>

// Реализация в заголовке из-за шаблона

namespace async_logger {

// Потокобезопасная неограниченная FIFO-очередь (many producers / many consumers).
// Размер не ограничивает
// Порядок между потребителями не восстанавливает.
template <typename T>
class Queue {
    // pop перемещает элемент в out под мьютексом; исключение здесь потеряло бы элемент.
    static_assert(std::is_nothrow_move_assignable_v<T>);
    static_assert(std::is_nothrow_move_constructible_v<T>);

public:
    Queue() = default;

    // Мьютекс и condvar не копируются и не перемещаются.
    Queue(const Queue&) = delete;
    Queue& operator=(const Queue&) = delete;

    // Кладёт элемент в конец. Возвращает false, если очередь остановлена:
    // в этом случае value НЕ тронут, вызывающий может откатить резерв и обработать отказ.
    [[nodiscard]] bool push(T&& value) {
        {
            std::lock_guard lock(mutex_);
            if (stopped_) {
                return false;
            }
            q_.push_back(std::move(value));
        }

        cv_.notify_one();
        return true;
    }

    // Блокируется, пока нет элементов и очередь не остановлена.
    // Возвращает false ТОЛЬКО если очередь остановлена и пуста.
    [[nodiscard]] bool pop(T& out) noexcept {
        std::unique_lock lock(mutex_);
        cv_.wait(lock, [this] { return stopped_ || !q_.empty(); });
        if (q_.empty()) {
            return false;  // сюда попадаем только при stopped_
        }
        out = std::move(q_.front());
        q_.pop_front();
        return true;
    }

    // Запрещает новые push и будит всех ожидающих pop. Идемпотентна.
    void stop() noexcept {
        {
            std::lock_guard lock(mutex_);
            stopped_ = true;
        }
        cv_.notify_all();
    }

    // Мгновенный снимок: к моменту использования результата он может устареть.
    // Предназначены для метрик и тестов, не для принятия решений в логике логгера.
    [[nodiscard]] bool empty() const noexcept {
        std::lock_guard lock(mutex_);
        return q_.empty();
    }

    [[nodiscard]] std::size_t size() const noexcept {
        std::lock_guard lock(mutex_);
        return q_.size();
    }

private:
    mutable std::mutex mutex_;
    std::condition_variable cv_;
    std::deque<T> q_;
    bool stopped_{false};
};

}  // namespace async_logger