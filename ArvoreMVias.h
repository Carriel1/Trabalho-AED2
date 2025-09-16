// Integrantes do grupo:
// Caio Monteiro Sartori   N° 15444598
// Mateus Henrique Carriel   N° 15698362
// Murilo Augusto Jorge   N° 15552251

// Arquivo .h de definição da classe Árvore de M-vias

#ifndef ARVOREMVIAS_H
#define ARVOREMVIAS_H

#include <iostream>
#include <fstream>
#include <iomanip>
#include <string>

using namespace std;

const int M = 3; // Ordem da Árvore M-vias

// Resultado da busca
struct Resultado {
    int indice_no;  // índice do nó no arquivo
    int posicao;        // posição dentro do nó
    bool encontrou;     // true se encontrou
};

// Classe da Árvore M-vias
class ArvoreMVias {
private:
    string arquivoTxt;
    string arquivoBin;

    struct No {
        int n;                // número de chaves no nó
        int chaves[M - 1];      // até M-1 chaves
        int filhos[M];      // até M filhos (índices de nós em disco)
    };

public:
    ArvoreMVias(const string& txt, const string& bin); // construtor da classe
    void geradorBinario();        // lê txt e cria binário
    void print();              // imprime a árvore
    Resultado mSearch(int chave); // busca chave
};

#endif
