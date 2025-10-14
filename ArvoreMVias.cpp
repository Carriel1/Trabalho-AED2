// Integrantes do grupo:
// Caio Monteiro Sartori   N° 15444598
// Mateus Henrique Carriel   N° 15698362
// Murilo Augusto Jorge   N° 15552251

// Arquivo .cpp de implementação da classe árvore M-vias

#include "ArvoreMVias.h"
#include <cstring>
#include <sstream>
#include <cmath>
#include <iostream>
#include <iomanip>

ArvoreMVias::ArvoreMVias(const string& txt, const string& bin, const string& dados, int ordem) {
    arquivoTxt = txt;
    arquivoBin = bin;
    arquivoDados = dados;
    M = ordem;
    raiz = 1;
    nextNodeId = 1;
    leituraDisco = 0;
    escritaDisco = 0;
}

// [n (1 int), folha (1 int), chaves (M-1 ints), filhos (M ints)]
int ArvoreMVias::nodeInts() const {
    return 2 + (M - 1) + M; // 2 * M + 1
}

void ArvoreMVias::writeHeader() {
    // Abre o arquivo binario, criando-o se nao existir
    fstream fout(arquivoBin, ios::binary | ios::in | ios::out);
    if (!fout) {
        ofstream cr(arquivoBin, ios::binary);
        cr.close();
        fout.open(arquivoBin, ios::binary | ios::in | ios::out);
    }
    // Grava M, raiz e nextNodeId no inicio
    fout.seekp(0);
    int aM = M, aRaiz = raiz, aNext = nextNodeId;
    fout.write((char*)&aM, sizeof(int));
    fout.write((char*)&aRaiz, sizeof(int));
    fout.write((char*)&aNext, sizeof(int));
    fout.close();
}

bool ArvoreMVias::readHeader() {
    ifstream fin(arquivoBin, ios::binary);
    if (!fin) return false;
    fin.seekg(0);
    int aM, aRaiz, aNext;
    if (!fin.read((char*)&aM, sizeof(int))) { fin.close(); return false; }
    if (!fin.read((char*)&aRaiz, sizeof(int))) { fin.close(); return false; }
    if (!fin.read((char*)&aNext, sizeof(int))) { fin.close(); return false; }

    // Alerta/Reset se a ordem no arquivo for diferente da ordem atual
    if (aM != M) {
        cerr << "Aviso: Ordem (M=" << aM << ") do indice e diferente da ordem atual (M=" << M << "). Reiniciando indice E arquivo de dados.\n";
        fin.close();

        // Define as variaveis de estado para 1, para que geradorBinario as use
        raiz = 1;
        nextNodeId = 1;

        ofstream foutDados(arquivoDados, ios::trunc);
        if (!foutDados) {
            cerr << "Erro fatal: Nao foi possivel reiniciar o arquivo de dados.\n";
            return false;
        }
        foutDados.close();

        return false;
    }
    raiz = aRaiz;
    nextNodeId = aNext;
    fin.close();
    return true;
}

// Simula a escrita de um bloco no disco (incrementa escritaDisco)
void ArvoreMVias::writeNode(int id, const vector<int>& vals) {
    fstream fout(arquivoBin, ios::in | ios::out | ios::binary);
    if (!fout) { cerr << "Erro de arquivo (writeNode).\n"; return; }

    int headerBytes = 3 * sizeof(int);
    int ints = nodeInts();
    int nodeBytes = ints * sizeof(int);
    // Calcula a posicao no arquivo: Header + (ID - 1) * Tamanho do No
    streampos pos = static_cast<std::streamoff>(headerBytes) + static_cast<std::streamoff>((id - 1) * nodeBytes);
    fout.seekp(pos);
    for (int i = 0; i < ints; ++i) {
        int v = vals[i];
        fout.write((char*)&v, sizeof(int));
    }
    fout.close();
    escritaDisco++; // Incrementa o contador de acesso a disco (escrita)
}

// Simula a leitura de um bloco no disco (incrementa leituraDisco)
bool ArvoreMVias::readNode(int id, vector<int>& vals) {
    ifstream fin(arquivoBin, ios::binary);
    if (!fin) return false;
    int headerBytes = 3 * sizeof(int);
    int ints = nodeInts();
    int nodeBytes = ints * sizeof(int);
    // Calcula a posicao no arquivo
    streampos pos = static_cast<std::streamoff>(headerBytes) + static_cast<std::streamoff>((id - 1) * nodeBytes);

    // Verifica se o no existe (evita leitura fora do limite)
    fin.seekg(0, ios::end);
    streampos fileSize = fin.tellg();
    if (pos + static_cast<std::streamoff>(nodeBytes) > fileSize) {
        fin.close();
        return false;
    }

    fin.seekg(pos);
    vals.assign(ints, 0);
    for (int i = 0; i < ints; ++i) {
        int v = 0;
        fin.read((char*)&v, sizeof(int));
        vals[i] = v;
    }
    fin.close();
    leituraDisco++; // Incrementa o contador de acesso a disco (leitura)
    return true;
}

