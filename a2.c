#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/sem.h>
#include <sys/wait.h>
#include <pthread.h>
#include "a2_helper.h"
#include <semaphore.h>
#include    <sys/ipc.h>

//ierarhie procese
//t22 inainte de t25 (inceput) se termina dupa t25
//t6.15 se termina cand tuleaza 5 thr -> (maxim 5 deodata!)
//t5.3 gata inainte ca t23 sa inceapa +  t23 gata-> t52 porneste

sem_t *sem52;
sem_t *sem53;

typedef struct {
    int id;
    sem_t *sem1;
    sem_t *sem2;
    sem_t *sem3;
} TH_STRUCT;


void* th_func6(void* arg)
{   
    TH_STRUCT *s = (TH_STRUCT*)arg;
    sem_wait(s->sem2);
    info(BEGIN, 6, s->id);
        
    if(s->id == 15){
         //sel=1;
         sem_wait(s->sem1);
         sem_wait(s->sem1);
         sem_wait(s->sem1);
         sem_wait(s->sem1);
         //sel = 2;
         info(END, 6, s->id);
         sem_post(s->sem3);
         sem_post(s->sem3);
         sem_post(s->sem3);
         sem_post(s->sem3);
         }
         
    if( s->id >20 && s->id<25){
         sem_post(s->sem1);
         sem_wait(s->sem3);
          }

    if(s->id != 15)
        info(END, 6, s->id);

    sem_post(s->sem2);
    return NULL;
}



void* th_func5(void* arg)
{   
    TH_STRUCT *s = (TH_STRUCT*)arg;
     if(s->id == 2)
       sem_wait(sem53);
    info(BEGIN, 5, s->id);
    
        
    info(END, 5, s->id);
    if(s->id == 3)
       sem_post(sem52);
    return NULL;
}



void* th_func2(void* arg)
{
    TH_STRUCT *s = (TH_STRUCT*)arg;
    if(s->id == 3)
       sem_wait(sem52);
       
     if(s->id == 5)
       sem_wait(s->sem1); 
        
    info(BEGIN, 2, s->id);
    
      if(s->id == 2)
       sem_post(s->sem1);
    
      if(s->id == 2)
       sem_wait(s->sem2);
    info(END, 2, s->id);
      if(s->id == 5)
       sem_post(s->sem2);
    
  /*  pthread_mutex_lock(s->lock);
    while(n2 != s->id) 
        pthread_cond_wait(s->cond, s->lock);

    n2++;
    info(BEGIN, 2, s->id);
    pthread_cond_broadcast(s->cond);
    while(s->id != n3)
        pthread_cond_wait(s->cond, s->lock);
    
    n3 = n3-1;
    info(END, 2, s->id);
    pthread_cond_broadcast(s->cond);
    pthread_mutex_unlock(s->lock);*/
    if(s->id == 3)
       sem_post(sem53);
    return NULL;
}



