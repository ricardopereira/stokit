#include <stdio.h>
#include <stdlib.h>

#include "Stokit.Common.h"
#include "Stokit.Corredores.h"
#include "Stokit.Armarios.h"
#include "Stokit.Produtos.h"

void freeDB(pDatabase p)
{
    freeCorredor(p->corredores);
    free(p);
}

pDatabase readFile(FILE *f)
{
    int aux;
    int idx=0, idxCorredor, idxArmario, idxProduto;
    int maxProdutos;
    int doNextCorredor, doNextArmario;
    
    if (!f)
    {
        printf("(readFile)Erro: sem ficheiro\n");
        return NULL;
    }
    
    pDatabase result = malloc(sizeof(Database));
    pCorredor auxCorredor = NULL;
    pArmario auxArmario;
    pProduto auxProduto;
    //Inicializar
    result->corredores = NULL;
    result->maxCorredores = 0;
    result->maxArmarios = 0;
    
    //Inicialização
    idxCorredor = 0;
    doNextCorredor = 1;
    idxArmario = 0;
    doNextArmario = 1;
    
    //Interpretar o ficheiro
    while (fread(&aux,sizeof(int),1,f) == 1)
    {
        //Número de Corredores
        if (idx == 0)
        {
            result->maxCorredores = aux;
            idx++;
            continue;
        }
        //Número de Armários
        if (idx == 1)
        {
            result->maxArmarios = aux;
            idx++;
            continue;
        }
        
        if (doNextCorredor)
        {
            auxCorredor = newCorredor(auxCorredor);
            //Primeiro elemento da lista
            if (idxCorredor == 0)
                result->corredores = auxCorredor;
            //Ultimo elemento
            result->lastCorredor = auxCorredor;
            //Inicializar a lista de armários para cada corredor
            auxArmario = NULL;
            idxArmario = 0;
            doNextArmario = 1;
            
            idxCorredor++;
            doNextCorredor = 0;
        }
        
        if (doNextArmario)
        {
            auxArmario = newArmario(auxCorredor,auxArmario);
            //Inicializar a lista de produtos para cada armário
            auxProduto = NULL;
            idxProduto = 0;
            maxProdutos = 0;
            
            idxArmario++;
            doNextArmario = 0;
        }
        
        if (idxProduto == 0)
        {
            maxProdutos = aux;
            //printf("\nA%d.%d\n",idxCorredor,idxArmario);
            //printf("Produtos: %d\n",maxProdutos);
        }
        else
        {
            if (idxProduto % 2 == 1)
            {
                //printf("P%d ",aux);
                auxProduto = newProduto(auxArmario,auxProduto,aux,0);
            }
            else
            {
                //printf("Qtd: %d\n",aux);
                auxProduto->qtd = aux;
            }
        }
        
        if (idxProduto == maxProdutos*2)
        {
            if (idxArmario == result->maxArmarios)
                doNextCorredor = 1;
            else
                doNextArmario = 1;
        }
        
        idxProduto++;
        idx++;
    }
    return result;
}

void writeFile(FILE *f, pDatabase db)
{
    int aux;
    pCorredor auxCorredor;
    pArmario auxArmario;
    pProduto auxProduto;
    
    if (!db) return;

    if (!f)
    {
        printf("(writeFile)Erro: sem ficheiro\n");
        return;
    }
    //Gravar número de corredores
    aux = db->maxCorredores;
    fwrite(&aux,sizeof(int),1,f);
    //Gravar número de armários
    aux = db->maxArmarios;
    fwrite(&aux,sizeof(int),1,f);
    //Gravar estrutura
    auxCorredor = db->corredores;
    while(auxCorredor)
    {
        auxArmario = auxCorredor->armarios;
        while (auxArmario)
        {
            //Grava número de produtos do armário
            aux = auxArmario->produtosTotal;
            fwrite(&aux,sizeof(int),1,f);
            
            if (auxArmario->produtos)
            {
                auxProduto = auxArmario->produtos;
                while (auxProduto)
                {
                    aux = auxProduto->num;
                    fwrite(&aux,sizeof(int),1,f);
                    
                    aux = auxProduto->qtd;
                    fwrite(&aux,sizeof(int),1,f);
                    
                    auxProduto = auxProduto->next;
                }
            }
            auxArmario = auxArmario->next;
        }
        auxCorredor = auxCorredor->next;
    }
}

pDatabase loadDB(const char *path)
{
    FILE *f = NULL;
    pDatabase startDB = NULL;
    
    f = fopen(path,"rb");
    if (!f)
    {
        printf("(loadDB)Erro: não foi possível abrir %s\n",path);
        return NULL;
    }
    
    //Carregar a estrutura do ficheiro
    startDB = readFile(f);
    
    fclose(f);
    return startDB;
}

int saveDB(const char *path, pDatabase db)
{
    FILE *f = NULL;
    
    f = fopen(path,"wb");
    if (!f)
    {
        printf("(saveDB)Erro: não foi possível criar %s\n",path);
        return 1;
    }
    
    //Gravar para o ficheiro
    writeFile(f,db);
    
    fclose(f);
    return 0;
}