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
task_t *current_task; 
task_t *main_task;
task_t *dispatcher_task;
ucontext_t main_context;

queue_t *task_queue = NULL;
queue_t *ready_queue = NULL;

task_t *scheduler(){
    #ifdef DEBUG
        printf ("scheduler: entrando\n") ;
    #endif
    task_t *next;
    next = (task_t*)ready_queue;
    queue_remove(&ready_queue, (queue_t*)next);
    // queue_append(&task_queue, (queue_t*)next);
    #ifdef DEBUG
        printf ("scheduler: tarefa %d\n", next->id) ;
        printf ("scheduler: %d tarefas na fila\n", queue_size(task_queue));
    #endif
    return next;
}

void dispatcher(){
    // queue_remove(&task_queue, (queue_t *) dispatcher_task);
    #ifdef DEBUG
        printf ("dispatcher: entrando\n") ;
    #endif
    task_t *next;
    while (next = scheduler()){
        #ifdef DEBUG
            printf ("dispatcher: %d tarefas na fila\n", queue_size(task_queue));
        #endif
        if (next){
            next->status = 1; // status = 1 significa que a tarefa está rodando
            task_switch(next);
            // queue_append(&ready_queue, (queue_t*) next_task);
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
    #ifdef DEBUG
        printf ("dispatcher: saindo\n") ;
    #endif
    task_exit(0);
}

void task_yield (){
    #ifdef DEBUG
        printf ("task_yield: entrando\n") ;
    #endif
    // current_task->status = 0;
    queue_append(&ready_queue, (queue_t*) current_task);
    task_switch(dispatcher_task);
    #ifdef DEBUG
        printf ("task_yield: saindo\n") ;
    #endif
}

void ppos_init (){
    #ifdef DEBUG
        printf ("ppos_init: entrando\n") ;
    #endif
    setvbuf (stdout, 0, _IONBF, 0) ;
    
    // task_queue = malloc(sizeof(queue_t));
    task_queue = NULL;
    ready_queue = NULL;

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
    #ifdef DEBUG
        printf ("criando tarefa: dispatcher\n") ;
    #endif
    task_init(dispatcher_task, dispatcher, NULL);

    // queue_append(&ready_queue, (queue_t*) dispatcher_task);
    // queue_append(&ready_queue, (queue_t*) main_task);
    current_task = main_task;
    // task_yield();
    #ifdef DEBUG
        printf ("ppos_init: saindo\n") ;
    #endif

}

int task_init (task_t *task, void (*start_routine)(void *),  void *arg){
    #ifdef DEBUG
        printf ("task_init: entrando\n") ;
    #endif
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
    task->status = 0; // status = 0 significa que a tarefa está pronta
    task->prev = NULL;
    task->next = NULL;
    task->id = task_id_counter++;

    if (task != dispatcher_task) {
        queue_append(&task_queue, (queue_t*) task);
    }

    #ifdef DEBUG
        printf ("task_init: saindo\n") ;
    #endif
    return task->id;
}

int task_switch (task_t *task){
    #ifdef DEBUG
        printf ("task_switch: entrando\n") ;
    #endif
    task_t *prev_task = current_task;
    current_task = task;
    swapcontext (&(prev_task->context), &(current_task->context)) ;
    #ifdef DEBUG
        printf ("task_switch: saindo\n") ;
    #endif

    return 0;
}

void task_exit (int exit_code){
    #ifdef DEBUG
        printf ("task_exit: entrando\n") ;
    #endif
    if (current_task != main_task) {
        queue_remove((queue_t**)&task_queue, (queue_t*)current_task);
    }
    task_switch(main_task);
    // exit(exit_code);
    // free(current_task->context.uc_stack.ss_sp);
    #ifdef DEBUG
        printf ("task_exit: saindo\n") ;
    #endif
}

int task_id (){
    return current_task->id;
}