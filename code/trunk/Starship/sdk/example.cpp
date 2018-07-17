#include "ev.h"
#include <stdio.h>
#include <pthread.h>

#define TIMEOUT 1.

struct ev_loop *loop = NULL;
ev_timer timer_watcher;

static void timer_cb(EV_P_ ev_timer *w, int revents)
{
    static int cb_count = 0;
    printf("timer_cb() call, cb_count = %d\n", cb_count++);
    //ev_break (EV_A_ EVBREAK_ONE);

    //若只调用ev_timer_stop方法，则就是一次性定时器。
    ev_timer_stop(loop, &timer_watcher);
    ev_timer_set(&timer_watcher, TIMEOUT, 0.);
    ev_timer_start(loop, &timer_watcher);
}

void *ev_timer_create(void *p)
{
    printf("ev_timer_create() start !\n");
    loop = EV_DEFAULT;

    ev_timer_init(&timer_watcher, timer_cb, TIMEOUT, 0);
    ev_timer_start(loop, &timer_watcher); // after this invoke, ev_is_active() will no longer return 0
    ev_run(loop, 0);

    printf("ev_timer_create() end !\n");
}

int main(void)
{
    pthread_t tid;
    pthread_create(&tid, NULL, ev_timer_create, NULL);
    while (1){}
    return 0;
}