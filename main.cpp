// Integrantes do grupo:
// Caio Monteiro Sartori   N° 15444598
// Mateus Henrique Carriel   N° 15698362
// Murilo Augusto Jorge   N° 15552251

// Arquivo main do trabalho, rodando o mSearch

#include <iostream>
#include "ArvoreMVias.h"

using namespace std;

int main() {
    ArvoreMVias arvore("mvias1-1.txt", "mvias.bin");   // Árvore a esquerda do item D do trabalho, no primeiro caso
    // ArvoreMVias arvore("mvias1-2.txt", "mvias.bin");   // Árvore a direita do item D do trabalho, no primeiro caso
    // ArvoreMVias arvore("mvias2-1.txt", "mvias.bin");   // Árvore a esquerda do item D do trabalho, no segundo caso
    // ArvoreMVias arvore("mvias2-2.txt", "mvias.bin");   // Árvore a direita do item D do trabalho, no segundo caso

    arvore.geradorBinario();

    cout << "Indice mvias.bin aberto" << endl;

    char cont = 's'; // Váriavel de continuação das buscas

    while (cont == 's' || cont == 'S') {
        arvore.print();

        cout << "Chave de busca: ";
        int chave;
        cin >> chave;

        Resultado res = arvore.mSearch(chave);
        cout << " " << chave << " (" << res.indice_no << "," << res.posicao << ","
             << (res.encontrou ? "true" : "false") << ")" << endl;   // ternário para facilitar a impressão de true ou false

        cout << "Continuar busca (s/n)? ";
        cin >> cont;
    }

    return 0;
}
