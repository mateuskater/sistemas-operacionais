/*

Nome: Mateus Kater Pombeiro

GRR: 20190366

*/

#include <stdio.h>
#include <stdlib.h>
#include <ucontext.h>
#include <string.h>
#include "ppos.h"
#include "ppos_data.h"

#define STACKSIZE 32*1024
int task_id_counter = 0;
struct task_t *current_task; 
struct task_t *main_task;
ucontext_t main_context;

void ppos_init (){
    setvbuf (stdout, 0, _IONBF, 0) ;

    getcontext (&main_context) ;
    char *stack = malloc (STACKSIZE) ;
    if (stack)
    {
        main_context.uc_stack.ss_sp = stack ;
        main_context.uc_stack.ss_size = STACKSIZE ;
        main_context.uc_stack.ss_flags = 0 ;
        main_context.uc_link = 0 ;
    }
    else
    {
       perror ("Erro na criação da pilha: ") ;
       exit (1) ;
    }

    main_task = malloc(sizeof(task_t));
    if (main_task == NULL)
    {
        perror ("Erro na criação da tarefa principal: ") ;
        exit (1) ;
    }

    main_task->context = main_context;
    main_task->id = 0;
    main_task->status = 0;
    main_task->prev = NULL;
    main_task->next = NULL;
    current_task = main_task;
    task_id_counter = 1;
}

int task_init (task_t *task, void (*start_routine)(void *),  void *arg){
    ucontext_t new_context;

    getcontext (&new_context) ;

    char *stack = malloc (STACKSIZE) ;
    if (stack)
    {
        new_context.uc_stack.ss_sp = stack ;
        new_context.uc_stack.ss_size = STACKSIZE ;
        new_context.uc_stack.ss_flags = 0 ;
        new_context.uc_link = 0 ;

    }
    else
    {
       perror ("Erro na criação da pilha: ") ;
       return -1 ;
    }
 
    makecontext (&new_context, (void*)start_routine, 1, arg) ;
    task->context = new_context;
    task->id = task_id_counter++;
    return task->id;
}

int task_switch (task_t *task){
    struct task_t *prev_task = current_task;
    current_task = task;
    swapcontext (&(prev_task->context), &(current_task->context)) ;
    return 0;
}

void task_exit (int exit_code){
    task_switch(main_task);
}

int task_id (){
    return current_task->id;
}