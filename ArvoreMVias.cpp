// ====================================================================================================================

// INTEGRANTES DO GRUPO:
// Caio Monteiro Sartori            N° 15444598
// Mateus Henrique J. S. Carriel    N° 15698362
// Murilo Augusto Jorge             N° 15552251

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
ArvoreMVias::ArvoreMVias(const string& txt, const string& bin, const string& dados, int ordem) {
    arquivoTxt = txt;       // Inicializa o nome do Arquivo de Texto.
    arquivoBin = bin;       // Inicializa o nome do Arquivo de Indice Binario (onde a Arvore eh Salva).
    arquivoDados = dados;   // Inicializa o nome do Arquivo de Dados Principal.
    M = ordem;              // Define a Ordem da Arvore (M).
    T = (M + 1) / 2;        // Define o Grau Minimo (T) necessario.
    raiz = 1;               // Inicializa o ID do Noh Raiz.
    nextNodeId = 1;         // Inicializa o ID do proximo Noh a ser Criado.
    // leituraDisco = 0;    // Leitura do Disco Nao Implementado.
    // escritaDisco = 0;    // Escrita no Disco Nao Implementado.

} // Fim do CONSTRUTOR

// ====================================================================================================================

// ====================================================================================================================

// METODOS DE SEQUENCIAMENTO E I/O DE DISCO:
// Calcula o Numero de Inteiros que compoem um Noh no Disco.
// FORMATO: [n (1 int), folha (1 int), chaves (M-1 ints), filhos (M ints)]}
int ArvoreMVias::nodeInts() const {
    return 2 + (M - 1) + M;
} // Fim do nodeInts

// --------------------------------------------------------------------------------------------------------------------

// Escreve o Cabecalho da Arvore no Disco (M, raiz, nextNodeId).
void ArvoreMVias::writeHeader() {

    fstream fout(arquivoBin, ios::binary | ios::in | ios::out);     // Abre o Arquivo Binario para Leitura e Escrita.
    if (!fout) {                                                    // Verifica se a Abertura falhou.
        ofstream cr(arquivoBin, ios::binary);                       // Tenta criar o Arquivo antes, caso falhe.
        cr.close();                                                 // Fecha a Criacao.
        fout.open(arquivoBin, ios::binary | ios::in | ios::out);    // Tenta reabrir para Escrita/Leitura.
    } // Fim do IF

    fout.seekp(0);                                  // Posiciona o Ponteiro de Escrita no Inicio do Arquivo (Posicao 0)
    int aM = M, aRaiz = raiz, aNext = nextNodeId;   // Cria Copias Temporarias das Variaveis para Escrita.
    fout.write((char*)&aM, sizeof(int));            // Escreve a Ordem M.
    fout.write((char*)&aRaiz, sizeof(int));         // Escreve o ID da Raiz.
    fout.write((char*)&aNext, sizeof(int));         // Escreve o Proximo ID Disponivel.
    fout.close();                                   // Fecha o Arquivo.

} // Fim do writeHeader

// --------------------------------------------------------------------------------------------------------------------

// Leh o Cabecalho da Arvore do Disco.
bool ArvoreMVias::readHeader() {

    ifstream fin(arquivoBin, ios::binary);      // Abre o Arquivo Binario para Leitura.
    if (!fin) return false;                     // Retorna Falso se o Arquivo nao puder ser Aberto.
    fin.seekg(0);                               // Declara Variaveis de Leitura no Inicio
    int aM, aRaiz, aNext;                       // Declara Variaveis para Armazenar o Header Lido.

    // Tentativas de Leitura. No caso de Falhas, Fecham o Arquivo e Retornam Falso:
    if (!fin.read((char*)&aM, sizeof(int))) { fin.close(); return false; }      // Tenta ler M.
    if (!fin.read((char*)&aRaiz, sizeof(int))) { fin.close(); return false; }   // Tenta ler a Raiz.
    if (!fin.read((char*)&aNext, sizeof(int))) { fin.close(); return false; }   // Tenta ler o Proximo ID.

    // Alerta/Reseta caso a Ordem no Arquivo (aM) for DIFERENTE da Ordem Atual (M).
    if (aM != M) {

        // Imprime um Aviso de Incompatibilidade:
        cerr << "Aviso: Ordem (M=" << aM << ") do indice e diferente da ordem atual (M=" << M << ")."
             << "Reiniciando indice E arquivo de dados.\n";

        fin.close();        // Fecha o Arquivo de Indice.

        // Define as Variaveis de Estado para a Inicializacao da Nova Arvore:
        raiz = 1;
        nextNodeId = 1;
        T = (M + 1) / 2;

        ofstream foutDados(arquivoDados, ios::trunc);   // Tenta Limpar (Truncar) o Arquivo de Dados.
        if (!foutDados) {                               // Se Falhar ao Abrir/Truncar o Arquivo de Dados, imprime o erro:
            cerr << "Erro fatal: Nao foi possivel reiniciar o arquivo de dados.\n";
            return false;   // Retorna Falso.
        } // Fim do IF

        foutDados.close();  // Fecha o Arquivo de Dados.

        return false;       // Retorna Falso.

    } // Fim do IF

    raiz = aRaiz;           // Atualiza o ID da Raiz com o Valor Lido.
    nextNodeId = aNext;     // Atualiza o Proximo ID de Noh com o Valor Lido.
    fin.close();            // Fecha o Arquivo de Indice.
    return true;            // Retorna Verdadeiro, indicando Sucesso na Leitura do Header.

} // Fim do readHeader

// --------------------------------------------------------------------------------------------------------------------

// Escreve um Noh (Vetor de Ints) no Disco.
void ArvoreMVias::writeNode(int id, const vector<int>& vals) {

    fstream fout(arquivoBin, ios::in | ios::out | ios::binary);         // Abre o Arquivo para Leitura e Escrita.
    if (!fout) { cerr << "Erro de arquivo (writeNode).\n"; return; }    // Verifica se houve Erro na Abertura.

    int headerBytes = 3 * sizeof(int);      // Calcula o Tamanho do Cabecalho em Bytes.
    int ints = nodeInts();                  // Obtem o Numero Total de Inteiros por Noh.
    int nodeBytes = ints * sizeof(int);     // Calcula o Tamanho do Noh em Butes

    // Calcula a Posicao do Noh no Arquivo: Header + (ID - 1) * Tamanho do Noh:
    streampos pos = static_cast<std::streamoff>(headerBytes) + static_cast<std::streamoff>((id - 1) * nodeBytes);
    fout.seekp(pos);        // Posiciona o Ponteiro de Escrita.

    for (int i = 0; i < ints; ++i) {        // Itera sobre Todos os Inteiros do Noh.
        int v = vals[i];                    // Obtem o Valor.
        fout.write((char*)&v, sizeof(int)); // Escreve o Inteiro no Arquivo.
    } // Fim do FOR

    fout.close();   // Fecha o Arquivo.

} // Fim do writeNode

// --------------------------------------------------------------------------------------------------------------------

// Leh um Noh (Vetor de Ints) do Disco.
bool ArvoreMVias::readNode(int id, vector<int>& vals) {

    ifstream fin(arquivoBin, ios::binary);  // Abre o Arquivo para Leitura.
    if (!fin) return false;                 // Retorna Falso se nao Abrir.
    int headerBytes = 3 * sizeof(int);      // Calcula o Tamanho do Cabecalho.
    int ints = nodeInts();                  // Obtem o Numero de Inteiros por Noh.
    int nodeBytes = ints * sizeof(int);     // Calcula o Tamanho do Noh em Bytes.

    // Calcula a Posicao (Offset) do Noh no Arquivo:
    streampos pos = static_cast<std::streamoff>(headerBytes) + static_cast<std::streamoff>((id - 1) * nodeBytes);

    // Verifica o Tamanho do Arquivo para Evitar Leitura Fora do Limite (Corrupcao).
    fin.seekg(0, ios::end);             // Posiciona no Final do Arquivo.
    streampos fileSize = fin.tellg();   // Obtem o Tamanho Total do Arquivo.

    // Se o Fim do Noh Calculado Ultrapassa o Fim do Arquivo:
    if (pos + static_cast<std::streamoff>(nodeBytes) > fileSize) {
        fin.close();    // Fecha o Arquivo.
        return false;   // Retorna Falso.
    } // Fim do IF

    fin.seekg(pos);         // Posiciona o Ponteiro de Leitura no Inicio do Noh.
    vals.assign(ints, 0);   // Redimensiona o Vetor para o Numero Correto de Inteiros (Inicializa com 0).

    // Itera sobre os Inteiros do Noh:
    for (int i = 0; i < ints; ++i) {
        int v = 0;                          // Armazena o Valor Lido.
        fin.read((char*)&v, sizeof(int));   // Leh o Inteiro.
        vals[i] = v;                        // Armazena no Vetor.
    } // Fim do FOR

    fin.close();    // Fecha o Arquivo.
    // leituraDisco++; // Incrementa o contador de acesso a disco (leitura)

    return true;    // Retorna Verdadeiro

} // Fim do readNode

