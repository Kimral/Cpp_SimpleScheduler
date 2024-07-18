#pragma once

#include "ThreadState.h"

class Worker {
public:
    Worker() : state_{std::make_unique<ThreadState>()} {};
   
    void finish() {
        state_->finish();
    }

    void Start() {
        state_.get()->thread_ = std::thread(std::ref(*state_));
    }

    std::unique_ptr<ThreadState>& GetState() {
        return state_;
    }

private:
    std::unique_ptr<ThreadState> state_ = nullptr;
};