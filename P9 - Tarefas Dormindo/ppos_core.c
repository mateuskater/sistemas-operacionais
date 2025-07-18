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
#include <signal.h>
#include <sys/time.h>

#define READY 0
#define RUNNING 1
#define SUSPENDED 2
#define FINISHED 3

#define STACKSIZE 32*1024
#define QUANTUM_RESET 20
int task_id_counter = 0;
task_t main_task;
task_t dispatcher_task;
task_t *current_task = &main_task; 
ucontext_t main_context;
unsigned int system_time = 0;

queue_t *task_queue;
queue_t *sleep_queue;

struct sigaction action;
struct itimerval timer;

void task_sleep(int t){
    // suspende a tarefa atual por t milissegundos
    #ifdef DEBUG
        printf ("task_sleep: entrando\n") ;
    #endif
    if (t <= 0 || current_task == &dispatcher_task){
        return;
    }
    // current_task->status = SUSPENDED; // muda o status da tarefa para suspensa
    // current_task->start_time = systime(); // armazena o tempo de início da tarefa
    // current_task->quantum = 0; // zera o quantum da tarefa
    current_task->wake_time = systime() + t; // armazena o tempo em que a tarefa deve ser acordada

    task_suspend((task_t**)&sleep_queue); // suspende a tarefa atual
    // queue_append((queue_t **)&sleep_queue, (queue_t*) current_task); // adiciona a tarefa na fila de tarefas suspensas
    task_switch(&dispatcher_task); // transfere o controle para o dispatcher
    #ifdef DEBUG
        printf ("task_sleep: saindo\n") ;
    #endif
}

int task_wait (task_t *task){
    // espera a tarefa indicada terminar
    #ifdef DEBUG
        printf ("task_wait: entrando\n") ;
    #endif
    if (task == NULL || task == current_task || task->status == FINISHED){
        return -1;
    }
    task_suspend(&task->suspended_queue); // suspende a tarefa atual
    task_awake(current_task, &task->suspended_queue); // acorda a tarefa atual
    #ifdef DEBUG
        printf ("task_wait: saindo\n") ;
    #endif
    return task->exit_code;
}

void task_suspend (task_t **queue){
    // suspende a tarefa atual
    #ifdef DEBUG
        printf ("task_suspend: entrando\n") ;
    #endif
    if (current_task == &dispatcher_task) {
        return;
    }
    if (queue_search(task_queue, (queue_t *)current_task)){
        queue_remove((queue_t **)&task_queue, (queue_t*) current_task); // remove a tarefa da fila
    }
    current_task->status = SUSPENDED; // muda o status da tarefa para suspensa
    if (queue != NULL && !queue_search((queue_t*)*queue, (queue_t *)current_task)){
        queue_append((queue_t **)queue, (queue_t*) current_task); // adiciona a tarefa na fila de tarefas
    }
    // task_yield();
    task_switch(&dispatcher_task);
    #ifdef DEBUG
        printf ("task_suspend: saindo\n") ;
    #endif
}

void task_awake (task_t * task, task_t **queue){
    // acorda a tarefa indicada
    #ifdef DEBUG
        printf ("task_awake: entrando\n") ;
    #endif
    if (task == NULL) {
        return;
    }
    if (queue != NULL && *queue != NULL && queue_search((queue_t*)*queue, (queue_t *)task)){
        queue_remove((queue_t **)queue, (queue_t*) task); // remove a tarefa da fila
    }
    task->status = READY; // muda o status da tarefa para pronta
    queue_append((queue_t **)&task_queue, (queue_t*) task); // adiciona a tarefa na fila de tarefas
    #ifdef DEBUG
        printf ("task_awake: saindo\n") ;
    #endif
}

void tick(){
    system_time++;
    if(current_task->user_task){
        current_task->quantum--;
        current_task->exec_time++;
        if(current_task->quantum == 0)
            task_yield();
    }
}

unsigned int systime(){
  return system_time;
}

