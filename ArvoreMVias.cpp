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
int ArvoreMVias::nodeInts() const {return 2 + (M - 1) + M;}

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
    // escritaDisco++; // Incrementa o contador de acesso a disco (escrita)

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

// Divide o Noh quando um Noh Filho estah Cheio.
void splitChild(int parentId, int childIndex, int childId){}    // Nao usado diretamente.

// --------------------------------------------------------------------------------------------------------------------

// Insercao Principal - lida com Split de Filho Cheio.
void ArvoreMVias::insertNonFull(int nodeId, int chave) {
    vector<int> nodeVals;                                       // Declara vetor para o noh.
    readNode(nodeId, nodeVals);                                 // Leh o noh atual.

    cout << "DEBUG insertNonFull: noh " << nodeId << ", folha=" << node_get_folha(nodeVals) // Log
         << ", n=" << node_get_n(nodeVals) << ", chave=" << chave << endl;

    // CASO 1: O noh eh folha (Insercao Direta)
    if (node_get_folha(nodeVals)) {
        int n = node_get_n(nodeVals);                           // Obtem n.
        int pos = 0;                                            // Inicializa a posicao.

        while (pos < n && node_get_chave(nodeVals, pos) < chave) { // Encontra a posicao de insercao.
            pos++;
        } // Fim do while

        cout << "Inserindo na folha " << nodeId << " na posicao " << pos << endl; // Log

        for (int i = n; i > pos; i--) {                         // Shift das chaves para a direita.
            node_set_chave(nodeVals, i, node_get_chave(nodeVals, i - 1));
        } // Fim do for

        node_set_chave(nodeVals, pos, chave);                   // Insere a nova chave.
        node_set_n(nodeVals, n + 1);                            // Incrementa n.
        writeNode(nodeId, nodeVals);                            // Grava o noh.

        cout << "Folha " << nodeId << " agora tem " << node_get_n(nodeVals) << " chaves: "; // Log
        for (int i = 0; i < node_get_n(nodeVals); i++) {
            cout << node_get_chave(nodeVals, i) << " ";
        } // Fim do for
        cout << endl;

    } // Fim do if (folha)
    // CASO 2: O noh eh interno (Descida ou Split)
    else {
        int n = node_get_n(nodeVals);                           // Obtem n.
        int childIndex = 0;                                     // Inicializa o indice do filho.

        while (childIndex < n && chave > node_get_chave(nodeVals, childIndex)) { // Encontra o ponteiro A[childIndex] para descer.
            childIndex++;
        } // Fim do while

        int childId = node_get_filho(nodeVals, childIndex);     // Obtem ID do filho.
        cout << "Descendo para filho " << childId << " (indice " << childIndex << ")" << endl; // Log

        vector<int> childVals;                                  // Declara vetor.
        readNode(childId, childVals);                           // Leh o noh filho.

        // Se o filho estiver CHEIO (M-1 chaves), realiza o SPLIT ESPECIAL.
        if (node_get_n(childVals) == M - 1) {
            cout << "Filho " << childId << " estah cheio. Fazendo split especial..." << endl; // Log

            vector<int> tempChildVals = childVals;              // Cria uma copia temporaria para simular a insercao de M chaves.
            int temp_n = node_get_n(tempChildVals);             // Obtem n temporario.

            int pos = 0;                                        // Encontra a posicao para a nova chave no noh temporario.
            while (pos < temp_n && chave > node_get_chave(tempChildVals, pos)) {
                pos++;
            } // Fim do while

            for (int i = temp_n; i > pos; i--) {                // Shift para a direita na copia temporaria.
                node_set_chave(tempChildVals, i, node_get_chave(tempChildVals, i - 1));
            } // Fim do for

            node_set_chave(tempChildVals, pos, chave);          // Insere a chave temporariamente (noh agora tem M chaves).
            node_set_n(tempChildVals, temp_n + 1);              // Incrementa n.

            cout << "DEBUG: Noh temporario apos insercao de " << chave << ": "; // Log
            for (int i = 0; i < node_get_n(tempChildVals); i++) {
                cout << node_get_chave(tempChildVals, i) << " ";
            } // Fim do for
            cout << endl;

            // --- LÓGICA DO SPLIT COM M CHAVES ---
            int t = T;
            int medianaIndex = t - 1;                           // A chave mediana a ser promovida estah em T-1.
            int med = node_get_chave(tempChildVals, medianaIndex); // Obtem o valor da mediana.

            cout << "Mediana index: " << medianaIndex << ", valor: " << med << endl; // Log

            int zId = createNode(node_get_folha(tempChildVals)); // Cria o novo noh direito (zId).
            vector<int> zVals;                                  // Declara vetor.
            readNode(zId, zVals);                               // Leh o novo noh.

            node_set_n(childVals, medianaIndex);                // O noh original (childVals) fica com as primeiras T-1 chaves.
            for (int i = 0; i < medianaIndex; ++i) {            // Copia as chaves.
                node_set_chave(childVals, i, node_get_chave(tempChildVals, i));
            } // Fim do for
            for (int i = medianaIndex; i < M - 1; ++i) {        // Zera chaves restantes no noh esquerdo.
                node_set_chave(childVals, i, 0);
            } // Fim do for

            int chavesRestantes = (temp_n + 1) - medianaIndex - 1; // O novo noh (zVals) fica com as chaves APOS a mediana.
            node_set_n(zVals, chavesRestantes);
            for (int j = 0; j < chavesRestantes; ++j) {         // Copia as chaves para o novo noh.
                node_set_chave(zVals, j, node_get_chave(tempChildVals, j + medianaIndex + 1));
            } // Fim do for

            if (!node_get_folha(tempChildVals)) {               // Move filhos correspondentes (se nao for folha).
                for (int j = 0; j <= chavesRestantes; ++j) {
                    node_set_filho(zVals, j, node_get_filho(tempChildVals, j + medianaIndex + 1));
                } // Fim do for
            } // Fim do if

            cout << "Mediana a ser promovida: " << med << endl; // Log

            // --- Insere a Mediana no Noh Pai (nodeVals) ---
            int parent_n = node_get_n(nodeVals);                // Obtem n do pai.

            for (int j = parent_n; j > childIndex; --j) {       // Shift de chaves no pai para a direita.
                node_set_chave(nodeVals, j, node_get_chave(nodeVals, j - 1));
            } // Fim do for
            for (int j = parent_n + 1; j > childIndex + 1; --j) { // Shift de ponteiros no pai para a direita.
                node_set_filho(nodeVals, j, node_get_filho(nodeVals, j - 1));
            } // Fim do for

            node_set_chave(nodeVals, childIndex, med);          // Insere a chave promovida (mediana).
            node_set_filho(nodeVals, childIndex + 1, zId);      // Insere o ponteiro para o novo noh direito (zId).
            node_set_n(nodeVals, parent_n + 1);                 // Incrementa o numero de chaves do pai.

            writeNode(childId, childVals);                      // Grava o noh esquerdo.
            writeNode(zId, zVals);                              // Grava o novo noh direito.
            writeNode(nodeId, nodeVals);                        // Grava o noh pai.

            cout << "Split especial concluido. Mediana " << med << " promovida." << endl; // Log de conclusao.

        } // Fim do if (split)
        else {
            insertNonFull(childId, chave);                      // Filho nao estah cheio, insere normalmente (recursivamente).
        } // Fim do else
    } // Fim do else (noh interno)
} // Fim de insertNonFull

