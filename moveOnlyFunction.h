#pragma once

#include <type_traits>

struct move_only_function
{
    struct Callable {
        Callable() = default;
        Callable(const Callable& other) = delete;
        Callable& operator=(const Callable& other) = delete;
        Callable(Callable&& other) = delete;
        Callable& operator=(Callable&& other) = delete;
        virtual ~Callable() = default;
        virtual void call() = 0;
    };

    template<class Func>
    struct TaskInner : Callable {
        template<typename F>
        explicit TaskInner(F&& f) : item_{ std::forward<F>(f) } {}

        void call() override { item_(); }

    private:
        Func item_;
    };

    move_only_function() : lambda_{ nullptr }, function_{ nullptr } {}
    ~move_only_function() { delete lambda_; }

    template<typename F>
    move_only_function(F&& f) noexcept : lambda_{ new TaskInner<F>{std::forward<F>(f)} }, function_{ nullptr } {}

    move_only_function(void(*function)()) : lambda_{ nullptr }, function_{ function } {}

    move_only_function(const move_only_function& other) = delete;
    move_only_function& operator=(const move_only_function& other) = delete;

    move_only_function(move_only_function&& other) noexcept : lambda_{ other.lambda_ }, function_{ other.function_ } {
        other.lambda_ = nullptr;
    }

    move_only_function& operator=(move_only_function&& other) noexcept {
        if (this == &other)
            return *this;

        delete lambda_;
        lambda_ = other.lambda_;
        function_ = other.function_;
        other.lambda_ = nullptr;
        return *this;
    }

    void operator()() {
        if (lambda_ != nullptr) {
            lambda_->call();
            return;
        }

        if (function_ != nullptr) {
            function_();
            return;
        }

        throw std::bad_function_call{};
    }

    void swap(move_only_function& other) noexcept {
        std::swap(lambda_, other.lambda_);
        std::swap(function_, other.function_);
    }

private:
    Callable* lambda_;
    void (*function_)();
};