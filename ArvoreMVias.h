// Integrantes do grupo:
// Caio Monteiro Sartori   N° 15444598
// Mateus Henrique Carriel   N° 15698362
// Murilo Augusto Jorge   N° 15552251

// Arquivo .h de definiçãoo da classe árvore de M-vias

#ifndef ARVOREMVIAS_H
#define ARVOREMVIAS_H

#include <iostream>
#include <fstream>
#include <iomanip>
#include <string>
#include <vector>

using namespace std;

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
    int M;      // ordem (numero máximo de filhos)
    int raiz;   // id do nó raiz (1-based)
    int nextNodeId; // próximo id disponível (1-based)

    // contadores de I/O para inserção e busca
    int leituraDisco;
    int escritaDisco;

    // helper: numero de inteiros por nó no arquivo binario = 2 + (M-1) + M
    int nodeInts() const;

    // leitura/escrita de header (M, raiz, nextNodeId)
    void writeHeader();
    bool readHeader();

    // leitura/escrita de nós serializados como inteiros
    void writeNode(int id, const vector<int>& vals); // vals size = nodeInts()
    bool readNode(int id, vector<int>& vals);

    // helpers para manipular o nó em vetor<int>:
    int node_get_n(const vector<int>& vals) const;
    void node_set_n(vector<int>& vals, int n);
    bool node_get_folha(const vector<int>& vals) const;
    void node_set_folha(vector<int>& vals, bool folha);
    int node_get_chave(const vector<int>& vals, int idx) const; // idx: 0..M-2
    void node_set_chave(vector<int>& vals, int idx, int chave);
    int node_get_filho(const vector<int>& vals, int idx) const; // idx: 0..M-1
    void node_set_filho(vector<int>& vals, int idx, int filho);

    // criação de nó novo
    int createNode(bool folha);

    // operações clássicas de B-tree em disco
    void splitChild(int parentId, int childIndex /* 0-based index in parent */, int childId);
    void insertNonFull(int nodeId, int chave);

public:
    ArvoreMVias(const string& txt, const string& bin, const string& dados, int ordem);
    void geradorBinario(); // inicializa o arquivo binario (header + raiz vazia)
    void print(); // imprime todos os nós contidos no binario (em ordem de id)
    Resultado mSearch(int chave);
    void insertB(int chave, const string& dadosElemento);
    void imprimirIndice();
    void imprimirArquivoPrincipal(); // imprime tudo
    void imprimirArquivoPrincipal(int chave); // imprime apenas linhas com a chave (se encontrada)
};

#endif
