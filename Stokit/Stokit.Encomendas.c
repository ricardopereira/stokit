#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "Stokit.Common.h"
#include "Stokit.DB.h"
#include "Stokit.Produtos.h"
#include "Stokit.Armarios.h"

pEncomenda getEncomenda(FILE *f)
{
    int num, qtd;
    pEncomenda resEncomenda;
    pProduto auxProduto = NULL;
    char nome[MAXNAME];
    resEncomenda = malloc(sizeof(Encomenda));
    resEncomenda->lastProduto = NULL;
    resEncomenda->produtosTotal = 0;
    if (f)
    {
        while (!feof(f))
        {
            /*Lê a linha da encomenda*/
            if (fscanf(f,"%d:%d",&num,&qtd) == 2)
            {
                /*Adiciona o produto*/
                auxProduto = addProdutoEncomenda(resEncomenda,num,qtd);
            }
            else if (fgets(nome,sizeof(nome),f))
            {
                /*Ler nome da encomenda*/
                removeBreakLine(nome);
                strcpy(resEncomenda->nome,nome);
            }
            else
                fseek(f,1,SEEK_CUR);
        }
    }
    else
        printf("(getEncomenda)Erro: sem ficheiro\n");
    return resEncomenda;
}

int doSatisfazerEncomenda(pDatabase db, pEncomenda enc)
{
    int total = 0, needRollback = 0, i = 0;
    pProduto encProduto,auxProduto;
    /*Guardar cada produto que foi alterado*/
    pProduto* trackChanges = calloc(enc->produtosTotal,sizeof(pArmario));
    
    /*Verificar se existe algum produto a encomendar*/
    if (!db || !enc) return 0;
    /*Lista da encomenda*/
    encProduto = enc->produtos;
    while (encProduto)
    {
        auxProduto = getProdutoByNum(db->corredores,encProduto->num);
        /*Encontrado o produto?*/
        if (auxProduto && auxProduto->qtd >= encProduto->qtd)
        {
            /*Satisfazer a encomenda*/
            if (!needRollback)
            {
                auxProduto->lastQtd = auxProduto->qtd;
                auxProduto->qtd -= encProduto->qtd;
                trackChanges[total] = auxProduto;
                
                /*ToDo - gerar relatório da venda dos produtos*/
            }
            total++;
        }
        else
        {
            /*Criar aviso para encomendar o produto*/
            encProduto->needStock = 1;
            needRollback = 1;
        }
        encProduto = encProduto->next;
    }
    /*Rollback*/
    if (needRollback)
    {
        /*Colocar o produto como estava antes da satisfação da encomenda*/
        auxProduto = trackChanges[i];
        while (auxProduto)
        {
            auxProduto->qtd = auxProduto->lastQtd;
            auxProduto = trackChanges[++i];
        }
    }
    free(trackChanges);
    return total;
}

void freeEncomenda(pEncomenda p)
{
    if (p)
    {
        if (p->produtos)
        {
            freeProduto(p->produtos);
            p->produtos = NULL;
        }
        free(p);
    }
}

void checkEncomenda(pEncomenda enc)
{
    if (!enc) return;
    pProduto auxProduto;
    auxProduto = enc->produtos;
    if (auxProduto)
        printf("\nEncomendar os seguintes produtos:");
    while (auxProduto)
    {
        if (auxProduto->needStock)
        {
            printf("\nP%d: %d",auxProduto->num,auxProduto->qtd);
        }
        auxProduto = auxProduto->next;
    }
}

void doRouteProduto(pArmario armario, pProduto p)
{
    pProduto auxProduto, routeProduto;
    int showCabecalho = 1;
    /*Para cada armário*/
    if (armario && p)
    {
        /*Indicar o caminho a percorrer para cada produto*/
        routeProduto = p;
        while (routeProduto)
        {
            /*Tenta encontrar o produto no armário atual*/
            auxProduto = findProduto(armario,routeProduto->num);
            if (auxProduto)
            {
                if (showCabecalho)
                {
                    printf("\nIr ao corredor %d, armario %d e retirar:",armario->parent->ID,armario->ID);
                    showCabecalho = 0;
                }
                printf("\n P%d: %d unidades",
                       routeProduto->num,routeProduto->qtd);
            }
            routeProduto = routeProduto->next;
        }
    }
}

