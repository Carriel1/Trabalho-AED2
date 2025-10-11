// Integrantes do grupo:
// Caio Monteiro Sartori   N° 15444598
// Mateus Henrique Carriel   N° 15698362
// Murilo Augusto Jorge   N° 15552251

// Arquivo .cpp de implementação da classe árvore M-vias

#include "ArvoreMVias.h"
#include <cstring>
#include <sstream>
#include <cmath>

ArvoreMVias::ArvoreMVias(const string& txt, const string& bin, const string& dados, int ordem) {
    arquivoTxt = txt;
    arquivoBin = bin;
    arquivoDados = dados;
    M = ordem;
    raiz = 1;
    nextNodeId = 1;
    leituraDisco = 0;
    escritaDisco = 0;
}

int ArvoreMVias::nodeInts() const {
    return 2 * M + 1;  // [n, folha, chaves (M-1), filhos (M)]
}

void ArvoreMVias::writeHeader() {
    fstream fout(arquivoBin, ios::binary | ios::in | ios::out);
    if (!fout) {
        ofstream cr(arquivoBin, ios::binary);
        cr.close();
        fout.open(arquivoBin, ios::binary | ios::in | ios::out);
    }
    fout.seekp(0);
    int aM = M, aRaiz = raiz, aNext = nextNodeId;
    fout.write((char*)&aM, sizeof(int));
    fout.write((char*)&aRaiz, sizeof(int));
    fout.write((char*)&aNext, sizeof(int));
    fout.close();
}

bool ArvoreMVias::readHeader() {
    ifstream fin(arquivoBin, ios::binary);
    if (!fin) return false;
    fin.seekg(0);
    int aM, aRaiz, aNext;
    if (!fin.read((char*)&aM, sizeof(int))) { fin.close(); return false; }
    if (!fin.read((char*)&aRaiz, sizeof(int))) { fin.close(); return false; }
    if (!fin.read((char*)&aNext, sizeof(int))) { fin.close(); return false; }
    if (aM != M) { fin.close(); writeHeader(); return true; }
    raiz = aRaiz;
    nextNodeId = aNext;
    fin.close();
    return true;
}

void ArvoreMVias::writeNode(int id, const vector<int>& vals) {
    fstream fout(arquivoBin, ios::in | ios::out | ios::binary);
    if (!fout) { ofstream cr(arquivoBin, ios::binary); cr.close(); fout.open(arquivoBin, ios::in | ios::out | ios::binary); }

    int headerBytes = 3 * sizeof(int);
    int ints = nodeInts();
    int nodeBytes = ints * sizeof(int);
    streampos pos = static_cast<std::streamoff>(headerBytes) + static_cast<std::streamoff>((id - 1) * nodeBytes);  // <-- correção
    fout.seekp(pos);
    for (int i = 0; i < ints; ++i) {
        int v = vals[i];
        fout.write((char*)&v, sizeof(int));
    }
    fout.close();
    escritaDisco++;
}

bool ArvoreMVias::readNode(int id, vector<int>& vals) {
    ifstream fin(arquivoBin, ios::binary);
    if (!fin) return false;
    int headerBytes = 3 * sizeof(int);
    int ints = nodeInts();
    int nodeBytes = ints * sizeof(int);
    streampos pos = static_cast<std::streamoff>(headerBytes) + static_cast<std::streamoff>((id - 1) * nodeBytes);  // <-- correção
    fin.seekg(0, ios::end);
    streampos fileSize = fin.tellg();
    if (pos + static_cast<std::streamoff>(nodeBytes) > fileSize) {  // <-- correção
        fin.close();
        return false;
    }
    fin.seekg(pos);
    vals.assign(ints, 0);
    for (int i = 0; i < ints; ++i) {
        int v = 0;
        fin.read((char*)&v, sizeof(int));
        vals[i] = v;
    }
    fin.close();
    leituraDisco++;
    return true;
}