task_t *scheduler(){
    // escalonador de tarefas
    // retorna a próxima tarefa a ser executada
    #ifdef DEBUG
        printf ("scheduler: entrando\n") ;
    #endif
    if (queue_size(task_queue) == 0){
        #ifdef DEBUG
            printf ("scheduler: saindo\n") ;
        #endif
        return NULL;
    }
    queue_t *cur_q = task_queue; // ponteiro para o nodo atual da fila
    task_t *cur_task; // ponteiro para a tarefa atual
    task_t *next = (task_t*)cur_q; // ponteiro para a próxima tarefa

    do{
        cur_task = (task_t *) cur_q;
        if (cur_task->prio_dinamica < next->prio_dinamica){
            next = cur_task;
            // cur_task = next;
        }
        cur_q = cur_q->next;
    }while (cur_q != task_queue);

    cur_q = task_queue;
    do {
        cur_task = (task_t*) cur_q;
        if (cur_task != next && cur_task->prio_dinamica > -20){
            cur_task->prio_dinamica--;
        }
        cur_q = cur_q->next;
    } while (cur_q != task_queue);

    next->prio_dinamica = next->prio_estatica;
    queue_remove((queue_t **) &task_queue, (queue_t *) next); // remove a tarefa da fila 
    #ifdef DEBUG
        printf ("scheduler: tarefa %d\n", next->id) ;
        printf ("scheduler: %d tarefas na fila\n", queue_size(task_queue));
    #endif
    next->quantum = QUANTUM_RESET;
    return next;
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
    // responsavel por executar as tarefas uma após a outra
    // queue_remove(&task_queue, (queue_t *) dispatcher_task);
    #ifdef DEBUG
        printf ("dispatcher: entrando\n") ;
    #endif
    task_t *next; // ponteiro para a próxima tarefa
    
    // Modificado: continua rodando enquanto houver tarefas na fila de prontas OU na fila de adormecidas
    while (queue_size(task_queue) > 0 || sleep_queue != NULL){
        #ifdef DEBUG
            printf ("dispatcher: %d tarefas na fila\n", queue_size(task_queue));
        #endif
        
        // Check sleeping tasks first to wake up any that are ready
        if (sleep_queue != NULL) {
            task_t *cur_task = (task_t*)sleep_queue; // ponteiro para a tarefa atual
            task_t *next_task;
            do {
                next_task = cur_task->next != (task_t*)sleep_queue ? (task_t*)cur_task->next : NULL;
                if (cur_task->wake_time <= systime()) {
                    task_awake(cur_task, (task_t**)&sleep_queue); // acorda a tarefa
                    #ifdef DEBUG
                        printf ("dispatcher: acordando tarefa %d\n", cur_task->id) ;
                    #endif
                }
                cur_task = next_task; // atualiza o ponteiro para a próxima tarefa
            } while (sleep_queue != NULL && next_task != NULL && next_task != (task_t*)sleep_queue);
        }
        
        // Process ready tasks only if there are any
        if (queue_size(task_queue) > 0) {
            next = scheduler(); // chama o escalonador para pegar a próxima tarefa
            if (next){
                next->status = RUNNING; 
                task_switch(next); // troca o contexto da tarefa atual pelo da próxima tarefa
                switch (next->status){
                    case READY: // tarefa pronta
                        break;
                    case RUNNING: // tarefa rodando
                        next->status = READY; // muda o status da tarefa para pronta
                        queue_append((queue_t **)&task_queue, (queue_t*) next); // adiciona a tarefa na fila de tarefas
                        break;
                    case SUSPENDED: // tarefa suspensa
                        break;
                    default:
                        break;
                }
            }
        } else {
            // No ready tasks but there are sleeping tasks
            // Need to wait until a task wakes up
            // A simple approach is to yield the CPU momentarily
            // This allows the timer to advance and possibly wake up tasks
            // Since we're in the dispatcher, the yield will just return here
            // This prevents busy-waiting in a tight loop
            task_yield();
        }
    }
    task_exit(0); // encerra o dispatcher
    #ifdef DEBUG
        printf ("dispatcher: saindo\n") ;
    #endif
}

void task_yield (){
    #ifdef DEBUG
        printf ("task_yield: entrando\n") ;
    #endif
    // se a tarefa atual for o dispatcher, não faz nada
    if (current_task != &dispatcher_task){ 
        if (current_task->status != SUSPENDED){
            current_task->status = READY; // muda o status da tarefa para pronta
            queue_append((queue_t **)&task_queue, (queue_t*) current_task);
        }
    }
    task_switch(&dispatcher_task);
    #ifdef DEBUG
        printf ("task_yield: saindo\n") ;
    #endif
}

