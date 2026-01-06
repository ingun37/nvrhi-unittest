#include <stdio.h>
#include <unistd.h>
#include <pthread.h>

void *thread_callback(void *arg) {
    // sleep() allows the main thread to continue and ensures the thread runs in a worker
    sleep(1);
    printf("Inside the thread: %d\n", *(int *) arg);
    return NULL;
}

int main() {
    puts("Before the thread");
    pthread_t thread_id;
    int arg = 42;
    // Create a new thread, passing the address of 'arg' as the argument
    pthread_create(&thread_id, nullptr, thread_callback, &arg);
    // Wait for the created thread to finish
    pthread_join(thread_id, nullptr);
    puts("After the thread");
    return 0;
}
