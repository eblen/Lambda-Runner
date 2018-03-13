template<typename L>
class lambdaRunner {
    lambdaRunner(L l) : finished(false), lambda(l) {
        id_ = nextId_++;
        thread_.reset(new std::thread([&](){
            myId_ = gid_;
            pause();
            lambda_();
        }
        finished_ = true;
    }
    // Get Id of this runner
    int getGid() {
        return id_;
    }
    // Get Id from inside thread
    // Note: Undefined if called outside thread
    static int getMyId() {
        return myId_;
    }
private:
    // Id needs to be accessible both outside and inside the thread
    // Should always have gid_ == lid_
    int id_;
    static thread_local int myId_;
    bool finished_;
    std::thread thread_;
    L lambda_;
    static std::atomic<int> nextId_ = 0;
};

void pause() {
    // Wait on condition variable
}

void run() {
    // Should wakeup and resume
}
