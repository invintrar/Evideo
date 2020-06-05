#include <stdio.h>
#include <stdlib.h>
#include <gnu/libc-version.h>
#include <glib.h>

GTimer *timer;

int main(int argc, char *argv[]) {
    timer = g_timer_new();
    printf("glibc version: %s\n", gnu_get_libc_version());
}