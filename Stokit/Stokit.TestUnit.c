#include <stdio.h>
#include <string.h>
#include "Stokit.Common.h"

void testReposicaoStock(pDatabase db)
{
    char fname[64];
    printf("\nIndique o ficheiro:\n");
    /*Necessita de ter um \n antes do fgets para ler a linha*/
    fgets(fname,sizeof(fname),stdin);
    int n = loadReposicaoStock(fname,db);
    printf("\n%d produtos\n",n);
    if (n) showCorredores(db->corredores,1);
}

void testSatisfazerEncomenda(pDatabase db)
{
    char fname[64];
    printf("\nIndique o ficheiro:\n");
    /*Necessita de ter um \n antes do fgets para ler a linha*/
    fgets(fname,sizeof(fname),stdin);
    int n = loadEncomenda(fname,db);
    printf("\n%d produtos\n",n);
    if (n) showCorredores(db->corredores,1);
}