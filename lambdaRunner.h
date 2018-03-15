#include <condition_variable>
#include <mutex>
#include <thread>

template<typename L>
class LambdaRunner {
    LambdaRunner<L>(L l) : finished_(false), lambda_(l), isRunning_(true) {
        thread_.reset(new std::thread([&](){
            myRunner_ = this;
            pause();
            lambda_();
        }
        finished_ = true;
    }
    void pause();
    void run();
    void wait();
    bool isFinished() {return finished_;}

    // Enclosing instance from inside thread or nullptr otherwise
    static thread_local LambdaRunner<L>* instance;
private:
    bool finished_;
    std::thread thread_;
    L lambda_;

    // Synchronization Primitives
    bool isRunning_;
    std::condition_variable cv_;
    std::mutex mut_;
};
