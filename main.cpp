#include <iostream>
#include <fstream>
#include <string>
using namespace std;

const int M = 3; // ordem da árvore (máximo 2 chaves por nó)

class Node {
public:
    int n;                 // número de chaves
    int keys[M - 1];       // até 2 chaves
    int children[M];       // até 3 filhos

    Node() {
        n = 0;
        for (int i = 0; i < M - 1; i++) keys[i] = -1;
        for (int i = 0; i < M; i++) children[i] = -1;
    }
};

// ---- Parte (a): Converte txt → bin ----
void txtToBin(const string &txtFile, const string &binFile) {
    ifstream fin(txtFile);
    ofstream fout(binFile, ios::binary);

    while (true) {
        Node node;
        if (!(fin >> node.n)) break;

        for (int i = 0; i < M; i++) {
            if (i < node.n) {
                fin >> node.children[i] >> node.keys[i];
            } else if (i == node.n) {
                fin >> node.children[i];
            } else {
                node.children[i] = -1;
            }
        }
        fout.write((char*)&node, sizeof(Node));
    }
    fin.close();
    fout.close();
}

// ---- Parte (b): Algoritmo de busca ----
bool mSearch(const string &binFile, int rootPos, int key) {
    ifstream fin(binFile, ios::binary);
    if (!fin) return false;

    int pos = rootPos;
    Node node;

    while (pos != -1) {
        fin.seekg(pos * sizeof(Node));
        fin.read((char*)&node, sizeof(Node));

        int i = 0;
        while (i < node.n && key > node.keys[i]) i++;

        if (i < node.n && key == node.keys[i]) {
            return true; // chave encontrada
        }
        pos = node.children[i]; // desce para o filho
    }
    return false; // não achou
}

// ---- Parte (c): Teste ----
int main() {
    txtToBin("mvias.txt", "mvias.bin");

    cout << "Digite a chave para buscar: ";
    int key;
    cin >> key;

    if (mSearch("mvias.bin", 0, key))
        cout << "Chave encontrada!\n";
    else
        cout << "Chave nao encontrada.\n";

    return 0;
}
