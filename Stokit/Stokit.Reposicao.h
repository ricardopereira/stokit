int loadReposicaoStock(char *filename, pDatabase db);
pArmario getReposicaoStock(FILE *f);
int doReporStock(pDatabase db, pProduto list);
/*Interfaced*/
void doReposicao(pDatabase db);