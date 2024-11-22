#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <semaphore.h>
#include <pthread.h>

sem_t Sn; // gère les n threads entrant
int i = 1; // i ème thread

void *fthread(void *arg) {
    int arg_n = *((int*)arg);
    while(1) {
        sem_wait(&Sn);
        if(i>arg_n) {
            i = 1;
        }
        printf("thread%d\n", i++);
        sem_post(&Sn);
    }
    pthread_exit(NULL);
}

int main(int argc, char *argv[]) {

    int n;
    if(argc < 2 || (n = atoi(argv[1])) <= 0) {
        printf("Usage: %s n\nn étant un entier strictement positif\n", argv[0]);
        exit(-1);
    }

    if(sem_init(&Sn, 0, 1)==-1) { // initialisation du semaphore Sn
        perror("Error semaphore");
    }
    pthread_t th[n];
    for(int i=0; i<n; i++) { // creation des n threads
        if(pthread_create(&th[i], NULL, fthread, &n) == -1) {
            perror("Error thread");
            exit(-1);
        }
    }

    for(int i=0; i<n; i++) 
        pthread_join(th[i], NULL);

    sem_destroy(&Sn);

    return 0;
}