// --------------------------------------------------------------------------------------------------------------------

// Insere Chave e Dados.
void ArvoreMVias::insertB(int chave, const string& dadosElemento) {
    // leituraDisco = 0; // Zera contadores (comentado)
    // escritaDisco = 0; // Zera contadores (comentado)

    Resultado r = mSearch(chave);                               // 1. Verifica se a chave jah existe no indice.
    if (r.encontrou) {
        cout << "Aviso: Chave " << chave << " jah existe no indice. Insercao cancelada.\n";
        return;
    } // Fim do if

    ofstream foutDados(arquivoDados, ios::binary | ios::app);   // 2. Escreve no arquivo principal (cria o registro de dados).
    if (!foutDados) {
        cerr << "Erro ao abrir " << arquivoDados << " para escrita.\n";
        return;
    } // Fim do if

    Registro novoReg;                                           // Preenche o novo registro.
    novoReg.chave = chave;
    size_t len = min(dadosElemento.length(), (size_t)MAX_DATA_SIZE - 1);
    dadosElemento.copy(novoReg.dados, len);
    novoReg.dados[len] = '\0';                                  // Terminador nulo
    novoReg.deletado = false;                                   // Registro ativo

    foutDados.write(reinterpret_cast<const char*>(&novoReg), Registro::getSize()); // Grava o registro.
    foutDados.close();                                          // Fecha o arquivo de dados.

    if (!readHeader()) {                                        // 3. Insere a chave no indice (Arvore B).
        cerr << "Erro ao ler header antes de inserir.\n";
        return;
    } // Fim do if

    vector<int> rootVals;                                       // Leh a raiz.
    readNode(raiz, rootVals);

    cout << "=== INICIANDO INSERcAO DA CHAVE " << chave << " ===" << endl; // Log
    cout << "Raiz atual: " << raiz << ", n=" << node_get_n(rootVals) << endl; // Log

    if (node_get_n(rootVals) == M - 1) {                        // Se a raiz estah cheia, realiza o split da raiz.
        cout << "Raiz cheia. Criando nova raiz..." << endl;      // Log

        int newRootId = createNode(false);                      // Cria a nova raiz (noh interno, 0 chaves).
        vector<int> newRootVals;
        readNode(newRootId, newRootVals);

        node_set_filho(newRootVals, 0, raiz);                   // Faz a raiz antiga ser o primeiro filho da nova raiz.
        writeNode(newRootId, newRootVals);

        raiz = newRootId;                                       // Atualiza a variavel raiz.
        writeHeader();                                          // Atualiza o cabecalho.

        cout << "Nova raiz criada: " << raiz << endl;           // Log

        insertNonFull(raiz, chave);                             // Chama insertNonFull.
    } // Fim do if
    else {
        insertNonFull(raiz, chave);                             // Se a raiz nao estah cheia, insere diretamente.
    } // Fim do else

    cout << "Insercao da chave " << chave << " realizada com sucesso no indice e no arquivo principal.\n"; // Mensagem de sucesso.

    cout << "=== ESTRUTURA FINAL APOS INSERcAO ===" << endl;    // Logs
    print();                                                    // Mostra a estrutura final do indice.
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
    fstream file(arquivoDados, ios::binary | ios::in | ios::out); // Abre o arquivo para leitura, escrita e binario.
    if (!file) {                                                // Verifica erro na abertura.
        cerr << "Erro: Arquivo de dados binario nao existe ou nao pode ser aberto para marcacao de delecao.\n";
        return;
    } // Fim do if

    Registro reg;                                               // Variaveis auxiliares.
    bool marked = false;
    streampos currentPos = 0;
    size_t regSize = reg.getSize();

    while (file.read(reinterpret_cast<char*>(&reg), regSize)) { // Itera sobre os registros.
        if (reg.chave == chave && !reg.deletado) {              // Se a chave coincide e o registro estah ativo.
            reg.deletado = true;                                // Marca como deletado.

            file.seekp(currentPos);                             // Retorna ao inicio do registro.

            file.write(reinterpret_cast<const char*>(&reg), regSize); // Grava APENAS o registro modificado.

            marked = true;                                      // Marca como encontrado e sai do loop.
            break;
        } // Fim do if
        currentPos = file.tellg();                              // Guarda a posicao de onde comecara a proxima leitura.
    } // Fim do while

    file.close();                                               // Fecha o arquivo.
    if (marked) {                                               // Imprime o resultado.
        cout << "Registro com chave " << chave << " marcado como deletado no arquivo principal (acesso direto a disco).\n";
    } // Fim do if
    else {
        cerr << "Aviso: Registro ATIVO para a chave " << chave << " nao foi encontrado no arquivo principal.\n";
    } // Fim do else
    // escritaDisco++; // Conta o acesso de escrita
} // Fim de markAsDeleted

