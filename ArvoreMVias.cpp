// ====================================================================================================================
    
// INTEGRANTES DO GRUPO:
// Caio Monteiro Sartori                 N° 15444598
// Mateus Henrique J. S. Carriel         N° 15698362
// Murilo Augusto Jorge                  N° 15552251

// ====================================================================================================================

// ====================================================================================================================

// INCLUSAO DE BIBLIOTECAS E ARQUIVOS:
#include "ArvoreMVias.h"    // Arquivo Header do Presente Arquivo.
#include <cstring>          // Funcoes de Manipulacao de Memoria (memset).
#include <sstream>          // Manipulacao de Streams de Strings.
#include <cmath>            // Funcoes Matematicas.
#include <iostream>         // Operacoes de Entradas e Saidas.
#include <iomanip>          // Formatacao de Saida.

// ====================================================================================================================

// ====================================================================================================================
// CONSTRUTOR: Configura os Nomes de Arquivo e a Ordem (M):
ArvoreMVias::ArvoreMVias(const string& txt, const string& bin, const string& dados) {
    arquivoTxt = txt;       // Inicializa o nome do Arquivo de Texto.
    arquivoBin = bin;       // Inicializa o nome do Arquivo de Indice Binario (onde a Arvore eh Salva).
    arquivoDados = dados;   // Inicializa o nome do Arquivo de Dados Principal.
    T = (M + 1) / 2;        // Define o Grau Minimo (T) necessario.
    raiz = 1;               // Inicializa o ID do Noh Raiz.
    nextNodeId = 1;         // Inicializa o ID do proximo Noh a ser Criado.

} // Fim do CONSTRUTOR
// ====================================================================================================================

// ====================================================================================================================
// METODOS DE SEQUENCIAMENTO E I/O DE DISCO:

// Calcula o Numero de Inteiros que compoem um Noh no Disco.
// FORMATO: [n (1 int), folha (1 int), chaves (M-1 ints), filhos (M ints)]
int ArvoreMVias::nodeInts() const {
    return 2 + (M - 1) + M + 1; // 2 * M + 1
}

void ArvoreMVias::writeHeader() {
    // Abre o arquivo binario, criando-o se nao existir
    fstream fout(arquivoBin, ios::binary | ios::in | ios::out); // Abre o arquivo para leitura e escrita, em modo binario.
    if (!fout) {                                                    // Verifica se a abertura falhou.
        ofstream cr(arquivoBin, ios::binary);                       // Tenta criar o arquivo.
        cr.close();                                                 // Fecha a criacao.
        fout.open(arquivoBin, ios::binary | ios::in | ios::out);    // Tenta reabrir para Escrita/Leitura.
    } // Fim do IF

    fout.seekp(0);                                                  // Posiciona o ponteiro de escrita no inicio do arquivo (posicao 0).
    int aM = M, aRaiz = raiz, aNext = nextNodeId;                   // Cria copias temporarias das variaveis.
    fout.write((char*)&aM, sizeof(int));                            // Escreve a Ordem M.
    fout.write((char*)&aRaiz, sizeof(int));                         // Escreve o ID da Raiz.
    fout.write((char*)&aNext, sizeof(int));                         // Escreve o Proximo ID disponivel.
    fout.close();                                                   // Fecha o arquivo.
}

bool ArvoreMVias::readHeader() {
    ifstream fin(arquivoBin, ios::binary);                           // Abre o arquivo binario para leitura.
    if (!fin) return false;                                         // Se a abertura falhar, retorna falso.
    fin.seekg(0);                                                   // Posiciona o ponteiro de leitura no inicio.
    int aM, aRaiz, aNext;                                           // Variaveis para armazenar o cabecalho.

    if (!fin.read((char*)&aM, sizeof(int))) { fin.close(); return false; } // Leh M. Se falhar, retorna falso.
    if (!fin.read((char*)&aRaiz, sizeof(int))) { fin.close(); return false; } // Leh o ID da Raiz. Se falhar, retorna falso.
    if (!fin.read((char*)&aNext, sizeof(int))) { fin.close(); return false; } // Leh o Proximo ID. Se falhar, retorna falso.

    raiz = aRaiz;                                                   // Atualiza a variavel membro 'raiz'.
    nextNodeId = aNext;                                             // Atualiza a variavel membro 'nextNodeId'.
    fin.close();                                                    // Fecha o arquivo.
    return true;                                                    // Retorna verdadeiro se a leitura foi bem sucedida.
}

// Simula a escrita de um bloco no disco (incrementa escritaDisco)
void ArvoreMVias::writeNode(int id, const vector<int>& vals) {
    fstream fout(arquivoBin, ios::in | ios::out | ios::binary);     // Abre o arquivo para escrita e leitura.
    if (!fout) { cerr << "Erro de arquivo (writeNode).\n"; return; } // Imprime erro se falhar.

    int headerBytes = 3 * sizeof(int);                              // Calcula o tamanho do cabecalho (M, raiz, nextNodeId).
    int ints = nodeInts();                                          // Obtem o numero de inteiros por noh.
    int nodeBytes = ints * sizeof(int);                             // Calcula o tamanho de um noh em bytes.

    // Calcula a posicao no arquivo: Header + (ID - 1) * Tamanho do Noh
    streampos pos = static_cast<std::streamoff>(headerBytes) + static_cast<std::streamoff>((id - 1) * nodeBytes);
    fout.seekp(pos);                                                // Posiciona o ponteiro de escrita.

    for (int i = 0; i < ints; ++i) {                                 // Itera sobre todos os inteiros do noh.
        int v = vals[i];                                            // Obtem o valor do vetor em memoria.
        fout.write((char*)&v, sizeof(int));                         // Escreve o inteiro no arquivo.
    } // Fim do FOR

    fout.close();                                                   // Fecha o arquivo.
}

// Simula a leitura de um bloco no disco (incrementa leituraDisco)
bool ArvoreMVias::readNode(int id, vector<int>& vals) {
    ifstream fin(arquivoBin, ios::binary);                           // Abre o arquivo para leitura.
    if (!fin) return false;                                         // Retorna falso se a abertura falhar.

    int headerBytes = 3 * sizeof(int);                              // Tamanho do cabecalho.
    int ints = nodeInts();                                          // Numero de inteiros por noh.
    int nodeBytes = ints * sizeof(int);                             // Tamanho do noh em bytes.

    // Calcula a posicao no arquivo
    streampos pos = static_cast<std::streamoff>(headerBytes) + static_cast<std::streamoff>((id - 1) * nodeBytes);

    // Verifica se o no existe (evita leitura fora do limite)
    fin.seekg(0, ios::end);                                         // Move o ponteiro para o fim do arquivo.
    streampos fileSize = fin.tellg();                               // Obtem o tamanho total do arquivo.

    if (pos + static_cast<std::streamoff>(nodeBytes) > fileSize) {  // Se a posicao de leitura excede o tamanho do arquivo.
        fin.close();                                                // Fecha e retorna falso (noh nao existe).
        return false;
    } // Fim do IF

    fin.seekg(pos);                                                 // Posiciona o ponteiro de leitura no inicio do noh.
    vals.assign(ints, 0);                                           // Redimensiona o vetor de valores para receber a leitura, inicializando com zeros.

    for (int i = 0; i < ints; ++i) {                                 // Itera para ler cada inteiro do noh.
        int v = 0;
        fin.read((char*)&v, sizeof(int));                           // Leh o inteiro.
        vals[i] = v;                                                // Armazena no vetor.
    } // Fim do FOR

    fin.close();                                                    // Fecha o arquivo.
    return true;                                                    // Retorna verdadeiro.
}

