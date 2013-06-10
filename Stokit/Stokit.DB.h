#define DB "retail.bin"
#define DBPath "/Users/ricardopereira/Desktop/Alias/2Semestre/Programação/Trabalho Prático/"
#define PathReposicao "Reposicao/"
#define PathEncomendas "Encomendas/"

void freeDB(pDatabase p);
pDatabase loadDB(const char *path);
int saveDB(const char *path, pDatabase list);

void saveResumoDia(pDatabase db);
void loadAlertasStock(pDatabase db);
void saveAlertasStock(pDatabase db);