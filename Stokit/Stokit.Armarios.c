#include <stdio.h>
#include <stdlib.h>
// External
#include "Stokit.Common.h"
#include "Stokit.Produtos.h"

pArmario newArmario(pCorredor parent, pArmario p)
{
    pArmario aux;
    //Novo elemento
    aux = malloc(sizeof(Armario));
    if (!aux)
    {
        printf("Não foi possível alocar memória\n");
        return p;
    }
    if (!p)
    {
        //Primeiro elemento
        aux->ID = 1;
    }
    else
    {
        //Próximo elemento - sequencial
        aux->ID = p->ID + 1;
        p->next = aux;
    }
    if (parent)
    {
        if (!parent->armarios)
            parent->armarios = aux;
        parent->lastArmario = aux;
    }
    aux->produtosTotal = 0;
    aux->lastProduto = NULL;
    aux->parent = parent;
    aux->prev = p;
    aux->next = NULL;
    return aux;
}

void freeArmario(pArmario p)
{
    pArmario aux;
    while (p)
    {
        if (p->produtos)
        {
            freeProduto(p->produtos);
            p->produtos = NULL;
        }
        aux = p;
        p = p->next;
        free(aux);
    }
}

void deleteArmario(pCorredor parent, pArmario p)
{
    //Se for primeiro elemento
    
    //Se for elemento interior, sem ser dos extremos
    
    //Ultimo elemento
    if (p->prev && !p->next)
    {
        if (parent)
        {
            parent->lastArmario = p->prev;
        }
        p->prev->next = NULL;
        freeArmario(p);
    }
}

pArmario getArmario(pCorredor listDB, int IDCorredor, int IDArmario)
{
    pArmario auxArmario;
    while (listDB)
    {
        //Verificar o corredor
        if (listDB->ID != IDCorredor)
        {
            listDB = listDB->next;
            continue;
        }
        //Verificar o armário
        if (!listDB->armarios) return NULL;
        auxArmario = listDB->armarios;
        while (auxArmario)
        {
            //Armário conicide
            if (auxArmario->ID == IDArmario)
                return auxArmario;
            auxArmario = auxArmario->next;
        }
        break;
    }
    return NULL;
}

pArmario getArmarioComMenorProdutos(pDatabase db)
{
    if (!db) return NULL;
    pCorredor auxCorredor;
    pArmario auxArmario, menorArmario = NULL;
    //Verificar em cada corredor o total de produtos de cada armário
    auxCorredor = db->corredores;
    while (auxCorredor)
    {
        auxArmario = auxCorredor->armarios;
        if (!menorArmario)
          menorArmario = auxArmario;
        while (auxArmario)
        {
            //Obter o armário com menor número de produtos
            if (menorArmario->produtosTotal > auxArmario->produtosTotal)
            {
                menorArmario = auxArmario;
                //O mínimo de produtos é 0, por isso, não vale a pena continuar
                if (menorArmario->produtosTotal == 0)
                    return menorArmario;
            }
            auxArmario = auxArmario->next;
        }
        auxCorredor = auxCorredor->next;
    }
    return menorArmario;
}