// Getters/Setters do no serializado
int ArvoreMVias::node_get_n(const vector<int>& vals) const { return vals[0]; } // Obtem o numero de chaves (posicao 0).
void ArvoreMVias::node_set_n(vector<int>& vals, int n) { vals[0] = n; }        // Define o numero de chaves (posicao 0).

bool ArvoreMVias::node_get_folha(const vector<int>& vals) const { return vals[1] != 0; } // Obtem a flag de folha (posicao 1).
void ArvoreMVias::node_set_folha(vector<int>& vals, bool folha) { vals[1] = folha ? 1 : 0; } // Define a flag de folha (posicao 1).

int ArvoreMVias::node_get_chave(const vector<int>& vals, int idx) const { return vals[2 + idx]; }   // Obtem a chave na posicao 2 + indice.
void ArvoreMVias::node_set_chave(vector<int>& vals, int idx, int chave) { vals[2 + idx] = chave; }  // Define a chave na posicao 2 + indice.

int ArvoreMVias::node_get_filho(const vector<int>& vals, int idx) const { return vals[2 + (M - 1) + 1 + idx]; } // Obtem o ID do filho na posicao apos chaves + indice.
void ArvoreMVias::node_set_filho(vector<int>& vals, int idx, int filho) { vals[2 + (M - 1) + 1 + idx] = filho; }// Define o ID do filho na posicao apos chaves + indice.


int ArvoreMVias::createNode(bool folha) {
    int id = nextNodeId++;                                          // Atribui o proximo ID disponivel e incrementa o contador.
    int ints = nodeInts();                                          // Obtem o tamanho do noh.
    vector<int> vals(ints, 0);                                      // Inicializa o vetor do noh com zeros.

    node_set_n(vals, 0);                                            // Define o numero de chaves como 0.
    node_set_folha(vals, folha);                                    // Define se o noh eh uma folha.

    // Inicializa filhos e chaves com 0 (ja feito pelo constructor do vetor)
    for (int i = 0; i < M; ++i) node_set_filho(vals, i, 0);
    for (int i = 0; i < M - 1; ++i) node_set_chave(vals, i, 0);

    writeNode(id, vals);                                            // Grava o novo noh no disco.
    writeHeader();                                                  // Atualiza nextNodeId no cabecalho.
    return id;                                                      // Retorna o ID do noh criado.
}

void ArvoreMVias::geradorBinario() {
    // Tenta ler o header; se falhar, inicializa o arquivo
    bool ok = readHeader();                                         // Tenta ler o cabecalho existente.
    if (!ok) {                                                      // Se falhar (arquivo novo ou corrompido).
        raiz = 1;                                                   // Define a raiz como ID 1.
        nextNodeId = 1;                                             // Comeca a contagem de IDs em 1.
        writeHeader();                                              // Escreve o novo cabecalho.
        createNode(true);                                           // Cria o primeiro noh (raiz, ID 1) como folha.
        cout << "Arquivo binario de indice inicializado com raiz vazia.\n\n"; // Imprime inicializacao.
    } else {
        cout << "Arquivo binario existente lido. Raiz ID=" << raiz << " Proximo ID=" << nextNodeId << "\n\n"; // Imprime leitura existente.
    } // Fim do ELSE
}

void ArvoreMVias::print() {

    if (!readHeader()) {                                            // Leh o cabecalho.
        cout << "Arquivo binario nao encontrado ou corrompido.\n";   // Imprime erro se falhar.
        return;
    } // Fim do IF

    cout << "\n--- Conteudo da Arvore B (Indice) ---\n";             // Imprime cabecalho da saida.
    cout << "Ordem (M): " << M << ", Grau Minimo (T): " << (M + 1) / 2 << endl; // Imprime M e T.
    cout << "Raiz ID: " << raiz << endl;                             // Imprime o ID da raiz.
    cout << "------------------------------------------------------------------\n";
    // Formato: No ID | n (chaves) | A[0] (filho) | (K[0],A[1]),...,(K[n-1],A[n])
    cout << "ID n | A[0] | (K[0],A[1]),...,(K[n-1],A[n])\n";
    cout << "------------------------------------------------------------------\n";

    for (int id = 1; id < nextNodeId; ++id) {                        // Itera sobre todos os possiveis IDs de noh.
        vector<int> vals;                                           // Vetor para o noh.
        if (!readNode(id, vals)) continue;                           // Leh o noh. Se falhar, pula para o proximo ID.

        int n = node_get_n(vals);                                   // Obtem o numero de chaves.

        cout << setw(2) << id << " " << n << " | " << node_get_filho(vals, 0); // Imprime ID, n e o primeiro ponteiro A[0].

        for (int i = 0; i < n; ++i) {                                // Itera sobre as chaves e os ponteiros restantes.
            cout << " | (" << setw(2) << node_get_chave(vals, i) << "," << node_get_filho(vals, i + 1) << ")"; // Imprime (K[i], A[i+1]).
        } // Fim do FOR

        cout << endl;                                                 // Quebra de linha apos o noh.
    } // Fim do FOR

    cout << "------------------------------------------------------------------\n";
}
// ====================================================================================================================

// ====================================================================================================================
// METODOS DE BUSCA:

// Requisito C: mSearch - Retorna o triplo (ID No, Posicao, Encontrado)
Resultado ArvoreMVias::mSearch(int chave) {

    if (!readHeader()) {                                            // Leh o cabecalho.
        cerr << "Erro ao abrir " << arquivoBin << " (header).\n";   // Imprime erro se falhar.
        return {-1, -1, false};                                     // Retorna resultado de erro.
    } // Fim do IF

    int p = raiz;                                               // Inicia a busca no ID da raiz.
    while (p != 0) {                                            // Loop: desce enquanto o noh atual nao for nulo.
        vector<int> vals;                                       // Vetor para o noh.

        if (!readNode(p, vals)) {
            cerr << "Erro ao ler noh " << p << " do arquivo.\n"; // Imprime erro se falhar.
            return {-1, -1, false};
        } // Fim do IF

        int n = node_get_n(vals);                               // Obtem o numero de chaves.
        int i = 0;                                              // Inicializa o indice de busca.

        // Encontra a posicao de insercao (ou a chave, se existir)
        while (i < n && chave > node_get_chave(vals, i))        // Avanca 'i' enquanto a chave a buscar for maior que a chave K[i].
            i++;                                                // i aponta para K[i] >= chave ou i=n.

        if (i < n && chave == node_get_chave(vals, i)) {        // Se encontrou a chave na posicao i.
            // Encontrou a chave
            return {p, i + 1, true};                            // Retorna ID do noh, posicao (1-based), e 'encontrou=true'.
        } // Fim do IF

        // Desce para o filho correspondente
        int filho = node_get_filho(vals, i);                    // Obtem o ID do filho A[i].
        if (filho == 0) {                                       // Se o ponteiro for nulo (chegou em uma folha).
            // Chegou em folha — chave deve ser inserida aqui na posicao i
            return {p, i + 1, false};                           // Chave nao encontrada. Retorna noh atual e posicao de insercao.
        } // Fim do IF

        p = filho;                                              // Continua a busca no noh filho.
    } // Fim do WHILE

    // Arvores vazias ou erro inesperado
    return {-1, -1, false};
}
// ====================================================================================================================

