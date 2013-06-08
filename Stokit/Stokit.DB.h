#define DB "retail.bin"
#define DBPath "/Users/ricardopereira/Desktop/Alias/2Semestre/Programação/Trabalho Prático/"
#define PathReposicao "reposicao/"
#define PathEncomendas "encomendas/"

void freeDB(pDatabase p);
pDatabase loadDB(const char *path);
int saveDB(const char *path, pDatabase list);