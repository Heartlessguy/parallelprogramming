//? LAB5-11
//? Alexandr Komnantyy
//! ФИЛОСОФЫ. ЗАМКИ.
//? for more info see ../PPlab3.MD

#include <omp.h>        
#include <stdio.h>      
#include <stdlib.h>     
#include <cstdlib>
#include <ctime>
#include <unistd.h>     

#define PHILO_COUNT 5      
#define MEALS 5       

static omp_lock_t chopsticks[PHILO_COUNT];   //Locks to represent chopsticks


void philosopher()
{
  #pragma omp barrier

  int id = omp_get_thread_num();
  int left_chopstick;
  int right_chopstick;

  //Philosophers 0 through PHILO_COUNT-2 reach to the left first then the right.
  if(id < PHILO_COUNT -1)
  {
    right_chopstick = id;
    left_chopstick = id+1;
  }
  //Philosopher PHILO_COUNT-1 has its left and right swapped.  Notice that it reaches in the
  //opposite order of all other philosophers.
  else
  {
    right_chopstick = 0;
    left_chopstick = id;
  }

  //Acquire chopsticks (semaphores), eat, wait, then release 
  int i;
  int totalTime = 0;
  for(i = 0; i < MEALS; i++)
  {
    omp_set_lock(&chopsticks[left_chopstick]);
    omp_set_lock(&chopsticks[right_chopstick]);
    int sleepTime = (10 + (rand() % 100)); // in ms
    totalTime += sleepTime;
    usleep(sleepTime * 1000);
    printf("Philosopher %d is eating %d-time for %dms from total %dms\n", id + 1, i + 1, sleepTime, totalTime);

    omp_unset_lock(&chopsticks[left_chopstick]);
    omp_unset_lock(&chopsticks[right_chopstick]);
  }
  printf("Philosopher %d done eating for %dms\n", id + 1, totalTime);
}

//Main function.
int main(int argc, char ** argv)
{
  int i;
  srand(time(0));
  //Initialize locks
  for(i = 0; i < PHILO_COUNT; i++)
    omp_init_lock(&chopsticks[i]);

  //Create and start philosopher threads.
  #pragma omp parallel num_threads(PHILO_COUNT)
  {
    philosopher();
  }

  //Wait for philosophers to finish then destroy locks.
  for(i = 0; i < PHILO_COUNT; i++)
    omp_destroy_lock(&chopsticks[i]);

  //End program.
  return 0;
}