// getters/setters do nó serializado (vetor<int>)
int ArvoreMVias::node_get_n(const vector<int>& vals) const {
    return vals[0];
}
void ArvoreMVias::node_set_n(vector<int>& vals, int n) {
    vals[0] = n;
}
bool ArvoreMVias::node_get_folha(const vector<int>& vals) const {
    return vals[1] != 0;
}
void ArvoreMVias::node_set_folha(vector<int>& vals, bool folha) {
    vals[1] = folha ? 1 : 0;
}
int ArvoreMVias::node_get_chave(const vector<int>& vals, int idx) const {
    // chaves começam em vals[2], idx 0..M-2
    return vals[2 + idx];
}
void ArvoreMVias::node_set_chave(vector<int>& vals, int idx, int chave) {
    vals[2 + idx] = chave;
}
int ArvoreMVias::node_get_filho(const vector<int>& vals, int idx) const {
    // filhos começam em vals[2 + (M-1)], idx 0..M-1
    return vals[2 + (M - 1) + idx];
}
void ArvoreMVias::node_set_filho(vector<int>& vals, int idx, int filho) {
    vals[2 + (M - 1) + idx] = filho;
}

int ArvoreMVias::createNode(bool folha) {
    int id = nextNodeId++;
    int ints = nodeInts();
    vector<int> vals(ints, 0);
    node_set_n(vals, 0);
    node_set_folha(vals, folha);
    // inicializa filhos com 0
    for (int i = 0; i < M; ++i) node_set_filho(vals, i, 0);
    // chaves preenchidas com 0
    for (int i = 0; i < M - 1; ++i) node_set_chave(vals, i, 0);
    writeNode(id, vals);
    writeHeader();
    return id;
}

void ArvoreMVias::geradorBinario() {
    // cria arquivo bin e inicializa header e raiz vazia, a menos que ja exista um header
    bool ok = readHeader();
    if (!ok) {
        raiz = 1;
        nextNodeId = 1;
        writeHeader();
        createNode(true); // cria raiz (id 1)
        cout << "Arquivo binario inicializado com raiz vazia.\n";
    } else {
        cout << "Arquivo binario existente lido. raiz=" << raiz << " nextNodeId=" << nextNodeId << "\n";
    }

    // Observação: se quiser popular a árvore com chaves vindas do arquivoTexto (arquivoTxt),
    // é possível fazer parsing aqui e inserir chave por chave usando insertB.
    // Por ora, mantemos a inicialização da árvore vazia para garantir consistência.
}

void ArvoreMVias::print() {
    // lê header e imprime todos os nós de 1 .. nextNodeId-1
    if (!readHeader()) {
        cout << "Arquivo binario nao encontrado ou corrompido.\n";
        return;
    }
    cout << "T = 1, m = " << M << endl;
    cout << "------------------------------------------------------------------\n";
    cout << "No  n,A[0],(K[1],A[1]),...,(K[n],A[n])\n";
    cout << "------------------------------------------------------------------\n";

    for (int id = 1; id < nextNodeId; ++id) {
        vector<int> vals;
        if (!readNode(id, vals)) continue;
        int n = node_get_n(vals);
        cout << setw(2) << id << " " << n << ", " << node_get_filho(vals, 0);
        for (int i = 0; i < n; ++i) {
            cout << ",(" << setw(2) << node_get_chave(vals, i) << "," << node_get_filho(vals, i + 1) << ")";
        }
        cout << endl;
    }

    cout << "------------------------------------------------------------------\n";
}