// ====================================================================================================================

// Remove Chave K_idx e Ponteiro A_{idx+1} de um Noh na Memoria, deslocando os elementos.
void ArvoreMVias::removeKeyAndPointer(vector<int>& vals, int idx) {
    int n = node_get_n(vals);                                   // Obtem n.

    for (int i = idx; i < n - 1; ++i) {                         // Desloca chaves K_{idx+1} para a posicao K_idx.
        node_set_chave(vals, i, node_get_chave(vals, i + 1));
    } // Fim do for
    node_set_chave(vals, n - 1, 0);                             // Zera o ultimo espaco de chave.

    for (int i = idx + 1; i <= n; ++i) {                        // Desloca filhos A_{idx+2} para a posicao A_{idx+1}.
        node_set_filho(vals, i, node_get_filho(vals, i + 1));
    } // Fim do for
    node_set_filho(vals, n, 0);                                 // Zera o ultimo espaco de ponteiro.

    node_set_n(vals, n - 1);                                    // Decrementa n.
} // Fim de removeKeyAndPointer

// Funcao Recursiva Principal para Remocao no Indice.
void ArvoreMVias::deleteFromNode(int nodeId, int chave) {
    cout << "DEBUG: M=" << M << ", T=" << T << ", T-1=" << T-1 << endl; // Log

    vector<int> nodeVals;                                       // Leh o noh atual.
    readNode(nodeId, nodeVals);

    int n = node_get_n(nodeVals);                               // Variaveis auxiliares.
    int i = 0;

    while (i < n && chave > node_get_chave(nodeVals, i)) {      // Encontra a chave ou a posicao de descida.
        i++;
    } // Fim do while

    // CASO 1: Chave encontrada neste noh (chave == K[i]).
    if (i < n && chave == node_get_chave(nodeVals, i)) {
        if (node_get_folha(nodeVals)) {
            cout << "Removendo chave " << chave << " da folha " << nodeId << endl; // Log

            removeFromLeaf(nodeVals, i);                        // Remove a chave do vetor em memoria.
            writeNode(nodeId, nodeVals);                        // Grava o noh de volta.

            int nAfter = node_get_n(nodeVals);                  // Verifica underflow na folha (se nao for a raiz).
            cout << "Folha " << nodeId << ": nAfter=" << nAfter << ", T-1=" << T-1 << endl; // Log

            if (nAfter < T - 1) {
                cout << "=== UNDERFLOW DETECTADO na folha " << nodeId << " ===" << endl; // Log
                int parentId = findParent(raiz, nodeId, 0);     // Busca o pai.
                if (parentId != 0) {
                    int childIndex = findChildIndex(parentId, nodeId); // Encontra o indice do filho.
                    if (childIndex != -1) {
                        cout << "Pai encontrado: " << parentId << ", childIndex: " << childIndex << endl; // Log
                        cout << "Chamando fillChild..." << endl;
                        fillChild(parentId, childIndex);        // Corrige o underflow (empresta ou funde).
                    } // Fim do if
                } // Fim do if
            } // Fim do if
        } // Fim do if
        else {
            cout << "Removendo chave " << chave << " do noh interno " << nodeId << endl; // Log
            removeFromInternalNode(nodeId, i);                  // Remove em noh interno (substituicao ou merge).
        } // Fim do else
    } // Fim do if
    // CASO 2: Chave nao estah neste noh - descer recursivamente por A[i].
    else {
        if (node_get_folha(nodeVals)) {                         // Se for folha, erro (chave nao encontrada).
            cout << "Erro: Chave " << chave << " nao encontrada na folha " << nodeId << "\n";
            return;
        } // Fim do if

        int childId = node_get_filho(nodeVals, i);              // Obtem o ID do filho para descer.
        vector<int> childVals;
        readNode(childId, childVals);

        if (node_get_n(childVals) == T - 1) {                   // Pre-condicao: Filho tem o minimo, corrige o underflow antes de descer.
            cout << "Filho " << childId << " tem minimo de chaves. Executando fillChild...\n"; // Log
            fillChild(nodeId, i);                               // Emprestimo ou merge.
            readNode(nodeId, nodeVals);                         // Re-leh o noh pai.
            childId = node_get_filho(nodeVals, i);              // Recalcula o ID do filho (pode ter mudado).
        } // Fim do if

        if (childId != 0) {                                     // Continua a recursao.
            cout << "Descendo para filho " << childId << " na busca pela chave " << chave << "\n";
            deleteFromNode(childId, chave);
        } // Fim do if
    } // Fim do else
} // Fim de deleteFromNode

