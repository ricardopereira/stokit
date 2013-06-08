typedef struct recDatabase Database, *pDatabase;
typedef struct recCorredor Corredor, *pCorredor;
typedef struct recArmario Armario, *pArmario;
typedef struct recProduto Produto, *pProduto;
typedef struct recEncomenda Encomenda, *pEncomenda;

struct recDatabase {
    int maxCorredores;
    int maxArmarios;
    pCorredor corredores;
    pCorredor lastCorredor;
};

struct recCorredor {
    int ID; //sequencial
    pCorredor next;
    pCorredor prev;
    pArmario armarios;
    pArmario lastArmario;
};

struct recArmario {
    int ID; //sequencial
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
    char *nome;
    pProduto produtos;
    pProduto lastProduto;
    int produtosTotal;
};

typedef void (*funcArmario)(pArmario,pProduto);

void showCorredores(pCorredor p, int showProdutos);
void removeBreakLine(char *result);
void getFullPath(char *result, int size, char *path, char *filename);