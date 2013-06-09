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
void doPesquisa(pDatabase db);

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
        option = getch() - ASCIIZERO;
        
        switch (option) {
            case 1:
                /*Visualização de produtos de corredores e armários*/
                doVisualizar(db);
                break;
            case 2:
                /*Pesquisa de produtos*/
                doPesquisa(db);
                break;
            case 3:
                /*Reposição de stock através dum ficheiro*/
                doReposicao(db);
                break;
            case 4:
                /*Satisfação de encomendas através dum ficheiro*/
                //doEncomenda(db);
                break;
            case 5:
                /*Resumo do dia*/
                //doResumo(db);
                break;
        }
        if (option == KEYESCAPE || option == 0)
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

void showCorredoresArmarios(pDatabase db, int idCorredor, int idArmario, void (*f)(int /*Página*/, int*/*Corredor*/, int*/*Armario*/))
{
    pCorredor auxCorredor;
    pArmario auxArmario;
    pProduto auxProduto;
    int page = 1;
    
    auxCorredor = db->corredores;
    while (auxCorredor)
    {
        /*Verificar limitação por corredores*/
        if (idCorredor != 0 && idCorredor != auxCorredor->ID)
        {
            auxCorredor = auxCorredor->next;
            continue;
        }
        
        auxArmario = auxCorredor->armarios;
        while (auxArmario)
        {
            /*Verificar limitação por armários*/
            if (idArmario != 0 && idArmario != auxArmario->ID)
            {
                auxArmario = auxArmario->next;
                continue;
            }
            
            mvprintw(activeRow++,STARTCOL," A%d.%d",auxCorredor->ID,auxArmario->ID);
            
            /*Verificar se chegou ao limite da janela*/
            if (checkWindowLimit(&page) == 1)
                (*f)(page,&idCorredor,&idArmario);
            
            if (auxArmario->produtos)
            {
                mvprintw(activeRow++,STARTCOL,"  Produtos: %d",auxArmario->produtosTotal);
                
                /*Verificar se chegou ao limite da janela*/
                if (checkWindowLimit(&page) == 1)
                    (*f)(page,&idCorredor,&idArmario);
                
                auxProduto = auxArmario->produtos;
                while (auxProduto)
                {
                    mvprintw(activeRow++,STARTCOL,"   P%d: %d",auxProduto->num,auxProduto->qtd);
                    auxProduto = auxProduto->next;
                    
                    /*Verificar se chegou ao limite da janela*/
                    if (checkWindowLimit(&page) == 1)
                        (*f)(page,&idCorredor,&idArmario);
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
    while (option != 1 && option != 2 && option != 3  && option != KEYESCAPE)
    {
        /*Inicializar a janela*/
        printWindow();
        mvprintw(activeRow++,STARTCOL,"1. Visualizar tudo");
        mvprintw(activeRow++,STARTCOL,"2. Visualizar corredor");
        mvprintw(activeRow++,STARTCOL,"3. Visualizar corredor & armário");
        activeRow++;
        mvprintw(activeRow++,STARTCOL,"ESC. Voltar ao menu");
        refresh();
        option = getch() - ASCIIZERO;
    }
    return option;
}

void doVisualizarTudoCabecalho(int page, int *idCorredor, int *idArmario)
{
    mvprintw(activeRow,STARTCOL,"1. Visualizar tudo");
    activeRow += 2;
    mvprintw(activeRow++,STARTCOL,"Lista de produtos (Página %d)",page);
}

void doVisualizarTudo(pDatabase db)
{
    int IDCorredor = 0, IDArmario = 0;
    /*Cabeçalho*/
    printWindow();
    doVisualizarTudoCabecalho(1,&IDCorredor,&IDArmario);
    /*Mostrar dados*/
    showCorredoresArmarios(db,0,0,doVisualizarTudoCabecalho);
}

void doVisualizarCorredorCabecalho(int page, int *idCorredor, int *idArmario)
{
    char corr[LIMIT_CORREDOR+1]; /* +1 por causa da leitura pelo getnstr */
    
    if (*idCorredor == 0)
    {
        mvprintw(activeRow++,STARTCOL,"2. Visualizar corredor");
        /*Enquanto for igual a zero*/
        while (!*idCorredor)
        {
            /*Limpar resposta inválida*/
            /*ToDo - espaços deviam ser consoante o número de caracteres válidos para o corredor*/
            mvprintw(activeRow,STARTCOL," Indique o corredor:     ");
            refresh();
            /*Pedir o corredor*/
            mvprintw(activeRow,STARTCOL," Indique o corredor: ");
            getnstr(corr,LIMIT_CORREDOR);
            *idCorredor = atoi(corr);
        }
        activeRow++;
    }
    else
    {
        mvprintw(activeRow++,STARTCOL,"2. Visualizar corredor");
        mvprintw(activeRow++,STARTCOL," Indique o corredor: %d",*idCorredor);
    }
    activeRow++;
    mvprintw(activeRow++,STARTCOL,"Lista de produtos (Página %d)",page);
}

void doVisualizarCorredor(pDatabase db)
{
    int IDCorredor = 0, IDArmario = 0;  
    /*Cabeçalho*/
    printWindow();
    doVisualizarCorredorCabecalho(1,&IDCorredor,&IDArmario);
    /*Mostrar dados*/
    showCorredoresArmarios(db,IDCorredor,0,doVisualizarCorredorCabecalho);
}

void doVisualizarCorredorArmarioCabecalho(int page, int *idCorredor, int *idArmario)
{
    char corr[LIMIT_CORREDOR+1]; /* +1 por causa da leitura pelo getnstr */
    char arm[LIMIT_ARMARIO+1]; /* +1 por causa da leitura pelo getnstr */
    
    if (*idCorredor == 0)
    {
        mvprintw(activeRow++,STARTCOL,"3. Visualizar corredor & armário");
        /*Enquanto for igual a zero*/
        while (!*idCorredor)
        {
            /*Limpar resposta inválida*/
            /*ToDo - espaços deviam ser consoante o número de caracteres válidos para o corredor*/
            mvprintw(activeRow,STARTCOL," Indique o corredor:     ");
            refresh();
            /*Pedir o corredor*/
            mvprintw(activeRow,STARTCOL," Indique o corredor: ");
            getnstr(corr,LIMIT_CORREDOR);
            *idCorredor = atoi(corr);
        }
        activeRow++;
        while (!*idArmario)
        {
            /*Limpar resposta inválida*/
            /*ToDo - espaços deviam ser consoante o número de caracteres válidos para o armário*/
            mvprintw(activeRow,STARTCOL," Indique o armário:       ");
            refresh();
            /*Pedir o armário*/
            mvprintw(activeRow,STARTCOL," Indique o armário: ");
            getnstr(arm,LIMIT_ARMARIO);
            *idArmario = atoi(arm);
        }
        activeRow++;
    }
    else
    {
        mvprintw(activeRow++,STARTCOL,"3. Visualizar corredor & armário");
        mvprintw(activeRow++,STARTCOL," Indique o corredor: %d",*idCorredor);
        mvprintw(activeRow++,STARTCOL," Indique o armário: %d",*idArmario);
    }
    activeRow++;
    mvprintw(activeRow++,STARTCOL,"Lista de produtos (Página %d)",page);
}

void doVisualizarCorredorArmario(pDatabase db)
{
    int IDCorredor = 0, IDArmario = 0;
    /*Cabeçalho*/
    printWindow();
    doVisualizarCorredorArmarioCabecalho(1,&IDCorredor,&IDArmario);
    /*Mostrar dados*/
    showCorredoresArmarios(db,IDCorredor,IDArmario,doVisualizarCorredorArmarioCabecalho);
}

void doVisualizar(pDatabase db)
{
    /*MENU de seleção*/
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
        case KEYESCAPE:
            return;
        default:
            break;
    }

    mvprintw(activeRow++,STARTCOL,"Prima escape para voltar ao menu");
    refresh();
    while (getch() != ASCIIESC);
}

int doPesquisaMenu()
{
    int option = 0;
    while (option != 1 && option != KEYESCAPE)
    {
        /*Inicializar a janela*/
        printWindow();
        mvprintw(activeRow++,STARTCOL,"1. Pesquisar produto");
        activeRow++;
        mvprintw(activeRow++,STARTCOL,"ESC. Voltar ao menu");
        refresh();
        option = getch() - ASCIIZERO;
    }
    return option;
}

int doPesquisaProduto(pDatabase db)
{
    pProduto auxProduto = NULL;
    pArmario auxArmario = NULL;
    pCorredor auxCorredor = NULL;
    int idProduto = 0, tryAgain = 1, res = 0;
    char aux;
    char prod[LIMIT_PRODUTO+1]; /* +1 por causa da leitura pelo getnstr */
        
    /*Cabeçalho*/
    printWindow();

    mvprintw(activeRow++,STARTCOL,"1. Pesquisar produto");
    /*Enquanto for igual a zero*/
    while (!idProduto)
    {
        /*Limpar resposta inválida*/
        /*ToDo - espaços deviam ser consoante o número de caracteres válidos*/
        mvprintw(activeRow,STARTCOL," Indique o id do produto:        ");
        refresh();
        /*Pedir o corredor*/
        mvprintw(activeRow,STARTCOL," Indique o id do produto: ");
        getnstr(prod,LIMIT_PRODUTO);
        idProduto = atoi(prod);
    }
    activeRow += 2;
    mvprintw(activeRow++,STARTCOL,"Informação do produto P%d:",idProduto);
    
    auxProduto = getProdutoByNum(db->corredores,idProduto);
    if (auxProduto)
    {
        auxArmario = auxProduto->parent;
        if (auxArmario)
            auxCorredor = auxArmario->parent;
        
        if (auxCorredor)
            mvprintw(activeRow++,STARTCOL," Corredor: %d",auxCorredor->ID);
        if (auxArmario)
            mvprintw(activeRow++,STARTCOL," Armário: %d",auxArmario->ID);
        mvprintw(activeRow++,STARTCOL," Quantidade: %d",auxProduto->qtd);
        activeRow++;
    }
    else
    {
        mvprintw(activeRow++,STARTCOL," Não existe");
        activeRow++;
        while (tryAgain)
        {
            /*Limpar caracter inválido*/
            mvprintw(activeRow,STARTCOL," Deseja fazer nova pesquisa?        ");
            mvprintw(activeRow,STARTCOL," Deseja fazer nova pesquisa? (s/n)");
            refresh();
            aux = getch();
            /*Volta a tentar se colocar caracter inválido*/
            tryAgain = aux != 's' && aux != 'S' && aux != 'n' && aux != 'N';
        }
        if (aux == 's' || aux == 'S')
        {
            /*Recursivo*/
            res = doPesquisaProduto(db);
        }
        else
            return KEYESCAPE;
    }
    return res;
}

void doPesquisa(pDatabase db)
{
    /*MENU de seleção*/
    switch (doPesquisaMenu()) {
        case KEYESCAPE:
            return;
        default:
            if (doPesquisaProduto(db) == KEYESCAPE)
                return;
            break;
    }
    
    mvprintw(activeRow++,STARTCOL,"Prima escape para voltar ao menu");
    refresh();
    while (getch() != ASCIIESC);
}