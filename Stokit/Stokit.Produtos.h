pProduto newProduto(pArmario parent, pProduto p, int n, int qtd);
void freeProduto(pProduto p);
pProduto getProduto(pCorredor corredor, int IDCorredor, int IDArmario);
pProduto getProdutoByNum(pCorredor corredor, int n);
pProduto addProdutoEncomenda(pEncomenda encomenda, int n, int qtd);
pProduto findProduto(pArmario armario, int n);