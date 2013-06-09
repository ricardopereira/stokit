#include <stdio.h>
#include <string.h>
#include "Stokit.Common.h"

void showCorredores(pCorredor p, int showProdutos)
{
    pCorredor auxCorredor;
    pArmario auxArmario;
    pProduto auxProduto;
    
    auxCorredor = p;
    while (auxCorredor)
    {
        auxArmario = auxCorredor->armarios;
        while (auxArmario)
        {
            printf("\nA%d.%d",auxCorredor->ID,auxArmario->ID);
            if (showProdutos && auxArmario->produtos)
            {
                printf("\n Produtos: %d",auxArmario->produtosTotal);
                auxProduto = auxArmario->produtos;
                while (auxProduto)
                {
                    printf("\n P%d: %d",auxProduto->num,auxProduto->qtd);
                    auxProduto = auxProduto->next;
                }
            }
            auxArmario = auxArmario->next;
        }
        printf("\n");
        auxCorredor = auxCorredor->next;
    }
}

void forEachArmario(pDatabase db, pProduto p, funcArmario doSomething)
{
    pCorredor auxCorredor;
    pArmario auxArmario;
    if (!db) return;
    auxCorredor = db->corredores;
    while (auxCorredor)
    {
        auxArmario = auxCorredor->armarios;
        while (auxArmario)
        {
            doSomething(auxArmario,p);
            auxArmario = auxArmario->next;
        }
        auxCorredor = auxCorredor->next;
    }    
}

void removeBreakLine(char *result)
{
    size_t ln = strlen(result)-1;
    if (result[ln] == '\n')
        result[ln] = '\0';
}

void getFullPath(char *result, int size, char *path, char *filename)
{
    /*Concatenar para o caminho final*/
    snprintf(result,size,"%s%s",path,filename);
    /*Retirar o \n atribuído ao fazer fgets*/
    removeBreakLine(result);
}