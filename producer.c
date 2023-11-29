#include "materials.h"

int main () {
  int count = 0;
  int total_bytes_written = 0;
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
  file_id = open("text.txt", O_RDONLY);
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

  sleep(5);

  // initialize semaphore values
  if (!set_semvalue(sem_id_s, 1)) {
    fprintf(stderr, "Failed to initialize semaphore s.\n");
    exit(EXIT_FAILURE);
  }
  if (!set_semvalue(sem_id_n, 0)) {
    fprintf(stderr, "Failed to initialize semaphore n.\n");
    exit(EXIT_FAILURE);
  }
  if (!set_semvalue(sem_id_e, NUM_BUFF)) {
    fprintf(stderr, "Failed to initialize semaphore e.\n");
    exit(EXIT_FAILURE);
  }

  //read from file
  strcpy(buffer.some_text, "");
  int exit_character_triggered = 0;
  int size_read = read(file_id, buffer.some_text, TEXT_SZ);
  while (size_read > 1) {
    total_bytes_written += size_read;

    // wait until allowed to enter critical section (accessing buffers)

    printf("producer waiting to enter critical section\n");
    semaphore_p(sem_id_e);
    semaphore_p(sem_id_s);
    strcpy(share->buff_list[count].some_text, buffer.some_text);
    printf("producer entered critical section\n");

    //signal that buffer is not empty
    semaphore_v(sem_id_n);

    //signal out of critical section
    semaphore_v(sem_id_s);
    printf("producer exited critical section\n");

    memset(buffer.some_text, 0, TEXT_SZ);


    size_read = read(file_id, buffer.some_text, TEXT_SZ);

    count += 1;

  }

  printf("Total bytes written: %d\n", total_bytes_written);

  //detach memory
  if (shmdt(share) == -1) {
      fprintf(stderr, "shmdt() failed.\n");
      exit(EXIT_FAILURE);
  }

  // close file
  file_id = close(file_id);
  if (file_id == -1) {
      perror("close(in) error.\n");
      exit(EXIT_FAILURE);
  }


}