// ====================================================================================================================

// METODOS DE ACESSO/MANIPULACAO A CAMPOS DO NOH EM MEMORIA:
// Getters e Setters - para N:
int ArvoreMVias::node_get_n(const vector<int>& vals) const { return vals[0]; }
void ArvoreMVias::node_set_n(vector<int>& vals, int n) { vals[0] = n; }
// --------------------------------------------------------------------------------------------------------------------
// Getters e Setters - para Folha:
bool ArvoreMVias::node_get_folha(const vector<int>& vals) const { return vals[1] != 0; }
void ArvoreMVias::node_set_folha(vector<int>& vals, bool folha) { vals[1] = folha ? 1 : 0; }
// --------------------------------------------------------------------------------------------------------------------
// Getters e Setters - para Chave:
int ArvoreMVias::node_get_chave(const vector<int>& vals, int idx) const { return vals[2 + idx]; }
void ArvoreMVias::node_set_chave(vector<int>& vals, int idx, int chave) { vals[2 + idx] = chave; }
// --------------------------------------------------------------------------------------------------------------------
// Getters e Setters - para Filho:
int ArvoreMVias::node_get_filho(const vector<int>& vals, int idx) const { return vals[2 + (M - 1) + idx]; }
void ArvoreMVias::node_set_filho(vector<int>& vals, int idx, int filho) { vals[2 + (M - 1) + idx] = filho; }

// ====================================================================================================================

// METODOS DE INSERCAO (INSERTB):
// Cria e inicializa um novo Noh no Disco.
int ArvoreMVias::createNode(bool folha) {

    int id = nextNodeId++;          // Atribui o Proximo ID e Incrementa o Contador.
    int ints = nodeInts();          // Obtem o Numero de Inteiros.
    vector<int> vals(ints, 0);      // Inicializa o Vetor do Noh com Zeros.
    node_set_n(vals, 0);            // Define o Noh com 0 Chaves.
    node_set_folha(vals, folha);    // Define se o Noh eh Folha.

    // Inicializa filhos e chaves com 0 (Feito pela Inicializacao do Vetor):
    for (int i = 0; i < M; ++i) node_set_filho(vals, i, 0);
    for (int i = 0; i < M - 1; ++i) node_set_chave(vals, i, 0);

    writeNode(id, vals);    // Escreve o Noh Inicializado no Disco.
    writeHeader();          // Atualiza nextNodeId no Header do Arquivo.

    return id;      // Retorna ID do Noh Recem-Criado.

} // Fim do createNode

// --------------------------------------------------------------------------------------------------------------------

// Insercao Principal.
void ArvoreMVias::insertNonFull(int nodeId, int chave) {

    vector<int> nodeVals;                                       // Declara vetor para armazenar os valores do noh.
    readNode(nodeId, nodeVals);                                 // Leh o noh atual do disco, preenchendo 'nodeVals'.

    // Imprime o status de debug (ID do noh, se eh folha, n, e chave a inserir):
    cout << "DEBUG insertNonFull: noh " << nodeId << ", folha=" << node_get_folha(nodeVals)
         << ", n=" << node_get_n(nodeVals) << ", chave=" << chave << endl;

    // CASO 1: O noh eh folha (Insercao Direta)
    if (node_get_folha(nodeVals)) {
        int n = node_get_n(nodeVals);                           // Obtem o numero atual de chaves (n).
        int pos = 0;                                            // Inicializa a posicao para insercao.

        while (pos < n && node_get_chave(nodeVals, pos) < chave) {  // Encontra a posicao correta (indice) para a nova chave.
            pos++;                                                  // Avanca para o proximo indice.
        } // Fim do WHILE

        cout << "Inserindo na folha " << nodeId << " na posicao " << pos << endl; // Imprime onde a chave serah inserida.

        for (int i = n; i > pos; i--) {                                     // Move as chaves maiores que a nova (shift a direita).
            node_set_chave(nodeVals, i, node_get_chave(nodeVals, i - 1));   // Copia a chave 'i-1' para a posicao 'i'.
        } // Fim do FOR

        node_set_chave(nodeVals, pos, chave);                   // Insere a nova chave na posicao encontrada.
        node_set_n(nodeVals, n + 1);                            // Incrementa o contador de chaves (n).
        writeNode(nodeId, nodeVals);                            // Grava o noh atualizado de volta no disco.

        // Imprime o novo estado do noh (quantas chaves):
        cout << "Folha " << nodeId << " agora tem " << node_get_n(nodeVals) << " chaves: ";

        for (int i = 0; i < node_get_n(nodeVals); i++) {
            cout << node_get_chave(nodeVals, i) << " ";         // Imprime as chaves contidas no noh.
        } // Fim do FOR

        cout << endl;

    } // Fim do IF (CASO 1)

    // CASO 2: O noh eh interno (Descida ou Split)
    else {
        int n = node_get_n(nodeVals);                           // Obtem o **numero** atual de chaves (n).
        int childIndex = 0;                                     // Inicializa o indice do ponteiro (filho).

        while (childIndex < n && chave > node_get_chave(nodeVals, childIndex)) {    // Encontra o indice do filho para descer (A[childIndex]).
            childIndex++;                                                           // Avanca para o proximo indice de ponteiro.
        } // Fim do WHILE

        int childId = node_get_filho(nodeVals, childIndex);     // Obtem o ID do noh filho para onde a descida ocorrerah.
        cout << "Descendo para filho " << childId << " (indice " << childIndex << ")" << endl; // Imprime a descida.

        vector<int> childVals;                                  // Declara vetor para o noh filho.
        readNode(childId, childVals);                           // Leh o noh filho do disco.

        // Se o filho estiver CHEIO (M-1 chaves), realiza o SPLIT ESPECIAL.
        if (node_get_n(childVals) == M - 1) {
            // Imprime que o split eh necessario:
            cout << "Filho " << childId << " estah cheio. Fazendo split especial..." << endl;

            vector<int> tempChildVals = childVals;              // Cria uma copia temporaria do filho (simulando M chaves).
            int temp_n = node_get_n(tempChildVals);             // Obtem n temporario (M-1).

            int pos = 0;                                        // Inicializa a posicao para insercao.
            while (pos < temp_n && chave > node_get_chave(tempChildVals, pos)) { // Encontra a posicao da nova chave na copia temporaria.
                pos++;
            } // Fim do WHILE

            for (int i = temp_n; i > pos; i--) {                // Shift para a direita na copia temporaria para abrir espaco.
                node_set_chave(tempChildVals, i, node_get_chave(tempChildVals, i - 1));
            } // Fim do FOR

            node_set_chave(tempChildVals, pos, chave);          // Insere a chave temporariamente (o noh tem M chaves agora).
            node_set_n(tempChildVals, temp_n + 1);              // Incrementa n para M.

            cout << "DEBUG: Noh temporario apos insercao de " << chave << ": "; // Imprime o noh temporario apos a insercao simulada.
            for (int i = 0; i < node_get_n(tempChildVals); i++) {
                cout << node_get_chave(tempChildVals, i) << " ";
            } // Fim do FOR
            cout << endl;

            int t = T;                                              // T eh o numero minimo de chaves (M/2, arredondado para cima).
            int medianaIndex = t - 1;                               // A chave mediana a ser promovida estah em T-1.
            int med = node_get_chave(tempChildVals, medianaIndex);  // Obtem o valor da mediana.

            cout << "Mediana index: " << medianaIndex << ", valor: " << med << endl; // Imprime o indice e valor da mediana.

            int zId = createNode(node_get_folha(tempChildVals));    // Cria o novo noh direito (zId), mantendo o status de folha/interno.
            vector<int> zVals;                                      // Declara vetor para o novo noh.
            readNode(zId, zVals);                                   // Leh o novo noh (vazio) do disco.

            node_set_n(childVals, medianaIndex);                    // O noh original (childVals) fica com as primeiras T-1 chaves.
            for (int i = 0; i < medianaIndex; ++i) {                // Copia as primeiras T-1 chaves para o noh esquerdo.
                node_set_chave(childVals, i, node_get_chave(tempChildVals, i));
            } // Fim do FOR

            for (int i = medianaIndex; i < M - 1; ++i) {            // Zera chaves restantes no noh esquerdo para limpeza.
                node_set_chave(childVals, i, 0);
            } // Fim do FOR

            int chavesRestantes = (temp_n + 1) - medianaIndex - 1;  // Calcula o **numero** de chaves a serem movidas para o novo noh.
            node_set_n(zVals, chavesRestantes);                     // Atualiza n do novo noh (direito).
            for (int j = 0; j < chavesRestantes; ++j) {             // Copia as chaves APOS a mediana para o novo noh direito.
                node_set_chave(zVals, j, node_get_chave(tempChildVals, j + medianaIndex + 1));
            } // Fim do FOR

            if (!node_get_folha(tempChildVals)) {               // Se o noh original NAO for folha, move os ponteiros de filhos.
                for (int j = 0; j <= chavesRestantes; ++j) {    // Move os ponteiros APOS o ponteiro correspondente a mediana.
                    node_set_filho(zVals, j, node_get_filho(tempChildVals, j + medianaIndex + 1));
                } // Fim do FOR
            } // Fim do IF

            // DEBUG: Mostrar estado antes de inserir no parent
            // Imprime informacoes de debug antes da insercao no pai:
            cout << "DEBUG ANTES DE INSERIR NO PARENT:" << endl;
            cout << "Parent (noh " << nodeId << ") n atual: " << node_get_n(nodeVals) << endl;
            cout << "Child (noh " << childId << "): ";

            for (int i = 0; i < node_get_n(childVals); i++) {
                cout << node_get_chave(childVals, i) << " ";
            } // Fim do FOR
            cout << endl;
            cout << "Novo noh (noh " << zId << "): ";
            for (int i = 0; i < node_get_n(zVals); i++) {
                cout << node_get_chave(zVals, i) << " ";
            } // Fim do FOR

            cout << endl;
            cout << "Mediana a ser promovida: " << med << endl;

            int parent_n = node_get_n(nodeVals);                // Obtem o numero de chaves do noh pai.

            // Move as chaves do pai que sao maiores que a mediana (shift a direita).
            for (int j = parent_n; j > childIndex; --j) {
                node_set_chave(nodeVals, j, node_get_chave(nodeVals, j - 1));
            } // Fim do FOR

            // Move os ponteiros de filhos (shift a direita) para abrir espaco.
            for (int j = parent_n + 1; j > childIndex + 1; --j) {
                node_set_filho(nodeVals, j, node_get_filho(nodeVals, j - 1));
            } // Fim do FOR

            node_set_chave(nodeVals, childIndex, med);          // Insere a chave promovida (mediana) na posicao correta.
            node_set_filho(nodeVals, childIndex + 1, zId);      // Insere o ponteiro para o novo noh direito (zId).
            node_set_n(nodeVals, parent_n + 1);                 // Incrementa o **numero** de chaves do pai.

            writeNode(childId, childVals);                      // Grava o noh filho esquerdo atualizado.
            writeNode(zId, zVals);                              // Grava o novo noh direito criado.
            writeNode(nodeId, nodeVals);                        // Grava o noh pai atualizado.

            // Imprime a conclusao do split.
            cout << "Split especial concluido. Mediana " << med << " promovida." << endl;
            cout << "Noh " << childId << " agora tem: ";
            for (int i = 0; i < node_get_n(childVals); i++) {
                cout << node_get_chave(childVals, i) << " ";
            } // Fim do FOR
            cout << endl;
            cout << "Noh " << zId << " agora tem: ";
            for (int i = 0; i < node_get_n(zVals); i++) {
                cout << node_get_chave(zVals, i) << " ";
            } // Fim do FOR
            cout << endl;

        } // Fim do IF (Split)

        else {
            insertNonFull(childId, chave);      // Filho nao estah cheio, desce recursivamente para o filho.
        } // Fim do ELSE

    } // Fim do ELSE (CASO 2)

} // Fim do insertNonFull

