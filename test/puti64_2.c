#include <stdint.h>
#include <unistd.h>

void puti64(int64_t num) {
    char buf[32];
    int sz = 1;
    buf[sizeof(buf) - sz - 1] = '\n';
    do {
        buf[sizeof(buf) - sz - 1] = num % 10 + '0';
        num /= 10;
        sz++;
    } while(num);
    write(1, &buf[sizeof(buf) - sz], sz);
}