void doRouteEncomenda(pDatabase db, pEncomenda encomenda)
{
    /*ToDo - poderá necessitar de optimização*/
    forEachArmario(db,encomenda->produtos,doRouteProduto);
}

pEncomenda loadEncomenda(char *path, pDatabase db)
{
    FILE *f = NULL;
    pEncomenda encomenda = NULL;
    /*Ler o ficheiro de texto*/
    f = fopen(path,"r");
    if (f)
    {
        encomenda = getEncomenda(f);
    }
    else
        printf("(loadEncomenda)Erro: ficheiro não existe");
    fclose(f);
    return encomenda;
}

int doEncomendaMenu()
{
    int option = 0;
    while (option != 1 && option != KEYESCAPE)
    {
        /*Inicializar a janela*/
        printWindow();
        mvprintw(activeRow++,STARTCOL,"Encomendas");
        mvprintw(activeRow++,STARTCOL," 1. Importar ficheiro da pasta \"%s\"",PathEncomendas);
        activeRow++;
        mvprintw(activeRow++,STARTCOL,"ESC. Voltar ao menu");
        refresh();
        option = getch() - ASCIIZERO;
    }
    return option;
}

int doEncomendaPorFicheiro(pDatabase db)
{
    char fileName[MAXFILENAME+1]; /* +1 por causa da leitura pelo getnstr */
    char path[MAXPATH];
    int totalSatisfeitos, res = 0;
    pEncomenda encomenda;
    pProduto auxProduto;
    
    /*Cabeçalho*/
    printWindow();
    mvprintw(activeRow++,STARTCOL,"Encomendas");
    mvprintw(activeRow++,STARTCOL," 1. Importar ficheiro da pasta \"%s\"",PathEncomendas);
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
    getFullPath(path,sizeof(path),PathEncomendas,fileName);
    
    if (access(path,W_OK) == -1)
    {
        mvprintw(activeRow++,STARTCOL," Ficheiro não existe");
        activeRow++;
        if (ask("Deseja tentar novamente?"))
        {
            /*Recursivo*/
            res = doEncomendaPorFicheiro(db);
        }
        else
            return KEYESCAPE;
    }
    else
    {
        /*Carregar lista de produtos*/
        encomenda = loadEncomenda(path,db);
        
        /*Encomenda com os produtos*/
        if (encomenda)
        {
            mvprintw(activeRow++,STARTCOL," Nome: %s",encomenda->nome);
            mvprintw(activeRow++,STARTCOL," Total de produtos: %d",encomenda->produtosTotal);
            activeRow++;
            
            auxProduto = encomenda->produtos;
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
            if (ask("Deseja satisfazer?"))
            {
                /*Satisfazer encomenda*/
                totalSatisfeitos = doSatisfazerEncomenda(db,encomenda);
                /*Verificar se foi satisfeita*/
                if (encomenda->produtosTotal - totalSatisfeitos == 0)
                {
                    mvprintw(activeRow++,STARTCOL,"Encomenda satisfeita com sucesso");
                    
                    if (ask("Deseja visualizar o caminho para os produtos?"))
                    {
                        doRouteEncomenda(db,encomenda);
                    }
                }
                else
                {
                    mvprintw(activeRow++,STARTCOL,"Encomenda não foi satisfeita por falta de stock de %d produtos",encomenda->produtosTotal - totalSatisfeitos);
                    
                    if (ask("Deseja visualizar esses produtos?"))
                    {
                        //doShowProdutos(db,encomenda->produtos);
                    }
                    
                    //checkEncomenda(encomenda);
                }
            }
            else
                activeRow++;
            
            /*ToDo - verificar problema de memória com ncurses.h*/
            //freeEncomenda(encomenda);
        }
        else
        {
            mvprintw(activeRow++,STARTCOL," Ficheiro sem artigos ou inválido");
        }
        activeRow++;
    }
    return res;
}

void doEncomenda(pDatabase db)
{
    /*MENU de seleção*/
    switch (doEncomendaMenu()) {
        case KEYESCAPE:
            return;
        default:
            if (doEncomendaPorFicheiro(db) == KEYESCAPE)
                return;
            break;
    }
    
    mvprintw(activeRow++,STARTCOL,"Prima escape para voltar ao menu");
    refresh();
    while (getch() != ASCIIESC);
}