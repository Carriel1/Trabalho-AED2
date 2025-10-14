// Integrantes do grupo:
// Caio Monteiro Sartori   N 15444598
// Mateus Henrique Carriel   N 15698362
// Murilo Augusto Jorge   N 15552251

// Arquivo .CPP de definicao da classe arvoreB

#include "ArvoreMVias.h"
#include <cstring>
#include <sstream>
#include <cmath>
#include <iomanip>
#include <iostream>
#include <fstream>

using namespace std;

// ----------------- Construtor -------------------
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

// ----------------- Helper: tamanho do no -------------------
int ArvoreMVias::nodeInts() const {
    return 2 + (M - 1) + M; // n, folha, chaves (M-1), filhos (M)
}

// ----------------- Header -------------------
void ArvoreMVias::writeHeader() {
    fstream fout(arquivoBin, ios::binary | ios::in | ios::out);
    if (!fout) {
        ofstream cr(arquivoBin, ios::binary);
        cr.close();
        fout.open(arquivoBin, ios::binary | ios::in | ios::out);
        cout << "Arquivo binario criado e header inicializado.\n";
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
    fin.close();

    if (aM != M) {
        cout << "Ordem M diferente da existente no arquivo. Arquivo antigo sera apagado e recriado.\n";
        remove(arquivoBin.c_str()); // apaga arquivo binario antigo
        raiz = 1;
        nextNodeId = 1;
        writeHeader();
        createNode(true); // cria raiz vazia
        return true;
    }

    raiz = aRaiz;
    nextNodeId = aNext;
    return true;
}

// ----------------- Leitura/Escrita de nos -------------------
void ArvoreMVias::writeNode(int id, const vector<int>& vals) {
    fstream fout(arquivoBin, ios::in | ios::out | ios::binary);
    if (!fout) { 
        ofstream cr(arquivoBin, ios::binary); 
        cr.close(); 
        fout.open(arquivoBin, ios::in | ios::out | ios::binary); 
        cout << "Arquivo binario aberto/criado durante writeNode.\n";
    }

    int headerBytes = 3 * sizeof(int);
    int ints = nodeInts();
    int nodeBytes = ints * sizeof(int);
    streampos pos = static_cast<streamoff>(headerBytes) + static_cast<streamoff>((id - 1) * nodeBytes);

    fout.seekp(pos);
    for (int i = 0; i < ints; ++i) {
        fout.write((char*)&vals[i], sizeof(int));
    }
    fout.close();
    escritaDisco++;
    cout << "Node " << id << " escrito no disco.\n";
}

bool ArvoreMVias::readNode(int id, vector<int>& vals) {
    ifstream fin(arquivoBin, ios::binary);
    if (!fin) { 
        cerr << "Erro ao abrir arquivo binario para leitura.\n"; 
        return false; 
    }

    int headerBytes = 3 * sizeof(int);
    int ints = nodeInts();
    int nodeBytes = ints * sizeof(int);
    streampos pos = static_cast<streamoff>(headerBytes) + static_cast<streamoff>((id - 1) * nodeBytes);

    fin.seekg(0, ios::end);
    streampos fileSize = fin.tellg();
    if (pos + static_cast<streamoff>(nodeBytes) > fileSize) { 
        fin.close(); 
        cerr << "Tentativa de ler no alem do final do arquivo. id=" << id << endl;
        return false; 
    }

    fin.seekg(pos);
    vals.assign(ints, 0);
    for (int i = 0; i < ints; ++i) {
        fin.read((char*)&vals[i], sizeof(int));
    }
    fin.close();
    leituraDisco++;
    cout << "Node " << id << " lido do disco.\n";
    return true;
}

// ----------------- Getters/Setters -------------------
int ArvoreMVias::node_get_n(const vector<int>& vals) const { return vals[0]; }
void ArvoreMVias::node_set_n(vector<int>& vals, int n) { vals[0] = n; }
bool ArvoreMVias::node_get_folha(const vector<int>& vals) const { return vals[1] != 0; }
void ArvoreMVias::node_set_folha(vector<int>& vals, bool folha) { vals[1] = folha ? 1 : 0; }
int ArvoreMVias::node_get_chave(const vector<int>& vals, int idx) const { return vals[2 + idx]; }
void ArvoreMVias::node_set_chave(vector<int>& vals, int idx, int chave) { vals[2 + idx] = chave; }
int ArvoreMVias::node_get_filho(const vector<int>& vals, int idx) const { return vals[2 + (M-1) + idx]; }
void ArvoreMVias::node_set_filho(vector<int>& vals, int idx, int filho) { vals[2 + (M-1) + idx] = filho; }

// ----------------- Criacao de no -------------------
int ArvoreMVias::createNode(bool folha) {
    int id = nextNodeId++;
    vector<int> vals(nodeInts(), 0);
    node_set_n(vals, 0);
    node_set_folha(vals, folha);

    // inicializa filhos com 0
    for(int i=0;i<M;i++) node_set_filho(vals,i,0);

    // inicializa chaves com 0
    for(int i=0;i<M-1;i++) node_set_chave(vals,i,0);

    writeNode(id, vals);
    writeHeader();
    cout << "Node " << id << " criado. Folha=" << folha << "\n";
    return id;
}

// ----------------- Split de no filho -------------------
void ArvoreMVias::splitChild(int parentId, int childIndex, int childId) {
    vector<int> parentVals, childVals;
    if(!readNode(parentId, parentVals)) { cerr<<"Erro: nao conseguiu ler parent no split.\n"; return; }
    if(!readNode(childId, childVals)) { cerr<<"Erro: nao conseguiu ler child no split.\n"; return; }

    int t = (M+1)/2; // grau minimo
    int y_n = node_get_n(childVals);

    int zId = createNode(node_get_folha(childVals));
    vector<int> zVals;
    readNode(zId, zVals);

    int z_n = y_n - t;
    node_set_n(zVals, z_n);

    // mover chaves
    for(int j=0;j<z_n;j++)
        node_set_chave(zVals,j,node_get_chave(childVals,j+t));

    // mover filhos se necessario
    if(!node_get_folha(childVals)){
        for(int j=0;j<=z_n;j++)
            node_set_filho(zVals,j,node_get_filho(childVals,j+t));
    }

    node_set_n(childVals, t-1);

    // atualizar parent
    int parent_n = node_get_n(parentVals);
    for(int j=parent_n;j>childIndex;j--)
        node_set_filho(parentVals,j+1,node_get_filho(parentVals,j));
    node_set_filho(parentVals,childIndex+1,zId);

    for(int j=parent_n-1;j>=childIndex;j--)
        node_set_chave(parentVals,j+1,node_get_chave(parentVals,j));
    node_set_chave(parentVals,childIndex,node_get_chave(childVals,t-1));
    node_set_n(parentVals,parent_n+1);

    writeNode(childId,childVals); escritaDisco++;
    writeNode(zId,zVals); escritaDisco++;
    writeNode(parentId,parentVals); escritaDisco++;

    cout << "Split do no " << childId << " feito. Novo no " << zId << " criado.\n";
}

// ----------------- Insercao em no nao cheio -------------------
void ArvoreMVias::insertNonFull(int nodeId,int chave){
    vector<int> nodeVals;
    if(!readNode(nodeId,nodeVals)) { cerr<<"Erro: no nao lido no insertNonFull.\n"; return; }

    int n = node_get_n(nodeVals);
    if(node_get_folha(nodeVals)){
        int i=n-1;
        while(i>=0 && node_get_chave(nodeVals,i)>chave){
            node_set_chave(nodeVals,i+1,node_get_chave(nodeVals,i));
            i--;
        }
        node_set_chave(nodeVals,i+1,chave);
        node_set_n(nodeVals,n+1);
        writeNode(nodeId,nodeVals); escritaDisco++;
        cout << "Chave " << chave << " inserida na folha " << nodeId << "\n";
    } else {
        int i=n-1;
        while(i>=0 && node_get_chave(nodeVals,i)>chave) i--;
        int childIndex=i+1;
        int childId = node_get_filho(nodeVals,childIndex);
        vector<int> childVals;

        if(childId==0 || !readNode(childId,childVals)){
            childId = createNode(true);
            node_set_filho(nodeVals,childIndex,childId);
            writeNode(nodeId,nodeVals); escritaDisco++;
            cout << "Filho " << childId << " criado porque estava vazio.\n";
        }

        if(!readNode(childId,childVals)) return;

        if(node_get_n(childVals)==M-1){
            splitChild(nodeId,childIndex,childId);
            readNode(nodeId,nodeVals); // atualizar parent
            if(node_get_chave(nodeVals,childIndex)<chave) childIndex++;
        }

        insertNonFull(node_get_filho(nodeVals,childIndex),chave);
    }
}

// ----------------- Busca -------------------
Resultado ArvoreMVias::mSearch(int chave){
    if(!readHeader()) { cerr<<"Erro: nao conseguiu ler header.\n"; return {-1,-1,false}; }

    int p=raiz;
    int last_q=-1;
    while(p!=0){
        vector<int> vals;
        if(!readNode(p,vals)) { cerr<<"Erro: no "<<p<<" nao lido na busca.\n"; return {-1,-1,false}; }
        leituraDisco++;

        int n=node_get_n(vals);
        int i=0;
        while(i<n && chave>node_get_chave(vals,i)) i++;

        if(i<n && chave==node_get_chave(vals,i)){
            cout << "Chave " << chave << " encontrada no no " << p << ", posicao " << i+1 << "\n";
            return {p,i+1,true};
        }

        last_q=p;
        p=node_get_filho(vals,i);
    }
    cout << "Chave " << chave << " nao encontrada.\n";
    if(last_q==-1) last_q=0;
    return {last_q,0,false};
}

// ----------------- Insercao principal -------------------
void ArvoreMVias::insertB(int chave,const string& dadosElemento){
    ofstream foutDados(arquivoDados,ios::app);
    if(!foutDados) { cerr<<"Erro: nao conseguiu abrir arquivo de dados.\n"; return; }

    Resultado r = mSearch(chave);
    if(r.encontrou){
        cout<<"Aviso: chave "<<chave<<" ja existe. Dados adicionados somente ao arquivo.\n";
        foutDados<<chave<<" "<<dadosElemento<<endl;
        foutDados.close();
        return;
    }

    foutDados<<chave<<" "<<dadosElemento<<endl;
    foutDados.close();
    escritaDisco++;

    if(!readHeader()) return;
    vector<int> rootVals;
    if(!readNode(raiz,rootVals)) return;
    leituraDisco++;

    if(node_get_n(rootVals)==M-1){
        int s=createNode(false);
        vector<int> sVals;
        readNode(s,sVals);
        node_set_filho(sVals,0,raiz);
        writeNode(s,sVals); escritaDisco++;
        raiz=s;
        writeHeader();
        splitChild(s,0,node_get_filho(sVals,0));
    }

    insertNonFull(raiz,chave);
    cout << "Insercao da chave " << chave << " realizada com sucesso.\n";
}

// ----------------- Impressao -------------------
void ArvoreMVias::print(){
    if(!readHeader()){ cout<<"Arquivo binario nao encontrado.\n"; return; }

    cout << "T = 1, m = " << M << "\n--------------------------------------------\n";
    cout << "No n,A[0],(K[1],A[1]),...(K[n],A[n])\n--------------------------------------------\n";

    for(int id=1;id<nextNodeId;id++){
        vector<int> vals;
        if(!readNode(id,vals)) continue;
        int n=node_get_n(vals);
        cout<<setw(2)<<id<<" "<<n<<","<<node_get_filho(vals,0);
        for(int i=0;i<n;i++)
            cout<<",("<<setw(2)<<node_get_chave(vals,i)<<","<<node_get_filho(vals,i+1)<<")";
        cout<<endl;
    }
    cout<<"--------------------------------------------\n";
}

void ArvoreMVias::imprimirIndice(){
    print();
    cout<<"Numero de leituras de disco: "<<leituraDisco<<endl;
    cout<<"Numero de escritas de disco: "<<escritaDisco<<endl;
}

void ArvoreMVias::imprimirArquivoPrincipal(){
    ifstream fin(arquivoDados);
    if(!fin){ cout<<"Arquivo de dados vazio ou nao existe.\n"; return; }

    string linha;
    cout<<"Conteudo do arquivo principal ("<<arquivoDados<<"):\n";
    while(getline(fin,linha)) cout<<linha<<endl;
    fin.close();
}

void ArvoreMVias::imprimirArquivoPrincipal(int chave){
    ifstream fin(arquivoDados);
    if(!fin){ cout<<"Arquivo de dados vazio ou nao existe.\n"; return; }

    string linha;
    bool found=false;
    while(getline(fin,linha)){
        stringstream ss(linha);
        int k;
        if(ss>>k && k==chave){ cout<<linha<<endl; found=true; }
    }

    if(!found) cout<<"Nenhum registro com a chave "<<chave<<" encontrado.\n";
    fin.close();
}

// ----------------- Inicializacao -------------------
void ArvoreMVias::geradorBinario(){
    if(!readHeader()){
        raiz=1; nextNodeId=1; writeHeader(); createNode(true);
        cout<<"Arquivo binario inicializado com raiz vazia.\n";
    }else{
        cout<<"Arquivo binario existente lido. raiz="<<raiz<<" nextNodeId="<<nextNodeId<<"\n";
    }
}
