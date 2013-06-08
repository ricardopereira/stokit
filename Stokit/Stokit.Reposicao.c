#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "Stokit.Common.h"
#include "Stokit.DB.h"
#include "Stokit.Produtos.h"
#include "Stokit.Armarios.h"

pArmario getReposicaoStock(FILE *f)
{
    int num, qtd;
    pArmario resArmario;
    pProduto auxProduto = NULL;
    //Utilizar a lista de produtos com o contador
    resArmario = newArmario(NULL,NULL);
    if (f)
    {
        while(!feof(f))
        {
            //Lê a linha de reposição
            fscanf(f,"%d:%d",&num,&qtd);
            //Adiciona elemento
            auxProduto = newProduto(resArmario,auxProduto,num,qtd);
            //Mostra o que foi lido
            //printf("\nP%d:%d",auxProduto->num,auxProduto->qtd);
        }
    }
    else
        printf("(getReposicaoStock)Erro: sem ficheiro\n");
    return resArmario;
}

int doReporStock(pDatabase db, pProduto list)
{
    //Verificar se existe algum produto a repor
    if (!db || !list) return 0;
    //Lista de reposição de stock
    pProduto repProduto,auxProduto;
    pArmario auxArmario;
    int total = 0;
    repProduto = list;
    while (repProduto)
    {
        auxProduto = getProdutoByNum(db->corredores,repProduto->num);
        //Encontrado o produto?
        if (auxProduto)
        {
            auxProduto->qtd += repProduto->qtd;
        }
        else
        {
            //Encontrar o armário com menor número de produtos para adicionar o novo produto
            auxArmario = getArmarioComMenorProdutos(db);
            newProduto(auxArmario, auxArmario->lastProduto, repProduto->num, repProduto->qtd);
        }
        total++;
        repProduto = repProduto->next;
    }
    return total;
}

int loadReposicaoStock(char *filename, pDatabase db)
{
    FILE *f = NULL;
    char path[256];
    pArmario listaProdutos = NULL;
    pProduto auxProduto;
    int res = 0;
    
    //Concatenar para o caminho final
    getFullPath(path,sizeof(path),PathReposicao,filename);
    
    //Ler o ficheiro de texto
    f = fopen(path,"r");
    if (f)
    {
        listaProdutos = getReposicaoStock(f);
    }
    else
        printf("(loadReposicaoStock)Erro: ficheiro não existe");
    fclose(f);
    
    //Lista com os produtos a repor
    if (listaProdutos)
    {
        //Mostra o que foi lido
        auxProduto = listaProdutos->produtos;
        while (auxProduto)
        {
            printf("\nProduto %d:%d",auxProduto->num,auxProduto->qtd);
            auxProduto = auxProduto->next;
        }
        
        //Repor Stock
        doReporStock(db,listaProdutos->produtos);
        
        res = listaProdutos->produtosTotal;
    }
    freeArmario(listaProdutos);
    return res;
}