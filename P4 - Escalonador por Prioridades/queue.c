/*

Nome: Mateus Kater Pombeiro

GRR: 20190366

*/

#include <stdio.h>
#include <stdlib.h>
#include "queue.h"

int queue_size(queue_t *queue)
{
    #ifdef DEBUG
        printf("queue_size: entrando\n");
    #endif
    if (queue == NULL)
    {
        return 0;
    }

    int count = 1;
    queue_t *current, *first;

    first = queue->next;
    current = first->next;

    while (current != first)
    {
        count++;
        current = current->next;
    }

    #ifdef DEBUG
        printf("queue_size: entrando\n");
    #endif
    return count;
}

void queue_print(char *name, queue_t *queue, void print_elem(void *))
{
    printf("%s: [", name);

    if (queue != NULL)
    {
        queue_t *aux = queue;
        do
        {
            print_elem(aux);
            aux = aux->next;
            if (aux != queue)
            {
                printf(" ");
            }
        } while (aux != queue);
    }

    printf("]\n");
}

int queue_append(queue_t **queue, queue_t *elem)
{
    #ifdef DEBUG
        printf("queue_append: entrando\n");
    #endif
    if (queue == NULL)
    {
        printf("Erro ao adicionar elemento na fila: Fila nao existe\n");
        return -1;
    }
    if (elem == NULL)
    {
        printf("Erro ao adicionar elemento na fila: Elemento nao existe\n");
        return -1;
    }
    if (elem->next != NULL || elem->prev != NULL)
    {
        printf("Erro ao adicionar elemento na fila: Elemento ja esta em outra fila\n");
        return -1;
    }

    if (*queue == NULL)
    {
        *queue = elem;
        elem->next = elem;
        elem->prev = elem;
    }
    else
    {
        queue_t *last = (*queue)->prev;
        last->next = elem;
        elem->prev = last;
        elem->next = *queue;
        (*queue)->prev = elem;
        // last->next->prev = elem;
    }

    #ifdef DEBUG
        printf("queue_append: saindo\n");
    #endif
    return 0;
}

int queue_remove(queue_t **queue, queue_t *elem)
{
    #ifdef DEBUG
        printf("queue_remove: entrando\n");
    #endif

    if (queue == NULL || *queue == NULL)
    {
        printf("Erro ao remover elemento da fila: Fila nao existe\n");
        return -1;
    }
    if (elem == NULL){
        printf("Erro ao remover elemento da fila: Elemento nao existe\n");
        return -1;
    }

    queue_t *aux = (*queue)->next;

    while (aux != elem && aux != *queue){
        aux = aux->next;
    }
    if (aux == *queue && aux != elem){
        printf("Erro ao remover elemento da fila: Elemento nao pertence a fila indicada\n");
        return -1;
    }
    if (elem == *queue)
        *queue = (*queue)->next;

    if ((*queue)->next == (*queue))
        *queue = NULL;
    
    queue_t *prev = elem->prev;
    queue_t *next = elem->next;
    prev->next = next;
    next->prev = prev;

    elem->next = NULL;
    elem->prev = NULL;

    #ifdef DEBUG
        printf("queue_remove: saindo\n");
    #endif

    return 0;
}