// Getters/Setters do no serializado
int ArvoreMVias::node_get_n(const vector<int>& vals) const { return vals[0]; }
void ArvoreMVias::node_set_n(vector<int>& vals, int n) { vals[0] = n; }
bool ArvoreMVias::node_get_folha(const vector<int>& vals) const { return vals[1] != 0; }
void ArvoreMVias::node_set_folha(vector<int>& vals, bool folha) { vals[1] = folha ? 1 : 0; }
int ArvoreMVias::node_get_chave(const vector<int>& vals, int idx) const { return vals[2 + idx]; }
void ArvoreMVias::node_set_chave(vector<int>& vals, int idx, int chave) { vals[2 + idx] = chave; }
int ArvoreMVias::node_get_filho(const vector<int>& vals, int idx) const { return vals[2 + (M - 1) + idx]; }
void ArvoreMVias::node_set_filho(vector<int>& vals, int idx, int filho) { vals[2 + (M - 1) + idx] = filho; }

int ArvoreMVias::createNode(bool folha) {
    int id = nextNodeId++;
    int ints = nodeInts();
    vector<int> vals(ints, 0);
    node_set_n(vals, 0);
    node_set_folha(vals, folha);
    // Inicializa filhos e chaves com 0
    for (int i = 0; i < M; ++i) node_set_filho(vals, i, 0);
    for (int i = 0; i < M - 1; ++i) node_set_chave(vals, i, 0);
    writeNode(id, vals);
    writeHeader(); // Atualiza nextNodeId no header
    return id;
}

void ArvoreMVias::geradorBinario() {
    // Tenta ler o header; se falhar, inicializa o arquivo
    bool ok = readHeader();
    if (!ok) {
        raiz = 1;
        nextNodeId = 1;
        writeHeader();
        createNode(true); // Cria raiz (id 1) como folha
        cout << "Arquivo binario de indice inicializado com raiz vazia.\n";
    } else {
        cout << "Arquivo binario existente lido. Raiz ID=" << raiz << " Proximo ID=" << nextNodeId << "\n";
    }
}

void ArvoreMVias::print() {
    // Reseta contadores antes da impressao para isolar a operacao
    leituraDisco = 0;
    escritaDisco = 0;

    if (!readHeader()) {
        cout << "Arquivo binario nao encontrado ou corrompido.\n";
        return;
    }
    cout << "\n--- Conteudo da Arvore B (Indice) ---\n";
    cout << "Ordem (M): " << M << ", Grau Minimo (T): " << (M + 1) / 2 << endl;
    cout << "Raiz ID: " << raiz << endl;
    cout << "------------------------------------------------------------------\n";
    // Formato: No ID | n (chaves) | A[0] (filho) | (K[0],A[1]),...,(K[n-1],A[n])
    cout << "ID n | A[0] | (K[0],A[1]),...,(K[n-1],A[n])\n";
    cout << "------------------------------------------------------------------\n";

    for (int id = 1; id < nextNodeId; ++id) {
        vector<int> vals;
        if (!readNode(id, vals)) continue;
        int n = node_get_n(vals);

        cout << setw(2) << id << " " << n << " | " << node_get_filho(vals, 0);
        for (int i = 0; i < n; ++i) {
            cout << " | (" << setw(2) << node_get_chave(vals, i) << "," << node_get_filho(vals, i + 1) << ")";
        }
        cout << endl;
    }

    cout << "------------------------------------------------------------------\n";
}

// Requisito C: mSearch - Retorna o triplo (ID No, Posicao, Encontrado)
Resultado ArvoreMVias::mSearch(int chave) {
    // Reseta contadores antes da busca para isolar a operacao
    leituraDisco = 0;
    escritaDisco = 0;

    if (!readHeader()) {
        cerr << "Erro ao abrir " << arquivoBin << " (header).\n";
        return {-1, -1, false};
    }

    int p = raiz;
    int last_q = 0; // Se a raiz for vazia, o ultimo no 'visitado' sera 0
    while (p != 0) {
        vector<int> vals;
        if (!readNode(p, vals)) {
            cerr << "Erro ao ler no " << p << " do arquivo.\n";
            return {-1, -1, false};
        }
        int n = node_get_n(vals);
        int i = 0;

        // Encontra a posicao da chave no no
        while (i < n && chave > node_get_chave(vals, i)) i++;

        if (i < n && chave == node_get_chave(vals, i)) {
            // Chave encontrada! Retorna o ID do no e a posicao (1-based)
            return {p, i + 1, true};
        } else {
            // Desce para o filho A[i]
            last_q = p;
            p = node_get_filho(vals, i);
        }
    }

    // Chave nao encontrada. Retorna o ultimo no folha visitado e posicao 0
    return {last_q, 0, false};
}

