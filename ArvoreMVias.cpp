// ====================================================================================================================

// INTEGRANTES DO GRUPO:
// Caio Monteiro Sartori            N° 15444598
// Mateus Henrique J. S. Carriel    N° 15698362
// Murilo Augusto Jorge             N° 15552251

// ====================================================================================================================

// ====================================================================================================================

// INCLUSAO DE BIBLIOTECAS E ARQUIVOS:
#include "ArvoreMVias.h"    // Arquivo Header do Presente Arquivo.
#include <cstring>          // Funcoes de Manipulacao de Memoria (memset).
#include <sstream>          // Manipulacao de Streams de Strings.
#include <cmath>            // Funcoes Matematicas.
#include <iostream>         // Operacoes de Entradas e Saidas.
#include <iomanip>          // Formatacao de Saida.

// ====================================================================================================================

// ====================================================================================================================
// CONSTRUTOR: Configura os Nomes de Arquivo e a Ordem (M):
ArvoreMVias::ArvoreMVias(const string& txt, const string& bin, const string& dados) {
    arquivoTxt = txt;       // Inicializa o nome do Arquivo de Texto.
    arquivoBin = bin;       // Inicializa o nome do Arquivo de Indice Binario (onde a Arvore eh Salva).
    arquivoDados = dados;   // Inicializa o nome do Arquivo de Dados Principal.
    T = (M + 1) / 2;        // Define o Grau Minimo (T) necessario.
    raiz = 1;               // Inicializa o ID do Noh Raiz.
    nextNodeId = 1;         // Inicializa o ID do proximo Noh a ser Criado.

} // Fim do CONSTRUTOR
// ====================================================================================================================

// ====================================================================================================================
// METODOS DE SEQUENCIAMENTO E I/O DE DISCO:

// Calcula o Numero de Inteiros que compoem um Noh no Disco.
// FORMATO: [n (1 int), folha (1 int), chaves (M-1 ints), filhos (M ints)]
int ArvoreMVias::nodeInts() const {
    return 2 + (M - 1) + M + 1; // 2 * M + 1
}

void ArvoreMVias::writeHeader() {
    // Abre o arquivo binario, criando-o se nao existir
    fstream fout(arquivoBin, ios::binary | ios::in | ios::out);
    if (!fout) {
        ofstream cr(arquivoBin, ios::binary);
        cr.close();
        fout.open(arquivoBin, ios::binary | ios::in | ios::out);
    }
    // Grava M, raiz e nextNodeId no inicio
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

    raiz = aRaiz;
    nextNodeId = aNext;
    fin.close();
    return true;
}

// Simula a escrita de um bloco no disco (incrementa escritaDisco)
void ArvoreMVias::writeNode(int id, const vector<int>& vals) {
    fstream fout(arquivoBin, ios::in | ios::out | ios::binary);
    if (!fout) { cerr << "Erro de arquivo (writeNode).\n"; return; }

    int headerBytes = 3 * sizeof(int);
    int ints = nodeInts();
    int nodeBytes = ints * sizeof(int);
    // Calcula a posicao no arquivo: Header + (ID - 1) * Tamanho do No
    streampos pos = static_cast<std::streamoff>(headerBytes) + static_cast<std::streamoff>((id - 1) * nodeBytes);
    fout.seekp(pos);
    for (int i = 0; i < ints; ++i) {
        int v = vals[i];
        fout.write((char*)&v, sizeof(int));
    }
    fout.close();
}