Resultado ArvoreMVias::mSearch(int chave) {
    if (!readHeader()) {
        cerr << "Erro ao abrir " << arquivoBin << " (header).\n";
        return {-1, -1, false};
    }

    int p = raiz;
    int last_q = -1;
    while (p != 0) {
        vector<int> vals;
        if (!readNode(p, vals)) {
            cerr << "Erro ao ler no " << p << " do arquivo.\n";
            return {-1, -1, false};
        }
        int n = node_get_n(vals);
        int i = 0;
        // encontrar a primeira posição onde chave <= K[i]
        while (i < n && chave > node_get_chave(vals, i)) i++;
        if (i < n && chave == node_get_chave(vals, i)) {
            // encontrado
            cout << "Chave encontrada!\n";
            // posicao iremos retornar 1-based
            return {p, i + 1, true};
        } else {
            last_q = p;
            p = node_get_filho(vals, i);
        }
    }
    cout << "Chave nao encontrada.\n";
    if (last_q == -1) last_q = 0;
    return {last_q, 0, false};
}

void ArvoreMVias::splitChild(int parentId, int childIndex, int childId) {
    // childIndex: index 0..parent.n where child's id is childIndex child
    // read parent and child
    vector<int> parentVals, childVals;
    if (!readNode(parentId, parentVals)) {
        cerr << "Erro split: nao leu parent\n";
        return;
    }
    if (!readNode(childId, childVals)) {
        cerr << "Erro split: nao leu child\n";
        return;
    }

    int t = (M + 1) / 2; // grau minimo
    // cria novo nó z
    int zId = createNode(node_get_folha(childVals));
    vector<int> zVals;
    readNode(zId, zVals); // agora zVals existe

    int y_n = node_get_n(childVals);
    // z.n = t-1
    node_set_n(zVals, t - 1);
    // mover chaves de child (y) para z
    // y: keys 0..y_n-1, median index = t-1
    for (int j = 0; j < t - 1; ++j) {
        int key = node_get_chave(childVals, j + t);
        node_set_chave(zVals, j, key);
    }
    // mover filhos se nao for folha
    if (!node_get_folha(childVals)) {
        for (int j = 0; j < t; ++j) {
            int f = node_get_filho(childVals, j + t);
            node_set_filho(zVals, j, f);
        }
    }
    // reduzir y.n = t-1
    node_set_n(childVals, t - 1);

    // inserir novo filho zId em parent em posição childIndex+1 e chave mediana
    int parent_n = node_get_n(parentVals);
    // shift filhos e chaves no parent
    for (int j = parent_n; j >= childIndex + 1; --j) {
        int f = node_get_filho(parentVals, j);
        node_set_filho(parentVals, j + 1, f);
    }
    node_set_filho(parentVals, childIndex + 1, zId);

    for (int j = parent_n - 1; j >= childIndex; --j) {
        int k = node_get_chave(parentVals, j);
        node_set_chave(parentVals, j + 1, k);
    }
    // mover mediana childVals.chaves[t-1] para parent.chaves[childIndex]
    int med = node_get_chave(childVals, t - 1);
    node_set_chave(parentVals, childIndex, med);
    node_set_n(parentVals, parent_n + 1);

    // write back child (y), z, parent
    writeNode(childId, childVals);
    writeNode(zId, zVals);
    writeNode(parentId, parentVals);
}

void ArvoreMVias::insertNonFull(int nodeId, int chave) {
    vector<int> nodeVals;
    if (!readNode(nodeId, nodeVals)) {
        cerr << "Erro insertNonFull: nao leu no " << nodeId << endl;
        return;
    }
    int n = node_get_n(nodeVals);
    if (node_get_folha(nodeVals)) {
        // inserir chave no local correto
        int i = n - 1;
        // shift chaves maiores que chave
        while (i >= 0 && node_get_chave(nodeVals, i) > chave) {
            node_set_chave(nodeVals, i + 1, node_get_chave(nodeVals, i));
            i--;
        }
        node_set_chave(nodeVals, i + 1, chave);
        node_set_n(nodeVals, n + 1);
        writeNode(nodeId, nodeVals);
    } else {
        int i = n - 1;
        while (i >= 0 && node_get_chave(nodeVals, i) > chave) i--;
        int childIndex = i + 1; // position to descend
        int childId = node_get_filho(nodeVals, childIndex);
        if (childId == 0) {
            // cria novo filho vazio se apontador inexistente
            childId = createNode(true);
            node_set_filho(nodeVals, childIndex, childId);
            writeNode(nodeId, nodeVals);
        }
        vector<int> childVals;
        if (!readNode(childId, childVals)) {
            cerr << "Erro insertNonFull: nao leu child\n";
            return;
        }
        if (node_get_n(childVals) == M - 1) {
            // se cheio, split
            splitChild(nodeId, childIndex, childId);
            // após split, parent.chave[childIndex] contem a mediana
            if (node_get_chave(nodeVals, childIndex) < chave) {
                childIndex = childIndex + 1;
            }
        }
        int nextChild = node_get_filho(nodeVals, childIndex);
        insertNonFull(nextChild, chave);
    }
}

