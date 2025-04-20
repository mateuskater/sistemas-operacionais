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
task_t main_task;
task_t dispatcher_task;
task_t *current_task = &main_task; 
ucontext_t main_context;

queue_t *task_queue;

task_t *scheduler(){
    #ifdef DEBUG
        printf ("scheduler: entrando\n") ;
    #endif
    if (queue_size(task_queue) == 0){
        #ifdef DEBUG
            printf ("scheduler: saindo\n") ;
        #endif
        return NULL;
    }
    queue_t *cur_q = task_queue;
    task_t *cur_task; // ponteiro para a tarefa atual
    task_t *next = (task_t*)cur_q; // ponteiro para a próxima tarefa

    do{
        cur_task = (task_t *) cur_q;
        if (cur_task->prio_dinamica <= next->prio_dinamica){
            next = cur_task;
            // cur_task = next;
        }
        cur_q = cur_q->next;
    }while (cur_q != task_queue);

    cur_q = task_queue;
    do {
        cur_task = (task_t*) cur_q;
        if (cur_task != next)
            cur_task->prio_dinamica--;
        // if (next->prio_dinamica > -20){
        //     (next->prio_dinamica)--;
        // }
        cur_q = cur_q->next;
    } while (cur_q != task_queue);
    cur_task->prio_dinamica = cur_task->prio_estatica;
    queue_remove((queue_t **) &task_queue, (queue_t *) next);
    #ifdef DEBUG
        printf ("scheduler: tarefa %d\n", next->id) ;
        printf ("scheduler: %d tarefas na fila\n", queue_size(task_queue));
    #endif
    return cur_task;
}

void task_setprio (task_t *task, int prio){
    #ifdef DEBUG
        printf ("task_setprio: entrando\n") ;
    #endif
    if (task == NULL) {
        task = current_task;
    }
    task->prio_estatica = prio;
    #ifdef DEBUG
        printf ("task_setprio: saindo\n") ;
    #endif
}

int task_getprio (task_t *task){
    #ifdef DEBUG
        printf ("task_getprio: entrando\n") ;
    #endif
    if (task == NULL) {
        task = current_task;
    }
    #ifdef DEBUG
        printf ("task_getprio: saindo\n") ;
    #endif
    return task->prio_estatica;
}

void dispatcher(){
    // queue_remove(&task_queue, (queue_t *) dispatcher_task);
    #ifdef DEBUG
        printf ("dispatcher: entrando\n") ;
    #endif
    task_t *next;
    while (1){
        #ifdef DEBUG
            printf ("dispatcher: %d tarefas na fila\n", queue_size(task_queue));
        #endif
        next = scheduler();
        if (!next)
            break;
        // next->status = 1; // status = 1 significa que a tarefa está rodando
        task_switch(next);
        // queue_append(&task_queue, (queue_t*) next);
        switch (next->status){
            case 0: // tarefa pronta
                break;
            case 1: // tarefa rodando
                break;
            default:
                break;
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
    current_task->status = 0;
    queue_append((queue_t **)&task_queue, (queue_t*) current_task);
    task_switch(&dispatcher_task);
    #ifdef DEBUG
        printf ("task_yield: saindo\n") ;
    #endif
}

void ppos_init (){
    #ifdef DEBUG
        printf ("ppos_init: entrando\n") ;
    #endif
    setvbuf (stdout, 0, _IONBF, 0) ;
    
    current_task = &main_task;
    #ifdef DEBUG
        printf ("criando tarefa: dispatcher\n") ;
    #endif
    task_init(&dispatcher_task, dispatcher, NULL);
    main_task.id = task_id_counter++;
    main_task.status = 0; // status = 0 significa que a tarefa está pronta 
    queue_append((queue_t **)&task_queue, (queue_t*) &main_task);
    task_switch(&dispatcher_task);
    #ifdef DEBUG
        printf ("ppos_init: saindo\n") ;
    #endif

}

int task_init (task_t *task, void (*start_routine)(void *),  void *arg){
    #ifdef DEBUG
        printf ("task_init: entrando\n") ;
    #endif
    // ucontext_t new_context;

    getcontext (&task->context) ;

    char *stack = malloc (STACKSIZE) ;
    if (stack)
    {
        task->context.uc_stack.ss_sp = stack ;
        task->context.uc_stack.ss_size = STACKSIZE ;
        task->context.uc_stack.ss_flags = 0 ;
        task->context.uc_link = 0 ;
    }
    else
    {
       perror ("Erro na criação da pilha: ") ;
       return -1 ;
    }
 
    
    makecontext (&task->context, (void*)start_routine, 1, arg) ;
    
    // task->context = new_context;
    task->status = 0; // status = 0 significa que a tarefa está pronta
    task->prio_dinamica = 0; // prioridade padrão
    task->prio_estatica = 0; // prioridade padrão
    // task->prev = NULL;
    // task->next = NULL;
    task->id = task_id_counter++;

    if (task != &dispatcher_task) {
        queue_append((queue_t **)&task_queue, (queue_t*) task);
    }

    // task_yield();
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
    // if (current_task != main_task) {
    //     queue_remove(&task_queue, (queue_t*)current_task);
    // }
    if (current_task == &dispatcher_task) {
        // exit(exit_code);
        exit_code = exit_code;
    } else {
        current_task->status = 2; 
        task_switch(&dispatcher_task);
    }
    // exit(exit_code);
    // free(current_task->context.uc_stack.ss_sp);
    #ifdef DEBUG
        printf ("task_exit: saindo\n") ;
    #endif
}

int task_id (){
    return current_task->id;
}