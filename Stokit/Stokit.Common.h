#define STARTCOL 3
#define STARTROW 3

#define LIMIT_CORREDOR 3
#define LIMIT_ARMARIO 4
#define LIMIT_PRODUTO 6

#define MAXFILENAME 64
#define MAXPATH 256
#define MAXBUFFER 512
#define MAXNAME 100

#define ASCIIZERO 48
#define ASCIIESC 27
#define KEYESCAPE -21

typedef struct recDatabase Database, *pDatabase;
typedef struct recCorredor Corredor, *pCorredor;
typedef struct recArmario Armario, *pArmario;
typedef struct recProduto Produto, *pProduto;
typedef struct recEncomenda Encomenda, *pEncomenda;

/*Globais: exceção à regra*/
int maxCol, maxRow, activeRow, limitRow;

struct recDatabase {
    int maxCorredores;
    int maxArmarios;
    pCorredor corredores;
    pCorredor lastCorredor;
};

struct recCorredor {
    int ID; /*sequencial*/
    pCorredor next;
    pCorredor prev;
    pArmario armarios;
    pArmario lastArmario;
};

struct recArmario {
    int ID; /*sequencial*/
    pArmario next;
    pArmario prev;
    pCorredor parent;
    pProduto produtos;
    pProduto lastProduto;
    int produtosTotal;
};

struct recProduto {
    int num;
    int qtd;
    int needStock;
    int lastQtd;
    pProduto next;
    pProduto prev;
    pArmario parent;
};

struct recEncomenda {
    char nome[MAXNAME];
    pProduto produtos;
    pProduto lastProduto;
    int produtosTotal;
};

typedef void (*funcArmario)(pArmario,pProduto);

void showCorredores(pCorredor p, int showProdutos);
void removeBreakLine(char *result);
void getFullPath(char *result, int size, char *path, char *filename);
int ask(char *question);