// Remove Chave de um Noh Folha na Memoria e atualiza 'n'.
void ArvoreMVias::removeFromLeaf(vector<int>& vals, int idx) {
    int n = node_get_n(vals);                                   // Obtem n.
    cout << "removeFromLeaf: n=" << n << ", idx=" << idx << endl; // Log

    for (int i = idx; i < n - 1; i++) {                         // Desloca chaves para a esquerda.
        node_set_chave(vals, i, node_get_chave(vals, i + 1));
    } // Fim do for
    node_set_chave(vals, n - 1, 0);                             // Zera o ultimo espaco de chave.
    node_set_n(vals, n - 1);                                    // Decrementa n.

    cout << "removeFromLeaf: novo n=" << node_get_n(vals) << endl; // Log
} // Fim de removeFromLeaf

// Substitui a Chave por Predecessor/Sucessor ou faz Merge.
void ArvoreMVias::removeFromInternalNode(int nodeId, int idx) {
    vector<int> nodeVals;                                       // Leh o noh.
    readNode(nodeId, nodeVals);

    int chaveParaDeletar = node_get_chave(nodeVals, idx);       // Chave a ser deletada.

    int leftChildId = node_get_filho(nodeVals, idx);            // IDs dos filhos.
    int rightChildId = node_get_filho(nodeVals, idx + 1);

    vector<int> leftVals, rightVals;                            // Leitura dos filhos.
    readNode(leftChildId, leftVals);
    readNode(rightChildId, rightVals);

    int nLeft = node_get_n(leftVals);                           // Contadores dos filhos.
    int nRight = node_get_n(rightVals);

    // Caso 2A: Filho esquerdo tem pelo menos T chaves.
    if (nLeft >= T) {
        int predecessor = getPredecessor(leftChildId);          // Encontra o predecessor.
        cout << "Caso 2A: Predecessor = " << predecessor << endl; // Log
        node_set_chave(nodeVals, idx, predecessor);             // Substitui a chave K_idx pela chave predecessor.
        writeNode(nodeId, nodeVals);
        deleteFromNode(leftChildId, predecessor);               // Deleta recursivamente o predecessor no noh esquerdo.
    } // Fim do if
    // Caso 2B: Filho direito tem pelo menos T chaves.
    else if (nRight >= T) {
        int successor = getSuccessor(rightChildId);             // Encontra o sucessor.
        cout << "Caso 2B: Successor = " << successor << endl; // Log
        node_set_chave(nodeVals, idx, successor);               // Substitui a chave K_idx pela chave sucessor.
        writeNode(nodeId, nodeVals);
        deleteFromNode(rightChildId, successor);                // Deleta recursivamente o sucessor no noh direito.
    } // Fim do else if
    // Caso 2C: Ambos os filhos tem T-1 chaves (minimo).
    else {
        cout << "Caso 2C: Fazendo merge dos filhos..." << endl; // Log
        mergeNodes(nodeId, idx);                                // Realiza o merge (juncao).

        if (readNode(nodeId, nodeVals)) {                       // Re-leh o noh pai.
            int mergedChildId = node_get_filho(nodeVals, idx);
            if (mergedChildId != 0) {
                cout << "Deletando recursivamente do filho merged " << mergedChildId << endl; // Log
                deleteFromNode(mergedChildId, chaveParaDeletar); // Deleta a chave no noh fundido.
            } // Fim do if
        } // Fim do if
    } // Fim do else
} // Fim de removeFromInternalNode

// Faz o Filho em Underflow (T-1) receber uma Chave (Empresta ou Funde).
void ArvoreMVias::fillChild(int parentId, int idx) {
    vector<int> parentVals;                                     // Leh o noh pai.
    readNode(parentId, parentVals);

    int childId = node_get_filho(parentVals, idx);              // Obtem o ID do filho.
    if (childId == 0) return;

    vector<int> childVals;                                      // Leitura do noh filho.
    readNode(childId, childVals);

    cout << "fillChild: parent=" << parentId << ", idx=" << idx
         << ", child=" << childId << ", child.n=" << node_get_n(childVals) << endl; // Log

    // 1. Tenta emprestar do irmao esquerdo (se existir e tiver chaves suficientes >= T).
    if (idx > 0) {
        int leftSiblingId = node_get_filho(parentVals, idx - 1);
        vector<int> leftVals;
        readNode(leftSiblingId, leftVals);

        if (node_get_n(leftVals) >= T) {
            cout << "Emprestando do irmao esquerdo " << leftSiblingId << endl; // Log
            borrowFromLeft(parentId, idx);                      // Realiza o emprestimo.
            return;                                             // Sai apos o emprestimo.
        } // Fim do if
    } // Fim do if

    // 2. Tenta emprestar do irmao direito (se existir e tiver chaves suficientes >= T).
    if (idx < node_get_n(parentVals)) {
        int rightSiblingId = node_get_filho(parentVals, idx + 1);
        vector<int> rightVals;
        readNode(rightSiblingId, rightVals);

        if (node_get_n(rightVals) >= T) {
            cout << "Emprestando do irmao direito " << rightSiblingId << endl; // Log
            borrowFromRight(parentId, idx);                     // Realiza o emprestimo.
            return;                                             // Sai apos o emprestimo.
        } // Fim do if
    } // Fim do if

    // 3. Se nao pode emprestar, realiza a fusao (merge).
    cout << "Nao pode emprestar. Fazendo merge..." << endl;      // Log
    if (idx > 0) {
        mergeNodes(parentId, idx - 1);                          // Funde com o irmao esquerdo.
    } // Fim do if
    else if (idx < node_get_n(parentVals)) {
        mergeNodes(parentId, idx);                              // Funde com o irmao direito.
    } // Fim do else if
    else {
        cout << "ERRO: Nao foi possivel fazer merge - indices invalidos" << endl; // Erro
    } // Fim do else
} // Fim de fillChild