// ====================================================================================================================
// METODOS DE INSERCAO:

void ArvoreMVias::insertB(int chave, const string& titulo, const string& autor, int ano, const string& genero) {
    // 1. Verifica se a chave ja existe no indice
    Resultado r = mSearch(chave);                                   // Busca a chave no indice.
    if (r.encontrou) {                                              // Se a chave ja existe.
        cout << "Aviso: ID " << chave << " ja existe no indice. Insercao cancelada.\n"; // Imprime aviso.
        return;                                                     // Aborta.
    } // Fim do IF

    // 2. Escreve no arquivo principal
    ofstream foutDados(arquivoDados, ios::binary | ios::app);       // Abre o arquivo de dados em modo append.
    if (!foutDados) {                                               // Se a abertura falhar.
        cerr << "Erro ao abrir " << arquivoDados << " para escrita.\n"; // Imprime erro.
        return;                                                     // Aborta.
    } // Fim do IF

    Registro novoLivro;                                             // Preenche a estrutura de registro.
    novoLivro.chave = chave;
    novoLivro.ano = ano;

    size_t lenTitulo = min(titulo.length(), (size_t)99);            // Copia Titulo.
    titulo.copy(novoLivro.titulo, lenTitulo);
    novoLivro.titulo[lenTitulo] = '\0';

    size_t lenAutor = min(autor.length(), (size_t)49);              // Copia Autor.
    autor.copy(novoLivro.autor, lenAutor);
    novoLivro.autor[lenAutor] = '\0';

    size_t lenGenero = min(genero.length(), (size_t)29);            // Copia Genero.
    genero.copy(novoLivro.genero, lenGenero);
    novoLivro.genero[lenGenero] = '\0';

    novoLivro.deletado = false;                                     // Marca como ativo.

    foutDados.write(reinterpret_cast<const char*>(&novoLivro), Registro::getSize()); // Grava o registro no arquivo de dados.
    foutDados.close();                                              // Fecha o arquivo de dados.

    // 3. Insere na arvore B seguindo o pseudocodigo
    if (!readHeader()) {                                            // Leh o cabecalho.
        cerr << "Erro ao ler header antes de inserir.\n";
        return;
    } // Fim do IF

    // Linha 1: A ← 0; K ← X
    int K = chave;                                                  // Chave promovida (inicialmente a chave a inserir).
    int A = 0;                                                      // Ponteiro promovido (inicialmente nulo, 0).

    // Linha 2: (p,i,achou) ← mSearch(T,X)
    int p = r.indice_no;                                            // Noh onde a chave deve ser inserida.
    bool achou = r.encontrou;

    if (achou) {                                                    // Re-verifica se encontrou (dupla checagem).
        cout << "ID ja encontrado. Insercao cancelada." << endl;
        return;
    } // Fim do IF

    // Linha 4: while p ≠ 0 do
    while (p != 0) {                                                // Loop: propaga a chave K e o ponteiro A para cima na arvore.

        // Linha 5: insira (K,A) nas posicoes apropriadas de P
        vector<int> nodeVals;
        readNode(p, nodeVals);                                      // Leh o noh pai atual.

        int n = node_get_n(nodeVals);                               // Obtem o numero de chaves.

        // Encontrar posicao de insercao correta
        int pos = 0;
        while (pos < n && K > node_get_chave(nodeVals, pos)) {       // Encontra a posicao para K (chave promovida).
            pos++;
        } // Fim do WHILE

        // *** INSERCAO CORRIGIDA: Implementacao do Shift e Insercao ***

            // Primeiro: Shift das chaves para a direita
            for (int j = n; j > pos; j--) {
                node_set_chave(nodeVals, j, node_get_chave(nodeVals, j - 1));
            } // Fim do FOR

            // Segundo: Shift dos filhos para a direita (se nao for folha)
            if (!node_get_folha(nodeVals)) {
                // Shift dos filhos
                for (int j = n + 1; j > pos + 1; j--) {
                    node_set_filho(nodeVals, j, node_get_filho(nodeVals, j - 1));
                } // Fim do FOR
            } // Fim do IF

            // Inserir a CHAVE
            node_set_chave(nodeVals, pos, K);                      // Insere a chave K na posicao aberta.

            // Inserir o FILHO (se nao for folha)
            if (!node_get_folha(nodeVals)) {
                // A posicao do filho eh pos + 1
                node_set_filho(nodeVals, pos + 1, A);             // Insere o ponteiro A (o novo noh direito) na posicao pos + 1.
            } // Fim do IF

            node_set_n(nodeVals, n + 1);                          // Incrementa o numero de chaves.

        // Linha 6: if n ≤ m-1 then (noh nao esta cheio)
        if (node_get_n(nodeVals) <= M - 1) {                       // Se o noh nao estah cheio apos a insercao.
            writeNode(p, nodeVals);                               // Grava o noh atualizado no disco.
            cout << "Livro inserido com sucesso no indice e no arquivo principal.\n";
            return;                                               // Retorna (insercao concluida).
        } // Fim do IF

        // Linha 7: p precisa ser fracionado (Split)

        bool ehFolha = node_get_folha(nodeVals);                    // Guarda se o noh eh folha.
        int t = T;                                                  // Obtem o grau minimo.
        int medianaIndex = t - 1;                                   // Indice da chave mediana.
        int mediana = node_get_chave(nodeVals, medianaIndex);       // Valor da chave mediana a ser promovida.

        // *** SPLIT CORRIGIDO: Coleta e Redistribuicao ***

        // Salvar dados originais ANTES de qualquer modificacao
        vector<int> chavesOriginais;                                // Vetor para armazenar M chaves temporariamente.
        vector<int> filhosOriginais;                                // Vetor para armazenar M+1 ponteiros temporariamente.

        // Salvar APENAS as chaves validas (n, que agora eh M)
        for (int i = 0; i < node_get_n(nodeVals); i++) {
            chavesOriginais.push_back(node_get_chave(nodeVals, i));
        } // Fim do FOR

        // Salvar APENAS os filhos validos (n+1, que agora eh M)
        if (!ehFolha) {
            for (int i = 0; i <= node_get_n(nodeVals); i++) {
                filhosOriginais.push_back(node_get_filho(nodeVals, i));
            } // Fim do FOR
        } // Fim do IF

        // Criar novo noh
        int q = createNode(ehFolha);                                // Cria o novo noh direito 'q'.
        vector<int> qVals;
        readNode(q, qVals);                                         // Leh o novo noh.

        // LIMPEZA COMPLETA: Zera p e q (redundante, mas seguro)
        for (int i = 0; i < M - 1; i++) node_set_chave(nodeVals, i, 0); // Limpa chaves de p.
        for (int i = 0; i < M; i++) node_set_filho(nodeVals, i, 0); // Limpa ponteiros de p.
        node_set_n(nodeVals, 0);
        node_set_folha(nodeVals, ehFolha);

        for (int i = 0; i < M - 1; i++) node_set_chave(qVals, i, 0); // Limpa chaves de q.
        for (int i = 0; i < M; i++) node_set_filho(qVals, i, 0); // Limpa ponteiros de q.
        node_set_n(qVals, 0);
        node_set_folha(qVals, ehFolha);

        // REDISTRIBUIcAO CORRETA
        // Noh p: primeiras t-1 chaves
        node_set_n(nodeVals, t - 1);                                // p fica com T-1 chaves.
        for (int i = 0; i < t - 1; i++) {
            node_set_chave(nodeVals, i, chavesOriginais[i]);        // Copia as primeiras T-1 chaves para p.
        } // Fim do FOR

        // Noh q: ultimas t-1 chaves
        node_set_n(qVals, chavesOriginais.size() - t);              // q fica com o restante das chaves (apos a mediana).
        for (int j = 0; j < node_get_n(qVals); j++) {
            node_set_chave(qVals, j, chavesOriginais[j + t]);       // Copia as chaves [T] ate o final para q.
        } // Fim do FOR

        // Distribuir filhos CORRETAMENTE
        if (!ehFolha) {

            // p recebe primeiros t filhos [0..t-1] + A_{mediana} = A[0] ate A[t-1]
            for (int j = 0; j <= node_get_n(nodeVals); j++) {
                node_set_filho(nodeVals, j, filhosOriginais[j]);    // Copia os T primeiros ponteiros para p.
            } // Fim do FOR

            // q recebe ultimos filhos restantes [t+1..fim] = A[t] ate A[M]
            for (int j = 0; j <= node_get_n(qVals); j++) {
                node_set_filho(qVals, j, filhosOriginais[j + t]);   // Copia os T ponteiros restantes para q.
            } // Fim do FOR
        } // Fim do IF

        // Linha 8: Escreva p e q para o disco
        writeNode(p, nodeVals);                                     // Grava noh esquerdo (p) atualizado.
        writeNode(q, qVals);                                      // Grava noh direito (q) atualizado.

        // Linha 9: K ← K⌈m/2⌉; A ← q; P ← pai(p)
        K = mediana;                                                // A chave promovida K eh a mediana.
        A = q;                                                      // O ponteiro promovido A eh o noh q.

        // Encontrar pai de p para proxima iteracao
        int parent_p = 0;
        if (p == raiz) {
            // Se p era a raiz, sair do loop para criar nova raiz
            break;                                                  // Sai para tratar a criacao da nova raiz.
        } else {
            // Buscar pai de p
            parent_p = findParent(raiz, p, 0);                      // Busca o pai de p na arvore.
            if (parent_p == 0) {
                break;                                              // Se nao encontrar o pai, sai para evitar loop infinito.
            } // Fim do IF
        } // Fim do ELSE
        p = parent_p;                                               // O pai se torna o novo noh a ser processado no proximo loop.

    } // Fim do WHILE

    // Linha 10-13: Criar nova raiz (se saiu do loop porque p era raiz e houve split)
    int novaRaiz = createNode(false);                               // Cria a nova raiz (sempre noh interno).
    vector<int> novaRaizVals;
    readNode(novaRaiz, novaRaizVals);                               // Leh o novo noh.

    // Limpar novo noh (redundante apos createNode, mas seguro)
    for (int i = 0; i < M - 1; i++) node_set_chave(novaRaizVals, i, 0);
    for (int i = 0; i < M; i++) node_set_filho(novaRaizVals, i, 0);

    node_set_n(novaRaizVals, 1);                                    // Define 1 chave.
    node_set_chave(novaRaizVals, 0, K);                             // Insere a chave promovida (mediana).
    node_set_filho(novaRaizVals, 0, raiz);                          // O noh antigo (p) se torna o filho esquerdo.
    node_set_filho(novaRaizVals, 1, A);                             // O noh novo (q) se torna o filho direito.
    node_set_folha(novaRaizVals, false);                            // Confirma que nao eh folha.

    writeNode(novaRaiz, novaRaizVals);                              // Grava a nova raiz.

    // Atualizar raiz
    raiz = novaRaiz;                                                // Atualiza a variavel membro.
    writeHeader();                                                  // Atualiza o cabecalho.
    cout << "Livro inserido com sucesso no indice e no arquivo principal.\n";
}
// ====================================================================================================================

