#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <ncurses.h>

#include "Stokit.Common.h"
#include "Stokit.Armarios.h"
#include "Stokit.Produtos.h"

pProduto createProduto(pProduto p)
{
    pProduto aux;
    aux = malloc(sizeof(Produto));
    if (!aux)
    {
        printf("(createProduto)Erro: não foi possível alocar memória\n");
        return p;
    }
    if (p)
    {
        /*Próximo elemento*/
        p->next = aux;
    }
    return aux;
}

void initProduto(pProduto new, pProduto p, int n, int qtd)
{
    if (!new) return;
    new->num = n;
    new->qtd = qtd;
    new->prev = p;
    new->next = NULL;
    new->needStock = 0;
    new->lastQtd = 0;
}

pProduto newProduto(pArmario parent, pProduto p, int n, int qtd)
{
    pProduto aux;
    /*Novo elemento*/
    aux = createProduto(p);
    if (parent)
    {
        if (!parent->produtos)
            parent->produtos = aux;
        parent->lastProduto = aux;
        parent->produtosTotal++;
    }
    initProduto(aux,p,n,qtd);
    aux->parent = parent;
    return aux;
}

pProduto addProduto(pProduto last, int n, int qtd)
{
    pProduto new;
    /*Novo elemento*/
    new = createProduto(last);
    initProduto(new,last,n,qtd);
    return new;
}

pProduto addProdutoEncomenda(pEncomenda encomenda, int n, int qtd)
{
    pProduto new, auxProduto;
    int endOfList = 1;
    
    /*Novo elemento*/
    new = createProduto(NULL);
    initProduto(new,NULL,n,qtd);
    
    if (encomenda)
    {
        auxProduto = encomenda->produtos;
        while (auxProduto)
        {
            /*Verificar duplicação*/
            if (auxProduto->num == new->num)
            {
                /*Duplicado*/
                auxProduto->qtd += qtd;
                return auxProduto;
            }
            
            /*Verificar se é primeiro elemento da lista*/
            if (!auxProduto->prev && new->num < auxProduto->num)
            {
                /*Inserir na primeira posição*/
                new->prev = NULL;
                new->next = auxProduto;
                auxProduto->prev = new;
                encomenda->produtos = new;
                endOfList = 0;
                break;
            }
            
            /*Verificar elementos da lista:*/
            if (new->num < auxProduto->num)
            {
                /*Inserir no meio da lista*/
                new->prev = auxProduto->prev;
                auxProduto->prev->next = new;
                auxProduto->prev = new;
                new->next = auxProduto;
                endOfList = 0;
                break;
            }
            
            auxProduto = auxProduto->next;
        }
        
        if (endOfList)
        {
            /*Inicio da lista*/
            if (!encomenda->produtos)
                encomenda->produtos = new;
            /*Guardar o último produto adicionado*/
            if (encomenda->lastProduto)
            {
                encomenda->lastProduto->next = new;
                new->prev = encomenda->lastProduto;
            }
            encomenda->lastProduto = new;
        }
        /*Incrementar o número de produtos*/
        encomenda->produtosTotal++;
    }
    return new;
}

void freeProduto(pProduto p)
{
    pProduto aux;
    while (p)
    {
        aux = p;
        p = p->next;
        free(aux);
    }
}

pProduto getProduto(pCorredor corredor, int IDCorredor, int IDArmario)
{
    pArmario auxArmario;
    /*Parent válido*/
    if (!corredor)
    {
        printf("Need list");
        return NULL;
    }
    auxArmario = getArmario(corredor,IDCorredor,IDArmario);
    if (!auxArmario) return NULL;
    return auxArmario->produtos;
}

pProduto getProdutoByNum(pCorredor corredor, int n)
{
    pArmario auxArmario;
    pProduto auxProduto;
    
    while (corredor)
    {
        auxArmario = corredor->armarios;
        while (auxArmario)
        {
            if (auxArmario->produtos)
            {
                auxProduto = auxArmario->produtos;
                while (auxProduto)
                {
                    if (auxProduto->num == n)
                        return auxProduto;
                    auxProduto = auxProduto->next;
                }
            }
            auxArmario = auxArmario->next;
        }
        corredor = corredor->next;
    }
    return NULL;
}

pProduto findProduto(pArmario armario, int n)
{
    if (!armario) return NULL;
    pProduto auxProduto = armario->produtos;
    while (auxProduto)
    {
        if (auxProduto->num == n)
            return auxProduto;
        auxProduto = auxProduto->next;
    }
    return NULL;
}

void doShowProdutosCabecalho(int page, char *text)
{
    /*Inicializar a janela*/
    printWindow();
    mvprintw(activeRow++,STARTCOL,"%s (Página %d)",text,page);
    activeRow++;
}

void doShowProdutos(pProduto produtos, char *text, int (*check)(pProduto))
{
    pProduto auxProduto;
    int page = 1;
    
    doShowProdutosCabecalho(page,text);
    
    if (!produtos)
        mvprintw(activeRow++,STARTCOL," Sem produtos");
    
    auxProduto = produtos;
    while (auxProduto)
    {
        if (!check || (*check)(auxProduto))
        {
            /*Verificar se chegou ao limite da janela*/
            if (checkWindowLimit(&page) == 1)
                doShowProdutosCabecalho(page,text);
            
            mvprintw(activeRow++,STARTCOL," Produto %d - Q: %d",auxProduto->num,auxProduto->qtd);
        }
        auxProduto = auxProduto->next;
    }
}

void doProdutosSemStock(pDatabase db)
{
    int option = 0;
    doShowProdutos(db->produtosSemStock,"Produtos com quantidade em falta:",NULL);
    activeRow++;
    mvprintw(activeRow++,STARTCOL,"Prima X para limpar a lista");
    mvprintw(activeRow++,STARTCOL,"Prima ESC para voltar ao menu");
    refresh();
    
    while (option != 'X' && option != 'x' && option != ASCIIESC)
    {
        option = getch();
    }
    
    if (toupper(option) == 'X')
    {
        freeProduto(db->produtosSemStock);
        db->produtosSemStock = NULL;
        db->lastSemStock = NULL;
    }
}

void doProdutosResumoDia(pDatabase db)
{
    doShowProdutos(db->produtosResumoDia,"Resumo de vendas da sessão atual:",NULL);
    activeRow++;
    mvprintw(activeRow++,STARTCOL,"Prima escape para voltar ao menu");
    refresh();
    while (getch() != ASCIIESC);
}