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

int doVisualizarMenu()
{
    int option = 0;
    mvprintw(activeRow++,STARTCOL,"1. Visualizar tudo\n");
    mvprintw(activeRow++,STARTCOL,"2. Visualizar corredor\n");
    mvprintw(activeRow++,STARTCOL,"3. Visualizar corredor & armário\n");
    refresh();
    option = getch();
    while (option != 49 && option != 50 && option != 51)
        option = getch();
    return option - 48;
}

void doVisualizarTudoCabecalho(int page)
{
    mvprintw(activeRow,STARTCOL,"1. Visualizar\n");
    activeRow += 2;
    mvprintw(activeRow++,STARTCOL,"Lista de Corredores (Página %d)\n",page);
}

void doVisualizarTudo(pDatabase db)
{
    pCorredor auxCorredor;
    pArmario auxArmario;
    pProduto auxProduto;
    int page = 1;
    
    /*Cabeçalho*/
    printWindow();
    doVisualizarTudoCabecalho(page);
    
    auxCorredor = db->corredores;
    while (auxCorredor)
    {
        auxArmario = auxCorredor->armarios;
        while (auxArmario)
        {
            mvprintw(activeRow++,STARTCOL," A%d.%d",auxCorredor->ID,auxArmario->ID);
            
            /*Verificar se chegou ao limite da janela*/
            if (checkWindowLimit(&page) == 1)
                doVisualizarTudoCabecalho(page);
            
            if (auxArmario->produtos)
            {
                mvprintw(activeRow++,STARTCOL,"  Produtos: %d",auxArmario->produtosTotal);
                
                /*Verificar se chegou ao limite da janela*/
                if (checkWindowLimit(&page) == 1)
                    doVisualizarTudoCabecalho(page);
                
                auxProduto = auxArmario->produtos;
                while (auxProduto)
                {
                    mvprintw(activeRow++,STARTCOL,"   P%d: %d",auxProduto->num,auxProduto->qtd);
                    auxProduto = auxProduto->next;
                    
                    /*Verificar se chegou ao limite da janela*/
                    if (checkWindowLimit(&page) == 1)
                        doVisualizarTudoCabecalho(page);
                }
            }
            auxArmario = auxArmario->next;
        }
        auxCorredor = auxCorredor->next;
    }
}

void doVisualizar(pDatabase db)
{
    /*Inicializar a janela*/
    printWindow();
    
    switch (doVisualizarMenu()) {
        case 1:
            doVisualizarTudo(db);
            break;
            
        default:
            break;
    }

    mvprintw(activeRow++,STARTCOL,"Prima escape para voltar ao menu");
    refresh();
    while (getch() != 27);
}