// ====================================================================================================================
// METODOS DE BUSCA DE ESTRUTURA (HELPERS):

int ArvoreMVias::findParent(int currentNode, int targetId, int parentId) {
    if (currentNode == 0) return 0;                                 // Caso base: ponteiro nulo.
    if (currentNode == targetId) return parentId;                   // Caso base: encontrou o noh alvo, retorna o ID do pai passado.

    vector<int> nodeVals;
    readNode(currentNode, nodeVals);                                // Leh o noh atual.

    // Se eh folha, nao eh pai de ninguem
    if (node_get_folha(nodeVals)) return 0;

    // Verificar todos os filhos
    for (int i = 0; i <= node_get_n(nodeVals); i++) {               // Itera sobre todos os n+1 filhos.
        int child = node_get_filho(nodeVals, i);                    // Obtem o ID do filho.
        if (child != 0) {                                           // Se o filho nao for nulo.
            int result = findParent(child, targetId, currentNode);  // Chamada recursiva, passando o noh atual como novo pai.
            if (result != 0) return result;                         // Se o pai for encontrado na sub-arvore, retorna.
        } // Fim do IF
    } // Fim do FOR

    return 0;                                                       // Retorna 0 se o pai nao for encontrado.
}

int ArvoreMVias::findChildIndex(int parentId, int childId) {

    vector<int> parentVals;                                         // Leh o noh pai.
    if (!readNode(parentId, parentVals)) return -1;                 // Retorna -1 se falhar.

    int n = node_get_n(parentVals);                                 // Obtem o numero de chaves.

    for (int i = 0; i <= n; i++) {                                  // Itera sobre todos os ponteiros A[i].
        int currentChild = node_get_filho(parentVals, i);           // Obtem o ID do filho.
        if (currentChild == childId) {                              // Se encontrar o ID.
            return i;                                               // Retorna o indice do ponteiro.
        } // Fim do IF
    } // Fim do FOR

    return -1;                                                      // Retorna -1 se nao encontrar.
}
// ====================================================================================================================

