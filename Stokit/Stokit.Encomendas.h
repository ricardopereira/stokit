pEncomenda loadEncomenda(char *path, pDatabase db);
pEncomenda getEncomenda(FILE *f);
int doSatisfazerEncomenda(pDatabase db, pEncomenda enc);
void freeEncomenda(pEncomenda p);
/*Interfaced*/
void doEncomenda(pDatabase db);