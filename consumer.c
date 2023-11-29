#include "materials.h"

int main () {
  int count = 0;
  int total_bytes_read = 0;
  int *memory_space = (void *)0;
  int shm_id, file_id;
  int sem_id_e, sem_id_n, sem_id_s;
  struct buffer buffer;
  struct shared_memory *share;


  //initialize shared memory
  shm_id = shmget((key_t)1111, sizeof(struct shared_memory), 0666 | IPC_CREAT);
  if (shm_id == -1) {
    fprintf(stderr, "shmget() failed.\n");
    exit(EXIT_FAILURE);
  }

  // attach the shared memory to the process
  memory_space = shmat(shm_id, (void *)0, 0);
  if (memory_space == (void *)-1) {
      fprintf(stderr, "shmat() failed.\n");
      exit(EXIT_FAILURE);
  }

  // define the variable that references the shared space
  share = (struct shared_memory *)memory_space;

  //open file to read from
  file_id = open("output.txt", O_WRONLY);
  if (file_id == -1) {
    printf("Couldn't open file\n");
    exit(EXIT_FAILURE);
  }

  //define semaphores S, N, E
  // S -> mutual exclusion
  // N -> # items in the buffer
  // E -> # empty spots in buffer
  sem_id_s = semget((key_t)2222, 1, 0666 | IPC_CREAT);
  sem_id_n = semget((key_t)2223, 1, 0666 | IPC_CREAT);
  sem_id_e = semget((key_t)2224, 1, 0666 | IPC_CREAT);

  int exit_character_triggered = 0;

  //read from file
  int buffer_has_content = 1;
  while (buffer_has_content) {
    // wait until allowed to enter critical section (accessing buffers)
    printf("consumer waiting to enter critical section\n");
    semaphore_p(sem_id_n);
    semaphore_p(sem_id_s);
    buffer = share->buff_list[count];
    printf("consumer entered critical section\n");

    //signal out of critical section
    semaphore_v(sem_id_e);
    semaphore_v(sem_id_s);

    printf("consumer exited critical section\n\n");


    printf("----- BUFFER %d CONTENT ------\n %s\n\n",count, buffer.some_text);
    for (int i = 0; i < TEXT_SZ; i++) {

      if (!exit_character_triggered) {
          total_bytes_read += 1;
      }

      if (buffer.some_text[i] == '%') {
        buffer_has_content = 0;
        exit_character_triggered = 1;
      }
    }

    count = (count + 1) % NUM_BUFF;
    if (write(file_id, buffer.some_text, sizeof(buffer.some_text)) <= 0) {
      perror("write() error.\n");
      exit(EXIT_FAILURE);
    }
    memset(buffer.some_text, 0, TEXT_SZ);

  }
  //this is to account for automatic newline character added on save (not sure if this happens on every editor)
  //on Atom this was necessary
  total_bytes_read += 1;

  printf("Total bytes read: %d\n", total_bytes_read);

  del_semvalue(sem_id_e);
  del_semvalue(sem_id_s);
  del_semvalue(sem_id_n);
  //detach memory
  if (shmdt(share) == -1) {
      fprintf(stderr, "shmdt() failed.\n");
      exit(EXIT_FAILURE);
  }

  if(shmctl(shm_id,IPC_RMID,0)==-1) {
    fprintf(stderr, "shmctl(IPC_RMID) failed\n");
    exit(EXIT_FAILURE);
  }

  // close file
  file_id = close(file_id);
  if (file_id == -1) {
      perror("close(in) error.\n");
      exit(EXIT_FAILURE);
  }
}
