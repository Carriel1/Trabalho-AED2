// Integrantes do grupo:
// Caio Monteiro Sartori   N° 15444598
// Mateus Henrique Carriel   N° 15698362
// Murilo Augusto Jorge   N° 15552251

// Arquivo main do trabalho, rodando o mSearch

#include <iostream>
#include "ArvoreMVias.h"

using namespace std;

int main() {
    ArvoreMVias arvore("mvias.txt", "mvias.bin");
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
             << (res.encontrou ? "true" : "false") << ")" << endl;

        cout << "Continuar busca (s/n)? ";
        cin >> cont;
    }

    return 0;
}
