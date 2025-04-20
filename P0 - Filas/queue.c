/*

Nome: Mateus Kater Pombeiro

GRR: 20190366

*/

#include <stdio.h>
#include <stdlib.h>
#include "queue.h"

int queue_size(queue_t *queue)
{
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

    return 0;
}

int queue_remove(queue_t **queue, queue_t *elem)
{
    if (queue == NULL || *queue == NULL)
    {
        printf("Erro ao remover elemento da fila: Fila nao existe\n");
        return -1;
    }
    if (elem == NULL){
        printf("Erro ao remover elemento da fila: Elemento nao existe\n");
        return -1;
    }

    queue_t *aux = *queue;
    do
    {
        if (aux == elem)
        {
            if (aux == *queue)
            {
                if (aux->next == aux)
                {
                    *queue = NULL;
                }
                else
                {
                    *queue = aux->next;
                }
            }

            aux->prev->next = aux->next;
            aux->next->prev = aux->prev;
            aux->next = NULL;
            aux->prev = NULL;
            return 0;
        }
        aux = aux->next;
    } while (aux != *queue);
    printf("Erro ao remover elemento da fila: Elemento nao pertence a fila indicada\n");
    return -1;
}