// --------------------------------------------------------------------------------------------------------------------

// Insere Chave e Dados.
void ArvoreMVias::insertB(int chave, const string& dadosElemento) {

    // 1. Verifica se a chave jah existe no indice.
    Resultado r = mSearch(chave);                               // Busca a chave no indice B.
    if (r.encontrou) {                                          // Se a busca encontrar a chave (encontrou == true).
        // Imprime o aviso e cancela:
        cout << "Aviso: Chave " << chave << " jah existe no indice. Insercao cancelada.\n";
        return;                                                 // Sai da funcao.
    } // Fim do IF

    // 2. Escreve no arquivo principal (cria o registro de dados).
    ofstream foutDados(arquivoDados, ios::binary | ios::app);   // Abre o arquivo de dados em modo binario e append.
    if (!foutDados) {                                           // Verifica se a abertura falhou.
        // Imprime o erro na saida de erro.
        cerr << "Erro ao abrir " << arquivoDados << " para escrita.\n";
        return;                                                 // Sai da funcao.
    } // Fim do IF

    Registro novoReg;                                           // Cria um novo objeto Registro.
    novoReg.chave = chave;                                      // Preenche a chave no novo registro.
    size_t len = min(dadosElemento.length(), (size_t)MAX_DATA_SIZE - 1); // Calcula o tamanho dos dados a copiar (limitado por MAX_DATA_SIZE).
    dadosElemento.copy(novoReg.dados, len);                     // Copia a string de dados para o campo 'dados' do registro.
    novoReg.dados[len] = '\0';                                  // Adiciona o terminador nulo ao final da string.
    novoReg.deletado = false;                                   // Marca o registro como ativo.

    foutDados.write(reinterpret_cast<const char*>(&novoReg), Registro::getSize());  // Grava o registro completo no final do arquivo de dados.
    foutDados.close();                                                              // Fecha o arquivo de dados.

    // 3. Insere a chave no indice (Arvore B).
    if (!readHeader()) {                                        // Leh o cabecalho da arvore (raiz, M, nextNodeId).
        cerr << "Erro ao ler header antes de inserir.\n";       // Imprime o erro se a leitura falhar.
        return;                                                 // Sai da funcao.
    } // Fim do IF

    vector<int> rootVals;                                       // Declara vetor para o noh raiz.
    readNode(raiz, rootVals);                                   // Leh o noh raiz do disco.

    cout << "=== INICIANDO INSERCAO DA CHAVE " << chave << " ===" << endl;      // Imprime o inicio do processo de insercao.
    cout << "Raiz atual: " << raiz << ", n=" << node_get_n(rootVals) << endl;   // Imprime informacoes sobre o noh raiz.

    if (node_get_n(rootVals) == M - 1) {                        // Verifica se a raiz estah cheia (M-1 chaves).
        cout << "Raiz cheia. Criando nova raiz..." << endl;     // Imprime a necessidade de split da raiz.

        int newRootId = createNode(false);                      // Cria uma nova raiz (ID) que **sera** um noh interno.
        vector<int> newRootVals;                                // Declara vetor para a nova raiz.
        readNode(newRootId, newRootVals);                       // Leh o novo noh (vazio).

        node_set_filho(newRootVals, 0, raiz);                   // Faz a raiz antiga ser o primeiro filho da nova raiz.
        writeNode(newRootId, newRootVals);                      // Grava a nova raiz no disco.

        raiz = newRootId;                                       // Atualiza a variavel membro 'raiz' para o ID do novo noh.
        writeHeader();                                          // Escreve o cabecalho atualizado no disco.

        cout << "Nova raiz criada: " << raiz << endl;           // Imprime o ID da nova raiz.

        insertNonFull(raiz, chave);                             // Chama a insercao a partir da nova raiz.
    } // Fim do IF
    else {
        insertNonFull(raiz, chave);                             // Se a raiz nao estah cheia, chama a insercao diretamente.
    } // Fim do ELSE

    // Imprime a mensagem de sucesso.
    cout << "Insercao da chave " << chave << " realizada com sucesso no indice e no arquivo principal.\n";

    cout << "=== ESTRUTURA FINAL APOS INSERCAO ===" << endl;    // Imprime um cabecalho para a estrutura final.
    print();                                                    // Chama a funcao para imprimir a estrutura final do indice.

} // Fim de insertB

// ====================================================================================================================

// METODOS DE REMOCAO NO ARQUIVO DE DADOS:
// Encontra e marca o Registro como Deletado (Delecao Logica).
void ArvoreMVias::removeDataFromFile(int chave) {
    markAsDeleted(chave);                                       // Chama a implementacao de delecao logica.
} // Fim de removeDataFromFile

// --------------------------------------------------------------------------------------------------------------------