// ====================================================================================================================
// METODOS DE REMOcao (DELETE):

void ArvoreMVias::deleteB(int chave) {
    if (!readHeader()) {                                            // Leh o cabecalho.
        cerr << "Erro ao ler header antes da remocao.\n";
        return;
    } // Fim do IF

    // Verifica se a chave existe no indice
    Resultado r = mSearch(chave);                                   // Busca a chave.
    if (!r.encontrou) {                                             // Se nao encontrar.
        cout << "Aviso: ID " << chave << " nao existe no indice. Remocao cancelada.\n";
        return;                                                     // Aborta.
    } // Fim do IF

    // Chama a funcao recursiva de delecao
    deleteFromNode(raiz, chave);                                    // Inicia a remocao na raiz.

    // Verifica se a raiz ficou vazia
    vector<int> rootVals;
    if (readNode(raiz, rootVals)) {
        if (node_get_n(rootVals) == 0 && !node_get_folha(rootVals)) { // Se a raiz tem 0 chaves e nao eh folha.
            int newRootId = node_get_filho(rootVals, 0);            // Obtem o ID do unico filho (A[0]).
            deleteNode(raiz);                                       // Limpa a raiz antiga.
            raiz = newRootId;                                       // Promove o filho a nova raiz.
            writeHeader();                                          // Atualiza o cabecalho.
        } // Fim do IF
    } // Fim do IF

    removeDataFromFile(chave);                                      // Marca o registro no arquivo de dados como deletado.
    cout << "Remocao do ID " << chave << " concluida.\n";            // Imprime sucesso.
}

void ArvoreMVias::deleteFromNode(int nodeId, int chave) {

    vector<int> nodeVals;
    readNode(nodeId, nodeVals);                                     // Leh o noh atual.

    int n = node_get_n(nodeVals);
    int i = 0;

    // Encontra a chave ou a posicao de descida
    while (i < n && chave > node_get_chave(nodeVals, i)) {          // Busca o indice.
        i++;
    } // Fim do WHILE

    // CASO 1: Chave encontrada neste noh
    if (i < n && chave == node_get_chave(nodeVals, i)) {
        if (node_get_folha(nodeVals)) {                             // Se for folha.
            // Caso 1A: Remocao simples em folha

            removeFromLeaf(nodeVals, i);                            // Remove a chave.
            writeNode(nodeId, nodeVals);                            // Grava o noh.

            // *** VERIFICAR UNDERFLOW NA FOLHA APOS REMOCAO ***
            int nAfter = node_get_n(nodeVals);                      // Obtem n apos a remocao.

            if (nAfter < T - 1) {                                   // Se ha underflow (n < T-1).
                // Precisamos encontrar o pai desta folha para corrigir o underflow
                int parentId = findParent(raiz, nodeId, 0);         // Busca o pai.
                if (parentId != 0) {
                    int childIndex = findChildIndex(parentId, nodeId); // Encontra o indice do filho no pai.
                    if (childIndex != -1) {
                        fillChild(parentId, childIndex);            // Chama a correcao (empresta ou funde).
                    } // Fim do IF
                } // Fim do IF
            } // Fim do IF
        } else {
            // Caso 1B: Remocao em noh interno
            removeFromInternalNode(nodeId, i);                      // Chama a remocao complexa em noh interno.
        } // Fim do ELSE
    } else {
        // CASO 2: Chave nao esta neste noh - descer recursivamente
        if (node_get_folha(nodeVals)) {                             // Se for folha, a chave nao existe (ja foi checado por mSearch).
            return;
        } // Fim do IF

        int childId = node_get_filho(nodeVals, i);                  // Obtem o ID do filho para descer.
        vector<int> childVals;
        readNode(childId, childVals);                               // Leh o noh filho.

        // Caso 2A: Filho tem numero minimo de chaves - garantir antes de descer
        if (node_get_n(childVals) == T - 1) {                       // Se o filho tem o minimo (T-1).
            fillChild(nodeId, i);                                   // Corrige o underflow (empresta ou funde).
            // Re-ler o noh apos possiveis mudancas
            readNode(nodeId, nodeVals);                             // Leh o pai novamente.
            // Recalcular childId apos fill (pode ter mudado)
            childId = node_get_filho(nodeVals, i);                  // Obtem o ID do filho atual (pode ser o mesmo ou o noh fundido).
        } // Fim do IF

        if (childId != 0) {                                         // Se o filho nao foi nulo (ou fundido).
            deleteFromNode(childId, chave);                         // Desce recursivamente.
        } // Fim do IF
    } // Fim do ELSE
}

void ArvoreMVias::removeFromLeaf(vector<int>& vals, int idx) {
    int n = node_get_n(vals);                                       // Obtem n.

    // Desloca chaves para a esquerda
    for (int i = idx; i < n - 1; i++) {                             // Move as chaves K[i+1] para K[i].
        node_set_chave(vals, i, node_get_chave(vals, i + 1));
    } // Fim do FOR

    node_set_chave(vals, n - 1, 0);                                 // Zera a ultima chave.
    node_set_n(vals, n - 1);                                        // Decrementa n.
}

void ArvoreMVias::removeFromInternalNode(int nodeId, int idx) {
    vector<int> nodeVals;
    readNode(nodeId, nodeVals);                                     // Leh o noh.

    int chaveParaDeletar = node_get_chave(nodeVals, idx);           // Guarda a chave a ser deletada.

    int leftChildId = node_get_filho(nodeVals, idx);                // ID do filho esquerdo (A[idx]).
    int rightChildId = node_get_filho(nodeVals, idx + 1);           // ID do filho direito (A[idx+1]).

    vector<int> leftVals, rightVals;
    readNode(leftChildId, leftVals);                                // Leh filhos.
    readNode(rightChildId, rightVals);

    int nLeft = node_get_n(leftVals);                               // n do filho esquerdo.
    int nRight = node_get_n(rightVals);                             // n do filho direito.

    // Caso 2A: Filho esquerdo tem pelo menos T chaves
    if (nLeft >= T) {
        int predecessor = getPredecessor(leftChildId);              // Encontra o predecessor no filho esquerdo.
        node_set_chave(nodeVals, idx, predecessor);                 // Substitui K[idx] pelo predecessor.
        writeNode(nodeId, nodeVals);                                // Grava o noh modificado.
        deleteFromNode(leftChildId, predecessor);                   // Deleta recursivamente o predecessor no noh esquerdo.
    } // Fim do IF

    // Caso 2B: Filho direito tem pelo menos T chaves
    else if (nRight >= T) {
        int successor = getSuccessor(rightChildId);                 // Encontra o sucessor no filho direito.
        node_set_chave(nodeVals, idx, successor);                   // Substitui K[idx] pelo sucessor.
        writeNode(nodeId, nodeVals);                                // Grava o noh modificado.
        deleteFromNode(rightChildId, successor);                    // Deleta recursivamente o sucessor no noh direito.
    } // Fim do ELSE IF

    // Caso 2C: Ambos os filhos tem T-1 chaves (minimo)
    else {

        // *** CORRECAO: Verificacao de merge (nLeft + nRight + 1 deve caber) ***
        if (nLeft + nRight + 1 <= M - 1) {                          // Verifica se o merge eh possivel.
            mergeNodes(nodeId, idx);                                // Realiza a fusao dos nos.

            // Apos merge, re-ler o noh (pode ter mudado drasticamente)
            if (readNode(nodeId, nodeVals)) {
                // Agora a chave foi movida para o filho esquerdo (que agora eh o noh fundido)
                int mergedChildId = node_get_filho(nodeVals, idx);
                if (mergedChildId != 0) {
                    deleteFromNode(mergedChildId, chaveParaDeletar); // Deleta a chave no noh fundido.
                } // Fim do IF
            } // Fim do IF
        } else {
            // *** CORRECAO ALTERNATIVA: Tentar redistribuicao (se o merge nao for possivel, o que nao deve ocorrer em um B-tree valido) ***
            if (nLeft > 0) {
                int predecessor = getPredecessor(leftChildId);
                node_set_chave(nodeVals, idx, predecessor);
                writeNode(nodeId, nodeVals);
                deleteFromNode(leftChildId, predecessor);
            } else if (nRight > 0) {
                int successor = getSuccessor(rightChildId);
                node_set_chave(nodeVals, idx, successor);
                writeNode(nodeId, nodeVals);
                deleteFromNode(rightChildId, successor);
            } // Fim do ELSE IF
        } // Fim do ELSE (Correcão alternativa)
    } // Fim do ELSE (Caso 2C)
}

