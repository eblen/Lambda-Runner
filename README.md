Lambda Runner (LR)
A lightweight coroutine library

LR combines C++ threads and C++ lambdas to create a lightweight coroutine library. It is lightweight in the sense that it only supports stops and restarts of coroutines, called "lambda runners" (LRs). Users can create an LR that runs a given lambda in a separate thread. Inside this lambda, a "pause" can be issued to halt execution. Other threads can "wait" on an LR to pause and can "run" (restart) a paused LR.