// Implementacao da Remocao Logica no arquivo principal.
void ArvoreMVias::markAsDeleted(int chave) {
    fstream file(arquivoDados, ios::binary | ios::in | ios::out);   // Abre o arquivo para leitura, escrita e modo binario.

    if (!file) {        // Verifica se houve erro na abertura do arquivo.
        cerr << "Erro: Arquivo de dados binario nao existe ou nao pode ser aberto para marcacao de delecao.\n";
        return;         // Aborta a operacao.
    } // Fim do IF

    Registro reg;                                               // Declara a estrutura do Registro.
    bool marked = false;                                        // Flag para indicar se a delecao foi realizada.
    streampos currentPos = 0;                                   // Variavel para guardar a posicao de leitura (ponteiro de arquivo).
    size_t regSize = reg.getSize();                             // Obtem o tamanho fixo de um registro.

    while (file.read(reinterpret_cast<char*>(&reg), regSize)) { // Le um registro por vez do arquivo.
        if (reg.chave == chave && !reg.deletado) {              // Se a chave coincide E o registro estah ativo.
            reg.deletado = true;                                // Marca o campo de delecao no registro (remocao logica).

            file.seekp(currentPos);                             // Retorna o ponteiro de escrita (put pointer) ao inicio do registro.

            file.write(reinterpret_cast<const char*>(&reg), regSize); // Grava APENAS o registro modificado de volta no disco.

            marked = true;                                      // Sinaliza que a chave foi encontrada e marcada.
            break;                                              // Sai do loop de leitura.

        } // Fim do IF

        currentPos = file.tellg();                              // Obtem a posicao de leitura atual (get pointer) para o proximo registro.

    } // Fim do WHILE

    file.close();                                               // Fecha o arquivo.

    if (marked) {                                               // Verifica se a chave foi deletada.
        // Imprime sucesso:
        cout << "Registro com chave " << chave << " marcado como deletado no arquivo principal (acesso direto a disco).\n";
    } // Fim do IF

    else {
        // Imprime aviso de falha.
        cerr << "Aviso: Registro ATIVO para a chave " << chave << " nao foi encontrado no arquivo principal.\n";
    } // Fim do ELSE

} // Fim de markAsDeleted

// ====================================================================================================================

// METODOS DE REMOCAO NO INDICE (DELETEB)
// Remove Chave K_idx e Ponteiro A_{idx+1} de um Noh na Memoria, deslocando os elementos.
void ArvoreMVias::removeKeyAndPointer(vector<int>& vals, int idx) {
    int n = node_get_n(vals);                                   // Obtem o numero de chaves atual (n).

    for (int i = idx; i < n - 1; ++i) {                         // Loop: Desloca chaves a partir da posicao `idx`.
        node_set_chave(vals, i, node_get_chave(vals, i + 1));   // Move a chave K_{i+1} para a posicao K_i.
    } // Fim do FOR

    node_set_chave(vals, n - 1, 0);                             // Zera o ultimo espaco de chave (que agora eh uma duplicata).

    for (int i = idx + 1; i <= n; ++i) {                        // Loop: Desloca ponteiros a partir da posicao `idx + 1`.
        node_set_filho(vals, i, node_get_filho(vals, i + 1));   // Move o ponteiro A_{i+1} para a posicao A_i.
    } // Fim do FOR

    node_set_filho(vals, n, 0);                                 // Zera o ultimo espaco de ponteiro (que agora eh uma duplicata).
    node_set_n(vals, n - 1);                                    // Decrementa o numero de chaves.

} // Fim de removeKeyAndPointer

// --------------------------------------------------------------------------------------------------------------------

// Funcao Recursiva Principal para Remocao no Indice.
void ArvoreMVias::deleteFromNode(int nodeId, int chave) {
    cout << "DEBUG: M=" << M << ", T=" << T << ", T-1=" << T-1 << endl; // Imprime parametros para debug.

    vector<int> nodeVals;                                       // Declara vetor para o noh.
    readNode(nodeId, nodeVals);                                 // Leh o noh atual.

    int n = node_get_n(nodeVals);                               // Obtem o numero de chaves.
    int i = 0;                                                  // Inicializa o indice de busca.

    while (i < n && chave > node_get_chave(nodeVals, i)) {      // Busca a chave ou a posicao do ponteiro de descida.
        i++;
    } // Fim do WHILE

    // CASO 1: Chave encontrada neste noh (chave == K[i]).
    if (i < n && chave == node_get_chave(nodeVals, i)) {
        if (node_get_folha(nodeVals)) {                         // Sub-Caso 1.A: O noh eh folha.

            // CASO 1.A: Remocao simples em folha
            // Imprime remocao:
            cout << "Removendo chave " << chave << " da folha " << nodeId << endl;

            int nBefore = node_get_n(nodeVals);                 // Guarda n antes da remocao.

            removeFromLeaf(nodeVals, i);                        // Remove a chave do vetor em memoria (shift).
            writeNode(nodeId, nodeVals);                        // Grava o noh folha de volta no disco.

            int nAfter = node_get_n(nodeVals);                  // Obtem n apos a remocao.
            // Imprime verificacao do tamanho:
            cout << "Folha " << nodeId << ": nAfter=" << nAfter << ", T-1=" << T-1 << endl;

            if (nAfter < T - 1) {                               // Verifica underflow (menos que o minimo T-1).
                // Imprime alerta:
                cout << "=== UNDERFLOW DETECTADO na folha " << nodeId << " ===" << endl;
                int parentId = findParent(raiz, nodeId, 0);     // Busca o ID do noh pai.
                if (parentId != 0) {
                    int childIndex = findChildIndex(parentId, nodeId); // Encontra o indice A[i] do filho no noh pai.
                    if (childIndex != -1) {
                        // Imprime informacoes do pai:
                        cout << "Pai encontrado: " << parentId << ", childIndex: " << childIndex << endl;
                        cout << "Chamando fillChild..." << endl;
                        fillChild(parentId, childIndex);        // Corrige o underflow (empresta uma chave ou realiza merge).
                    } else {
                        // Erro logico:
                        cout << "ERRO: Indice do filho nao encontrado" << endl;
                    } // Fim do ELSE
                } else {
                    cout << "Folha " << nodeId << " eh a raiz ou pai nao encontrado" << endl; // Noh raiz nao tem pai.
                    if (nodeId == raiz) {
                        cout << "Folha EH a raiz. Nada a fazer." << endl; // Raiz pode ter menos que T-1 chaves.
                    } // Fim do IF
                } // Fim do ELSE
            } else {
                cout << "Sem underflow na folha " << nodeId << endl; // Nao ha underflow.
            } // Fim do ELSE
        } // Fim do IF (Noh eh folha)
        else {
            // CASO 1.B: Remocao em noh interno
            // Imprime remocao:
            cout << "Removendo chave " << chave << " do noh interno " << nodeId << endl;
            removeFromInternalNode(nodeId, i);                  // Remove em noh interno (usa predecessores/sucessores ou merge).
        } // Fim do ELSE

    } // Fim do IF (CASO 1: Chave encontrada)

    // CASO 2: Chave nao estah neste noh - descer recursivamente por A[i].
    else {
        if (node_get_folha(nodeVals)) {                         // Se chegou na folha e nao encontrou, a chave nao existe.
            cout << "Erro: Chave " << chave << " nao encontrada na folha " << nodeId << "\n";
            return;                                             // Encerra a recursao com erro.
        } // Fim do IF

        int childId = node_get_filho(nodeVals, i);              // Obtem o ID do filho para onde a busca continua.
        vector<int> childVals;
        readNode(childId, childVals);                           // Leh o noh filho para verificar o tamanho.

        if (node_get_n(childVals) == T - 1) {                   // Pre-condicao: Filho tem o minimo de chaves (T-1).
            // Imprime acao preventiva:
            cout << "Filho " << childId << " tem minimo de chaves. Executando fillChild...\n";
            fillChild(nodeId, i);                               // Corrige o underflow no filho antes de descer.
            readNode(nodeId, nodeVals);                         // Re-leh o noh pai (pois fillChild pode ter alterado o pai).
            childId = node_get_filho(nodeVals, i);              // Recalcula o ID do filho (pode ter mudado devido a merge).
        } // Fim do IF

        if (childId != 0) {                                     // Se o filho ainda existe (nao foi fundido).
            cout << "Descendo para filho " << childId << " na busca pela chave " << chave << "\n";
            deleteFromNode(childId, chave);                     // Continua a recursao.
        } // Fim do IF

    } // Fim do ELSE (CASO 2: Descer)

} // Fim de deleteFromNode

