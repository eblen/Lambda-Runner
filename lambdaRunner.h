#include <cassert>
#include <condition_variable>
#include <mutex>
#include <thread>

#if (__GNUC__ == 4 && __GNUC_MINOR__ <= 7) || (defined __ICC && __ICC <= 1400)
#define thread_local __thread
#endif
#if defined _MSC_VER && _MSC_VER == 1800
#define thread_local __declspec(thread)
#endif

void setAffinity(int core);

// This umbrella class exists to allow referencing a LambdaRunner without
// knowing the type of the lambda (essential for pausing from inside a lambda).
class LambdaRunner {
public:
    virtual void pause() = 0;
    virtual void run() = 0;
    virtual void wait() = 0;
    virtual bool isFinished() = 0;
    // NULL if accessed outside a lambda
    static thread_local LambdaRunner* instance;
};

template<typename L, typename... Args>
class LambdaRunnerImpl : LambdaRunner {
public:
    // Called inside lambda to pause execution
    // It is an error to call outside of lambda
    void pause() override {
        assert(instance != nullptr);
        std::unique_lock<std::mutex> lk(mut_);
        isRunning_ = false;
        cv_.notify_all();
        while (!isRunning_) {
            cv_.wait(lk);
        }
    }

    // Called outside of lambda to resume execution
    // Does nothing if called inside lambda
    void run() override {
        std::unique_lock<std::mutex> lk(mut_);
        isRunning_ = true;
        lk.unlock();
        cv_.notify_all();
    }

    // Called outside of lambda to wait for lambda to pause
    // Does nothing if called inside lambda
    void wait() override {
        std::unique_lock<std::mutex> lk(mut_);
        while (isRunning_) {
            cv_.wait(lk);
        }
    }

    bool isFinished() override {return finished_;}

private:
    // Create runners with "createLambdaRunner" friend function
    LambdaRunnerImpl<L, Args...>(int core, L l, Args... args) : finished_(false),
    lambda_(l), isRunning_(true) {
        thread_.reset(new std::thread([core,this,args...](){
            instance = this;
            if (core >= 0) {
                setAffinity(core);
            }
            pause();
            lambda_(args...);
            finished_ = true;
            while(true) {
                pause();
            }
        }));
        // Calling thread waits for first pause, when runner is fully initialized.
        wait();
    }
    template<typename L, typename... Args>
    friend LambdaRunnerImpl<L, Args...>* createPinnedLambdaRunner(int core, L l, Args... args);

    bool finished_;
    std::unique_ptr<std::thread> thread_;
    L lambda_;

    // Synchronization Primitives
    bool isRunning_;
    std::condition_variable cv_;
    std::mutex mut_;
};

template<typename L, typename... Args>
LambdaRunnerImpl<L, Args...>* createPinnedLambdaRunner(int core, L l, Args... args) {
    return new LambdaRunnerImpl<L, Args...>(core, l, args...);
}

template<typename L, typename... Args>
LambdaRunnerImpl<L, Args...>* createLambdaRunner(L l, Args... args) {
    return createPinnedLambdaRunner(-1, l, args...);
}