// Funde 2 Nohs Irmaos e a Chave do Pai entre eles.
void ArvoreMVias::mergeNodes(int parentId, int idx) {
    vector<int> parentVals;                                     // Leh o noh pai.
    readNode(parentId, parentVals);

    int leftId = node_get_filho(parentVals, idx);               // Obtem IDs dos nos a serem fundidos.
    int rightId = node_get_filho(parentVals, idx + 1);

    vector<int> leftVals, rightVals;                            // Leitura dos nos a serem fundidos.
    readNode(leftId, leftVals);
    readNode(rightId, rightVals);

    int nLeft = node_get_n(leftVals);                           // Contadores.
    int nRight = node_get_n(rightVals);
    int parentKey = node_get_chave(parentVals, idx);            // Chave do pai que desce.

    cout << "DEBUG MERGE: left=" << leftId << "(" << nLeft << "), right=" << rightId
         << "(" << nRight << "), parentKey=" << parentKey << endl; // Log

    if (nLeft + nRight + 1 > M - 1) {                           // Verifica se a fusao nao excede a capacidade.
        cerr << "ERRO: Merge excederia capacidade maxima do noh!\n";
        return;
    } // Fim do if

    node_set_chave(leftVals, nLeft, parentKey);                 // 1. Move a chave do pai para o noh esquerdo.

    for (int i = 0; i < nRight; i++) {                          // 2. Copia chaves do filho direito para o esquerdo.
        node_set_chave(leftVals, nLeft + 1 + i, node_get_chave(rightVals, i));
    } // Fim do for

    if (!node_get_folha(leftVals)) {                            // 3. Copia filhos (se nao forem folhas).
        for (int i = 0; i <= nRight; i++) {
            node_set_filho(leftVals, nLeft + 1 + i, node_get_filho(rightVals, i));
        } // Fim do for
    } // Fim do if

    node_set_n(leftVals, nLeft + nRight + 1);                   // 4. Atualiza o contador de chaves do noh fundido.

    removeKeyAndPointer(parentVals, idx);                       // 5. Remove chave e ponteiro do noh pai.

    cout << "DEBUG: Apos merge - left.n=" << node_get_n(leftVals)
         << ", parent.n=" << node_get_n(parentVals) << endl; // Log

    writeNode(leftId, leftVals);                                // 6. Atualiza nos.
    writeNode(parentId, parentVals);

    deleteNode(rightId);                                        // 7. Remove noh direito (limpeza).

    // 8. VERIFICAcAO CRITICA: Se o pai eh a raiz e ficou vazio.
    if (parentId == raiz && node_get_n(parentVals) == 0) {
        cout << "Raiz " << parentId << " ficou vazia apos merge. Promovendo filho " << leftId << " como nova raiz.\n"; // Log
        raiz = leftId;                                          // Define a nova raiz.
        writeHeader();                                          // Atualiza o cabecalho.
        deleteNode(parentId);                                   // Limpa a antiga raiz.
    } // Fim do if
    // 9. Se nao eh raiz e ficou com underflow.
    else if (parentId != raiz && node_get_n(parentVals) < T - 1) {
        cout << "Pai " << parentId << " com underflow. Buscando avo...\n"; // Log
        int grandParentId = findParent(raiz, parentId, 0);      // Busca o avo.
        if (grandParentId != 0) {
            int parentIndex = findChildIndex(grandParentId, parentId); // Encontra o indice do pai no avo.
            if (parentIndex != -1) {
                cout << "Chamando fillChild no avo " << grandParentId << " para indice " << parentIndex << endl; // Log
                fillChild(grandParentId, parentIndex);          // Propaga a correcao de underflow.
            } // Fim do if
        } // Fim do if
    } // Fim do else if
} // Fim de mergeNodes

// Encontra a MAIOR Chave na Subarvore a ESQUERDA (predecessor).
int ArvoreMVias::getPredecessor(int nodeId) {
    vector<int> vals;                                           // Leh o noh inicial.
    readNode(nodeId, vals);

    while (!node_get_folha(vals)) {                             // Desce sempre pelo ponteiro mais a direita (A_n) ate chegar em uma folha.
        int lastChild = node_get_filho(vals, node_get_n(vals));
        readNode(lastChild, vals);                              // Leh o noh filho.
    } // Fim do while

    return node_get_chave(vals, node_get_n(vals) - 1);          // Retorna a ultima chave da folha.
} // Fim de getPredecessor

// Encontra a MENOR Chave na Subarvore a DIREITA (sucessor).
int ArvoreMVias::getSuccessor(int nodeId) {
    vector<int> vals;                                           // Leh o noh inicial.
    readNode(nodeId, vals);

    while (!node_get_folha(vals)) {                             // Desce sempre pelo ponteiro mais a esquerda (A_0) ate chegar em uma folha.
        int firstChild = node_get_filho(vals, 0);
        readNode(firstChild, vals);                             // Leh o noh filho.
    } // Fim do while

    return node_get_chave(vals, 0);                             // Retorna a primeira chave da folha.
} // Fim de getSuccessor

