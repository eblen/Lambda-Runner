#include <stdio.h>
#include "lambdaRunner.h"

const int MAX_NUM = 999999;
void primePrinter() {
    for (int i=2; i<MAX_NUM; i++) {
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
    for (int i=0; i<10; i++) {
        lr->run();
        lr->wait();
        printf("is prime %d\n", i+1);
    }
    return 0;
}
