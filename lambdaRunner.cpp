#include "lambdaRunner.h"

template<typename L>
LambdaRunner<L>* thread_local LambdaRunner<L>::instance = nullptr;

template<typename L>
void LambdaRunner<L>::pause() {
    std::unique_lock<std::mutex> lk(mut_);
    isRunning_ = false;
    cv_.notify_all();
    while (!isRunning) {
        cv_.wait(lk);
    }    
}

template<typename L>
void LambdaRunner<L>::run() {
    std::unique_lock<std::mutex> lk(mut_);
    isRunning_ = true;
    lk.unlock();
    cv_.notify_all();
}

template<typename L>
void LambdaRunner<L>::wait() {
    std::unique_lock<std::mutex> lk(mut_);
    while (isRunning) {
        cv_.wait(lk);
    }
}