// Move Chave do Irmao ESQUERDO para o Noh em Underflow.
void ArvoreMVias::borrowFromLeft(int parentId, int childIndex) {
    vector<int> parentVals, leftVals, childVals;                // Declaracao de vetores para os tres nos.
    readNode(parentId, parentVals);

    int childId = node_get_filho(parentVals, childIndex);       // Obtem IDs.
    int leftId = node_get_filho(parentVals, childIndex - 1);

    readNode(childId, childVals);                               // Leitura dos nos.
    readNode(leftId, leftVals);

    int nChild = node_get_n(childVals);                         // Contadores.
    int nLeft = node_get_n(leftVals);

    for (int i = nChild; i > 0; --i) {                          // 1. Shift de chaves e filhos do noh filho para a direita (abre K_0 e A_0).
        node_set_chave(childVals, i, node_get_chave(childVals, i - 1));
    } // Fim do for
    if (!node_get_folha(childVals)) {
        for (int i = nChild + 1; i > 0; --i) {
            node_set_filho(childVals, i, node_get_filho(childVals, i - 1));
        } // Fim do for
    } // Fim do if

    node_set_chave(childVals, 0, node_get_chave(parentVals, childIndex - 1)); // 2. Move K_{idx-1} (chave do pai) para child[0].

    if (!node_get_folha(childVals))                             // 3. Move A_{nLeft} (ultimo filho do irmao) para child[0] (se nao for folha).
        node_set_filho(childVals, 0, node_get_filho(leftVals, nLeft));

    node_set_chave(parentVals, childIndex - 1, node_get_chave(leftVals, nLeft - 1)); // 4. Move K_{nLeft-1} (ultima chave do irmao) para parent[idx-1].

    node_set_n(childVals, nChild + 1);                          // 5. Atualiza contadores.
    node_set_n(leftVals, nLeft - 1);

    writeNode(parentId, parentVals);                            // 6. Grava os tres nos.
    writeNode(childId, childVals);
    writeNode(leftId, leftVals);
} // Fim de borrowFromLeft

// Move Chave do Irmao DIREITO para o Noh em Underflow.
void ArvoreMVias::borrowFromRight(int parentId, int childIndex) {
    vector<int> parentVals, rightVals, childVals;                // Declaracao de vetores para os tres nos.
    readNode(parentId, parentVals);

    int childId = node_get_filho(parentVals, childIndex);       // Obtem IDs.
    int rightId = node_get_filho(parentVals, childIndex + 1);

    readNode(childId, childVals);                               // Leitura dos nos.
    readNode(rightId, rightVals);

    int nChild = node_get_n(childVals);                         // Contadores.
    int nRight = node_get_n(rightVals);

    node_set_chave(childVals, nChild, node_get_chave(parentVals, childIndex)); // 1. Move K_{idx} (chave do pai) para child[nChild].

    if (!node_get_folha(childVals))                             // 2. Move A_0 (primeiro filho do irmao) para child[nChild + 1] (se nao for folha).
        node_set_filho(childVals, nChild + 1, node_get_filho(rightVals, 0));

    node_set_chave(parentVals, childIndex, node_get_chave(rightVals, 0)); // 3. Move K_0 (primeira chave do irmao) para parent[idx].

    for (int i = 0; i < nRight - 1; ++i) {                      // 4. Shift de chaves e filhos do irmao direito para a esquerda (remove K_0 e A_0).
        node_set_chave(rightVals, i, node_get_chave(rightVals, i + 1));
    } // Fim do for
    for (int i = 0; i < nRight; ++i) {
        node_set_filho(rightVals, i, node_get_filho(rightVals, i + 1));
    } // Fim do for

    node_set_chave(rightVals, nRight - 1, 0);                   // 5. Limpa os ultimos espacos e atualiza contadores.
    node_set_filho(rightVals, nRight, 0);
    node_set_n(childVals, nChild + 1);
    node_set_n(rightVals, nRight - 1);

    writeNode(parentId, parentVals);                            // 6. Grava os tres nos.
    writeNode(childId, childVals);
    writeNode(rightId, rightVals);
} // Fim de borrowFromRight

// Limpa o conteudo de um Noh no Disco.
void ArvoreMVias::deleteNode(int nodeId) {
    vector<int> empty(nodeInts(), 0);                           // Cria um vetor de zeros do tamanho de um noh.
    if (empty.size() >= 2) empty[1] = 1;                        // opcional: manter folha = 1 (para debug)
    writeNode(nodeId, empty);                                   // Escreve o noh vazio.
    cout << "[DIAG] deleteNode: limpo noh " << nodeId << endl;   // Log
} // Fim de deleteNode

