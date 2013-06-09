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
                printWindow();
                mvprintw(activeRow,STARTCOL,"1. Visualizar\n");
                activeRow += 2;

                mvprintw(activeRow,STARTCOL,"Lista de Corredores\n");
                
                mvprintw(limitRow,STARTCOL,"Para voltar, prima qualquer tecla");
                getch();
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