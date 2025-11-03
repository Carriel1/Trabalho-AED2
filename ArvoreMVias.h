// Integrantes do grupo:
// Caio Monteiro Sartori   N° 15444598
// Mateus Henrique Carriel   N° 15698362
// Murilo Augusto Jorge   N° 15552251

// Arquivo .h de definição da classe árvore de M-vias

#ifndef ARVOREMVIAS_H
#define ARVOREMVIAS_H

#include <iostream>
#include <fstream>
#include <iomanip>
#include <string>
#include <vector>
#include <cstring>

using namespace std;

const int MAX_DATA_SIZE = 120; // Tamanho máximo dos dados (excluindo a chave)

struct Registro {
    int chave;
    char dados[MAX_DATA_SIZE];
    bool deletado;

    Registro() {
        chave = 0;
        deletado = false;
        memset(dados, 0, MAX_DATA_SIZE);
    }

    static size_t getSize() {
        return sizeof(int) + MAX_DATA_SIZE * sizeof(char) + sizeof(bool);
    }
};

struct Resultado {
    int indice_no; // id do nó onde a busca terminou (ou onde deveria estar)
    int posicao;   // posição dentro do nó (1-based) ou posição de inserção
    bool encontrou;
};

class ArvoreMVias {
private:
    string arquivoTxt;
    string arquivoBin;
    string arquivoDados;
    int M;      // ordem (número máximo de filhos)
    int T;      // grau mínimo
    int raiz;   // id do nó raiz (1-based)
    int nextNodeId; // próximo id disponível (1-based)

    // helper: número de inteiros por nó no arquivo binário = 2 + (M-1) + M
    int nodeInts() const;

    // leitura/escrita de header (M, raiz, nextNodeId)
    void writeHeader();
    bool readHeader();

    // leitura/escrita de nós serializados como inteiros
    void writeNode(int id, const vector<int>& vals);
    bool readNode(int id, vector<int>& vals);

    // helpers para manipular o nó em vetor<int>:
    int node_get_n(const vector<int>& vals) const;
    void node_set_n(vector<int>& vals, int n);
    bool node_get_folha(const vector<int>& vals) const;
    void node_set_folha(vector<int>& vals, bool folha);
    int node_get_chave(const vector<int>& vals, int idx) const;
    void node_set_chave(vector<int>& vals, int idx, int chave);
    int node_get_filho(const vector<int>& vals, int idx) const;
    void node_set_filho(vector<int>& vals, int idx, int filho);

    // criação de nó novo
    int createNode(bool folha);

    // operações clássicas de B-tree em disco
    void splitChild(int parentId, int childIndex, int childId);
    void insertNonFull(int nodeId, int chave);

    // remoção de dados
    void removeDataFromFile(int chave);
    void markAsDeleted(int chave);

    // remoção de chave do índice
    void removeKeyAndPointer(vector<int>& vals, int idx);
    void deleteFromNode(int nodeId, int chave);
    void removeFromLeaf(vector<int>& vals, int idx);
    void removeFromInternalNode(int nodeId, int idx);
    void fillChild(int parentId, int idx);
    void mergeNodes(int parentId, int idx);
    int getPredecessor(int nodeId);
    int getSuccessor(int nodeId);
    void borrowFromLeft(int parentId, int childIndex);
    void borrowFromRight(int parentId, int childIndex);
    void deleteNode(int nodeId);
    int findParent(int currentNode, int targetId, int parentId);
    int findChildIndex(int parentId, int childId);

public:
    ArvoreMVias(const string& txt, const string& bin, const string& dados, int ordem);
    void geradorBinario();
    void print();
    Resultado mSearch(int chave);
    void insertB(int chave, const string& dadosElemento);
    void deleteB(int chave);
    void imprimirIndice();
    void imprimirArquivoPrincipal();
    void imprimirArquivoPrincipal(int chave);
};

#endif
