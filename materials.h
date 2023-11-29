#ifndef materials_h
#define materials_h

#include <fcntl.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <sys/stat.h>

#define TEXT_SZ 128

struct buffer {
  int count;
  char some_text[TEXT_SZ];
};

#define NUM_BUFF 100
struct shared_memory {
  struct buffer buff_list[NUM_BUFF];
};


int set_semvalue(int sem_id, int value) {
    union semun sem_union;
    sem_union.val = value;
    if (semctl(sem_id, 0, SETVAL, sem_union) == -1) return(0);
    return(1);
}

// deletes semaphore using an IPC_RMID command in a semctl call
void del_semvalue(int sem_id) {
    union semun sem_union;
    if (semctl(sem_id, 0, IPC_RMID, sem_union) == -1)
        fprintf(stderr, "del_semvalue() failed.\n");
}

// semaphore wait
int semaphore_p(int sem_id) {
    struct sembuf sem_b;
    sem_b.sem_num = 0;
    sem_b.sem_op = -1; // P() -> wait
    sem_b.sem_flg = SEM_UNDO;
    if (semop(sem_id, &sem_b, 1) == -1) {
        fprintf(stderr, "semaphore_p() failed.\n");
        return(0);
    }
    return(1);
}

// semaphore signal
int semaphore_v(int sem_id) {
    struct sembuf sem_b;
    sem_b.sem_num = 0;
    sem_b.sem_op = 1; // V() -> signal
    sem_b.sem_flg = SEM_UNDO;
    if (semop(sem_id, &sem_b, 1) == -1) {
        fprintf(stderr, "semaphore_v() failed.\n");
        return(0);
    }
    return(1);
}

int set_semvalue(int sem_id, int value);
void del_semvalue(int sem_id);
int semaphore_p(int sem_id);
int semaphore_v(int sem_id);

#endif