// --------------------------------------------------------------------------------------------------------------------

// Remove Chave de um Noh Folha na Memoria e atualiza 'n'.
void ArvoreMVias::removeFromLeaf(vector<int>& vals, int idx) {
    int n = node_get_n(vals);                                       // Obtem o numero atual de chaves (n).
    cout << "removeFromLeaf: n=" << n << ", idx=" << idx << endl;   // Imprime o status inicial da remocao.

    for (int i = idx; i < n - 1; i++) {                             // Loop: Move as chaves apos o indice `idx` para a esquerda.
        node_set_chave(vals, i, node_get_chave(vals, i + 1));       // Sobrescreve a chave na posicao `i` com a chave `i+1`.
    } // Fim do FOR

    node_set_chave(vals, n - 1, 0);                             // Zera o ultimo espaco de chave que ficou com uma duplicata.
    node_set_n(vals, n - 1);                                    // Decrementa o numero de chaves (n).

    cout << "removeFromLeaf: novo n=" << node_get_n(vals) << endl;  // Imprime o novo numero de chaves.

} // Fim de removeFromLeaf

// --------------------------------------------------------------------------------------------------------------------

// Substitui a Chave por Predecessor/Sucessor ou faz Merge.
void ArvoreMVias::removeFromInternalNode(int nodeId, int idx) {
    vector<int> nodeVals;                                       // Declara vetor.
    readNode(nodeId, nodeVals);                                 // Leh o noh pai.

    int chaveParaDeletar = node_get_chave(nodeVals, idx);       // Guarda a chave a ser deletada do noh interno.

    cout << "DEBUG removeFromInternalNode: noh=" << nodeId << ", idx=" << idx // Imprime a chave e noh a processar.
         << ", chave=" << chaveParaDeletar << endl;

    int leftChildId = node_get_filho(nodeVals, idx);            // Obtem o ID do filho esquerdo (ponteiro A_idx).
    int rightChildId = node_get_filho(nodeVals, idx + 1);       // Obtem o ID do filho direito (ponteiro A_idx+1).

    cout << "DEBUG: leftChild=" << leftChildId << ", rightChild=" << rightChildId << endl; // Imprime IDs dos filhos.

    vector<int> leftVals, rightVals;                            // Declara vetores para os filhos.
    readNode(leftChildId, leftVals);                            // Leitura do filho esquerdo.
    readNode(rightChildId, rightVals);                          // Leitura do filho direito.

    int nLeft = node_get_n(leftVals);                           // Obtem n do filho esquerdo.
    int nRight = node_get_n(rightVals);                         // Obtem n do filho direito.

    cout << "DEBUG: left.n=" << nLeft << ", right.n=" << nRight << ", T=" << T << endl; // Imprime contadores.

    // Caso 2A: Filho esquerdo tem pelo menos T chaves (pode emprestar).
    if (nLeft >= T) {
        int predecessor = getPredecessor(leftChildId);              // Encontra o predecessor da chave (maior chave no filho esquerdo).
        cout << "Caso 2A: Predecessor = " << predecessor << endl;   // Imprime o valor do predecessor.
        node_set_chave(nodeVals, idx, predecessor);                 // Substitui a chave K_idx pela chave predecessor (no noh pai).
        writeNode(nodeId, nodeVals);                                // Grava o noh pai modificado.
        deleteFromNode(leftChildId, predecessor);                   // Deleta recursivamente o predecessor no noh esquerdo.
    } // Fim do IF

    // Caso 2B: Filho direito tem pelo menos T chaves (pode emprestar).
    else if (nRight >= T) {
        int successor = getSuccessor(rightChildId);                 // Encontra o sucessor da chave (menor chave no filho direito).
        cout << "Caso 2B: Successor = " << successor << endl;       // Imprime o valor do sucessor.
        node_set_chave(nodeVals, idx, successor);                   // Substitui a chave K_idx pela chave sucessor (no noh pai).
        writeNode(nodeId, nodeVals);                                // Grava o noh pai modificado.
        deleteFromNode(rightChildId, successor);                    // Deleta recursivamente o sucessor no noh direito.
    } // Fim do ELSE-IF

    // Caso 2C: Ambos os filhos tem T-1 chaves (minimo), realiza fusao.
    else {
        cout << "Caso 2C: Fazendo merge dos filhos..." << endl; // Imprime a decisao de fusao.
        mergeNodes(nodeId, idx);                                // Realiza a fusao dos filhos (e move a chave do pai para baixo).

        if (readNode(nodeId, nodeVals)) {                       // Re-leh o noh pai (pois o merge alterou o pai).
            int mergedChildId = node_get_filho(nodeVals, idx);  // Obtem o ID do noh fundido.
            if (mergedChildId != 0) {
                cout << "Deletando recursivamente do filho merged " << mergedChildId << endl; // Imprime descida no noh fundido.
                deleteFromNode(mergedChildId, chaveParaDeletar); // Deleta a chave original no noh fundido.
            } // Fim do IF

        } // Fim do IF

    } // Fim do ELSE

} // Fim de removeFromInternalNode

// --------------------------------------------------------------------------------------------------------------------

// Faz o Filho em Underflow (T-1) receber uma Chave (Empresta ou Funde).
void ArvoreMVias::fillChild(int parentId, int idx) {
    vector<int> parentVals;                                     // Declara vetor para o pai.
    readNode(parentId, parentVals);                             // Leh o noh pai.

    int childId = node_get_filho(parentVals, idx);              // Obtem o ID do filho com underflow.
    if (childId == 0) return;                                   // Protecao: se o filho nao existe, sai.

    vector<int> childVals;                                      // Declara vetor para o filho.
    readNode(childId, childVals);                               // Leitura do noh filho.

    cout << "fillChild: parent=" << parentId << ", idx=" << idx // Imprime status do underflow.
         << ", child=" << childId << ", child.n=" << node_get_n(childVals) << endl;

    // 1. Tenta emprestar do irmao esquerdo (se existir e tiver chaves suficientes >= T).
    if (idx > 0) {                                              // Verifica se existe irmao esquerdo (idx > 0).
        int leftSiblingId = node_get_filho(parentVals, idx - 1); // Obtem ID do irmao esquerdo.
        vector<int> leftVals;
        readNode(leftSiblingId, leftVals);                      // Leh o irmao esquerdo.

        if (node_get_n(leftVals) >= T) {                        // Se o irmao esquerdo tem pelo menos T chaves (suficiente para emprestar).
            cout << "Emprestando do irmao esquerdo " << leftSiblingId << endl; // Imprime a acao.
            borrowFromLeft(parentId, idx);                      // Realiza o emprestimo da chave do pai para o filho.
            return;                                             // Sai apos a correcao do underflow.
        } // Fim do IF
    } // Fim do IF

    // 2. Tenta emprestar do irmao direito (se existir e tiver chaves suficientes >= T).
    if (idx < node_get_n(parentVals)) {                         // Verifica se existe irmao direito (idx eh menor que o numero de chaves do pai).
        int rightSiblingId = node_get_filho(parentVals, idx + 1); // Obtem ID do irmao direito.
        vector<int> rightVals;
        readNode(rightSiblingId, rightVals);                    // Leh o irmao direito.

        if (node_get_n(rightVals) >= T) {                       // Se o irmao direito tem pelo menos T chaves.
            cout << "Emprestando do irmao direito " << rightSiblingId << endl; // Imprime a acao.
            borrowFromRight(parentId, idx);                     // Realiza o emprestimo da chave do pai para o filho.
            return;                                             // Sai apos a correcao do underflow.
        } // Fim do IF
    } // Fim do IF

    // 3. Se nao pode emprestar de nenhum irmao, realiza a fusao (merge).
    cout << "Nao pode emprestar. Fazendo merge..." << endl;     // Imprime a acao.
    if (idx > 0) {
        mergeNodes(parentId, idx - 1);                          // Funde o filho com underflow com o irmao esquerdo.
    } // Fim do IF
    else if (idx < node_get_n(parentVals)) {
        mergeNodes(parentId, idx);                              // Funde o filho com underflow com o irmao direito (o primeiro filho).
    } // Fim do else IF
    else {
        cout << "ERRO: Nao foi possivel fazer merge - indices invalidos" << endl; // Erro em caso de noh solitario.
    } // Fim do ELSE

} // Fim de fillChild

// --------------------------------------------------------------------------------------------------------------------

