#include <stdint.h>
#include "../drivers/tty.h"
#include "../int/interrupts.h"
#include "../lib/error.h"
#include "../lib/io.h" 
static int utoa(uint32_t v, char *buf) {
    char tmp[12];
    int i = 0;
    if (v == 0) { buf[0] = '0'; buf[1] = '\0'; return 1; }
    while (v) { tmp[i++] = '0' + (v % 10); v /= 10; }
    for (int j = 0; j < i; ++j) buf[j] = tmp[i - 1 - j];
    buf[i] = '\0';
    return i;
}

static void ts(void) {
    uint32_t t = ticks;
    uint32_t sec = t / 1000;
    uint32_t ms  = t % 1000;
    char buf[33];
    int pos = 0;
    buf[pos++] = ' ';
    buf[pos++] = '[';
    buf[pos++] = ' ';
    buf[pos++] = '\t';
    pos += utoa(sec, &buf[pos]);
    buf[pos++] = '.';
    buf[pos++] = '0' + (ms / 100);
    buf[pos++] = '0' + ((ms / 10) % 10);
    buf[pos++] = '0' + (ms % 10);
    buf[pos++] = ']';
    buf[pos++] = ' ';
    buf[pos] = '\0';
    tty_puts(buf);
}

void klog(const char *msg) {
    ts();
    tty_puts(msg);
}
