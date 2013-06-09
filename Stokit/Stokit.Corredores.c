#include <stdio.h>
#include <stdlib.h>

#include "Stokit.Common.h"
#include "Stokit.Armarios.h"

pCorredor newCorredor(pCorredor p)
{
    pCorredor aux;
    /*Novo elemento*/
    aux = malloc(sizeof(Corredor));
    if (!aux)
    {
        printf("Não foi possível alocar memória\n");
        return p;
    }
    if (!p)
    {
        /*Primeiro elemento*/
        aux->ID = 1;
    }
    else
    {
        /*Próximo elemento - sequencial*/
        aux->ID = p->ID + 1;
        p->next = aux;
    }
    aux->prev = p;
    aux->next = NULL;
    return aux;
}

void freeCorredor(pCorredor p)
{
    pCorredor aux;
    while (p)
    {
        if (p->armarios)
        {
            freeArmario(p->armarios);
            p->armarios = NULL;
        }
        aux = p;
        p = p->next;
        free(aux);
    }
}

void deleteCorredor(pCorredor p)
{
    /*Se for primeiro elemento*/
    
    /*Se for elemento interior, sem ser dos extremos*/
    
    /*Ultimo elemento*/
    if (p->prev && !p->next)
    {
        /*lastCorredor*/
        
        p->prev->next = NULL;
        freeCorredor(p);
    }
}