void ArvoreMVias::fillChild(int parentId, int idx) {
    vector<int> parentVals;
    readNode(parentId, parentVals);                                 // Leh o noh pai.

    int childId = node_get_filho(parentVals, idx);                  // Obtem o ID do filho com underflow.
    if (childId == 0) return;                                       // Se o filho for nulo, sai.

    vector<int> childVals;
    readNode(childId, childVals);                                   // Leh o noh filho.

    // 1. Tenta emprestar do irmao esquerdo
    if (idx > 0) {                                                  // Se houver irmao esquerdo.
        int leftSiblingId = node_get_filho(parentVals, idx - 1);    // Obtem o ID.
        vector<int> leftVals;
        readNode(leftSiblingId, leftVals);                          // Leh o irmao.

        if (node_get_n(leftVals) >= T) {                            // Se o irmao esquerdo pode emprestar (n >= T).
            borrowFromLeft(parentId, idx);                          // Realiza o emprestimo.
            return;                                                 // Sai.
        } // Fim do IF
    } // Fim do IF

    // 2. Tenta emprestar do irmao direito
    if (idx < node_get_n(parentVals)) {                             // Se houver irmao direito.
        int rightSiblingId = node_get_filho(parentVals, idx + 1);   // Obtem o ID.
        vector<int> rightVals;
        readNode(rightSiblingId, rightVals);                        // Leh o irmao.

        if (node_get_n(rightVals) >= T) {                           // Se o irmao direito pode emprestar.
            borrowFromRight(parentId, idx);                         // Realiza o emprestimo.
            return;                                                 // Sai.
        } // Fim do IF
    } // Fim do IF

    // 3. Se nao pode emprestar, faz merge
    if (idx > 0) {
        mergeNodes(parentId, idx - 1);                              // Funde com o irmao esquerdo (posicao K[idx-1] do pai).
    } else if (idx < node_get_n(parentVals)) {
        mergeNodes(parentId, idx);                                  // Funde com o irmao direito (posicao K[idx] do pai).
    } // Fim do ELSE IF
}

int ArvoreMVias::getPredecessor(int nodeId) {
    vector<int> vals;
    readNode(nodeId, vals);                                         // Leh o noh inicial.

    // Vai para o filho mais a direita ate chegar em uma folha
    while (!node_get_folha(vals)) {                                 // Enquanto nao for folha.
        int lastChild = node_get_filho(vals, node_get_n(vals));     // Obtem o ID do filho mais a direita (ponteiro A[n]).
        readNode(lastChild, vals);                                  // Leh o noh filho.
    } // Fim do WHILE

    // Retorna a ultima chave da folha
    return node_get_chave(vals, node_get_n(vals) - 1);              // Retorna a chave K[n-1].
}

int ArvoreMVias::getSuccessor(int nodeId) {
    vector<int> vals;
    readNode(nodeId, vals);                                         // Leh o noh inicial.

    // Vai para o filho mais a esquerda ate chegar em uma folha
    while (!node_get_folha(vals)) {                                 // Enquanto nao for folha.
        int firstChild = node_get_filho(vals, 0);                   // Obtem o ID do primeiro filho (ponteiro A[0]).
        readNode(firstChild, vals);                                 // Leh o noh filho.
    } // Fim do WHILE

    // Retorna a primeira chave da folha
    return node_get_chave(vals, 0);                                 // Retorna a chave K[0].
}

// Metodo mergeNodes CORRIGIDO
void ArvoreMVias::mergeNodes(int parentId, int idx) {
    vector<int> parentVals;
    readNode(parentId, parentVals);                                 // Leh o noh pai.

    int leftId = node_get_filho(parentVals, idx);                   // ID do noh esquerdo (que recebera a fusao).
    int rightId = node_get_filho(parentVals, idx + 1);              // ID do noh direito (que sera deletado).

    vector<int> leftVals, rightVals;
    readNode(leftId, leftVals);                                     // Leh os nos a serem fundidos.
    readNode(rightId, rightVals);

    int nLeft = node_get_n(leftVals);                               // n do noh esquerdo.
    int nRight = node_get_n(rightVals);                             // n do noh direito.
    int parentKey = node_get_chave(parentVals, idx);                // Chave do pai que desce.

    // *** CORRECAO: Verificacao de capacidade CORRETA ***
    if (nLeft + nRight + 1 > M - 1) {                               // Se a soma das chaves + a chave do pai exceder M-1.
        return;                                                     // Nao faz o merge (erro ou caso nao esperado em B-Tree).
    } // Fim do IF

    // Move a chave do pai para o filho esquerdo
    node_set_chave(leftVals, nLeft, parentKey);                     // Move K[idx] do pai para a posicao nLeft do noh esquerdo.

    // Copia chaves do filho direito para o esquerdo
    for (int i = 0; i < nRight; i++) {                              // Copia todas as chaves de 'right' para 'left'.
        node_set_chave(leftVals, nLeft + 1 + i, node_get_chave(rightVals, i));
    } // Fim do FOR

    // Copia filhos (se nao forem folhas)
    if (!node_get_folha(leftVals)) {                                // Se for noh interno.
        for (int i = 0; i <= nRight; i++) {                          // Copia todos os nRight + 1 ponteiros de 'right'.
            node_set_filho(leftVals, nLeft + 1 + i, node_get_filho(rightVals, i));
        } // Fim do FOR
    } // Fim do IF

    // Atualiza contador
    node_set_n(leftVals, nLeft + nRight + 1);                       // O novo n eh a soma dos n's + 1 (a chave do pai).

    // *** CORRECAO: Remove chave e ponteiro do pai usando a funcao auxiliar ***
    removeKeyAndPointer(parentVals, idx);                           // Remove K[idx] e A[idx+1] do noh pai.

    // Atualiza nos
    writeNode(leftId, leftVals);                                    // Grava noh esquerdo fundido.
    writeNode(parentId, parentVals);                                // Grava noh pai modificado.

    // Remove noh direito
    deleteNode(rightId);                                            // Limpa o noh direito.

    // VERIFICACAO CRITICA: Se o pai eh a raiz e ficou vazio
    if (parentId == raiz && node_get_n(parentVals) == 0) {          // Se a raiz ficou vazia (0 chaves).
        raiz = leftId;                                              // Promove o noh fundido a nova raiz.
        writeHeader();                                              // Atualiza o cabecalho.
        deleteNode(parentId);                                       // Limpa a raiz antiga.
    } // Fim do IF

    // Se nao eh raiz e ficou com underflow
    else if (parentId != raiz && node_get_n(parentVals) < T - 1) {  // Se o pai nao eh raiz e tem menos que T-1 chaves.
        int grandParentId = findParent(raiz, parentId, 0);          // Busca o avo.
        if (grandParentId != 0) {
            int parentIndex = findChildIndex(grandParentId, parentId); // Encontra o indice do pai no avo.
            if (parentIndex != -1) {
                fillChild(grandParentId, parentIndex);              // Propaga a correcao de underflow para o avo.
            } // Fim do IF
        } // Fim do IF
    } // Fim do ELSE IF
}
// ====================================================================================================================