// Funde 2 Nohs Irmaos e a Chave do Pai entre eles.
void ArvoreMVias::mergeNodes(int parentId, int idx) {
    vector<int> parentVals;                                     // Declara vetor para o pai.
    readNode(parentId, parentVals);                             // Leh o noh pai.

    int leftId = node_get_filho(parentVals, idx);               // ID do noh esquerdo (que recebera a fusao).
    int rightId = node_get_filho(parentVals, idx + 1);          // ID do noh direito (que sera deletado).

    vector<int> leftVals, rightVals;                            // Declara vetores para os nos.
    readNode(leftId, leftVals);                                 // Leitura do noh esquerdo.
    readNode(rightId, rightVals);                               // Leitura do noh direito.

    int nLeft = node_get_n(leftVals);                           // Obtem n do noh esquerdo.
    int nRight = node_get_n(rightVals);                         // Obtem n do noh direito.
    int parentKey = node_get_chave(parentVals, idx);            // Obtem a chave do pai que ira descer na fusao.

    cout << "DEBUG MERGE: left=" << leftId << "(" << nLeft << "), right=" << rightId // Imprime informacoes da fusao.
         << "(" << nRight << "), parentKey=" << parentKey << endl;

    if (nLeft + nRight + 1 > M - 1) {                           // Protecao: Verifica se a fusao nao excede a capacidade maxima (M-1).
        cerr << "ERRO: Merge excederia capacidade maxima do noh!\n";
        return;
    } // Fim do IF

    node_set_chave(leftVals, nLeft, parentKey);                 // 1. Move a chave K_idx do pai para a ultima posicao do noh esquerdo.

    for (int i = 0; i < nRight; i++) {                          // 2. Copia todas as chaves do filho direito para o esquerdo.
        node_set_chave(leftVals, nLeft + 1 + i, node_get_chave(rightVals, i)); // As chaves comecam apos a chave promovida do pai.
    } // Fim do FOR

    if (!node_get_folha(leftVals)) {                            // 3. Se nao forem folhas, copia tambem os ponteiros de filhos.
        for (int i = 0; i <= nRight; i++) {
            node_set_filho(leftVals, nLeft + 1 + i, node_get_filho(rightVals, i)); // Move os nRight + 1 ponteiros.
        } // Fim do FOR
    } // Fim do IF

    node_set_n(leftVals, nLeft + nRight + 1);                   // 4. Atualiza o contador de chaves do noh fundido.

    removeKeyAndPointer(parentVals, idx);                       // 5. Remove a chave K_idx e o ponteiro A_{idx+1} do noh pai.

    cout << "DEBUG: Apos merge - left.n=" << node_get_n(leftVals) // Imprime o resultado da fusao.
         << ", parent.n=" << node_get_n(parentVals) << endl;

    writeNode(leftId, leftVals);                                // 6. Grava o noh esquerdo (fundido) atualizado.
    writeNode(parentId, parentVals);                            // Grava o noh pai atualizado.

    deleteNode(rightId);                                        // 7. Remove o noh direito (limpeza de memoria).

    // 8. Verificacao Critica: Se o pai eh a raiz e ficou vazio.
    if (parentId == raiz && node_get_n(parentVals) == 0) {
        cout << "Raiz " << parentId << " ficou vazia apos merge. Promovendo filho " << leftId << " como nova raiz.\n"; // Imprime promocao.
        raiz = leftId;                                          // Define o noh fundido como a nova raiz.
        writeHeader();                                          // Atualiza o cabecalho da arvore.
        deleteNode(parentId);                                   // Limpa a antiga raiz (agora vazia).
    } // Fim do IF

    // 9. Se nao eh raiz e ficou com underflow.
    else if (parentId != raiz && node_get_n(parentVals) < T - 1) { // O noh pai pode ter underflow apos perder uma chave.
        cout << "Pai " << parentId << " com underflow. Buscando avo...\n"; // Imprime a necessidade de propagar o underflow.
        int grandParentId = findParent(raiz, parentId, 0);      // Busca o avo (pai do pai).
        if (grandParentId != 0) {
            int parentIndex = findChildIndex(grandParentId, parentId); // Encontra o indice do pai no avo.
            if (parentIndex != -1) {
                cout << "Chamando fillChild no avo " << grandParentId << " para indice " << parentIndex << endl; // Imprime recursao.
                fillChild(grandParentId, parentIndex);          // Propaga a correcao de underflow no noh avo.
            } // Fim do IF

        } // Fim do IF

    } // Fim do ELSE-IF

} // Fim de mergeNodes

// --------------------------------------------------------------------------------------------------------------------

// Encontra a MAIOR Chave na Subarvore a ESQUERDA (predecessor).
int ArvoreMVias::getPredecessor(int nodeId) {
    vector<int> vals;                                           // Declara vetor para o noh.
    readNode(nodeId, vals);                                     // Leh o noh inicial.

    while (!node_get_folha(vals)) {                             // Loop: Desce na arvore.
        int lastChild = node_get_filho(vals, node_get_n(vals)); // Obtem o ID do filho mais a direita (ponteiro A_n).
        readNode(lastChild, vals);                              // Leh o noh filho, substituindo 'vals' pelo noh filho.
    } // Fim do WHILE

    return node_get_chave(vals, node_get_n(vals) - 1);          // Retorna a ultima chave da folha encontrada (chave K_{n-1}).

} // Fim de getPredecessor

// --------------------------------------------------------------------------------------------------------------------

// Encontra a MENOR Chave na Subarvore a DIREITA (sucessor).
int ArvoreMVias::getSuccessor(int nodeId) {
    vector<int> vals;                                           // Declara vetor para o noh.
    readNode(nodeId, vals);                                     // Leh o noh inicial.

    while (!node_get_folha(vals)) {                             // Loop: Desce na arvore.
        int firstChild = node_get_filho(vals, 0);               // Obtem o ID do primeiro filho (ponteiro A_0).
        readNode(firstChild, vals);                             // Leh o noh filho, substituindo 'vals' pelo noh filho.
    } // Fim do WHILE

    return node_get_chave(vals, 0);                             // Retorna a primeira chave da folha encontrada (chave K_0).

} // Fim de getSuccessor

// --------------------------------------------------------------------------------------------------------------------

// Move Chave do Irmao ESQUERDO para o Noh em Underflow.
void ArvoreMVias::borrowFromLeft(int parentId, int childIndex) {
    vector<int> parentVals, leftVals, childVals;                // Declaracao de vetores para os tres nos (Pai, Irmao Esquerdo, Filho).
    readNode(parentId, parentVals);                             // Leh o noh pai.

    int childId = node_get_filho(parentVals, childIndex);       // Obtem ID do filho com underflow.
    int leftId = node_get_filho(parentVals, childIndex - 1);    // Obtem ID do irmao esquerdo (que empresta).

    readNode(childId, childVals);                               // Leitura dos nos Filho e Irmao.
    readNode(leftId, leftVals);

    int nChild = node_get_n(childVals);                         // Obtem n do Filho.
    int nLeft = node_get_n(leftVals);                           // Obtem n do Irmao.

    for (int i = nChild; i > 0; --i) {                          // 1. Shift de chaves do noh filho para a direita.
        node_set_chave(childVals, i, node_get_chave(childVals, i - 1)); // Abre espaco para a nova chave na posicao 0 (K_0).
    } // Fim do FOR

    if (!node_get_folha(childVals)) {                           // Se nao for folha, faz shift dos ponteiros de filhos tambem.
        for (int i = nChild + 1; i > 0; --i) {                  // Abre espaco para o novo ponteiro na posicao 0 (A_0).
            node_set_filho(childVals, i, node_get_filho(childVals, i - 1));
        } // Fim do FOR
    } // Fim do IF

    node_set_chave(childVals, 0, node_get_chave(parentVals, childIndex - 1)); // 2. Move a chave K_{idx-1} do PAI para a posicao K_0 do Filho.

    if (!node_get_folha(childVals))                             // 3. Move o ultimo filho do irmao (A_{nLeft}) para a posicao A_0 do Filho.
        node_set_filho(childVals, 0, node_get_filho(leftVals, nLeft));

    node_set_chave(parentVals, childIndex - 1, node_get_chave(leftVals, nLeft - 1)); // 4. Move a ultima chave do irmao (K_{nLeft-1}) para a posicao do PAI.

    node_set_n(childVals, nChild + 1);                          // 5. Atualiza contadores: Filho ganha 1 chave.
    node_set_n(leftVals, nLeft - 1);                            // Irmao perde 1 chave.

    writeNode(parentId, parentVals);                            // 6. Grava os tres nos atualizados no disco.
    writeNode(childId, childVals);
    writeNode(leftId, leftVals);

} // Fim de borrowFromLeft

// --------------------------------------------------------------------------------------------------------------------

