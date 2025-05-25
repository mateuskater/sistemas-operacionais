// PingPongOS - PingPong Operating System
// Prof. Carlos A. Maziero, DINF UFPR
// Versão 1.5 -- Março de 2023

// Estruturas de dados internas do sistema operacional

#ifndef __PPOS_DATA__
#define __PPOS_DATA__

#include <ucontext.h>		// biblioteca POSIX de trocas de contexto

// Estrutura que define um Task Control Block (TCB)
typedef struct task_t
{
  struct task_t *prev, *next ;		// ponteiros para usar em filas
  int id ;				// identificador da tarefa
  ucontext_t context ;			// contexto armazenado da tarefa
  short status ;			// pronta, rodando, suspensa, ...
  int prio_estatica;      // prioridade estatica
  int prio_dinamica;     // prioridade dinamica
  short user_task; // booleano pra user task ou nao
  short quantum;
  unsigned int start_time;
  unsigned int exec_time; // tempo de execução da tarefa
  unsigned int activations; // contagem de ativações da tarefa
  int exit_code;
  struct task_t *suspended_queue; // fila de tarefas suspensas
  unsigned int wake_time; // tempo em que a tarefa deve ser acordada
} task_t ;

// estrutura que define um semáforo
typedef struct semaphore
{
  // preencher quando necessário
} semaphore_t ;

// estrutura que define um mutex
typedef struct mutex
{
  // preencher quando necessário
} mutex_t ;

// estrutura que define uma barreira
typedef struct barrier
{
  // preencher quando necessário
} barrier_t ;

// estrutura que define uma fila de mensagens
typedef struct mqueue
{
  // preencher quando necessário
} mqueue_t ;

#endif
