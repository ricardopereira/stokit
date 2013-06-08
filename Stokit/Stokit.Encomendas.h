int loadEncomenda(char *filename, pDatabase db);
pEncomenda getEncomenda(FILE *f);
int doSatisfazerEncomenda(pDatabase db, pEncomenda enc);
void freeEncomenda(pEncomenda p);