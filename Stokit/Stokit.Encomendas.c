#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "Stokit.Common.h"
#include "Stokit.DB.h"
#include "Stokit.Produtos.h"
#include "Stokit.Armarios.h"

pEncomenda getEncomenda(FILE *f)
{
    int num, qtd;
    pEncomenda resEncomenda;
    pProduto auxProduto = NULL;
    char buffer[512];
    resEncomenda = malloc(sizeof(Encomenda));
    resEncomenda->lastProduto = NULL;
    resEncomenda->produtosTotal = 0;
    if (f)
    {
        while (!feof(f))
        {
            //Lê a linha da encomenda
            if (fscanf(f,"%d:%d",&num,&qtd) == 2)
            {
                //Adiciona o produto
                auxProduto = addProdutoEncomenda(resEncomenda,num,qtd);
            }
            else if (fgets(buffer,sizeof(buffer),f))
            {
                //Ler nome da encomenda
                removeBreakLine(buffer);
                puts(buffer);
            }
            else
                fseek(f,1,SEEK_CUR);
        }
    }
    else
        printf("(getEncomenda)Erro: sem ficheiro\n");
    return resEncomenda;
}

int doSatisfazerEncomenda(pDatabase db, pEncomenda enc)
{
    //Verificar se existe algum produto a encomendar
    if (!db || !enc) return 0;
    pProduto encProduto,auxProduto;
    //Guardar cada produto que foi alterado
    pProduto* trackChanges = calloc(enc->produtosTotal,sizeof(pArmario));
    
    int total = 0, needRollback = 0, i = 0;
    //Lista da encomenda
    encProduto = enc->produtos;
    while (encProduto)
    {
        auxProduto = getProdutoByNum(db->corredores,encProduto->num);
        //Encontrado o produto?
        if (auxProduto && auxProduto->qtd >= encProduto->qtd)
        {
            //Satisfazer a encomenda
            if (!needRollback)
            {
                auxProduto->lastQtd = auxProduto->qtd;
                auxProduto->qtd -= encProduto->qtd;
                trackChanges[total++] = auxProduto;
                
                //ToDo - gerar relatório da venda dos produtos
            }
        }
        else
        {
            //Criar aviso para encomendar o produto
            encProduto->needStock = 1;
            needRollback = 1;
        }
        encProduto = encProduto->next;
    }
    //Rollback
    if (needRollback)
    {
        //Colocar o produto como estava antes da satisfação da encomenda
        auxProduto = trackChanges[i];
        while (auxProduto)
        {
            auxProduto->qtd = auxProduto->lastQtd;
            auxProduto = trackChanges[++i];
        }
    }
    free(trackChanges);
    return total;
}

void freeEncomenda(pEncomenda p)
{
    if (p)
    {
        if (p->produtos)
        {
            freeProduto(p->produtos);
            p->produtos = NULL;
        }
        free(p);
    }
}

void checkEncomenda(pEncomenda enc)
{
    if (!enc) return;
    pProduto auxProduto;
    auxProduto = enc->produtos;
    if (auxProduto)
        printf("\nEncomendar os seguintes produtos:");
    while (auxProduto)
    {
        if (auxProduto->needStock)
        {
            printf("\nP%d: %d",auxProduto->num,auxProduto->qtd);
        }
        auxProduto = auxProduto->next;
    }
}

void doRouteProduto(pArmario armario, pProduto p)
{
    pProduto auxProduto, routeProduto;
    int showCabecalho = 1;
    //Para cada armário
    if (armario && p)
    {
        //Indicar o caminho a percorrer para cada produto
        routeProduto = p;
        while (routeProduto)
        {
            //Tenta encontrar o produto no armário atual
            auxProduto = findProduto(armario,routeProduto->num);
            if (auxProduto)
            {
                if (showCabecalho)
                {
                    printf("\nIr ao corredor %d, armario %d e retirar:",armario->parent->ID,armario->ID);
                    showCabecalho = 0;
                }
                printf("\n P%d: %d unidades",
                       routeProduto->num,routeProduto->qtd);
            }
            routeProduto = routeProduto->next;
        }
    }
}

void showRouteEncomenda(pDatabase db, pEncomenda enc)
{
    /*ToDo - poderá necessitar de optimização*/
    forEachArmario(db,enc->produtos,doRouteProduto);
}

int loadEncomenda(char *filename, pDatabase db)
{
    FILE *f = NULL;
    char path[256];
    pEncomenda encomenda = NULL;
    pProduto auxProduto;
    char satisfazerEncomenda;
    int res = 0;
    
    //Concatenar para o caminho final
    getFullPath(path,sizeof(path),PathEncomendas,filename);
    
    //Ler o ficheiro de texto
    f = fopen(path,"r");
    if (f)
    {
        encomenda = getEncomenda(f);
    }
    else
        printf("(loadEncomenda)Erro: ficheiro não existe");
    fclose(f);
    
    //Encomenda com os produtos
    if (encomenda)
    {
        //Mostra o que foi lido
        auxProduto = encomenda->produtos;
        while (auxProduto)
        {
            printf("\nProduto %d:%d",auxProduto->num,auxProduto->qtd);
            auxProduto = auxProduto->next;
        }
        
        //Caminho da encomenda
        printf("\n");
        showRouteEncomenda(db,encomenda);
        printf("\n");
        
        //Verificar se é para satisfazer
        printf("\nDeseja satisfazer a encomenda? (s/n)");
        satisfazerEncomenda = getchar();
        if (satisfazerEncomenda == 's')
        {
            doSatisfazerEncomenda(db,encomenda);
            res = encomenda->produtosTotal;
            
            //Mostrar avisos
            checkEncomenda(encomenda);
        }
    }
    freeEncomenda(encomenda);
    return res;
}