// Busca recursivamente o Pai de um Noh.
int ArvoreMVias::findParent(int currentNode, int targetId, int parentId) {
    cout << "findParent: current=" << currentNode << ", target=" << targetId << ", parent=" << parentId << endl; // Log

    if (currentNode == targetId) {                              // Caso base 1: encontrou o noh target, retorna o pai.
        cout << "Encontrado! Pai eh: " << parentId << endl;
        return parentId;
    } // Fim do if
    if (currentNode == 0) return 0;                             // Caso base 2: ponteiro nulo (fim da busca).

    vector<int> vals;                                           // Leitura do noh atual.
    if (!readNode(currentNode, vals)) return 0;

    if (node_get_folha(vals)) return 0;                         // Se for folha, nao tem mais filhos para buscar.

    int n = node_get_n(vals);                                   // Itera sobre todos os filhos.
    cout << "Buscando nos " << n+1 << " filhos do noh " << currentNode << endl; // Log

    for (int i = 0; i <= n; i++) {
        int childId = node_get_filho(vals, i);                  // Obtem o ID do filho.
        cout << "Filho[" << i << "] = " << childId << endl;     // Log
        if (childId == targetId) {                              // Verifica se o filho eh o target.
            cout << "Encontrado pai direto: " << currentNode << " para filho " << targetId << endl; // Log
            return currentNode;
        } // Fim do if
        if (childId != 0) {                                     // Chamada recursiva para o filho.
            int result = findParent(childId, targetId, currentNode);
            if (result != 0) return result;
        } // Fim do if
    } // Fim do for
    return 0;                                                   // Retorna 0 se o pai nao for encontrado.
} // Fim de findParent

// Encontra a Posicao (indice) do Ponteiro para o Filho.
int ArvoreMVias::findChildIndex(int parentId, int childId) {
    cout << "findChildIndex: parent=" << parentId << ", child=" << childId << endl; // Log

    vector<int> parentVals;                                     // Leh o noh pai.
    if (!readNode(parentId, parentVals)) return -1;

    int n = node_get_n(parentVals);                             // Itera sobre todos os ponteiros do pai.
    cout << "Procurando em " << n+1 << " filhos do pai " << parentId << endl; // Log

    for (int i = 0; i <= n; i++) {
        int currentChild = node_get_filho(parentVals, i);       // Obtem o ID do filho.
        cout << "Filho[" << i << "] = " << currentChild << endl; // Log
        if (currentChild == childId) {                          // Se o ID coincidir.
            cout << "Encontrado no indice: " << i << endl;      // Log
            return i;                                           // Retorna o indice.
        } // Fim do if
    } // Fim do for
    cout << "Filho nao encontrado!" << endl;                     // Log
    return -1;                                                  // Se nao encontrar.
} // Fim de findChildIndex

// Remove Chave do Indice e Marca Dados como Deletados.
void ArvoreMVias::deleteB(int chave) {
    if (!readHeader()) {                                        // Tenta ler o cabecalho.
        cerr << "Erro ao ler header antes da delecao.\n";
        return;
    } // Fim do if

    Resultado r = mSearch(chave);                               // 1. Verifica se a chave existe (pre-condicao).
    if (!r.encontrou) {
        cout << "Aviso: Chave " << chave << " nao existe no indice. Delecao cancelada.\n";
        return;
    } // Fim do if

    cout << "=== INICIANDO DELEcAO da chave " << chave << " ===" << endl; // Logs
    cout << "Encontrada no noh " << r.indice_no << ", posicao " << r.posicao << "\n";

    deleteFromNode(raiz, chave);                                // 2. Chama a funcao recursiva de delecao no indice.

    vector<int> rootVals;                                       // 3. Pos-remocao: verifica se a raiz ficou vazia.
    if (readNode(raiz, rootVals)) {
        if (node_get_n(rootVals) == 0 && !node_get_folha(rootVals)) {
            int newRootId = node_get_filho(rootVals, 0);        // Promove o unico filho (A_0) para ser a nova raiz.
            cout << "Raiz " << raiz << " ficou vazia. Substituindo por filho " << newRootId << "\n";
            deleteNode(raiz);                                   // Limpa o noh da raiz antiga.
            raiz = newRootId;                                   // Define a nova raiz.
            writeHeader();                                      // Atualiza o cabecalho.
        } // Fim do if
    } // Fim do if

    removeDataFromFile(chave);                                  // 4. Marca o registro no arquivo de dados como deletado.
    cout << "Delecao da chave " << chave << " concluida.\n";     // Logs
    cout << "=== ESTRUTURA FINAL ===" << endl;
    print();                                                    // Imprime a estrutura final.
} // Fim de deleteB

Resultado ArvoreMVias::mSearch(int chave) {
    // leituraDisco = 0; // Zera contadores (comentado)
    // escritaDisco = 0; // Zera contadores (comentado)

    if (!readHeader()) {                                        // Tenta ler o cabecalho.
        cerr << "Erro ao abrir " << arquivoBin << " (header).\n";
        return {-1, -1, false};
    } // Fim do if

    int p = raiz;                                               // Inicia a busca na raiz.
    while (p != 0) {                                            // Loop de descida.
        vector<int> vals;                                       // Declara e leh o noh atual.
        if (!readNode(p, vals)) {
            cerr << "Erro ao ler noh " << p << " do arquivo.\n";
            return {-1, -1, false};
        } // Fim do if

        int n = node_get_n(vals);                               // Obtem n.
        int i = 0;

        while (i < n && chave > node_get_chave(vals, i))        // Encontra a posicao 'i' tal que K[i] >= chave, ou i=n.
            i++;

        if (i < n && chave == node_get_chave(vals, i)) {        // Se chave == K[i], encontrou.
            return {p, i + 1, true};                            // Retorna ID do noh e posicao (1-based).
        } // Fim do if

        int filho = node_get_filho(vals, i);                    // Se nao encontrou, desce para o filho A[i].
        if (filho == 0) {                                       // Se A[i] for nulo, chegou em uma folha.
            return {p, i + 1, false};                           // Chave nao encontrada. Retorna noh e posicao de insercao.
        } // Fim do if

        p = filho;                                              // Continua a busca no noh filho.
    } // Fim do while

    return {-1, -1, false};                                     // Retorno para arvores vazias ou erro inesperado.
} // Fim de mSearch