void ArvoreMVias::insertB(int chave, const string& dadosElemento) {
    // primeiro: escreve no arquivo principal
    ofstream foutDados(arquivoDados, ios::app);
    if (!foutDados) {
        cerr << "Erro ao abrir " << arquivoDados << " para escrita.\n";
        return;
    }
    // evitar duplicidade: se chave ja existe na arvore, avisar e nao inserir indice duplicado
    Resultado r = mSearch(chave);
    if (r.encontrou) {
        cout << "Aviso: chave " << chave << " ja existe no indice. Dados foram adicionados ao arquivo principal mas NAO inseridos no indice.\n";
        foutDados << chave << " " << dadosElemento << endl;
        foutDados.close();
        return;
    }

    foutDados << chave << " " << dadosElemento << endl;
    foutDados.close();
    escritaDisco++; // contagem de escrita no arquivo de dados (opcionalmente)

    // inserir na arvore B (arquivo bin)
    if (!readHeader()) {
        cerr << "Erro ao ler header antes de inserir.\n";
        return;
    }

    vector<int> rootVals;
    if (!readNode(raiz, rootVals)) {
        cerr << "Erro ao ler raiz.\n";
        return;
    }
    if (node_get_n(rootVals) == M - 1) {
        // raiz cheia -> criar nova raiz
        int s = createNode(false);
        // set s.filho[0] = old root
        vector<int> sVals;
        readNode(s, sVals);
        node_set_filho(sVals, 0, raiz);
        writeNode(s, sVals);
        raiz = s;
        writeHeader();
        splitChild(s, 0, /*childId=*/node_get_filho(sVals,0)); // split old root which is child 0
        insertNonFull(raiz, chave);
    } else {
        insertNonFull(raiz, chave);
    }

    cout << "Insercao da chave " << chave << " realizada com sucesso na arvore.\n";
}

void ArvoreMVias::imprimirIndice() {
    print();
    cout << "Numero de leituras de disco: " << leituraDisco << endl;
    cout << "Numero de escritas de disco: " << escritaDisco << endl;
}

void ArvoreMVias::imprimirArquivoPrincipal() {
    ifstream fin(arquivoDados);
    if (!fin) {
        cout << "Arquivo de dados vazio ou nao existe (" << arquivoDados << ").\n";
        return;
    }
    string linha;
    cout << "Conteudo do arquivo principal (" << arquivoDados << "):\n";
    while (getline(fin, linha)) {
        cout << linha << endl;
    }
    fin.close();
}

void ArvoreMVias::imprimirArquivoPrincipal(int chave) {
    ifstream fin(arquivoDados);
    if (!fin) {
        cout << "Arquivo de dados vazio ou nao existe (" << arquivoDados << ").\n";
        return;
    }
    string linha;
    bool found = false;
    while (getline(fin, linha)) {
        // tenta extrair a primeira palavra como inteiro
        stringstream ss(linha);
        int k;
        if (ss >> k) {
            if (k == chave) {
                cout << linha << endl;
                found = true;
            }
        }
    }
    if (!found) cout << "Nenhum registro com a chave " << chave << " encontrado no arquivo principal.\n";
    fin.close();
}
