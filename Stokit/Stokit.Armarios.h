pArmario newArmario(pCorredor parent, pArmario p);
void deleteArmario(pCorredor parent, pArmario p);
void freeArmario(pArmario p);
pArmario getArmario(pCorredor listDB, int IDCorredor, int IDArmario);
pArmario getArmarioComMenorProdutos(pDatabase db);