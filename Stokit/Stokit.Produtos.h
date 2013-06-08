pProduto newProduto(pArmario parent, pProduto p, int n, int qtd);
void freeProduto(pProduto p);
pProduto getProduto(pCorredor listDB, int IDCorredor, int IDArmario);
pProduto getProdutoByNum(pCorredor listDB, int n);
pProduto addProdutoEncomenda(pEncomenda encomenda, int n, int qtd);
pProduto findProduto(pArmario armario, int n);