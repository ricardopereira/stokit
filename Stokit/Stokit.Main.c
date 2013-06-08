#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ncurses.h>
// External
#include "Stokit.Common.h"
#include "Stokit.DB.h"
#include "Stokit.Corredores.h"
#include "Stokit.Armarios.h"
#include "Stokit.Produtos.h"
#include "Stokit.Reposicao.h"
#include "Stokit.Encomendas.h"

int main(int argc, const char * argv[])
{
    pDatabase db;
    //Ler ficheiro
    db = loadDB(DB);
    
    char mesg[]="Just a string";
    int row,col;

    initscr();
    getmaxyx(stdscr,row,col); // get the number of rows and columns
    mvprintw(row/2,(col-strlen(mesg))/2,"%s",mesg);

    mvprintw(row-2,0,"This screen has %d rows and %d columns\n",row,col);
    printw("Try resizing your window(if possible) and then run this program again");
    
    refresh();
    getch();
    endwin();
    
    freeDB(db);
    return 0;
}