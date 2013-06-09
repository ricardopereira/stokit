#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ncurses.h>

#include "Stokit.Common.h"
#include "Stokit.DB.h"
#include "Stokit.Corredores.h"
#include "Stokit.Armarios.h"
#include "Stokit.Produtos.h"
#include "Stokit.Reposicao.h"
#include "Stokit.Encomendas.h"

void printMenu();
void printCredits();
void printCaption();
void printClose();
void printWindow();

void doVisualizar(pDatabase db);

int main(int argc, const char * argv[])
{
    pDatabase db;
    int option;
    
    /*Ler ficheiro*/
    db = loadDB(DB);
    
    /*Iniciar interface*/
    #ifndef DEBUG
    initscr();
    #endif

    getmaxyx(stdscr,maxRow,maxCol);
    limitRow = maxRow - 3;
    
    /*MENU*/
    while (1)
    {
        printWindow();
        printMenu();
    
        refresh();
        option = getch();
        
        switch (option) {
            case 49:
                /*Visualização de corredores, armários e produtos*/
                doVisualizar(db);
                break;
        }
        if (option == 27 || option == 48)
            break;
    }
    
    /*Finish*/
    clear();
    printCaption();
    printCredits();
    printClose();
    refresh();
    getch();
    
    /*Terminar aplicação*/
    endwin();
    freeDB(db);
    return 0;
}

void printWindow()
{
    clear();
    printCaption();
    printCredits();
    activeRow = STARTROW;
}

void printMenu()
{
    mvprintw(activeRow+0,STARTCOL,"1. Visualizar");
    mvprintw(activeRow+1,STARTCOL,"2. Pesquisar");
    mvprintw(activeRow+2,STARTCOL,"3. Resposição de stock");
    mvprintw(activeRow+3,STARTCOL,"4. Encomendas");
    mvprintw(activeRow+4,STARTCOL,"5. Resumo do dia");
    mvprintw(activeRow+6,STARTCOL,"0. Fechar");
}

void printCaption()
{
    mvprintw(maxRow-1,0,"Ricardo Pereira & Francisco Abrantes © ISEC 2013");
}

void printCredits()
{
    char text[50];
    snprintf(text,sizeof(text),"Stokit v1");
    mvprintw(0,(int)(maxCol-strlen(text))/2,text);
}

void printClose()
{
    char text[50];
    snprintf(text,sizeof(text),"Prima qualquer tecla para sair");
    mvprintw(maxRow/2,(int)(maxCol-strlen(text))/2,text);
}

int checkWindowLimit(int *page)
{
    /*Verificar se chegou ao limite da janela*/
    if (activeRow >= limitRow)
    {
        mvprintw(activeRow,STARTCOL,"Prima enter para continuar...");
        refresh();
        getch();
        printWindow();
        (*page)++;
        return 1;
    }
    return 0;
}

void showCorredoresArmarios(pDatabase db, int idCorredor, int idArmario, int (*f)(int, int))
{
    pCorredor auxCorredor;
    pArmario auxArmario;
    pProduto auxProduto;
    int page = 1;
    
    auxCorredor = db->corredores;
    while (auxCorredor)
    {
        auxArmario = auxCorredor->armarios;
        while (auxArmario)
        {
            mvprintw(activeRow++,STARTCOL," A%d.%d",auxCorredor->ID,auxArmario->ID);
            
            /*Verificar se chegou ao limite da janela*/
            if (checkWindowLimit(&page) == 1)
                (*f)(page,idCorredor);
            
            if (auxArmario->produtos)
            {
                mvprintw(activeRow++,STARTCOL,"  Produtos: %d",auxArmario->produtosTotal);
                
                /*Verificar se chegou ao limite da janela*/
                if (checkWindowLimit(&page) == 1)
                    (*f)(page,idCorredor);
                
                auxProduto = auxArmario->produtos;
                while (auxProduto)
                {
                    mvprintw(activeRow++,STARTCOL,"   P%d: %d",auxProduto->num,auxProduto->qtd);
                    auxProduto = auxProduto->next;
                    
                    /*Verificar se chegou ao limite da janela*/
                    if (checkWindowLimit(&page) == 1)
                        (*f)(page,idCorredor);
                }
            }
            auxArmario = auxArmario->next;
        }
        auxCorredor = auxCorredor->next;
    }
}

int doVisualizarMenu()
{
    int option = 0;
    mvprintw(activeRow++,STARTCOL,"1. Visualizar tudo");
    mvprintw(activeRow++,STARTCOL,"2. Visualizar corredor");
    mvprintw(activeRow++,STARTCOL,"3. Visualizar corredor & armário");
    refresh();
    option = getch();
    while (option != 49 && option != 50 && option != 51)
        option = getch();
    return option - 48;
}

int doVisualizarTudoCabecalho(int page, int fiction)
{
    mvprintw(activeRow,STARTCOL,"1. Visualizar tudo");
    activeRow += 2;
    mvprintw(activeRow++,STARTCOL,"Lista de produtos (Página %d)",page);
    return 0;
}

void doVisualizarTudo(pDatabase db)
{    
    /*Cabeçalho*/
    printWindow();
    doVisualizarTudoCabecalho(1,0);
    /*Mostrar dados*/
    showCorredoresArmarios(db,0,0,doVisualizarTudoCabecalho);
}

int doVisualizarCorredorCabecalho(int page, int IDCorredor)
{
    char corr[LIMIT_CORREDOR+1]; /* +1 por causa da leitura pelo getnstr */
    
    if (IDCorredor == 0)
    {
        mvprintw(activeRow++,STARTCOL,"2. Visualizar corredor");
        /*Enquanto for igual a zero*/
        while (!IDCorredor)
        {
            /*Limpar resposta inválida*/
            /*ToDo - espaços deviam ser consoante o número de caracteres válidos para o corredor*/
            mvprintw(activeRow,STARTCOL," Indique o corredor:     ");
            /*Pedir o corredor*/
            mvprintw(activeRow,STARTCOL," Indique o corredor: ");
            getnstr(corr,LIMIT_CORREDOR);
            IDCorredor = atoi(corr);
        }
        activeRow++;
    }
    else
    {
        mvprintw(activeRow++,STARTCOL,"2. Visualizar corredor");
        mvprintw(activeRow++,STARTCOL," Indique o corredor: %d",IDCorredor);
    }
    activeRow++;
    mvprintw(activeRow++,STARTCOL,"Lista de produtos (Página %d)",page);
    return IDCorredor;
}

void doVisualizarCorredor(pDatabase db)
{
    int IDCorredor = 0;    
    /*Cabeçalho*/
    printWindow();
    IDCorredor = doVisualizarCorredorCabecalho(1,IDCorredor);
    /*Mostrar dados*/
    showCorredoresArmarios(db,IDCorredor,0,doVisualizarCorredorCabecalho);
}

void doVisualizarCorredorArmario(pDatabase db)
{
    //int IDCorredor, IDArmario;
    
}

void doVisualizar(pDatabase db)
{
    /*Inicializar a janela*/
    printWindow();
    
    switch (doVisualizarMenu()) {
        case 1:
            doVisualizarTudo(db);
            break;
        case 2:
            doVisualizarCorredor(db);
            break;
        case 3:
            doVisualizarCorredorArmario(db);
            break;
        default:
            break;
    }

    mvprintw(activeRow++,STARTCOL,"Prima escape para voltar ao menu");
    refresh();
    while (getch() != 27);
}