// ====================================================================================================================
// METODOS DE MANIPULACAO DE ARQUIVO DE DADOS:

// Requisito E: Novo metodo eficiente de delecao no arquivo de dados
void ArvoreMVias::removeDataFromFile(int chave) {
    markAsDeleted(chave);                                           // Simplesmente chama a funcao que faz a remocao logica.
}

void ArvoreMVias::markAsDeleted(int chave) {
    fstream file(arquivoDados, ios::binary | ios::in | ios::out);   // Abre o arquivo de dados para leitura/escrita.
    if (!file) {
        cerr << "Erro: Arquivo de dados binario nao existe ou nao pode ser aberto para marcacao de delecao.\n";
        return;
    } // Fim do IF

    Registro reg;
    bool marked = false;
    streampos currentPos = 0;
    size_t regSize = reg.getSize();

    // Itera sobre os registros
    while (file.read(reinterpret_cast<char*>(&reg), regSize)) {     // Leh um registro.
        if (reg.chave == chave && !reg.deletado) {                  // Se a chave coincide e o registro estah ativo.
            // Encontrado! Marca como deletado
            reg.deletado = true;

            // Retorna ao inicio do registro no arquivo (currentPos)
            file.seekp(currentPos);

            // Grava APENAS o registro modificado de volta (1 acesso a disco)
            file.write(reinterpret_cast<const char*>(&reg), regSize);

            marked = true;                                          // Marca como deletado.
            break;                                                  // Sai do loop, pois a chave eh unica.
        } // Fim do IF

        currentPos = file.tellg();                                  // Guarda a posicao de onde comecara a proxima leitura.
    } // Fim do WHILE

    file.close();                                                   // Fecha o arquivo.
    if (marked) {
        cout << "Livro com ID " << chave << " marcado como deletado no arquivo principal (acesso direto a disco).\n";
    } else {
        cerr << "Aviso: Livro ATIVO para o ID " << chave << " nao foi encontrado no arquivo principal.\n";
    } // Fim do ELSE
}

// Requisito A: Imprime o indice (e contadores)
void ArvoreMVias::imprimirIndice() {
    print(); // Imprime o indice
}

// Requisito B: Imprimir todo o arquivo principal
void ArvoreMVias::imprimirArquivoPrincipal() {
    ifstream fin(arquivoDados, ios::binary);                         // Abre o arquivo de dados.
    if (!fin) {
        cout << "Arquivo de dados vazio ou nao existe (" << arquivoDados << ").\n";
        return;
    } // Fim do IF

    Registro livro;
    cout << "\n--- BIBLIOTECA - Todos os Livros (" << arquivoDados << ") ---\n";
    cout << "Chave | Titulo                    | Autor           | Ano  | Genero\n";
    cout << "------------------------------------------------------------------\n";

    bool encontrouAtivo = false;

    while (fin.read(reinterpret_cast<char*>(&livro), livro.getSize())) { // Leh cada registro.
        if (!livro.deletado) {                                        // Se o registro NAO foi deletado.
            cout << setw(5) << livro.chave << " | "
                 << setw(25) << left << livro.titulo << " | "
                 << setw(15) << left << livro.autor << " | "
                 << setw(4) << livro.ano << " | "
                 << livro.genero << endl;                            // Imprime os dados formatados.
            encontrouAtivo = true;
        } // Fim do IF
    } // Fim do WHILE

    if (!encontrouAtivo) {
        cout << "Nenhum livro ativo encontrado na biblioteca.\n";      // Aviso se nao encontrou ativos.
    } // Fim do IF

    fin.close();                                                      // Fecha o arquivo.
}

// Requisito C: Imprimir livro especifico
void ArvoreMVias::imprimirArquivoPrincipal(int chave) {
    ifstream fin(arquivoDados, ios::binary);                         // Abre o arquivo de dados.
    if (!fin) {
        cerr << "Erro: Arquivo de dados nao existe ou nao pode ser aberto.\n";
        return;
    } // Fim do IF

    Registro livro;
    bool found = false;

    cout << "Chave | Titulo                    | Autor           | Ano  | Genero\n";
    cout << "------------------------------------------------------------------\n";

    while (fin.read(reinterpret_cast<char*>(&livro), livro.getSize())) { // Leh cada registro.
        if (livro.chave == chave && !livro.deletado) {                  // Se a chave coincide e estah ativo.
            cout << setw(5) << livro.chave << " | "
                 << setw(25) << left << livro.titulo << " | "
                 << setw(15) << left << livro.autor << " | "
                 << setw(4) << livro.ano << " | "
                 << livro.genero << endl;                            // Imprime os dados.
            found = true;
            break;                                                  // Sai do loop.
        } // Fim do IF
    } // Fim do WHILE

    if (!found) {
        cout << "Nenhum livro ATIVO com a chave " << chave << " encontrado.\n"; // Aviso se nao encontrou.
    } // Fim do IF

    fin.close();                                                      // Fecha o arquivo.
}
// ====================================================================================================================

// ====================================================================================================================
// METODOS DE EMPRESTIMO/FUSAO (HELPERS):

