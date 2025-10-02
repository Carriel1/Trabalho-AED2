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

// Resultado da busca
struct Resultado {
    int indice_no;  // índice do nó no arquivo
    int posicao;     // posição dentro do nó
    bool encontrou;  // true se encontrou
};

// Classe da árvore M-vias (árvore B parametrizada)
class ArvoreMVias {
private:
    string arquivoTxt;
    string arquivoBin;
    string arquivoDados; // arquivo principal com dados
    int M; // ordem da árvore

    struct No {
        int n;                  // número de chaves no nó
        vector<int> chaves;    // até M-1 chaves
        vector<int> filhos;    // até M filhos
    };

    int raiz; // índice do nó raiz

    void split(int noAtual, int chave, int filho);
    void inserirRec(int noAtual, int chave, int filho);

public:
    ArvoreMVias(const string& txt, const string& bin, const string& dados, int ordem);
    void geradorBinario();
    void print();
    Resultado mSearch(int chave);
    void insertB(int chave, const string& dadosElemento);
    void imprimirIndice();
    void imprimirArquivoPrincipal();
};

#endif
