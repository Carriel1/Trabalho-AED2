#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <iomanip>
using namespace std;

const int M = 3; // ordem da árvore

// ============================
// Estrutura de nó da árvore
// ============================
class Node {
public:
    int n;                // número de chaves no nó
    int keys[M - 1];      // até 2 chaves
    int children[M];      // até 3 ponteiros (índices de nós em disco)

    Node() {
        n = 0;
        for (int i = 0; i < M - 1; i++) keys[i] = -1;
        for (int i = 0; i < M; i++) children[i] = -1;
    }
};

// ============================
// (a) Converte txt → binário
// ============================
void txtToBin(const string &txtFile, const string &binFile) {
    ifstream fin(txtFile);
    ofstream fout(binFile, ios::binary);

    if (!fin) {
        cerr << "Erro ao abrir " << txtFile << endl;
        return;
    }

    int nodeIndex = 1;
    while (true) {
        Node node;
        if (!(fin >> node.n)) break;

        // lê A0
        fin >> node.children[0];

        // lê pares (Ki, Ai)
        for (int i = 0; i < node.n; i++) {
            fin >> node.keys[i] >> node.children[i + 1];
        }

        fout.write((char*)&node, sizeof(Node));
        nodeIndex++;
    }

    fin.close();
    fout.close();
    cout << "Lendo dados de " << txtFile << " e criando " << binFile << endl;
}

// ============================
// Mostra a árvore em disco
// ============================
void printTree(const string &binFile) {
    ifstream fin(binFile, ios::binary);
    if (!fin) {
        cerr << "Erro ao abrir " << binFile << endl;
        return;
    }

    cout << "T = 1, m = " << M << endl;
    cout << "------------------------------------------------------------------\n";
    cout << "No  n,A[0],(K[1],A[1]),...,(K[n],A[n])\n";
    cout << "------------------------------------------------------------------\n";

    Node node;
    int index = 1;
    while (fin.read((char*)&node, sizeof(Node))) {
        cout << setw(2) << index << " " << node.n << ", " << node.children[0];
        for (int i = 0; i < node.n; i++) {
            cout << ",(" << setw(2) << node.keys[i] << "," << node.children[i + 1] << ")";
        }
        cout << endl;
        index++;
    }

    cout << "------------------------------------------------------------------\n";
    fin.close();
}

// ============================
// (b) Algoritmo mSearch
// Retorna (no, pos, found)
// ============================
struct SearchResult {
    int nodeIndex;   // índice do nó no arquivo (1-based)
    int pos;         // posição dentro do nó
    bool found;      // true se encontrou
};

SearchResult mSearch(const string &binFile, int key) {
    ifstream fin(binFile, ios::binary);
    if (!fin) {
        cerr << "Erro ao abrir " << binFile << endl;
        return {-1, -1, false};
    }

    int posNode = 0; // começa na raiz (posição 0 em disco)
    Node node;
    int index = 1;   // índice lógico dos nós (1-based)

    while (true) {
        fin.seekg(posNode * sizeof(Node));
        if (!fin.read((char*)&node, sizeof(Node))) break;

        int i = 0;
        while (i < node.n && key > node.keys[i]) i++;

        if (i < node.n && key == node.keys[i]) {
            return {index, i+1, true}; // chave encontrada
        }

        if (node.children[i] == 0) { // chegou em folha
            return {index, i, false};
        }

        posNode = node.children[i] - 1; // nó filho (ajuste: 1-based -> 0-based)
        index = node.children[i];
    }

    return {-1, -1, false};
}

// ============================
// (c) Programa Principal
// ============================
int main() {
    txtToBin("mvias.txt", "mvias.bin");

    cout << "Indice mvias.bin aberto" << endl;

    char cont = 's';
    while (cont == 's' || cont == 'S') {
        printTree("mvias.bin");

        cout << "Chave de busca: ";
        int key;
        cin >> key;

        SearchResult res = mSearch("mvias.bin", key);

        cout << " " << key << " (" << res.nodeIndex << "," << res.pos << ","
             << (res.found ? "true" : "false") << ")" << endl;

        cout << "Continuar busca (s/n)? ";
        cin >> cont;
    }

    return 0;
}
