// Integrantes do grupo:
// Caio Monteiro Sartori   N° 15444598
// Mateus Henrique Carriel   N° 15698362
// Murilo Augusto Jorge   N° 15552251

// Arquivo .cpp de implementação da classe Árvore M-vias

#include "ArvoreMVias.h"

// Construtor da classe
ArvoreMVias::ArvoreMVias(const string& txt, const string& bin) {
    arquivoTxt = txt;
    arquivoBin = bin;
}

// Gera arquivo binário a partir do txt
void ArvoreMVias::geradorBinario() {
    ifstream fin(arquivoTxt);   // Arquivo de entrada txt
    ofstream fout(arquivoBin, ios::binary);   // Arquivo de saída bin

    if (!fin) {
        cout << "Erro ao abrir " << arquivoTxt << endl;
        return;
    }

    while(true) {
        No no;

        // Inicializa manualmente os campos para evitar lixo de memória
        no.n = 0;
        for (int i = 0; i < M - 1; i++) {
            no.chaves[i] = -1;
        }
        for (int i = 0; i < M; i++) {
            no.filhos[i] = 0;
        }

        if (!(fin >> no.n)) {   // Encerra processo
            break;
        }

        // Lê A0
        fin >> no.filhos[0];

        // Lê pares (Ki, Ai)
        for (int i = 0; i < no.n; i++) {
            fin >> no.chaves[i] >> no.filhos[i + 1];
        }

        fout.write((char*)&no, sizeof(No));
    }

    fin.close();
    fout.close();
    cout << "Lendo dados de " << arquivoTxt << " e criando " << arquivoBin << endl;
}

// Imprime a árvore
void ArvoreMVias::print() {
    ifstream fin(arquivoBin, ios::binary);

    if (!fin) {
        cout << "Erro ao abrir " << arquivoBin << endl;
        return;
    }

    cout << "T = 1, m = " << M << endl;
    cout << "------------------------------------------------------------------\n";
    cout << "No  n,A[0],(K[1],A[1]),...,(K[n],A[n])\n";
    cout << "------------------------------------------------------------------\n";

    No no;
    int _indice = 1;

    while (fin.read((char*)&no, sizeof(No))) {
        cout << setw(2) << _indice << " " << no.n << ", " << no.filhos[0];

        for (int i = 0; i < no.n; i++) {
            cout << ",(" << setw(2) << no.chaves[i] << "," << no.filhos[i + 1] << ")";
        }

        cout << endl;
        _indice++;
    }

    cout << "------------------------------------------------------------------\n";
    fin.close();
}

// Busca mSearch
Resultado ArvoreMVias::mSearch(int chave) {
    ifstream fin(arquivoBin, ios::binary);

    if (!fin) {
        cerr << "Erro ao abrir " << arquivoBin << endl;
        return {-1, -1, false};
    }

    int p = 1;  // raiz é nó 1
    int q = 0;  // pai de p
    No no;
    int i = 0;

    while (p != 0) {
        fin.seekg((p - 1) * sizeof(No));

        if (!fin.read((char*)&no, sizeof(No))) {   // Caso de erro no processo
            cout << "Erro ao ler nó " << p << " do arquivo." << endl;
            return {-1, -1, false};
        }

        // Inicializa campos que não foram lidos
        for (int j = no.n; j < M - 1; j++) {
            no.chaves[j] = -1;
        }
        for (int j = no.n + 1; j < M; j++) {
            no.filhos[j] = 0;
        }

        i = 0;
        while (i < no.n && chave >= no.chaves[i]) {
            if (chave == no.chaves[i]) {
                return {p, i + 1, true};
            }
            i++;
        }

        q = p;
        p = no.filhos[i];
    }

    return {q, i, false};
}
