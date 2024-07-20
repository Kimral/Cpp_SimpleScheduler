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

    move_only_function() : func_{ nullptr } {}
    ~move_only_function() { delete func_; }

    template<typename F>
    move_only_function(F&& f) noexcept : func_{ new TaskInner<F>{std::forward<F>(f)} } {}

    move_only_function(const move_only_function& other) = delete;
    move_only_function& operator=(const move_only_function& other) = delete;

    move_only_function(move_only_function&& other) noexcept : func_{ other.func_ } {
        other.func_ = nullptr;
    }

    move_only_function& operator=(move_only_function&& other) noexcept {
        if (this == &other)
            return *this;

        delete func_;
        func_ = other.func_;
        other.func_ = nullptr;
        return *this;
    }

    void operator()() {
        if (func_ == nullptr)
            throw std::bad_function_call{};

        func_->call();
    }

    void swap(move_only_function& other) noexcept {
        std::swap(func_, other.func_);
    }

private:
    Callable* func_;
};