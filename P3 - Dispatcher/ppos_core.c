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
#include "queue.h"

#define STACKSIZE 32*1024
int task_id_counter = 0;
struct task_t *current_task; 
struct task_t *main_task;
ucontext_t main_context;

queue_t *task_queue = NULL;
queue_t *ready_queue = NULL;
struct task_t *dispatcher_task;

struct task_t *scheduler(){
    struct task_t *next;
    next = (task_t*)task_queue;
    // queue_remove(&task_queue, (queue_t *) next);
    return next;
}

void dispatcher(){
    // queue_remove(&task_queue, (queue_t *) dispatcher_task);
    while (queue_size(task_queue) > 1){
        struct task_t *next_task = scheduler();
        if (next_task){
            next_task->status = 1; // status = 1 significa que a tarefa está rodando
            task_switch(next_task);
            queue_append(&ready_queue, (queue_t*) next_task);
            // switch (next_task->status){
            //     case 0: // tarefa pronta
            //         break;
            //     case 1: // tarefa rodando
            //         break;
            //     default:
            //         break;
            // }
        }
    }
    task_exit(0);
}

void task_yield (){
    // current_task->status = 0;
    // queue_append(&ready_queue, (queue_t*) current_task);
    task_switch(dispatcher_task);
}

void ppos_init (){
    setvbuf (stdout, 0, _IONBF, 0) ;
    
    task_queue = malloc(sizeof(queue_t));
    dispatcher_task = malloc(sizeof(task_t));
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
    main_task->status = 0; // status = 0 significa que a tarefa está pronta 
    main_task->prev = NULL;
    main_task->next = NULL;
    current_task = main_task;
    task_id_counter = 1;
    
    // Cria a tarefa dispatcher
    task_init(dispatcher_task, dispatcher, NULL);

    // queue_append(&ready_queue, (queue_t*) dispatcher_task);
    // queue_append(&ready_queue, (queue_t*) main_task);
    current_task = main_task;

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
 
    task->context = new_context;
    // task->status = 0; // status = 0 significa que a tarefa está pronta
    // task->prev = NULL;
    // task->next = NULL;
    task->id = task_id_counter++;
    makecontext (&new_context, (void*)start_routine, 1, arg) ;
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
    // exit(exit_code);
}

int task_id (){
    return current_task->id;
}