#ifndef _LR_POOL_H
#define _LR_POOL_H

#include <cassert>
#include <map>
#include <vector>
#include "lambdaRunner.h"

class LRPool {
public:
    std::unique_ptr<LambdaRunner> get(int core) {
        assert(core >= 0);
        std::lock_guard<std::mutex> lock(poolMut);
        if (lrs.find(core) == lrs.end() || lrs[core].empty()) {
            return std::unique_ptr<LambdaRunner>(new LambdaRunner(core));
        }
        auto lr = std::move(lrs[core].back());
        lrs[core].pop_back();
        return lr;
    }
    void release(std::unique_ptr<LambdaRunner> &lr) {
        assert(lr->isFinished());
        std::lock_guard<std::mutex> lock(poolMut);
        int core = lr->getCore();
        assert(core >= 0);
        lrs[core].emplace_back(std::move(lr));
    }
    std::map<int,size_t> getStats() {
        std::lock_guard<std::mutex> lock(poolMut);
        std::map<int,size_t> m;
        for (const auto &lr_vec : lrs) {
            m.insert(std::pair<int, size_t>(lr_vec.first, lr_vec.second.size()));
        }
        return m;
    }

    // A global pool, so that users don't have to create and somehow store a
    // pool if the application only needs one.
    static LRPool gpool;

private:
    std::map<int,std::vector<std::unique_ptr<LambdaRunner>>> lrs;
    std::mutex poolMut;
};

#endif // _LR_POOL_H
