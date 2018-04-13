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
    LambdaRunner lr(0);
    lr.run([=]{primePrinter(max);});
    int i=1;
    while (true) {
        lr.wait();
        if (lr.isFinished()) {
            break;
        }
        else {
            printf("is prime %d\n", i++);
        }
        lr.cont();
    }
    return 0;
}