// METODOS DE GERADOR DE ARQUIVO BINARIO E IMPRESSOES:

// Inicializa ou Carrega a Estrutura do Indice Binario.
void ArvoreMVias::geradorBinario() {
    // Tenta ler o header; se falhar, inicializa o arquivo.
    bool ok = readHeader();
    // Se a leitura falhar (primeira execução ou incompatibilidade de M).
    if (!ok) {
        // Reinicializa o estado da árvore.
        raiz = 1;
        nextNodeId = 1;
        // Grava o novo header.
        writeHeader();
        // Cria a raiz (ID 1) como folha e a grava no disco.
        createNode(true);
        // Imprime mensagem de inicialização.
        cout << "Arquivo binario de indice inicializado com raiz vazia.\n";
    } else {
        // Imprime mensagem se o arquivo existente foi lido.
        cout << "Arquivo binario existente lido. Raiz ID=" << raiz << " Proximo ID=" << nextNodeId << "\n";
    }
} // Fim de geradorBinario

// Imprime o Conteudo de Todos os Nohs do Indice.
void ArvoreMVias::print() {
    // Reseta contadores (comentado)
    // leituraDisco = 0;
    // escritaDisco = 0;

    // Tenta ler o cabeçalho.
    if (!readHeader()) {
        // Imprime erro se falhar.
        cout << "Arquivo binario nao encontrado ou corrompido.\n";
        return;
    }
    // Imprime informações do cabeçalho da visualização.
    cout << "\n--- Conteudo da Arvore B (Indice) ---\n";
    cout << "Ordem (M): " << M << ", Grau Minimo (T): " << (M + 1) / 2 << endl;
    cout << "Raiz ID: " << raiz << endl;
    cout << "------------------------------------------------------------------\n";
    // Formato: No ID | n (chaves) | A[0] (filho) | (K[0],A[1]),...,(K[n-1],A[n])
    cout << "ID n | A[0] | (K[0],A[1]),...,(K[n-1],A[n])\n";
    cout << "------------------------------------------------------------------\n";

    // Itera sobre todos os IDs de nós que foram alocados.
    for (int id = 1; id < nextNodeId; ++id) {
        // Declara vetor para o nó.
        vector<int> vals;
        // Tenta ler o nó. Se falhar, pula (nó deletado/limpo).
        if (!readNode(id, vals)) continue;
        // Obtém o número de chaves.
        int n = node_get_n(vals);

        // Imprime ID, n e o primeiro ponteiro (A[0]).
        cout << setw(2) << id << " " << n << " | " << node_get_filho(vals, 0);
        // Itera e imprime os pares (Chave K[i], Ponteiro A[i+1]).
        for (int i = 0; i < n; ++i) {
            cout << " | (" << setw(2) << node_get_chave(vals, i) << "," << node_get_filho(vals, i + 1) << ")";
        }
        // Nova linha.
        cout << endl;
    }

    // Imprime o rodapé.
    cout << "------------------------------------------------------------------\n";
} // Fim de print

// Imprime o Indice.
void ArvoreMVias::imprimirIndice() {
    // Imprime a estrutura do índice.
    print();
    // cout << "\n--- Contadores de Acesso a Disco ---\n";
    // cout << "Numero de leituras de disco: " << leituraDisco << endl;
    // cout << "Numero de escritas de disco: " << escritaDisco << endl;
} // Fim de imprimirIndice

// Imprime o Conteudo Completo do Arquivo de Dados.
void ArvoreMVias::imprimirArquivoPrincipal() {
    // Abre o arquivo de dados para leitura.
    ifstream fin(arquivoDados, ios::binary);
    // Verifica erro.
    if (!fin) {
        cout << "Arquivo de dados vazio ou nao existe (" << arquivoDados << ").\n";
        return;
    }
    // Declara registro.
    Registro reg;
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
    }
    // Fecha o arquivo.
    fin.close();
} // Fim de imprimirArquivoPrincipal

// Imprime Registros Ativos Associados a uma Chave.
void ArvoreMVias::imprimirArquivoPrincipal(int chave) {
    // Abre o arquivo de dados para leitura.
    ifstream fin(arquivoDados, ios::binary);
    // Verifica erro.
    if (!fin) {
        cerr << "Erro: Arquivo de dados nao existe ou nao pode ser aberto.\n";
        return;
    }
    // Declara registro.
    Registro reg;
    // Flag de busca.
    bool found = false;

    // Imprime cabeçalho.
    cout << "Chave | Deletado | Dados\n";
    cout << "------------------------------------------------------------------\n";

    // Loop de leitura.
    while (fin.read(reinterpret_cast<char*>(&reg), reg.getSize())) {
        // Verifica se a chave coincide E se o registro NÃO está deletado (ativo).
        if (reg.chave == chave && !reg.deletado) {
            // Exibe o registro.
            cout << setw(5) << reg.chave
                 << " | " << setw(8) << (reg.deletado ? "TRUE" : "FALSE")
                 << " | " << reg.dados << endl;
            found = true;
        }
    }
    // Mensagem se nenhum registro ativo for encontrado.
    if (!found) cout << "Nenhum registro ativo com a chave " << chave << " encontrado no arquivo principal.\n";
    // Fecha o arquivo.
    fin.close();
} // Fim de imprimirArquivoPrincipal
// ====================================================================================================================