#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <semaphore.h>
#include <pthread.h>

typedef struct {
    int thread_id;
    int n_threads;
    sem_t* sem_courant;
    sem_t* sem_suivant;
} thread_d;

void *fthread(void *arg) {
    thread_d *t= (thread_d*)arg;
    while(1) {
        sem_wait(t->sem_courant);

        printf("thread%d\n", t->thread_id);

        sem_post(t->sem_suivant);
    }
    pthread_exit(NULL);
}

int main(int argc, char *argv[]) {

    int n;
    if(argc < 2 || (n = atoi(argv[1])) <= 0) {
        printf("Usage: %s n\nn étant un entier strictement positif\n", argv[0]);
        exit(-1);
    }

    sem_t Sn[n];
    thread_d thread_base[n];
    if(sem_init(&Sn[0], 0, 1)==-1) { // initialisation du semaphore Sn
        perror("Error semaphore");
    }
    for(int i=1; i<n; i++) {
        if(sem_init(&Sn[i], 0, 0)==-1) { // initialisation du semaphore Sn
            perror("Error semaphore");
        }
    }

    pthread_t th[n];
    for(int i=0; i<n; i++) { // creation des n threads
        thread_base[i].thread_id = i + 1;
        thread_base[i].n_threads = n;
        thread_base[i].sem_courant = &Sn[i];
        thread_base[i].sem_suivant = &Sn[(i + 1) % n];
        if(pthread_create(&th[i], NULL, fthread, &thread_base[i]) == -1) {
            perror("Error thread");
            exit(-1);
        }
    }

    for(int i=0; i<n; i++) {
        pthread_join(th[i], NULL);
        sem_destroy(&Sn[i]);
    }

    return 0;
}