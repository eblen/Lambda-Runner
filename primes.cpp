#include <stdio.h>
#include "lambdaRunner.h"

const int MAX_NUM = 100;
void primePrinter() {
    for (int i=2; i<=MAX_NUM; i++) {
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

int main()
{
    auto lr = createLambdaRunner([]{
        primePrinter();
    });
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
