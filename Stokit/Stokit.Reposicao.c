#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "Stokit.Common.h"
#include "Stokit.DB.h"
#include "Stokit.Produtos.h"
#include "Stokit.Armarios.h"

pArmario getReposicaoStock(FILE *f)
{
    int num, qtd;
    pArmario resArmario;
    pProduto auxProduto = NULL;
    /*Utilizar a lista de produtos com o contador*/
    resArmario = newArmario(NULL,NULL);
    if (f)
    {
        while(!feof(f))
        {
            /*Lê a linha de reposição*/
            fscanf(f,"%d:%d",&num,&qtd);
            /*Adiciona elemento*/
            auxProduto = newProduto(resArmario,auxProduto,num,qtd);
            /*Mostra o que foi lido*/
            /*printf("\nP%d:%d",auxProduto->num,auxProduto->qtd);*/
        }
    }
    else
        printf("(getReposicaoStock)Erro: sem ficheiro\n");
    return resArmario;
}

int doReporStock(pDatabase db, pProduto list)
{
    /*Verificar se existe algum produto a repor*/
    if (!db || !list) return 0;
    /*Lista de reposição de stock*/
    pProduto repProduto,auxProduto;
    pArmario auxArmario;
    int total = 0;
    repProduto = list;
    while (repProduto)
    {
        auxProduto = getProdutoByNum(db->corredores,repProduto->num);
        /*Encontrado o produto?*/
        if (auxProduto)
        {
            auxProduto->qtd += repProduto->qtd;
        }
        else
        {
            /*Encontrar o armário com menor número de produtos para adicionar o novo produto*/
            auxArmario = getArmarioComMenorProdutos(db);
            newProduto(auxArmario, auxArmario->lastProduto, repProduto->num, repProduto->qtd);
        }
        total++;
        repProduto = repProduto->next;
    }
    return total;
}

pArmario loadReposicaoStock(char *path, pDatabase db)
{
    FILE *f = NULL;
    pArmario listaProdutos = NULL;
    
    /*Ler o ficheiro de texto*/
    f = fopen(path,"r");
    if (f)
    {
        listaProdutos = getReposicaoStock(f);
    }
    else
        printf("(loadReposicaoStock)Erro: ficheiro não existe");
    fclose(f);
    return listaProdutos;
}

int doReposicaoMenu()
{
    int option = 0;
    while (option != 1 && option != KEYESCAPE)
    {
        /*Inicializar a janela*/
        printWindow();
        mvprintw(activeRow++,STARTCOL,"Reposição de Stock");
        mvprintw(activeRow++,STARTCOL," 1. Importar ficheiro da pasta \"%s\"",PathReposicao);
        activeRow++;
        mvprintw(activeRow++,STARTCOL,"ESC. Voltar ao menu");
        refresh();
        option = getch() - ASCIIZERO;
    }
    return option;
}

int doReposicaoPorFicheiro(pDatabase db)
{
    char fileName[MAXFILENAME+1]; /* +1 por causa da leitura pelo getnstr */
    char path[MAXPATH];
    int tryAgain = 1, res = 0;
    char aux;
    pArmario listaProdutos;
    pProduto auxProduto;
    
    /*Cabeçalho*/
    printWindow();
    mvprintw(activeRow++,STARTCOL,"Reposição de Stock");
    mvprintw(activeRow++,STARTCOL," 1. Importar ficheiro da pasta \"%s\"",PathReposicao);
    activeRow++;
    
    fileName[0] = 0;
    /*Enquanto for igual a zero*/
    while (fileName[0] == 0)
    {
        refresh();
        /*Pedir o corredor*/
        mvprintw(activeRow,STARTCOL," Indique o nome do ficheiro: ");
        getnstr(fileName,MAXFILENAME);
    }
    activeRow += 2;
    mvprintw(activeRow++,STARTCOL,"Importação: %s",fileName);
    
    /*Concatenar para o caminho final*/
    getFullPath(path,sizeof(path),PathReposicao,fileName);
    
    if (access(path,W_OK) == -1)
    {
        mvprintw(activeRow++,STARTCOL," Ficheiro não existe");
        activeRow++;
        while (tryAgain)
        {
            /*Limpar caracter inválido*/
            mvprintw(activeRow,STARTCOL," Deseja tentar novamente?        ");
            mvprintw(activeRow,STARTCOL," Deseja tentar novamente? (s/n)");
            refresh();
            aux = getch();
            /*Volta a tentar se colocar caracter inválido*/
            tryAgain = aux != 's' && aux != 'S' && aux != 'n' && aux != 'N';
        }
        if (aux == 's' || aux == 'S')
        {
            /*Recursivo*/
            res = doReposicaoPorFicheiro(db);
        }
        else
            return KEYESCAPE;
    }
    else
    {
        /*Carregar lista de produtos*/
        listaProdutos = loadReposicaoStock(path,db);
        
        /*Lista com os produtos a repor*/
        if (listaProdutos)
        {
            mvprintw(activeRow++,STARTCOL," Total de produtos: %d",listaProdutos->produtosTotal);
            activeRow++;
            
            /*Mostra o que foi lido*/
            auxProduto = listaProdutos->produtos;
            while (auxProduto)
            {
                mvprintw(activeRow++,STARTCOL," Produto %d - Q: %d",auxProduto->num,auxProduto->qtd);
                auxProduto = auxProduto->next;
                
                /*Verificar se chegou ao limite da janela*/
                if (activeRow >= limitRow-3 /*retirar 3 linhas para a pergunta da reposição*/)
                {
                    mvprintw(activeRow,STARTCOL," (...)");
                    break;
                }
            }
            
            activeRow++;
            while (tryAgain)
            {
                /*Limpar caracter inválido*/
                mvprintw(activeRow,STARTCOL," Deseja repor?        ");
                mvprintw(activeRow,STARTCOL," Deseja repor? (s/n)");
                refresh();
                aux = getch();
                /*Volta a tentar se colocar caracter inválido*/
                tryAgain = aux != 's' && aux != 'S' && aux != 'n' && aux != 'N';
            }
            if (aux == 's' || aux == 'S')
            {
                /*Repor Stock*/
                doReporStock(db,listaProdutos->produtos);
                mvprintw(activeRow++,STARTCOL,"Reposição com sucesso");
            }
            else
                activeRow++;
            
            freeArmario(listaProdutos);
        }
        else
        {
            mvprintw(activeRow++,STARTCOL," Ficheiro sem artigos ou inválido");
        }
        activeRow++;
    }
    return res;
}

void doReposicao(pDatabase db)
{
    /*MENU de seleção*/
    switch (doReposicaoMenu()) {
        case KEYESCAPE:
            return;
        default:
            if (doReposicaoPorFicheiro(db) == KEYESCAPE)
                return;
            break;
    }
    
    mvprintw(activeRow++,STARTCOL,"Prima escape para voltar ao menu");
    refresh();
    while (getch() != ASCIIESC);
}