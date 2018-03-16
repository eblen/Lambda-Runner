#include <stdio.h>
#include "lambdaRunner.h"

void primePrinter(int max) {
    for (int i=2; i<=max; i++) {
        bool isPrime = true;
        for (int j=2; j<=i/2; j++) {
            if (i % j == 0) {
                isPrime = false;
                break;
            }
        }
        if (isPrime) {
            printf("%d ", i);
            LambdaRunner::instance->pause();
        }
    }
}

int main(int argc, char **argv)
{
    int max = 100;
    if (argc > 1) {
        max = atoi(argv[1]);
    }
    auto lr = createPinnedLambdaRunner(0, [](int max){
        primePrinter(max);
    }, max);
    int i=1;
    while (true) {
        lr->run();
        lr->wait();
        if (lr->isFinished()) {
            break;
        }
        else {
            printf("is prime %d\n", i++);
        }
    }
    return 0;
}
