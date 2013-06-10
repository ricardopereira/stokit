#include <stdio.h>
#include <string.h>
#include <ncurses.h>

#include "Stokit.Common.h"

void printWindow()
{
    clear();
    printCaption();
    printCredits();
    activeRow = STARTROW;
}

void printCredits()
{
    mvprintw(maxRow-1,0,"Ricardo Pereira & Francisco Abrantes © ISEC 2013");
}

void printCaption()
{
    char text[50];
    snprintf(text,sizeof(text),"Stokit v1");
    mvprintw(0,(int)(maxCol-strlen(text))/2,text);
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

void forEachArmario(pDatabase db, pProduto p, funcArmario doSomething)
{
    pCorredor auxCorredor;
    pArmario auxArmario;
    if (!db) return;
    auxCorredor = db->corredores;
    while (auxCorredor)
    {
        auxArmario = auxCorredor->armarios;
        while (auxArmario)
        {
            doSomething(auxArmario,p);
            auxArmario = auxArmario->next;
        }
        auxCorredor = auxCorredor->next;
    }    
}

void removeBreakLine(char *result)
{
    size_t ln = strlen(result)-1;
    if (result[ln] == '\n')
        result[ln] = '\0';
}

void getFullPath(char *result, int size, char *path, char *filename)
{
    /*Concatenar para o caminho final*/
    snprintf(result,size,"%s%s",path,filename);
    /*Retirar o \n atribuído ao fazer fgets*/
    removeBreakLine(result);
}

int ask(char *question)
{
    int tryAgain = 1;
    char aux;
    while (tryAgain)
    {
        /*Limpar caracter inválido*/
        mvprintw(activeRow,STARTCOL," %s        ",question);
        mvprintw(activeRow,STARTCOL," %s (s/n)",question);
        refresh();
        aux = getch();
        /*Volta a tentar se colocar caracter inválido*/
        tryAgain = aux != 's' && aux != 'S' && aux != 'n' && aux != 'N';
    }
    if (aux == 's' || aux == 'S')
        return 1;
    else
        return 0;
}