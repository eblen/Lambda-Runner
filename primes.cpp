#include <stdio.h>
#include "lambdaRunner.h"

const int MAX_NUM = 999999;
void primePrinter() {
    for (int i=2; i<MAX_NUM; i++) {
        for (int j=2; j<=i/2; j++) {
            if (i % j == 0) {
                printf("Found a prime %d\n", i);
                LambdaRunner->instance->pause();
            }
        }
    }
}

int main()
{
    LambdaRunner lr = new LambdaRunner([]{
        primePrinter();
    });
    for (int i=0; i<10; i++) {
        lr.run();
        printf("Who cares!\n");
    }
    return 0;
}
