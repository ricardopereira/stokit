#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

#include "Stokit.Common.h"
#include "Stokit.Corredores.h"
#include "Stokit.Armarios.h"
#include "Stokit.Produtos.h"

#define RESUMODIA "ResumoVendas.txt"
#define ALERTASSTOCK "AlertasStock.dat"

void freeDB(pDatabase p)
{
    freeCorredor(p->corredores);
    freeProduto(p->produtosResumoDia);
    freeProduto(p->produtosSemStock);
    free(p);
}

pDatabase readFile(FILE *f)
{
    int aux;
    int idx=0, idxCorredor, idxArmario, idxProduto;
    int maxProdutos;
    int doNextCorredor, doNextArmario;
    
    pDatabase result;
    pCorredor auxCorredor = NULL;
    pArmario auxArmario;
    pProduto auxProduto;
    
    if (!f)
    {
        printf("(readFile)Erro: sem ficheiro\n");
        return NULL;
    }
    
    result = malloc(sizeof(Database));
    /*Inicializar*/
    result->corredores = NULL;
    result->maxCorredores = 0;
    result->maxArmarios = 0;
    result->produtosResumoDia = NULL;
    result->lastResumoDia = NULL;
    result->produtosSemStock = NULL;
    result->lastSemStock = NULL;
    
    /*Inicialização*/
    idxCorredor = 0;
    doNextCorredor = 1;
    idxArmario = 0;
    doNextArmario = 1;
    
    /*Interpretar o ficheiro*/
    while (fread(&aux,sizeof(int),1,f) == 1)
    {
        /*Número de Corredores*/
        if (idx == 0)
        {
            result->maxCorredores = aux;
            idx++;
            continue;
        }
        /*Número de Armários*/
        if (idx == 1)
        {
            result->maxArmarios = aux;
            idx++;
            continue;
        }
        
        if (doNextCorredor)
        {
            auxCorredor = newCorredor(auxCorredor);
            /*Primeiro elemento da lista*/
            if (idxCorredor == 0)
                result->corredores = auxCorredor;
            /*Ultimo elemento*/
            result->lastCorredor = auxCorredor;
            /*Inicializar a lista de armários para cada corredor*/
            auxArmario = NULL;
            idxArmario = 0;
            doNextArmario = 1;
            
            idxCorredor++;
            doNextCorredor = 0;
        }
        
        if (doNextArmario)
        {
            auxArmario = newArmario(auxCorredor,auxArmario);
            /*Inicializar a lista de produtos para cada armário*/
            auxProduto = NULL;
            idxProduto = 0;
            maxProdutos = 0;
            
            idxArmario++;
            doNextArmario = 0;
        }
        
        if (idxProduto == 0)
        {
            maxProdutos = aux;
        }
        else
        {
            if (idxProduto % 2 == 1)
            {
                auxProduto = newProduto(auxArmario,auxProduto,aux,0);
            }
            else
            {
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
    /*Gravar número de corredores*/
    aux = db->maxCorredores;
    fwrite(&aux,sizeof(int),1,f);
    /*Gravar número de armários*/
    aux = db->maxArmarios;
    fwrite(&aux,sizeof(int),1,f);
    /*Gravar estrutura*/
    auxCorredor = db->corredores;
    while(auxCorredor)
    {
        auxArmario = auxCorredor->armarios;
        while (auxArmario)
        {
            /*Grava número de produtos do armário*/
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
    
    /*Carregar a estrutura do ficheiro*/
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
    
    /*Gravar para o ficheiro*/
    writeFile(f,db);
    
    fclose(f);
    return 0;
}

void saveResumoDia(pDatabase db)
{
    pProduto auxProduto;
    FILE *f;
    time_t currentTime;
    
    if (!db->produtosResumoDia)
        return;
    
    /*Obter o dia e hora atual*/
    currentTime = time(NULL);
    
    /*Sempre a acrescentar no final do ficheiro*/
    f = fopen(RESUMODIA,"a");
    if (!f)
    {
        printf("(saveResumoDia)Erro: não foi possível criar %s\n",RESUMODIA);
        return;
    }
    
    /*Resumo de vendas*/
    auxProduto = db->produtosResumoDia;
    if (auxProduto)
        /*Escrever a hora do ficheiro*/
        fprintf(f,"%s",ctime(&currentTime));
    
    while (auxProduto)
    {
        fprintf(f," Produto %d - Q: %d\n",auxProduto->num,auxProduto->qtd);
        auxProduto = auxProduto->next;
    }
    fclose(f);
}

void loadAlertasStock(pDatabase db)
{
    pProduto itemSemStock = NULL;
    int num, qtd, total, i;
    FILE *f;
    
    /*Ficheiro já tem que existir*/
    if (access(ALERTASSTOCK,W_OK) == -1)
        return;
    
    f = fopen(ALERTASSTOCK,"rb");
    if (!f)
    {
        printf("(loadAlertasStock)Erro: não foi possível abrir %s\n",ALERTASSTOCK);
        return;
    }
    
    fread(&total,sizeof(int),1,f);
    for (i=0; i<total; i++)
    {
        fread(&num,sizeof(int),1,f);
        fread(&qtd,sizeof(int),1,f);
        
        itemSemStock = addProduto(itemSemStock,num,qtd);
        /*Verificar se é o primeiro elemento*/
        if (!db->produtosSemStock)
            db->produtosSemStock = itemSemStock;
    }
    db->lastSemStock = itemSemStock;
    fclose(f);
}

void saveAlertasStock(pDatabase db)
{
    pProduto auxProduto;
    int aux, total = 0;
    FILE *f;
    
    f = fopen(ALERTASSTOCK,"wb");
    if (!f)
    {
        printf("(saveAlertasStock)Erro: não foi possível criar %s\n",ALERTASSTOCK);
        return;
    }
    
    if (!db->produtosSemStock)
    {
        fclose(f);
        return;
    }
    
    /*ToDo - melhorar isto*/
    auxProduto = db->produtosSemStock;
    while (auxProduto)
    {
        total++;
        auxProduto = auxProduto->next;
    }
    fwrite(&total,sizeof(int),1,f);
    /*Resumo de vendas*/
    auxProduto = db->produtosSemStock;
    while (auxProduto)
    {
        aux = auxProduto->num;
        fwrite(&aux,sizeof(int),1,f);
        aux = auxProduto->qtd;
        fwrite(&aux,sizeof(int),1,f);
        auxProduto = auxProduto->next;
    }
    fclose(f);
}