// Implementacao do helper para remover a chave K_idx e o ponteiro A_{idx+1}
void ArvoreMVias::removeKeyAndPointer(vector<int>& vals, int idx) {
    int n = node_get_n(vals);                                       // Obtem n.

    // Desloca chaves a esquerda
    for (int i = idx; i < n - 1; ++i) {                             // Move K[i+1] para K[i].
        node_set_chave(vals, i, node_get_chave(vals, i + 1));
    } // Fim do FOR
    node_set_chave(vals, n - 1, 0);                                 // Zera a ultima chave.

    // Desloca filhos a esquerda (n+1 filhos)
    for (int i = idx + 1; i <= n; ++i) {                           // Move A[i+1] para A[i].
        node_set_filho(vals, i, node_get_filho(vals, i + 1));
    } // Fim do FOR
    node_set_filho(vals, n, 0);                                     // Zera o ultimo filho.

    node_set_n(vals, n - 1);                                        // Decrementa n.
}

// Limpa um noh no arquivo binario, marcando-o como vazio
void ArvoreMVias::deleteNode(int nodeId) {
    vector<int> empty(nodeInts(), 0);                               // Cria um vetor com o tamanho do noh, preenchido com zeros.
    if (empty.size() >= 2) empty[1] = 1;                           // Mantem a flag de folha (posicao 1) como 1 (para consistencia/debug).

    writeNode(nodeId, empty);                                       // Grava o noh vazio no disco.
}

// Emprestimo do irmao esquerdo
void ArvoreMVias::borrowFromLeft(int parentId, int childIndex) {
    vector<int> parentVals, leftVals, childVals;                    // Vetores para pai, irmao esquerdo e filho.
    readNode(parentId, parentVals);                                 // Leh o pai.

    int childId = node_get_filho(parentVals, childIndex);           // ID do filho.
    int leftId = node_get_filho(parentVals, childIndex - 1);        // ID do irmao esquerdo.

    readNode(childId, childVals);                                   // Leh filho e irmao.
    readNode(leftId, leftVals);

    int nChild = node_get_n(childVals);                             // n do filho.
    int nLeft = node_get_n(leftVals);                               // n do irmao.

    // Shift chaves e filhos do child para a direita
    for (int i = nChild; i > 0; --i) {                              // Move chaves para a direita (abre K[0]).
        node_set_chave(childVals, i, node_get_chave(childVals, i - 1));
    } // Fim do FOR
    if (!node_get_folha(childVals)) {
        for (int i = nChild + 1; i > 0; --i) {                      // Move ponteiros para a direita (abre A[0]).
            node_set_filho(childVals, i, node_get_filho(childVals, i - 1));
        } // Fim do FOR
    } // Fim do IF

    // Move chave do pai para child[0]
    node_set_chave(childVals, 0, node_get_chave(parentVals, childIndex - 1)); // K[idx-1] (pai) desce para K[0] (filho).

    // Move ultimo filho da esquerda para child[0] se nao for folha
    if (!node_get_folha(childVals))
        node_set_filho(childVals, 0, node_get_filho(leftVals, nLeft)); // A[nLeft] (irmao) desce para A[0] (filho).

    // Move ultima chave de left para o pai
    node_set_chave(parentVals, childIndex - 1, node_get_chave(leftVals, nLeft - 1)); // K[nLeft-1] (irmao) sobe para K[idx-1] (pai).

    // Atualiza contadores
    node_set_n(childVals, nChild + 1);                              // Filho ganha 1 chave.
    node_set_n(leftVals, nLeft - 1);                                // Irmao perde 1 chave.

    writeNode(parentId, parentVals);                                // Grava os 3 nos.
    writeNode(childId, childVals);
    writeNode(leftId, leftVals);
}

void ArvoreMVias::borrowFromRight(int parentId, int childIndex) {
    vector<int> parentVals, rightVals, childVals;                    // Vetores para pai, irmao direito e filho.
    readNode(parentId, parentVals);                                 // Leh o pai.

    int childId = node_get_filho(parentVals, childIndex);           // ID do filho.
    int rightId = node_get_filho(parentVals, childIndex + 1);       // ID do irmao direito.

    readNode(childId, childVals);                                   // Leh filho e irmao.
    readNode(rightId, rightVals);

    int nChild = node_get_n(childVals);                             // n do filho.
    int nRight = node_get_n(rightVals);                             // n do irmao.

    // Move chave do pai para child
    node_set_chave(childVals, nChild, node_get_chave(parentVals, childIndex)); // K[idx] (pai) desce para K[nChild] (filho).

    // Move filho inicial de right para child se nao for folha
    if (!node_get_folha(childVals))
        node_set_filho(childVals, nChild + 1, node_get_filho(rightVals, 0)); // A[0] (irmao) desce para A[nChild + 1] (filho).

    // Move primeira chave de right para o pai
    node_set_chave(parentVals, childIndex, node_get_chave(rightVals, 0)); // K[0] (irmao) sobe para K[idx] (pai).

    // Shift chaves e filhos de right para esquerda
    for (int i = 0; i < nRight - 1; ++i) {                          // Move chaves para a esquerda (remove K[0]).
        node_set_chave(rightVals, i, node_get_chave(rightVals, i + 1));
    } // Fim do FOR
    for (int i = 0; i < nRight; ++i) {                              // Move ponteiros para a esquerda (remove A[0]).
        node_set_filho(rightVals, i, node_get_filho(rightVals, i + 1));
    } // Fim do FOR

    node_set_chave(rightVals, nRight - 1, 0);                       // Limpa a ultima chave.
    node_set_filho(rightVals, nRight, 0);                           // Limpa o ultimo ponteiro.

    // Atualiza contadores
    node_set_n(childVals, nChild + 1);                              // Filho ganha 1 chave.
    node_set_n(rightVals, nRight - 1);                              // Irmao perde 1 chave.

    writeNode(parentId, parentVals);                                // Grava os 3 nos.
    writeNode(childId, childVals);
    writeNode(rightId, rightVals);
}

int ArvoreMVias::getPredecessor(int nodeId) {
    vector<int> vals;
    readNode(nodeId, vals);                                         // Leh o noh inicial.

    // Vai para o filho mais a direita ate chegar em uma folha
    while (!node_get_folha(vals)) {
        int lastChild = node_get_filho(vals, node_get_n(vals));     // Obtem o ID do filho mais a direita (A[n]).
        readNode(lastChild, vals);                                  // Leh o noh filho.
    } // Fim do WHILE

    // Retorna a ultima chave da folha
    return node_get_chave(vals, node_get_n(vals) - 1);              // Retorna K[n-1].
}

int ArvoreMVias::getSuccessor(int nodeId) {
    vector<int> vals;
    readNode(nodeId, vals);                                         // Leh o noh inicial.

    // Vai para o filho mais a esquerda ate chegar em uma folha
    while (!node_get_folha(vals)) {
        int firstChild = node_get_filho(vals, 0);                   // Obtem o ID do primeiro filho (A[0]).
        readNode(firstChild, vals);                                 // Leh o noh filho.
    } // Fim do WHILE

    // Retorna a primeira chave da folha
    return node_get_chave(vals, 0);                                 // Retorna K[0].
}
// ====================================================================================================================