// Move Chave do Irmao DIREITO para o Noh em Underflow.
void ArvoreMVias::borrowFromRight(int parentId, int childIndex) {
    vector<int> parentVals, rightVals, childVals;                // Declaracao de vetores para os tres nos (Pai, Irmao Direito, Filho).
    readNode(parentId, parentVals);                             // Leh o noh pai.

    int childId = node_get_filho(parentVals, childIndex);       // Obtem ID do filho com underflow.
    int rightId = node_get_filho(parentVals, childIndex + 1);   // Obtem ID do irmao direito (que empresta).

    readNode(childId, childVals);                               // Leitura dos nos Filho e Irmao.
    readNode(rightId, rightVals);

    int nChild = node_get_n(childVals);                         // Obtem n do Filho.
    int nRight = node_get_n(rightVals);                         // Obtem n do Irmao.

    node_set_chave(childVals, nChild, node_get_chave(parentVals, childIndex)); // 1. Move a chave K_{idx} do PAI para a ultima posicao do Filho (K_nChild).

    if (!node_get_folha(childVals))                             // 2. Move o primeiro filho do irmao (A_0) para a ultima posicao de ponteiro do Filho (A_{nChild+1}).
        node_set_filho(childVals, nChild + 1, node_get_filho(rightVals, 0));

    node_set_chave(parentVals, childIndex, node_get_chave(rightVals, 0)); // 3. Move a primeira chave do irmao (K_0) para a posicao do PAI (K_idx).

    for (int i = 0; i < nRight - 1; ++i) {                      // 4. Shift de chaves do irmao direito para a esquerda (remove K_0).
        node_set_chave(rightVals, i, node_get_chave(rightVals, i + 1));
    } // Fim do FOR

    for (int i = 0; i < nRight; ++i) {                          // Shift de ponteiros de filhos do irmao direito para a esquerda (remove A_0).
        node_set_filho(rightVals, i, node_get_filho(rightVals, i + 1));
    } // Fim do FOR

    node_set_chave(rightVals, nRight - 1, 0);                   // 5. Limpa o ultimo espaco de chave.
    node_set_filho(rightVals, nRight, 0);                       // Limpa o ultimo espaco de ponteiro.
    node_set_n(childVals, nChild + 1);                          // Atualiza contadores: Filho ganha 1 chave.
    node_set_n(rightVals, nRight - 1);                          // Irmao perde 1 chave.

    writeNode(parentId, parentVals);                            // 6. Grava os tres nos atualizados no disco.
    writeNode(childId, childVals);
    writeNode(rightId, rightVals);

} // Fim de borrowFromRight

// --------------------------------------------------------------------------------------------------------------------

// Limpa o conteudo de um Noh no Disco.
void ArvoreMVias::deleteNode(int nodeId) {
    vector<int> empty(nodeInts(), 0);                           // Cria um vetor de zeros do tamanho exato de um noh no disco.
    if (empty.size() >= 2) empty[1] = 1;                        // opcional: mantem a flag 'folha' como 1 (para debug/uso futuro).
    writeNode(nodeId, empty);                                   // Escreve o noh vazio/limpo no disco (marcacao de delecao fisica).

    cout << "[DIAG] deleteNode: limpo noh " << nodeId << endl;  // Imprime que o noh foi limpo.

} // Fim de deleteNode

// --------------------------------------------------------------------------------------------------------------------

// Busca recursivamente o Pai de um Noh.
int ArvoreMVias::findParent(int currentNode, int targetId, int parentId) {
    cout << "findParent: current=" << currentNode << ", target=" << targetId << ", parent=" << parentId << endl; // Imprime o caminho da busca.

    if (currentNode == targetId) {                              // Caso base 1: encontrou o noh alvo (targetId), o noh pai ja foi passado.
        cout << "Encontrado! Pai eh: " << parentId << endl;
        return parentId;                                        // Retorna o ID do noh pai.
    } // Fim do IF

    if (currentNode == 0) return 0;                             // Caso base 2: ponteiro nulo (chegou ao fim de um ramo).

    vector<int> vals;                                           // Leh o noh atual.
    if (!readNode(currentNode, vals)) return 0;                 // Se a leitura falhar, retorna 0 (erro/fim).

    if (node_get_folha(vals)) return 0;                         // Se for folha, nao ha mais nos internos para buscar.

    int n = node_get_n(vals);                                   // Obtem o numero de chaves.
    cout << "Buscando nos " << n+1 << " filhos do noh " << currentNode << endl; // Imprime quantos filhos serao inspecionados.

    for (int i = 0; i <= n; i++) {
        int childId = node_get_filho(vals, i);                  // Obtem o ID do filho A[i].
        cout << "Filho[" << i << "] = " << childId << endl;     // Imprime o ID do filho.

        if (childId == targetId) {                              // Verifica se o filho A[i] eh o noh alvo.
            cout << "Encontrado pai direto: " << currentNode << " para filho " << targetId << endl; // Imprime sucesso.
            return currentNode;                                 // Retorna o noh atual (que eh o pai).
        } // Fim do IF

        if (childId != 0) {                                     // Se o filho for valido (nao nulo).
            int result = findParent(childId, targetId, currentNode); // Chamada recursiva para o filho, passando o noh atual como novo pai.
            if (result != 0) return result;                     // Se a chamada recursiva encontrar o pai, retorna o resultado.
        } // Fim do IF
    } // Fim do FOR

    return 0;                                                   // Se todos os filhos forem inspecionados sem sucesso, retorna 0.

} // Fim de findParent

// --------------------------------------------------------------------------------------------------------------------

// Encontra a Posicao (indice) do Ponteiro para o Filho.
int ArvoreMVias::findChildIndex(int parentId, int childId) {
    cout << "findChildIndex: parent=" << parentId << ", child=" << childId << endl; // Imprime os IDs de entrada.

    vector<int> parentVals;                                     // Leh o noh pai.
    if (!readNode(parentId, parentVals)) return -1;             // Se falhar a leitura, retorna -1.

    int n = node_get_n(parentVals);                             // Obtem o numero de chaves (n).
    cout << "Procurando em " << n+1 << " filhos do pai " << parentId << endl;   // Imprime o numero de ponteiros a checar.

    for (int i = 0; i <= n; i++) {                              // Itera sobre todos os n+1 ponteiros de filhos.
        int currentChild = node_get_filho(parentVals, i);       // Obtem o ID do filho na posicao A[i].
        cout << "Filho[" << i << "] = " << currentChild << endl; // Imprime o ID do filho atual.

        if (currentChild == childId) {                          // Se o ID do filho atual for o ID procurado.
            cout << "Encontrado no indice: " << i << endl;      // Imprime o sucesso.
            return i;                                           // Retorna o indice 'i' do ponteiro.
        } // Fim do IF

    } // Fim do FOR

    cout << "Filho nao encontrado!" << endl;                    // Imprime falha.
    return -1;                                                  // Retorna -1 se o filho nao for encontrado.

} // Fim de findChildIndex

// --------------------------------------------------------------------------------------------------------------------

// Remove Chave do Indice e Marca Dados como Deletados.
void ArvoreMVias::deleteB(int chave) {
    if (!readHeader()) {                                        // Tenta ler o cabecalho da arvore.
        cerr << "Erro ao ler header antes da delecao.\n";
        return;                                                 // Aborta em caso de erro.
    } // Fim do IF

    Resultado r = mSearch(chave);                               // 1. Verifica se a chave existe no indice.
    if (!r.encontrou) {
        cout << "Aviso: Chave " << chave << " nao existe no indice. Delecao cancelada.\n";
        return;                                                 // Aborta se nao encontrar.
    } // Fim do IF

    cout << "=== INICIANDO DELEcAO da chave " << chave << " ===" << endl; // Imprime inicio da delecao.
    cout << "Encontrada no noh " << r.indice_no << ", posicao " << r.posicao << "\n"; // Imprime onde a chave foi encontrada.

    deleteFromNode(raiz, chave);                                // 2. Chama a funcao recursiva para remover a chave do indice.

    vector<int> rootVals;                                       // 3. Pos-remocao: verifica se a raiz ficou vazia (apenas chaves).
    if (readNode(raiz, rootVals)) {
        if (node_get_n(rootVals) == 0 && !node_get_folha(rootVals)) { // Raiz vazia E nao eh folha.
            int newRootId = node_get_filho(rootVals, 0);        // Obtem o ID do unico filho restante (A_0).
            cout << "Raiz " << raiz << " ficou vazia. Substituindo por filho " << newRootId << "\n"; // Imprime a promocao.
            deleteNode(raiz);                                   // Limpa o noh da raiz antiga.
            raiz = newRootId;                                   // Define o filho como a nova raiz.
            writeHeader();                                      // Atualiza o cabecalho.
        } // Fim do IF
    } // Fim do IF

    markAsDeleted(chave);                                       // 4. Marca o registro no arquivo de dados como deletado (remocao logica).
    cout << "Delecao da chave " << chave << " concluida.\n";    // Imprime conclusao.
    cout << "=== ESTRUTURA FINAL ===" << endl;
    print();                                                    // Imprime a estrutura final da arvore.

} // Fim de deleteB

