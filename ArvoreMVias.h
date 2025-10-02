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
    int indice_no;
    int posicao;
    bool encontrou;
};

class ArvoreMVias {
private:
    string arquivoTxt;
    string arquivoBin;
    string arquivoDados;
    int M;
    int raiz;

    int leituraDisco = 0;   // NOVO: contador de leituras
    int escritaDisco = 0;   // NOVO: contador de escritas

    struct No {
        int n;
        vector<int> chaves;
        vector<int> filhos;
    };

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