void ArvoreMVias::splitChild(int parentId, int childIndex, int childId) {
    // childIndex: o indice do ponteiro (0-based) no parent que aponta para childId
    vector<int> parentVals, childVals;
    readNode(parentId, parentVals);
    readNode(childId, childVals);

    int t = (M + 1) / 2; // Grau minimo

    // Cria novo no z (o irmao direito)
    int zId = createNode(node_get_folha(childVals));
    vector<int> zVals;
    readNode(zId, zVals);

    // Total de chaves: M-1. Chave mediana: K[t-1]. Chaves a mover para Z: K[t] a K[M-2].
    // Numero de chaves a mover para Z: M - t - 1
    int numKeysToMove = (M - 1) - t;

    node_set_n(zVals, numKeysToMove);

    // Move chaves de y[t..M-2] para z[0..numKeysToMove-1]
    for (int j = 0; j < numKeysToMove; ++j) {
        int key = node_get_chave(childVals, j + t); // Comeca em t, que e o indice da primeira chave a mover
        node_set_chave(zVals, j, key);
    }

    // Move filhos de y[t..M-1] para z[0..numKeysToMove] se nao for folha
    if (!node_get_folha(childVals)) {
        // Z recebe (numKeysToMove + 1) filhos
        for (int j = 0; j <= numKeysToMove; ++j) {
            int f = node_get_filho(childVals, j + t);
            node_set_filho(zVals, j, f);
        }
    }

    // Mediana que sobe (chave em childVals na posicao t-1)
    int med = node_get_chave(childVals, t - 1);

    // Reduz y.n para t-1 (chaves que ficam)
    node_set_n(childVals, t - 1);

    // Zero chaves de t-1 (promovida) ate M-2
    for (int j = t - 1; j < M - 1; ++j) {
        node_set_chave(childVals, j, 0);
    }
    // Zero filhos de t (primeiro filho de Z) ate M-1
    if (!node_get_folha(childVals)) {
        for (int j = t; j < M; ++j) {
            node_set_filho(childVals, j, 0);
        }
    }

    // Insere novo filho zId em parent em posicao childIndex+1 e chave mediana
    int parent_n = node_get_n(parentVals);

    // Shift filhos e chaves no parent para a direita
    for (int j = parent_n; j >= childIndex + 1; --j) {
        node_set_filho(parentVals, j + 1, node_get_filho(parentVals, j));
    }
    node_set_filho(parentVals, childIndex + 1, zId); // Aponta para o novo no Z

    for (int j = parent_n - 1; j >= childIndex; --j) {
        node_set_chave(parentVals, j + 1, node_get_chave(parentVals, j));
    }


    node_set_chave(parentVals, childIndex, med);
    node_set_n(parentVals, parent_n + 1);

    // write back child (y), z, parent
    writeNode(childId, childVals);
    writeNode(zId, zVals);
    writeNode(parentId, parentVals);
    cout << "Split do No " << childId << " realizado. Chave mediana " << med << " promovida ao No " << parentId << endl;
}

void ArvoreMVias::insertNonFull(int nodeId, int chave) {
    vector<int> nodeVals;
    readNode(nodeId, nodeVals);
    int n = node_get_n(nodeVals);

    if (node_get_folha(nodeVals)) {
        // Inserir chave no local correto
        int i = n - 1;

        // Shift chaves maiores que a nova chave
        while (i >= 0 && node_get_chave(nodeVals, i) > chave) {
            node_set_chave(nodeVals, i + 1, node_get_chave(nodeVals, i));
            i--;
        }
        node_set_chave(nodeVals, i + 1, chave);
        node_set_n(nodeVals, n + 1);
        writeNode(nodeId, nodeVals);
    } else {
        // Encontra o filho para descer
        int i = n - 1;
        while (i >= 0 && node_get_chave(nodeVals, i) > chave) i--;
        int childIndex = i + 1;
        int childId = node_get_filho(nodeVals, childIndex);

        if (childId == 0) {
            cerr << "Erro: Ponteiro para filho nulo encontrado durante a insercao (ID No: " << nodeId << ", Indice: " << childIndex << ").\n";
            return;
        }

        vector<int> childVals;
        // Não é necessário ler childVals aqui, apenas verificar o tamanho
        if (readNode(childId, childVals) && node_get_n(childVals) == M - 1) {
            // Se cheio, split antes de descer
            splitChild(nodeId, childIndex, childId);

            // Re-lê o pai, pois a chave mediana subiu e o no foi modificado
            readNode(nodeId, nodeVals);

            // Verifica qual dos 2 novos filhos deve receber a chave
            if (node_get_chave(nodeVals, childIndex) < chave) {
                childIndex = childIndex + 1;
            }
        }
        int nextChild = node_get_filho(nodeVals, childIndex);
        insertNonFull(nextChild, chave);
    }
}

