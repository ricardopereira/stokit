#include <stdio.h>
#include <string.h>

#include "Stokit.Common.h"
#include "Stokit.DB.h"
#include "Stokit.Reposicao.h"
#include "Stokit.Encomendas.h"
#include "Stokit.Armarios.h"
#include "Stokit.Corredores.h"

void testReposicaoStock(pDatabase db)
{
    char fname[64];
    char path[MAXPATH];
    pArmario listaProdutos;
    printf("\nIndique o ficheiro:\n");
    /*Necessita de ter um \n antes do fgets para ler a linha*/
    fgets(fname,sizeof(fname),stdin);
    /*Concatenar para o caminho final*/
    getFullPath(path,sizeof(path),PathReposicao,fname);
    listaProdutos = loadReposicaoStock(path,db);
    printf("\n%d produtos\n",listaProdutos->produtosTotal);
    freeArmario(listaProdutos);
}

void testSatisfazerEncomenda(pDatabase db)
{
    char fname[64];
    char path[MAXPATH];
    pEncomenda encomenda;
    printf("\nIndique o ficheiro:\n");
    /*Necessita de ter um \n antes do fgets para ler a linha*/
    fgets(fname,sizeof(fname),stdin);
    /*Concatenar para o caminho final*/
    getFullPath(path,sizeof(path),PathReposicao,fname);
    encomenda = loadEncomenda(path,db);
    printf("\n%d produtos\n",encomenda->produtosTotal);
    freeEncomenda(encomenda);
}