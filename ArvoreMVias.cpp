// Integrantes do grupo:
// Caio Monteiro Sartori   N° 15444598
// Mateus Henrique Carriel   N° 15698362
// Murilo Augusto Jorge   N° 15552251

// Arquivo .cpp de implementação da classe árvore M-vias

#include "ArvoreMVias.h"

// Construtor
ArvoreMVias::ArvoreMVias(const string& txt, const string& bin, const string& dados, int ordem) {
    arquivoTxt = txt;
    arquivoBin = bin;
    arquivoDados = dados;
    M = ordem;
    raiz = 1; // início padrão
}

// Gera arquivo binário a partir do txt
void ArvoreMVias::geradorBinario() {
    ifstream fin(arquivoTxt);
    ofstream fout(arquivoBin, ios::binary);

    if (!fin) {
        cout << "Erro ao abrir " << arquivoTxt << endl;
        return;
    }

    int indice_registro;
    No no;
    no.chaves.resize(M - 1);
    no.filhos.resize(M);

    while (fin >> indice_registro) {
        fout.seekp((indice_registro - 1) * sizeof(No));

        fin >> no.n;
        fin >> no.filhos[0];

        for (int i = 0; i < no.n; i++) {
            fin >> no.chaves[i] >> no.filhos[i + 1];
        }

        fout.write((char*)&no, sizeof(No));
    }

    fin.close();
    fout.close();
    cout << "Lendo dados de " << arquivoTxt << " e criando " << arquivoBin << endl;
}

// Imprime árvore
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

    int p = raiz;
    int q = 0;
    No no;
    int i = 0;

    while (p != 0) {
        fin.seekg((p - 1) * sizeof(No));

        if (!fin.read((char*)&no, sizeof(No))) {
            cout << "Erro ao ler no " << p << " do arquivo." << endl;
            return {-1, -1, false};
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

// Inserção em árvore B
void ArvoreMVias::insertB(int chave, const string& dadosElemento) {
    // Inserir em arquivo principal
    ofstream foutDados(arquivoDados, ios::app);
    foutDados << chave << " " << dadosElemento << endl;
    foutDados.close();

    // Aqui colocar lógica de inserção em árvore B com split
    cout << "Insercao da chave " << chave << " realizada (metodo insertB ainda precisa ser implementado completamente)." << endl;
}

// Imprime índice
void ArvoreMVias::imprimirIndice() {
    print();
}

// Imprime arquivo principal
void ArvoreMVias::imprimirArquivoPrincipal() {
    ifstream fin(arquivoDados);
    string linha;
    cout << "Conteudo do arquivo principal (" << arquivoDados << "):\n";
    while (getline(fin, linha)) {
        cout << linha << endl;
    }
    fin.close();
}