int main(int argc, char **argv)
{
init();

//-----------------------------------------------------[ P1 -> BEGIN ]--------------------------------------------------------
info(BEGIN, 1, 0);
sem53 = sem_open("s53", O_CREAT, 0644, 0);
sem52 = sem_open("s52", O_CREAT, 0644, 0);

pid_t p2, p4;

    p2 = fork();
    if(p2 == -1) { 
        perror("Could not create child process");
        return -1;
        
//-----------------------------------------------------[ P2 -> BEGIN ]--------------------------------------------------------
        
    } else if(p2 == 0) {   // -> p2
        info(BEGIN, 2, 0);
        pid_t p3 = fork();
        if(p3 == -1) {
              perror("Could not create child process");
              return -1;
              
//-----------------------------------------------------[ P3 -> BEGIN ]--------------------------------------------------------

        } else if(p3 == 0) {   // -> p3
               info(BEGIN, 3, 0);
               pid_t p5 = fork();
               if(p5 == -1) {
                   perror("Could not create child process");
                   return -1;
              
//----------------------------------------------------[ P5 ]---------------------------------------------------------
               } else if(p5 == 0) {  
                   info(BEGIN, 5, 0);
                   TH_STRUCT params[4];
                   pthread_t tids[4];
                   for(int i=0; i<4; i++) {
                           params[i].id = i+1;
                           pthread_create(&tids[i], NULL, th_func5, &params[i]);
                           }
                  for(int i=0; i<4; i++) {
                           pthread_join(tids[i], NULL);
                           }
                  info(END, 5, 0);
                  exit(1);
              
//-----------------------------------------------------[ P3 -> END ]--------------------------------------------------------
                } else {
                   wait(NULL);
                   info(END, 3, 0);
                   exit(1);
                 }
              
//-----------------------------------------------------[ P2 -> END]--------------------------------------------------------
       } else {
                TH_STRUCT params[5];
        pthread_t tids[5];
        sem_t sem22, sem25;
        sem_init(&sem22, 0, 0) ;
        sem_init(&sem25, 0, 0) ;
    
       for(int i=0; i<5; i++) {
           params[i].id = i+1;
           params[i].sem1 = &sem22;
           params[i].sem2 = &sem25;
           pthread_create(&tids[i], NULL, th_func2, &params[i]);
          }
       for(int i=0; i<5; i++) {
           pthread_join(tids[i], NULL);
       }
       sem_destroy(&sem25);
       sem_destroy(&sem22);
                   wait(NULL);
            info(END, 2, 0);
            exit(1);
       }
         
         
    } else {         // -> p1
          p4 = fork();
          if(p4 == -1) {
              perror("Could not create child process");
              return -1;
          
          
//----------------------------------------------------[ P4 -> begin]---------------------------------------------------------

          } else if(p4 == 0) {      // ->p4
               info(BEGIN, 4, 0);
               pid_t p6 = fork();
               if(p6 == -1) {
                      perror("Could not create child process");
                       return -1;
              
//----------------------------------------------------[ P6 -> begin]---------------------------------------------------------
               } else if(p6 == 0) {    // ->p6
                    info(BEGIN, 6, 0);
                    sem_t s, s2, s3 ;
                    sem_init(&s, 0, 0) ;
                    sem_init(&s2, 0, 5) ;
                    sem_init(&s3, 0, 0) ;
                    TH_STRUCT params[35];
                    pthread_t tids[35];
                   
                    for(int i=0; i<35; i++) {
                         params[i].id = i+1;
                         params[i].sem1 = &s;
                         params[i].sem2 = &s2;
                         params[i].sem3 = &s3;
                         pthread_create(&tids[i], NULL, th_func6, &params[i]);
                    }
                    for(int i=0; i<35; i++) 
                          pthread_join(tids[i], NULL);

                    sem_destroy(&s);
                    sem_destroy(&s2);
                    sem_destroy(&s3);
                    
                     pid_t p7 = fork();
                     if(p7 == -1) {
                         perror("Could not create child process");
                         return -1;
              
//----------------------------------------------------[ P7 ]---------------------------------------------------------
                     } else if(p7 == 0) {
                          info(BEGIN, 7, 0);
                          info(END, 7, 0);
                          exit(1);
              
//-----------------------------------------------------[ P6 - end ]--------------------------------------------------------
                      } else {
                           wait(NULL);
                           info(END, 6, 0);
                           exit(1);
                      }
              
//-----------------------------------------------------[ P4 end]--------------------------------------------------------
             } else {
                   wait(NULL);
                   info(END, 4, 0);
                   exit(1);
                     }
            
            }else{ 
//-----------------------------------------------------[ P1 -> END]--------------------------------------------------------
        wait(NULL);
         wait(NULL);
    }
    }

      
    sem_close(sem53);
    sem_close(sem52);
    sem_unlink("s53");
    sem_unlink("s52");

info(END, 1, 0);
return 0;
}


