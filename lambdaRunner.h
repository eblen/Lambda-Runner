#include <condition_variable>
#include <mutex>
#include <thread>

#if (__GNUC__ == 4 && __GNUC_MINOR__ <= 7) || (defined __ICC && __ICC <= 1400)
#define thread_local __thread
#endif
#if defined _MSC_VER && _MSC_VER == 1800
#define thread_local __declspec(thread)
#endif

// This umbrella class allows referencing a LambdaRunner without knowing the
// type of the lambda.
class LambdaRunner {
public:
    virtual void pause() = 0;
    virtual void run() = 0;
    virtual void wait() = 0;
    virtual bool isFinished() = 0;
    static thread_local LambdaRunner* instance;
};

template<typename L>
class LambdaRunnerImpl : LambdaRunner {
public:
    void pause() override {
        std::unique_lock<std::mutex> lk(mut_);
        isRunning_ = false;
        cv_.notify_all();
        while (!isRunning_) {
            cv_.wait(lk);
        }
    }

    void run() override {
        std::unique_lock<std::mutex> lk(mut_);
        isRunning_ = true;
        lk.unlock();
        cv_.notify_all();
    }

    void wait() override {
        std::unique_lock<std::mutex> lk(mut_);
        while (isRunning_) {
            cv_.wait(lk);
        }
    }

    bool isFinished() override {return finished_;}

private:
    LambdaRunnerImpl<L>(L l) : finished_(false), lambda_(l), isRunning_(true) {
        thread_.reset(new std::thread([&](){
            instance = this;
            pause();
            lambda_();
        }));
        wait();
        finished_ = true;
    }
    template<typename F>
    friend LambdaRunnerImpl<F>* createLambdaRunner(F f);
    bool finished_;
    std::unique_ptr<std::thread> thread_;
    L lambda_;

    // Synchronization Primitives
    bool isRunning_;
    std::condition_variable cv_;
    std::mutex mut_;
};

template<typename F>
LambdaRunnerImpl<F>* createLambdaRunner(F f) {
    return new LambdaRunnerImpl<F>(f);
}