void ppos_init (){
    #ifdef DEBUG
        printf ("ppos_init: entrando\n") ;
    #endif
    setvbuf (stdout, 0, _IONBF, 0) ; // desabilita o buffer da saída padrão

    action.sa_handler = tick;
    sigemptyset (&action.sa_mask) ;
    action.sa_flags = 0 ;
    sigaction(SIGALRM, &action,0);
    timer.it_value.tv_usec = 1000;
    timer.it_value.tv_sec = 0;
    timer.it_interval.tv_usec = 1000;
    timer.it_interval.tv_sec = 0;

    setitimer(ITIMER_REAL, &timer, 0);

    task_queue = NULL; // inicializa a fila de tarefas
    sleep_queue = NULL; // inicializa a fila de tarefas suspensas
    current_task = &main_task;
    #ifdef DEBUG
        printf ("criando tarefa: dispatcher\n") ;
    #endif
    main_task.id = 0;
    task_init(&dispatcher_task, dispatcher, NULL); // cria a tarefa dispatcher
    main_task.status = READY;
    main_task.prio_dinamica = 0; // prioridade padrão
    main_task.prio_estatica = 0; // prioridade padrão
    main_task.prev = NULL;
    main_task.next = NULL;
    main_task.context = main_context; // inicializa o contexto da tarefa main
    main_task.user_task = 0;

    queue_append((queue_t **)&task_queue, (queue_t*) &main_task);
    task_switch(&dispatcher_task); // transfere o controle para o dispatcher
    #ifdef DEBUG
        printf ("ppos_init: saindo\n") ;
    #endif

}

int task_init (task_t *task, void (*start_routine)(void *),  void *arg){
    #ifdef DEBUG
        printf ("task_init: entrando\n") ;
    #endif

    getcontext (&task->context) ; // pega o contexto atual da tarefa

    char *stack = malloc (STACKSIZE) ; // aloca a pilha para a tarefa
    if (stack){
        task->context.uc_stack.ss_sp = stack ;
        task->context.uc_stack.ss_size = STACKSIZE ;
        task->context.uc_stack.ss_flags = 0 ;
        task->context.uc_link = 0 ;
    }else{
       perror ("Erro na criação da pilha: ") ;
       return -1 ;
    }    
    makecontext (&task->context, (void*)start_routine, 1, arg) ; // cria o contexto da tarefa
    
    task->status = READY; // status = 0 significa que a tarefa está pronta
    task->prio_dinamica = 0; // prioridade padrão
    task->prio_estatica = 0; // prioridade padrão
    task->prev = NULL;
    task->next = NULL;
    task->id = task_id_counter++;
    task->user_task = 0;
    task->quantum = 0;
    task->start_time = systime();
    task->exec_time = 0;
    task->activations = 0;
    task->suspended_queue = NULL; // fila de tarefas suspensas
    if (task != &dispatcher_task) { // não adiciona o dispatcher na fila de tarefas
        task->user_task = 1;
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
    task_t *prev_task = current_task; // ponteiro para a tarefa atual
    current_task = task; // atualiza a tarefa atual
    current_task->status = RUNNING; // muda o status da tarefa para rodando
    current_task->activations++; // incrementa o contador de ativações da tarefa
    swapcontext (&(prev_task->context), &(current_task->context)) ; // troca o contexto da tarefa atual pelo da próxima tarefa
    #ifdef DEBUG
        printf ("task_switch: saindo\n") ;
    #endif

    return 0;
}

void task_exit (int exit_code){
    #ifdef DEBUG
        printf ("task_exit: entrando\n") ;
    #endif
    printf("Task %d exit: execution time %d ms, processor time %d ms, %d activations\n",current_task->id,
             systime() - current_task->start_time, current_task->exec_time, current_task->activations);
    if (current_task == &dispatcher_task) {
        task_switch(&main_task); // transfere o controle para a tarefa main
        // exit_code = exit_code;
    } else {
        current_task->exit_code = exit_code; // armazena o código de saída da tarefa
        current_task->status = FINISHED; // muda o status da tarefa para finalizada
        while (current_task->suspended_queue != NULL) {
            task_awake(current_task->suspended_queue, &current_task->suspended_queue);
        }
        task_switch(&dispatcher_task); // ttransfere o controle para o dispatcher
    }
    #ifdef DEBUG
        printf ("task_exit: saindo\n") ;
    #endif
}

int task_id (){
    return current_task->id;
}