// Simula a leitura de um bloco no disco (incrementa leituraDisco)
bool ArvoreMVias::readNode(int id, vector<int>& vals) {
    ifstream fin(arquivoBin, ios::binary);
    if (!fin) return false;
    int headerBytes = 3 * sizeof(int);
    int ints = nodeInts();
    int nodeBytes = ints * sizeof(int);
    // Calcula a posicao no arquivo
    streampos pos = static_cast<std::streamoff>(headerBytes) + static_cast<std::streamoff>((id - 1) * nodeBytes);

    // Verifica se o no existe (evita leitura fora do limite)
    fin.seekg(0, ios::end);
    streampos fileSize = fin.tellg();
    if (pos + static_cast<std::streamoff>(nodeBytes) > fileSize) {
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
    return true;
}

// Getters/Setters do no serializado
int ArvoreMVias::node_get_n(const vector<int>& vals) const { return vals[0]; }
void ArvoreMVias::node_set_n(vector<int>& vals, int n) { vals[0] = n; }
bool ArvoreMVias::node_get_folha(const vector<int>& vals) const { return vals[1] != 0; }
void ArvoreMVias::node_set_folha(vector<int>& vals, bool folha) { vals[1] = folha ? 1 : 0; }
int ArvoreMVias::node_get_chave(const vector<int>& vals, int idx) const {
    return vals[2 + idx];  // Chaves: 2, 3, 4
}

void ArvoreMVias::node_set_chave(vector<int>& vals, int idx, int chave) {
    vals[2 + idx] = chave;
}

int ArvoreMVias::node_get_filho(const vector<int>& vals, int idx) const {
    return vals[2 + (M - 1) + 1 + idx];  // Filhos: 2 + 2 + 1 + idx = 5 + idx
}

void ArvoreMVias::node_set_filho(vector<int>& vals, int idx, int filho) {
    vals[2 + (M - 1) + 1 + idx] = filho;
}

int ArvoreMVias::createNode(bool folha) {
    int id = nextNodeId++;
    int ints = nodeInts();
    vector<int> vals(ints, 0);
    node_set_n(vals, 0);
    node_set_folha(vals, folha);
    // Inicializa filhos e chaves com 0
    for (int i = 0; i < M; ++i) node_set_filho(vals, i, 0);
    for (int i = 0; i < M - 1; ++i) node_set_chave(vals, i, 0);
    writeNode(id, vals);
    writeHeader(); // Atualiza nextNodeId no header
    return id;
}

void ArvoreMVias::geradorBinario() {
    // Tenta ler o header; se falhar, inicializa o arquivo
    bool ok = readHeader();
    if (!ok) {
        raiz = 1;
        nextNodeId = 1;
        writeHeader();
        createNode(true); // Cria raiz (id 1) como folha
        cout << "Arquivo binario de indice inicializado com raiz vazia.\n\n";
    } else {
        cout << "Arquivo binario existente lido. Raiz ID=" << raiz << " Proximo ID=" << nextNodeId << "\n\n";
    }
}

void ArvoreMVias::print() {

    if (!readHeader()) {
        cout << "Arquivo binario nao encontrado ou corrompido.\n";
        return;
    }
    cout << "\n--- Conteudo da Arvore B (Indice) ---\n";
    cout << "Ordem (M): " << M << ", Grau Minimo (T): " << (M + 1) / 2 << endl;
    cout << "Raiz ID: " << raiz << endl;
    cout << "------------------------------------------------------------------\n";
    // Formato: No ID | n (chaves) | A[0] (filho) | (K[0],A[1]),...,(K[n-1],A[n])
    cout << "ID n | A[0] | (K[0],A[1]),...,(K[n-1],A[n])\n";
    cout << "------------------------------------------------------------------\n";

    for (int id = 1; id < nextNodeId; ++id) {
        vector<int> vals;
        if (!readNode(id, vals)) continue;
        int n = node_get_n(vals);

        cout << setw(2) << id << " " << n << " | " << node_get_filho(vals, 0);
        for (int i = 0; i < n; ++i) {
            cout << " | (" << setw(2) << node_get_chave(vals, i) << "," << node_get_filho(vals, i + 1) << ")";
        }
        cout << endl;
    }

    cout << "------------------------------------------------------------------\n";
}

// Requisito C: mSearch - Retorna o triplo (ID No, Posicao, Encontrado)
Resultado ArvoreMVias::mSearch(int chave) {

    if (!readHeader()) {
        cerr << "Erro ao abrir " << arquivoBin << " (header).\n";
        return {-1, -1, false};
    }

    int p = raiz;
    while (p != 0) {
        vector<int> vals;
        if (!readNode(p, vals)) {
            cerr << "Erro ao ler no " << p << " do arquivo.\n";
            return {-1, -1, false};
        }

        int n = node_get_n(vals);
        int i = 0;

        // Encontra a posição de inserção (ou a chave, se existir)
        while (i < n && chave > node_get_chave(vals, i))
            i++;

        if (i < n && chave == node_get_chave(vals, i)) {
            // Encontrou a chave
            return {p, i + 1, true};
        }

        // Desce para o filho correspondente
        int filho = node_get_filho(vals, i);
        if (filho == 0) {
            // Chegou em folha — chave deve ser inserida aqui na posição i
            return {p, i + 1, false};
        }

        p = filho;
    }

    // Árvores vazias ou erro inesperado
    return {-1, -1, false};
}

void ArvoreMVias::insertB(int chave, const string& titulo, const string& autor, int ano, const string& genero) {
    // 1. Verifica se a chave ja existe no indice
    Resultado r = mSearch(chave);
    if (r.encontrou) {
        cout << "Aviso: ID " << chave << " ja existe no indice. Insercao cancelada.\n";
        return;
    }

    // 2. Escreve no arquivo principal
    ofstream foutDados(arquivoDados, ios::binary | ios::app);
    if (!foutDados) {
        cerr << "Erro ao abrir " << arquivoDados << " para escrita.\n";
        return;
    }

    Registro novoLivro;
    novoLivro.chave = chave;
    novoLivro.ano = ano;

    size_t lenTitulo = min(titulo.length(), (size_t)99);
    titulo.copy(novoLivro.titulo, lenTitulo);
    novoLivro.titulo[lenTitulo] = '\0';

    size_t lenAutor = min(autor.length(), (size_t)49);
    autor.copy(novoLivro.autor, lenAutor);
    novoLivro.autor[lenAutor] = '\0';

    size_t lenGenero = min(genero.length(), (size_t)29);
    genero.copy(novoLivro.genero, lenGenero);
    novoLivro.genero[lenGenero] = '\0';

    novoLivro.deletado = false;

    foutDados.write(reinterpret_cast<const char*>(&novoLivro), Registro::getSize());
    foutDados.close();

    // 3. Insere na arvore B seguindo o pseudocódigo
    if (!readHeader()) {
        cerr << "Erro ao ler header antes de inserir.\n";
        return;
    }

    // Linha 1: A ← 0; K ← X
    int K = chave;
    int A = 0; // Para árvore de índice, A é 0

    // Linha 2: (p,i,achou) ← mSearch(T,X)
    int p = r.indice_no; // nó onde deveria estar a chave
    bool achou = r.encontrou;

    if (achou) {
        cout << "ID ja encontrado. Insercao cancelada." << endl;
        return;
    }

    // Linha 4: while p ≠ 0 do
    while (p != 0) {

        // Linha 5: insira (K,A) nas posições apropriadas de P
        vector<int> nodeVals;
        readNode(p, nodeVals);

        int n = node_get_n(nodeVals);

        // Encontrar posição de inserção correta
        int pos = 0;
        while (pos < n && K > node_get_chave(nodeVals, pos)) {
            pos++;
        }

        // *** CORREÇÃO: Shift para direita das chaves E filhos de forma segura ***

        // *** INSERÇÃO CORRIGIDA ***

            // Primeiro: Shift das chaves
            for (int j = n; j > pos; j--) {
                node_set_chave(nodeVals, j, node_get_chave(nodeVals, j - 1));
            }

            // Segundo: Shift dos filhos (se não for folha)
            if (!node_get_folha(nodeVals)) {
                // Shift dos filhos
                for (int j = n + 1; j > pos + 1; j--) {
                    node_set_filho(nodeVals, j, node_get_filho(nodeVals, j - 1));
                }
            }

            // Inserir a CHAVE
            node_set_chave(nodeVals, pos, K);

            // Inserir o FILHO (se não for folha) - ⭐⭐ CORREÇÃO ⭐⭐
            if (!node_get_folha(nodeVals)) {
                // ⭐⭐ CORREÇÃO: A posição do filho é pos + 1 ⭐⭐
                node_set_filho(nodeVals, pos + 1, A);
            }

            node_set_n(nodeVals, n + 1);

        // Linha 6: if n ≤ m-1 then (nó não está cheio)
        if (node_get_n(nodeVals) <= M - 1) {
            writeNode(p, nodeVals);
            cout << "Livro inserido com sucesso no indice e no arquivo principal.\n";
            return;
        }

        // Linha 7: p precisa ser fracionado

        bool ehFolha = node_get_folha(nodeVals);
        int t = T;
        int medianaIndex = t - 1;
        int mediana = node_get_chave(nodeVals, medianaIndex);

        // *** SPLIT CORRIGIDO ***

        // Salvar dados originais ANTES de qualquer modificação
        vector<int> chavesOriginais;
        vector<int> filhosOriginais;

        // Salvar APENAS as chaves válidas (n, não M-1)
        for (int i = 0; i < node_get_n(nodeVals); i++) {
            chavesOriginais.push_back(node_get_chave(nodeVals, i));
        }

        // Salvar APENAS os filhos válidos (n+1, não M)
        if (!ehFolha) {
            for (int i = 0; i <= node_get_n(nodeVals); i++) {
                filhosOriginais.push_back(node_get_filho(nodeVals, i));
            }
        }

        // Criar novo nó
        int q = createNode(ehFolha);
        vector<int> qVals;
        readNode(q, qVals);

        // LIMPEZA COMPLETA
        for (int i = 0; i < M - 1; i++) node_set_chave(nodeVals, i, 0);
        for (int i = 0; i < M; i++) node_set_filho(nodeVals, i, 0);
        node_set_n(nodeVals, 0);
        node_set_folha(nodeVals, ehFolha);

        for (int i = 0; i < M - 1; i++) node_set_chave(qVals, i, 0);
        for (int i = 0; i < M; i++) node_set_filho(qVals, i, 0);
        node_set_n(qVals, 0);
        node_set_folha(qVals, ehFolha);

        // REDISTRIBUIÇÃO CORRETA
        // Nó p: primeiras t-1 chaves
        node_set_n(nodeVals, t - 1);
        for (int i = 0; i < t - 1; i++) {
            node_set_chave(nodeVals, i, chavesOriginais[i]);
        }

        // Nó q: últimas t-1 chaves
        node_set_n(qVals, chavesOriginais.size() - t);
        for (int j = 0; j < node_get_n(qVals); j++) {
            node_set_chave(qVals, j, chavesOriginais[j + t]);
        }

        // Distribuir filhos CORRETAMENTE
        if (!ehFolha) {

            // p recebe primeiros t filhos [0..t]
            for (int j = 0; j <= node_get_n(nodeVals); j++) {
                node_set_filho(nodeVals, j, filhosOriginais[j]);
            }

            // q recebe últimos filhos restantes [t+1..fim]
            for (int j = 0; j <= node_get_n(qVals); j++) {
                node_set_filho(qVals, j, filhosOriginais[j + t]);
            }
        }

        // Linha 8: Escreva p e q para o disco
        writeNode(p, nodeVals);
        writeNode(q, qVals);

        // Linha 9: K ← K⌈m/2⌉; A ← q; P ← pai(p)
        K = mediana;
        A = q;

        // Encontrar pai de p para próxima iteração
        int parent_p = 0;
        if (p == raiz) {
            // Se p era a raiz, sair do loop para criar nova raiz
            break;
        } else {
            // Buscar pai de p
            parent_p = findParent(raiz, p, 0);
            if (parent_p == 0) {
                break;
            }
        }
        p = parent_p;

    }

    // Linha 10-13: Criar nova raiz (se saiu do loop porque p era raiz ou não encontrou pai)
    int novaRaiz = createNode(false);
    vector<int> novaRaizVals;
    readNode(novaRaiz, novaRaizVals);

    // Limpar novo nó
    for (int i = 0; i < M - 1; i++) node_set_chave(novaRaizVals, i, 0);
    for (int i = 0; i < M; i++) node_set_filho(novaRaizVals, i, 0);

    node_set_n(novaRaizVals, 1);
    node_set_chave(novaRaizVals, 0, K);
    node_set_filho(novaRaizVals, 0, raiz);
    node_set_filho(novaRaizVals, 1, A);
    node_set_folha(novaRaizVals, false);

    writeNode(novaRaiz, novaRaizVals);

    // Atualizar raiz
    raiz = novaRaiz;
    writeHeader();
    cout << "Livro inserido com sucesso no indice e no arquivo principal.\n";
}

int ArvoreMVias::findParent(int currentNode, int targetId, int parentId) {
    if (currentNode == 0) return 0;
    if (currentNode == targetId) return parentId;

    vector<int> nodeVals;
    readNode(currentNode, nodeVals);

    // Se é folha, não é pai de ninguém
    if (node_get_folha(nodeVals)) return 0;

    // Verificar todos os filhos
    for (int i = 0; i <= node_get_n(nodeVals); i++) {
        int child = node_get_filho(nodeVals, i);
        if (child != 0) {
            int result = findParent(child, targetId, currentNode);
            if (result != 0) return result;
        }
    }

    return 0;
}

// Requisito A: Imprime o indice (e contadores)
void ArvoreMVias::imprimirIndice() {
    print(); // Imprime o indice
}

// Requisito B: Imprimir todo o arquivo principal
void ArvoreMVias::imprimirArquivoPrincipal() {
    ifstream fin(arquivoDados, ios::binary);
    if (!fin) {
        cout << "Arquivo de dados vazio ou nao existe (" << arquivoDados << ").\n";
        return;
    }
    Registro livro;
    cout << "\n--- BIBLIOTECA - Todos os Livros (" << arquivoDados << ") ---\n";
    cout << "Chave | Titulo                    | Autor           | Ano  | Genero\n";
    cout << "------------------------------------------------------------------\n";

    bool encontrouAtivo = false;

    while (fin.read(reinterpret_cast<char*>(&livro), livro.getSize())) {
        if (!livro.deletado) {
            cout << setw(5) << livro.chave << " | "
                 << setw(25) << left << livro.titulo << " | "
                 << setw(15) << left << livro.autor << " | "
                 << setw(4) << livro.ano << " | "
                 << livro.genero << endl;
            encontrouAtivo = true;
        }
    }

    if (!encontrouAtivo) {
        cout << "Nenhum livro ativo encontrado na biblioteca.\n";
    }

    fin.close();
}

// Requisito C: Imprimir livro específico
void ArvoreMVias::imprimirArquivoPrincipal(int chave) {
    ifstream fin(arquivoDados, ios::binary);
    if (!fin) {
        cerr << "Erro: Arquivo de dados nao existe ou nao pode ser aberto.\n";
        return;
    }
    Registro livro;
    bool found = false;

    cout << "Chave | Titulo                    | Autor           | Ano  | Genero\n";
    cout << "------------------------------------------------------------------\n";

    while (fin.read(reinterpret_cast<char*>(&livro), livro.getSize())) {
        if (livro.chave == chave && !livro.deletado) {
            cout << setw(5) << livro.chave << " | "
                 << setw(25) << left << livro.titulo << " | "
                 << setw(15) << left << livro.autor << " | "
                 << setw(4) << livro.ano << " | "
                 << livro.genero << endl;
            found = true;
            break;
        }
    }

    if (!found) {
        cout << "Nenhum livro ATIVO com a chave " << chave << " encontrado.\n";
    }
    fin.close();
}

// Requisito E: Novo método eficiente de deleção no arquivo de dados
void ArvoreMVias::removeDataFromFile(int chave) {
    markAsDeleted(chave);
}

void ArvoreMVias::markAsDeleted(int chave) {
    fstream file(arquivoDados, ios::binary | ios::in | ios::out);
    if (!file) {
        cerr << "Erro: Arquivo de dados binario nao existe ou nao pode ser aberto para marcacao de delecao.\n";
        return;
    }

    Registro reg;
    bool marked = false;
    streampos currentPos = 0;
    size_t regSize = reg.getSize();

    // Itera sobre os registros
    while (file.read(reinterpret_cast<char*>(&reg), regSize)) {
        if (reg.chave == chave && !reg.deletado) {
            // Encontrado! Marca como deletado
            reg.deletado = true;

            // Retorna ao início do registro no arquivo (currentPos)
            file.seekp(currentPos);

            // Grava APENAS o registro modificado de volta (1 acesso a disco)
            file.write(reinterpret_cast<const char*>(&reg), regSize);

            // Se a chave for única no arquivo de dados, podemos parar aqui.
            // Se não for, continuaria para marcar todas as ocorrências.
            marked = true;
            break;
        }
        currentPos = file.tellg(); // Guarda a posição de onde começará a próxima leitura (início do próximo registro)
    }

    file.close();
    if (marked) {
        cout << "Livro com ID " << chave << " marcado como deletado no arquivo principal (acesso direto a disco).\n";
    } else {
        cerr << "Aviso: Livro ATIVO para o ID " << chave << " nao foi encontrado no arquivo principal.\n";
    }
}

// Implementação do helper para remover a chave K_idx e o ponteiro A_{idx+1}
void ArvoreMVias::removeKeyAndPointer(vector<int>& vals, int idx) {
    int n = node_get_n(vals);

    // Desloca chaves à esquerda
    for (int i = idx; i < n - 1; ++i) {
        node_set_chave(vals, i, node_get_chave(vals, i + 1));
    }
    node_set_chave(vals, n - 1, 0);

    // Desloca filhos à esquerda (n+1 filhos)
    for (int i = idx + 1; i <= n; ++i) {
        node_set_filho(vals, i, node_get_filho(vals, i + 1));
    }
    node_set_filho(vals, n, 0); // último filho limpo

    node_set_n(vals, n - 1);
}

// Limpa um nó no arquivo binário, marcando-o como vazio
void ArvoreMVias::deleteNode(int nodeId) {
    vector<int> empty(nodeInts(), 0);
    if (empty.size() >= 2) empty[1] = 1;

    writeNode(nodeId, empty);
}

// Empréstimo do irmão esquerdo
void ArvoreMVias::borrowFromLeft(int parentId, int childIndex) {
    vector<int> parentVals, leftVals, childVals;
    readNode(parentId, parentVals);

    int childId = node_get_filho(parentVals, childIndex);
    int leftId = node_get_filho(parentVals, childIndex - 1);

    readNode(childId, childVals);
    readNode(leftId, leftVals);

    int nChild = node_get_n(childVals);
    int nLeft = node_get_n(leftVals);

    // Shift chaves e filhos do child para a direita
    for (int i = nChild; i > 0; --i) {
        node_set_chave(childVals, i, node_get_chave(childVals, i - 1));
    }
    if (!node_get_folha(childVals)) {
        for (int i = nChild + 1; i > 0; --i) {
            node_set_filho(childVals, i, node_get_filho(childVals, i - 1));
        }
    }

    // Move chave do pai para child[0]
    node_set_chave(childVals, 0, node_get_chave(parentVals, childIndex - 1));

    // Move último filho da esquerda para child[0] se não for folha
    if (!node_get_folha(childVals))
        node_set_filho(childVals, 0, node_get_filho(leftVals, nLeft));

    // Move última chave de left para o pai
    node_set_chave(parentVals, childIndex - 1, node_get_chave(leftVals, nLeft - 1));

    // Atualiza contadores
    node_set_n(childVals, nChild + 1);
    node_set_n(leftVals, nLeft - 1);

    writeNode(parentId, parentVals);
    writeNode(childId, childVals);
    writeNode(leftId, leftVals);
}

void ArvoreMVias::borrowFromRight(int parentId, int childIndex) {
    vector<int> parentVals, rightVals, childVals;
    readNode(parentId, parentVals);

    int childId = node_get_filho(parentVals, childIndex);
    int rightId = node_get_filho(parentVals, childIndex + 1);

    readNode(childId, childVals);
    readNode(rightId, rightVals);

    int nChild = node_get_n(childVals);
    int nRight = node_get_n(rightVals);

    // Move chave do pai para child
    node_set_chave(childVals, nChild, node_get_chave(parentVals, childIndex));

    // Move filho inicial de right para child se não for folha
    if (!node_get_folha(childVals))
        node_set_filho(childVals, nChild + 1, node_get_filho(rightVals, 0));

    // Move primeira chave de right para o pai
    node_set_chave(parentVals, childIndex, node_get_chave(rightVals, 0));

    // Shift chaves e filhos de right para esquerda
    for (int i = 0; i < nRight - 1; ++i) {
        node_set_chave(rightVals, i, node_get_chave(rightVals, i + 1));
    }
    for (int i = 0; i < nRight; ++i) {
        node_set_filho(rightVals, i, node_get_filho(rightVals, i + 1));
    }

    node_set_chave(rightVals, nRight - 1, 0);
    node_set_filho(rightVals, nRight, 0);

    // Atualiza contadores
    node_set_n(childVals, nChild + 1);
    node_set_n(rightVals, nRight - 1);

    writeNode(parentId, parentVals);
    writeNode(childId, childVals);
    writeNode(rightId, rightVals);
}

void ArvoreMVias::deleteB(int chave) {
    if (!readHeader()) {
        cerr << "Erro ao ler header antes da remocao.\n";
        return;
    }

    // Verifica se a chave existe no índice
    Resultado r = mSearch(chave);
    if (!r.encontrou) {
        cout << "Aviso: ID " << chave << " nao existe no indice. Remocao cancelada.\n";
        return;
    }

    // Chama a função recursiva de deleção
    deleteFromNode(raiz, chave);

    // Verifica se a raiz ficou vazia
    vector<int> rootVals;
    if (readNode(raiz, rootVals)) {
        if (node_get_n(rootVals) == 0 && !node_get_folha(rootVals)) {
            int newRootId = node_get_filho(rootVals, 0);
            deleteNode(raiz);
            raiz = newRootId;
            writeHeader();
        }
    }

    removeDataFromFile(chave);
    cout << "Remocao do ID " << chave << " concluida.\n";
}

void ArvoreMVias::deleteFromNode(int nodeId, int chave) {

    vector<int> nodeVals;
    readNode(nodeId, nodeVals);

    int n = node_get_n(nodeVals);
    int i = 0;

    // Encontra a chave ou a posição de descida
    while (i < n && chave > node_get_chave(nodeVals, i)) {
        i++;
    }

    // CASO 1: Chave encontrada neste nó
    if (i < n && chave == node_get_chave(nodeVals, i)) {
        if (node_get_folha(nodeVals)) {
            // Caso 1A: Remoção simples em folha

            removeFromLeaf(nodeVals, i);
            writeNode(nodeId, nodeVals);

            // *** CORREÇÃO: VERIFICAR UNDERFLOW NA FOLHA APÓS REMOÇÃO ***
            int nAfter = node_get_n(nodeVals);

            if (nAfter < T - 1) {
                // Precisamos encontrar o pai desta folha para corrigir o underflow
                int parentId = findParent(raiz, nodeId, 0);
                if (parentId != 0) {
                    int childIndex = findChildIndex(parentId, nodeId);
                    if (childIndex != -1) {
                        fillChild(parentId, childIndex);
                    }
                }
            }
        } else {
            // Caso 1B: Remoção em nó interno
            removeFromInternalNode(nodeId, i);
        }
    } else {
        // CASO 2: Chave não está neste nó - descer recursivamente
        if (node_get_folha(nodeVals)) {
            return;
        }

        int childId = node_get_filho(nodeVals, i);
        vector<int> childVals;
        readNode(childId, childVals);

        // Caso 2A: Filho tem número mínimo de chaves - garantir antes de descer
        if (node_get_n(childVals) == T - 1) {
            fillChild(nodeId, i);
            // Re-ler o nó após possíveis mudanças
            readNode(nodeId, nodeVals);
            // Recalcular childId após fill (pode ter mudado)
            childId = node_get_filho(nodeVals, i);
        }

        if (childId != 0) {
            deleteFromNode(childId, chave);
        }
    }
}

void ArvoreMVias::removeFromLeaf(vector<int>& vals, int idx) {
    int n = node_get_n(vals);

    // Desloca chaves para a esquerda
    for (int i = idx; i < n - 1; i++) {
        node_set_chave(vals, i, node_get_chave(vals, i + 1));
    }
    node_set_chave(vals, n - 1, 0);
    node_set_n(vals, n - 1);
}

void ArvoreMVias::removeFromInternalNode(int nodeId, int idx) {
    vector<int> nodeVals;
    readNode(nodeId, nodeVals);

    int chaveParaDeletar = node_get_chave(nodeVals, idx);

    int leftChildId = node_get_filho(nodeVals, idx);
    int rightChildId = node_get_filho(nodeVals, idx + 1);

    vector<int> leftVals, rightVals;
    readNode(leftChildId, leftVals);
    readNode(rightChildId, rightVals);

    int nLeft = node_get_n(leftVals);
    int nRight = node_get_n(rightVals);

    // Caso 2A: Filho esquerdo tem pelo menos T chaves
    if (nLeft >= T) {
        int predecessor = getPredecessor(leftChildId);
        node_set_chave(nodeVals, idx, predecessor);
        writeNode(nodeId, nodeVals);
        deleteFromNode(leftChildId, predecessor);
    }
    // Caso 2B: Filho direito tem pelo menos T chaves
    else if (nRight >= T) {
        int successor = getSuccessor(rightChildId);
        node_set_chave(nodeVals, idx, successor);
        writeNode(nodeId, nodeVals);
        deleteFromNode(rightChildId, successor);
    }
    // Caso 2C: Ambos os filhos têm T-1 chaves
    else {

        // *** CORREÇÃO: Verificar se o merge é possível antes de chamar ***
        if (nLeft + nRight + 1 <= M - 1) {
            mergeNodes(nodeId, idx);

            // Após merge, re-ler o nó (pode ter mudado drasticamente)
            if (readNode(nodeId, nodeVals)) {
                // Agora a chave foi movida para o filho esquerdo
                int mergedChildId = node_get_filho(nodeVals, idx);
                if (mergedChildId != 0) {
                    deleteFromNode(mergedChildId, chaveParaDeletar);
                }
            }
        } else {
            // *** CORREÇÃO ALTERNATIVA: Tentar redistribuição ***
            if (nLeft > 0) {
                int predecessor = getPredecessor(leftChildId);
                node_set_chave(nodeVals, idx, predecessor);
                writeNode(nodeId, nodeVals);
                deleteFromNode(leftChildId, predecessor);
            } else if (nRight > 0) {
                int successor = getSuccessor(rightChildId);
                node_set_chave(nodeVals, idx, successor);
                writeNode(nodeId, nodeVals);
                deleteFromNode(rightChildId, successor);
            }
        }
    }
}

void ArvoreMVias::fillChild(int parentId, int idx) {
    vector<int> parentVals;
    readNode(parentId, parentVals);

    int childId = node_get_filho(parentVals, idx);
    if (childId == 0) return;

    vector<int> childVals;
    readNode(childId, childVals);

    // Tenta emprestar do irmão esquerdo
    if (idx > 0) {
        int leftSiblingId = node_get_filho(parentVals, idx - 1);
        vector<int> leftVals;
        readNode(leftSiblingId, leftVals);

        if (node_get_n(leftVals) >= T) {
            borrowFromLeft(parentId, idx);
            return;
        }
    }

    // Tenta emprestar do irmão direito
    if (idx < node_get_n(parentVals)) {
        int rightSiblingId = node_get_filho(parentVals, idx + 1);
        vector<int> rightVals;
        readNode(rightSiblingId, rightVals);

        if (node_get_n(rightVals) >= T) {
            borrowFromRight(parentId, idx);
            return;
        }
    }

    // Se não pode emprestar, faz merge
    if (idx > 0) {
        mergeNodes(parentId, idx - 1);
    } else if (idx < node_get_n(parentVals)) {
        mergeNodes(parentId, idx);
    }
}

int ArvoreMVias::getPredecessor(int nodeId) {
    vector<int> vals;
    readNode(nodeId, vals);

    // Vai para o filho mais à direita até chegar em uma folha
    while (!node_get_folha(vals)) {
        int lastChild = node_get_filho(vals, node_get_n(vals));
        readNode(lastChild, vals);
    }

    // Retorna a última chave da folha
    return node_get_chave(vals, node_get_n(vals) - 1);
}

int ArvoreMVias::getSuccessor(int nodeId) {
    vector<int> vals;
    readNode(nodeId, vals);

    // Vai para o filho mais à esquerda até chegar em uma folha
    while (!node_get_folha(vals)) {
        int firstChild = node_get_filho(vals, 0);
        readNode(firstChild, vals);
    }

    // Retorna a primeira chave da folha
    return node_get_chave(vals, 0);
}

// Método mergeNodes ESPECÍFICO para M=3
// Método mergeNodes CORRIGIDO para M=3
void ArvoreMVias::mergeNodes(int parentId, int idx) {
    vector<int> parentVals;
    readNode(parentId, parentVals);

    int leftId = node_get_filho(parentVals, idx);
    int rightId = node_get_filho(parentVals, idx + 1);

    vector<int> leftVals, rightVals;
    readNode(leftId, leftVals);
    readNode(rightId, rightVals);

    int nLeft = node_get_n(leftVals);
    int nRight = node_get_n(rightVals);
    int parentKey = node_get_chave(parentVals, idx);

    // *** CORREÇÃO: Verificação de capacidade CORRETA ***
    // Para M=3, M-1=2 (máximo de chaves por nó)
    // left(nLeft) + right(nRight) + parentKey(1) <= M-1
    if (nLeft + nRight + 1 > M - 1) {
        return;
    }

    // Move a chave do pai para o filho esquerdo
    node_set_chave(leftVals, nLeft, parentKey);

    // Copia chaves do filho direito para o esquerdo
    for (int i = 0; i < nRight; i++) {
        node_set_chave(leftVals, nLeft + 1 + i, node_get_chave(rightVals, i));
    }

    // Copia filhos (se não forem folhas)
    if (!node_get_folha(leftVals)) {
        for (int i = 0; i <= nRight; i++) {
            node_set_filho(leftVals, nLeft + 1 + i, node_get_filho(rightVals, i));
        }
    }

    // Atualiza contador
    node_set_n(leftVals, nLeft + nRight + 1);

    // *** CORREÇÃO: Remove chave e ponteiro do pai usando a função auxiliar ***
    removeKeyAndPointer(parentVals, idx);

    // Atualiza nós
    writeNode(leftId, leftVals);
    writeNode(parentId, parentVals);

    // Remove nó direito
    deleteNode(rightId);

    // VERIFICAÇÃO CRÍTICA: Se o pai é a raiz e ficou vazio
    if (parentId == raiz && node_get_n(parentVals) == 0) {
        raiz = leftId;
        writeHeader();
        deleteNode(parentId);
    }
    // Se não é raiz e ficou com underflow
    else if (parentId != raiz && node_get_n(parentVals) < T - 1) {
        int grandParentId = findParent(raiz, parentId, 0);
        if (grandParentId != 0) {
            int parentIndex = findChildIndex(grandParentId, parentId);
            if (parentIndex != -1) {
                fillChild(grandParentId, parentIndex);
            }
        }
    }
}

int ArvoreMVias::findChildIndex(int parentId, int childId) {

    vector<int> parentVals;
    if (!readNode(parentId, parentVals)) return -1;

    int n = node_get_n(parentVals);

    for (int i = 0; i <= n; i++) {
        int currentChild = node_get_filho(parentVals, i);
        if (currentChild == childId) {
            return i;
        }
    }
    return -1;
}