// ====================================================================================================================

// METODO DE BUSCA (MSEARCH):
Resultado ArvoreMVias::mSearch(int chave) {

    if (!readHeader()) {                                            // Tenta ler o cabecalho da arvore (para obter a raiz).
        cerr << "Erro ao abrir " << arquivoBin << " (header).\n";   // Avisa do erro.
        return {-1, -1, false};                                     // Retorna resultado falso (erro).
    } // Fim do IF

    int p = raiz;                                               // Inicia a busca no ID da raiz.
    while (p != 0) {                                            // Loop principal: continua enquanto o noh atual nao for nulo.
        vector<int> vals;                                       // Declara vetor para o noh.

        if (!readNode(p, vals)) {
            cerr << "Erro ao ler noh " << p << " do arquivo.\n"; // Avisa do erro de leitura.
            return {-1, -1, false};
        } // Fim do IF

        int n = node_get_n(vals);                               // Obtem o numero de chaves (n).
        int i = 0;                                              // Inicializa o indice de busca.

        while (i < n && chave > node_get_chave(vals, i))        // Loop: Encontra a posicao 'i' para a chave.
            i++;                                                //  Onde K[i] >= chave, ou i=n.

        if (i < n && chave == node_get_chave(vals, i)) {        // Se encontrou a chave (chave == K[i]).
            return {p, i + 1, true};                            // Retorna ID do noh, posicao (1-based), e 'encontrou=true'.
        } // Fim do IF

        int filho = node_get_filho(vals, i);                    // Se nao encontrou, obtém o ID do filho para descer (A[i]).
        if (filho == 0) {                                       // Se A[i] for nulo, chegou em uma folha.
            return {p, i + 1, false};                           // Chave nao encontrada. Retorna noh atual e posicao de insercao (1-based).
        } // Fim do IF

        p = filho;                                              // Continua a busca no noh filho (proxima iteracao do WHILE).
    } // Fim do WHILE

    return {-1, -1, false};                                     // Retorno padrao para arvore vazia (raiz == 0) ou erro.

} // Fim de mSearch

// ====================================================================================================================

// METODOS DE GERADOR DE ARQUIVO BINARIO E IMPRESSOES:
// Inicializa ou Carrega a Estrutura do Indice Binario.
void ArvoreMVias::geradorBinario() {

    bool ok = readHeader();     // Tenta ler o header; se falhar, inicializa o arquivo.

    if (!ok) {                  // Se a leitura falhar (primeira execução ou incompatibilidade de M).
        // Reinicializa o estado da árvore.
        raiz = 1;
        nextNodeId = 1;

        writeHeader();          // Grava o novo header.
        createNode(true);       // Cria a raiz (ID 1) como folha e a grava no disco.

        // Imprime mensagem de inicialização.
        cout << "Arquivo binario de indice inicializado com raiz vazia.\n";
    } else {
        // Imprime mensagem se o arquivo existente foi lido.
        cout << "Arquivo binario existente lido. Raiz ID=" << raiz << " Proximo ID=" << nextNodeId << "\n";
    } // Fim do IF/ELSE

} // Fim de geradorBinario

// --------------------------------------------------------------------------------------------------------------------

// Imprime o Conteudo de Todos os Nohs do Indice.
void ArvoreMVias::print() {

    if (!readHeader()) {                                            // Tenta ler o cabeçalho.
        cout << "Arquivo binario nao encontrado ou corrompido.\n";  // Imprime erro se falhar.
        return;                                                     // Encerra se falhar.
    } // Fim do IF

    // Imprime informações do cabeçalho da visualização.
    cout << "\n-----------------| Conteudo da Arvore B (Indice) |-----------------\n";
    cout << "Ordem (M): " << M << ", Grau Minimo (T): " << (M + 1) / 2 << endl;
    cout << "Raiz ID: " << raiz << endl;
    cout << "------------------------------------------------------------------\n";
    // Formato: No ID | n (chaves) | A[0] (filho) | (K[0],A[1]),...,(K[n-1],A[n])
    cout << "ID n | A[0] | (K[0],A[1]),...,(K[n-1],A[n])\n";
    cout << "------------------------------------------------------------------\n";

    for (int id = 1; id < nextNodeId; ++id) {       // Itera sobre todos os IDs de nós que foram alocados.
        vector<int> vals;                           // Declara vetor para o nó.
        if (!readNode(id, vals)) continue;          // Tenta ler o nó. Se falhar, pula (nó deletado/limpo).
        int n = node_get_n(vals);                   // Obtém o número de chaves.

        // Imprime ID, n e o primeiro ponteiro (A[0]).
        cout << setw(2) << id << " " << n << " | " << node_get_filho(vals, 0);

        for (int i = 0; i < n; ++i) {       // Itera e imprime os pares (Chave K[i], Ponteiro A[i+1]).
            cout << " | (" << setw(2) << node_get_chave(vals, i) << "," << node_get_filho(vals, i + 1) << ")";
        } // Fim do FOR

        cout << endl;   // Nova linha.

    } // Fim do FOR

    cout << "------------------------------------------------------------------\n"; // Rodapeh

} // Fim de print

// --------------------------------------------------------------------------------------------------------------------

// Imprime o Indice.
void ArvoreMVias::imprimirIndice() {
    print();    // Imprime a estrutura do índice.
} // Fim de imprimirIndice

// --------------------------------------------------------------------------------------------------------------------

// Imprime o Conteudo Completo do Arquivo de Dados.
void ArvoreMVias::imprimirArquivoPrincipal() {
    ifstream fin(arquivoDados, ios::binary);    // Abre o arquivo de dados para leitura.
    if (!fin) {                                 // Verifica erro.
        cout << "Arquivo de dados vazio ou nao existe (" << arquivoDados << ").\n";
        return;
    } // Fim do IF

    Registro reg;       // Declara registro.

    // Imprime cabeçalho.
    cout << "\n--- Conteudo COMPLETO do Arquivo Principal BINARIO (" << arquivoDados << ") ---\n";
    cout << "Chave | Deletado | Dados\n";
    cout << "------------------------------------------------------------------\n";

    // Loop de leitura: lê registro por registro.
    while (fin.read(reinterpret_cast<char*>(&reg), reg.getSize())) {
        // Exibe o registro.
        cout << setw(5) << reg.chave
             << " | " << setw(8) << (reg.deletado ? "TRUE" : "FALSE")
             << " | " << reg.dados << endl;
    } // Fim do WHILE

    fin.close();    // Fecha o arquivo.

} // Fim de imprimirArquivoPrincipal

// --------------------------------------------------------------------------------------------------------------------

// Imprime Registros Ativos Associados a uma Chave.
void ArvoreMVias::imprimirArquivoPrincipal(int chave) {
    ifstream fin(arquivoDados, ios::binary);        // Abre o arquivo de dados para leitura.
    if (!fin) {                                     // Verifica erro.
        cerr << "Erro: Arquivo de dados nao existe ou nao pode ser aberto.\n";
        return;
    } // Fim do IF

    Registro reg;       // Declara registro.
    bool found = false; // Flag de busca.

    // Imprime cabeçalho.
    cout << "Chave | Deletado | Dados\n";
    cout << "------------------------------------------------------------------\n";

    // Loop de leitura.
    while (fin.read(reinterpret_cast<char*>(&reg), reg.getSize())) {
        // Verifica se a chave coincide E se o registro NÃO está deletado (ativo).
        if (reg.chave == chave && !reg.deletado) {
            cout << setw(5) << reg.chave        // Exibe o registro.
                 << " | " << setw(8) << (reg.deletado ? "TRUE" : "FALSE")
                 << " | " << reg.dados << endl;
            found = true;
        } // Fim do IF
    } // Fim do WHILE

    // Mensagem se nenhum registro ativo for encontrado.
    if (!found) cout << "Nenhum registro ativo com a chave " << chave << " encontrado no arquivo principal.\n";

    fin.close();    // Fecha o arquivo.

} // Fim de imprimirArquivoPrincipal

// ====================================================================================================================