// Requisito D: InsertB
void ArvoreMVias::insertB(int chave, const string& dadosElemento) {
    // Zera contadores para contar apenas a insercao
    leituraDisco = 0;
    escritaDisco = 0;

    // 1. Verifica se a chave ja existe no indice (para evitar duplicidade no indice)
    // O mSearch e chamado com contadores resetados
    Resultado r = mSearch(chave);
    if (r.encontrou) {
        cout << "Aviso: Chave " << chave << " ja existe no indice. Insercao cancelada.\n";
        return;
    }

    // 2. Escreve no arquivo principal
    ofstream foutDados(arquivoDados, ios::app);
    if (!foutDados) {
        cerr << "Erro ao abrir " << arquivoDados << " para escrita.\n";
        return;
    }
    foutDados << chave << " " << dadosElemento << endl;
    foutDados.close();
    escritaDisco++; // Conta a escrita no arquivo de dados principal

    // 3. Insere na arvore B (arquivo bin)
    if (!readHeader()) {
        cerr << "Erro ao ler header antes de inserir.\n";
        return;
    }

    vector<int> rootVals;
    readNode(raiz, rootVals);

    // Verifica se a raiz esta cheia
    if (node_get_n(rootVals) == M - 1) {
        // Raiz cheia -> criar nova raiz
        int s = createNode(false);
        // set s.filho[0] = old root
        vector<int> sVals;
        readNode(s, sVals);
        node_set_filho(sVals, 0, raiz);
        writeNode(s, sVals);
        int oldRootId = raiz;
        raiz = s;
        writeHeader(); // Salva a nova raiz

        // Split da raiz antiga (que agora e o filho 0 da nova raiz)
        splitChild(s, 0, oldRootId);

        // Continua a insercao na nova estrutura
        insertNonFull(raiz, chave);
    } else {
        insertNonFull(raiz, chave);
    }

    cout << "Insercao da chave " << chave << " realizada com sucesso no indice e no arquivo principal.\n";
}

// Requisito A: Imprime o indice (e contadores)
void ArvoreMVias::imprimirIndice() {
    print(); // Imprime o indice
    cout << "\n--- Contadores de Acesso a Disco ---\n";
    cout << "Numero de leituras de disco: " << leituraDisco << endl;
    cout << "Numero de escritas de disco: " << escritaDisco << endl;
}

// Requisito B: Imprime todo o arquivo principal
void ArvoreMVias::imprimirArquivoPrincipal() {
    ifstream fin(arquivoDados);
    if (!fin) {
        cout << "Arquivo de dados vazio ou nao existe (" << arquivoDados << ").\n";
        return;
    }
    string linha;
    cout << "\n--- Conteudo COMPLETO do Arquivo Principal (" << arquivoDados << ") ---\n";
    while (getline(fin, linha)) {
        cout << linha << endl;
    }
    fin.close();
}

// Requisito C: Imprime o conteudo do arquivo principal associado a chave
void ArvoreMVias::imprimirArquivoPrincipal(int chave) {
    ifstream fin(arquivoDados);
    if (!fin) {
        cerr << "Erro: Arquivo de dados nao existe ou nao pode ser aberto.\n";
        return;
    }
    string linha;
    bool found = false;

    // Le o arquivo linha por linha.
    while (getline(fin, linha)) {
        // Tenta extrair a primeira palavra (que e a chave) como inteiro
        stringstream ss(linha);
        int k;
        if (ss >> k) {
            if (k == chave) {
                cout << linha << endl;
                found = true;
            }
        }
    }
    if (!found) cout << "Nenhum registro com a chave " << chave << " encontrado no arquivo principal (Busca Lenta).\n";
    fin.close();
}
