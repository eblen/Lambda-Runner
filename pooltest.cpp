#include <stdio.h>
#include <mutex>
#include <thread>
#include "lrPool.h"
#include "lambdaRunner.h"

// Program to test the LambdaRunner Pool

void printstuff(int t, int n, int c) {
    static std::mutex mut;
    std::lock_guard<std::mutex> lock(mut);
    printf("Thread %d lr %d core %d\n", t, n, c);
    size_t num_lrs = 0;
    for (auto s : LRPool::gpool.getStats()) {
        printf("%d %d\n", s.first, s.second);
        num_lrs += s.second;
    }
    printf("Total LRs: %d\n", num_lrs);
}

int main()
{
    const int nthreads = 10;
    const int ncores = 10;
    const int iter_per_thread = 10;
    const float delay_prob = 0.5;

    std::set<int> coreSet;
    for (int i=0; i<ncores; i++) {
        coreSet.insert(i);
    }
    LRPool &pool = LRPool::gpool;
    pool.addCores(coreSet);

    // Spawn threads that will exercise the LR pool
    std::vector<std::thread*> threads(nthreads);
    for (int i=0; i<nthreads; i++) {
        threads[i] = new std::thread([i,&pool]() {
            std::vector<std::unique_ptr<LambdaRunner>> lrs(iter_per_thread);

            // Request lambda runners and either use them immediately or store
            // store them for later.
            for (int j=0; j<iter_per_thread; j++)
            {
                int core = rand() % ncores;
                auto lr = pool.get(core);
                if (rand() % 100  < delay_prob * 100) {
                    lr->run([=](){printstuff(i,j,core);});
                    lr->wait();
                    pool.release(lr);
                }
                else {
                    lrs[j] = std::move(lr);
                }
            }

            // Go back and run stored runners.
            for (int j=0; j<iter_per_thread; j++) {
                if (lrs[j].get() != nullptr) {
                    int core = lrs[j]->getCore();
                    lrs[j]->run([=](){printstuff(i,j,core);});
                    lrs[j]->wait();
                    pool.release(lrs[j]);
                }
            }
        });
    }

    // Main thread waits for other threads to finish
    for (int i=0; i<nthreads; i++) {
        threads[i]->join();
    }
}
