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
ArvoreMVias::ArvoreMVias(const string& txt, const string& bin, const string& dados, int ordem) {
    arquivoTxt = txt;       // Inicializa o nome do Arquivo de Texto.
    arquivoBin = bin;       // Inicializa o nome do Arquivo de Indice Binario (onde a Arvore eh Salva).
    arquivoDados = dados;   // Inicializa o nome do Arquivo de Dados Principal.
    M = ordem;              // Define a Ordem da Arvore (M).
    T = (M + 1) / 2;        // Define o Grau Minimo (T) necessario.
    raiz = 1;               // Inicializa o ID do Noh Raiz.
    nextNodeId = 1;         // Inicializa o ID do proximo Noh a ser Criado.
    // leituraDisco = 0;    // Leitura do Disco Nao Implementado.
    // escritaDisco = 0;    // Escrita no Disco Nao Implementado.

} // Fim do CONSTRUTOR
// ====================================================================================================================

// ====================================================================================================================
// METODOS DE SEQUENCIAMENTO E I/O DE DISCO:

// Calcula o Numero de Inteiros que compoem um Noh no Disco.
// FORMATO: [n (1 int), folha (1 int), chaves (M-1 ints), filhos (M ints)]
int ArvoreMVias::nodeInts() const {
    return 2 + (M - 1) + M; // 2 * M + 1
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

    // Alerta/Reset se a ordem no arquivo for diferente da ordem atual
    if (aM != M) {
        cerr << "Aviso: Ordem (M=" << aM << ") do indice e diferente da ordem atual (M=" << M << "). Reiniciando indice E arquivo de dados.\n";
        fin.close();

        // Define as variaveis de estado para 1, para que geradorBinario as use
        raiz = 1;
        nextNodeId = 1;
        T = (M + 1) / 2;

        ofstream foutDados(arquivoDados, ios::trunc);
        if (!foutDados) {
            cerr << "Erro fatal: Nao foi possivel reiniciar o arquivo de dados.\n";
            return false;
        }
        foutDados.close();

        return false;
    }
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
    // escritaDisco++; // Incrementa o contador de acesso a disco (escrita)
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
    // leituraDisco++; // Incrementa o contador de acesso a disco (leitura)
    return true;
}

// Getters/Setters do no serializado
int ArvoreMVias::node_get_n(const vector<int>& vals) const { return vals[0]; }
void ArvoreMVias::node_set_n(vector<int>& vals, int n) { vals[0] = n; }
bool ArvoreMVias::node_get_folha(const vector<int>& vals) const { return vals[1] != 0; }
void ArvoreMVias::node_set_folha(vector<int>& vals, bool folha) { vals[1] = folha ? 1 : 0; }
int ArvoreMVias::node_get_chave(const vector<int>& vals, int idx) const { return vals[2 + idx]; }
void ArvoreMVias::node_set_chave(vector<int>& vals, int idx, int chave) { vals[2 + idx] = chave; }
int ArvoreMVias::node_get_filho(const vector<int>& vals, int idx) const { return vals[2 + (M - 1) + idx]; }
void ArvoreMVias::node_set_filho(vector<int>& vals, int idx, int filho) { vals[2 + (M - 1) + idx] = filho; }

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
        cout << "Arquivo binario de indice inicializado com raiz vazia.\n";
    } else {
        cout << "Arquivo binario existente lido. Raiz ID=" << raiz << " Proximo ID=" << nextNodeId << "\n";
    }
}

void ArvoreMVias::print() {
    // Reseta contadores antes da impressao para isolar a operacao
    // leituraDisco = 0;
    // escritaDisco = 0;

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
    // leituraDisco = 0;
    // escritaDisco = 0;

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

void ArvoreMVias::insertNonFull(int nodeId, int chave) {
    vector<int> nodeVals;
    readNode(nodeId, nodeVals);

    cout << "DEBUG insertNonFull: nó " << nodeId << ", folha=" << node_get_folha(nodeVals)
         << ", n=" << node_get_n(nodeVals) << ", chave=" << chave << endl;

    if (node_get_folha(nodeVals)) {
        // INSERÇÃO DIRETA EM FOLHA
        int n = node_get_n(nodeVals);
        int pos = 0;

        // Encontra posição de inserção
        while (pos < n && node_get_chave(nodeVals, pos) < chave) {
            pos++;
        }

        cout << "Inserindo na folha " << nodeId << " na posição " << pos << endl;

        // Shift para direita
        for (int i = n; i > pos; i--) {
            node_set_chave(nodeVals, i, node_get_chave(nodeVals, i - 1));
        }

        // Insere
        node_set_chave(nodeVals, pos, chave);
        node_set_n(nodeVals, n + 1);
        writeNode(nodeId, nodeVals);

        cout << "Folha " << nodeId << " agora tem " << node_get_n(nodeVals) << " chaves: ";
        for (int i = 0; i < node_get_n(nodeVals); i++) {
            cout << node_get_chave(nodeVals, i) << " ";
        }
        cout << endl;

    } else {
        // NÓ INTERNO - ENCONTRA FILHO
        int n = node_get_n(nodeVals);
        int childIndex = 0;

        while (childIndex < n && chave > node_get_chave(nodeVals, childIndex)) {
            childIndex++;
        }

        int childId = node_get_filho(nodeVals, childIndex);
        cout << "Descendo para filho " << childId << " (índice " << childIndex << ")" << endl;

        vector<int> childVals;
        readNode(childId, childVals);

        // *** CORREÇÃO: VERIFICAR SE PRECISAMOS INSERIR PRIMEIRO E DEPOIS SPLIT ***
        // SE FILHO ESTÁ CHEIO, FAZ SPLIT ESPECIAL QUE INSERE E DEPOIS DIVIDE
        if (node_get_n(childVals) == M - 1) {
            cout << "Filho " << childId << " está cheio. Fazendo split especial..." << endl;

            // *** NOVA LÓGICA: INSERIR NO FILHO E DEPOIS FAZER SPLIT ***
            vector<int> tempChildVals = childVals; // Cópia temporária
            int temp_n = node_get_n(tempChildVals);

            // Encontra posição para inserir a nova chave
            int pos = 0;
            while (pos < temp_n && chave > node_get_chave(tempChildVals, pos)) {
                pos++;
            }

            // Shift para direita
            for (int i = temp_n; i > pos; i--) {
                node_set_chave(tempChildVals, i, node_get_chave(tempChildVals, i - 1));
            }

            // Insere a chave temporariamente
            node_set_chave(tempChildVals, pos, chave);
            node_set_n(tempChildVals, temp_n + 1);

            cout << "DEBUG: Nó temporário após inserção de " << chave << ": ";
            for (int i = 0; i < node_get_n(tempChildVals); i++) {
                cout << node_get_chave(tempChildVals, i) << " ";
            }
            cout << endl;

            // *** AGORA FAZ SPLIT COM TODAS AS CHAVES (INCLUINDO A NOVA) ***
            int t = T;
            // *** CORREÇÃO: medianaIndex deve ser t-1, não t ***
            int medianaIndex = t - 1;  // Para M=3, t=2 → medianaIndex=1 (valor do meio)
            int med = node_get_chave(tempChildVals, medianaIndex);

            cout << "Mediana index: " << medianaIndex << ", valor: " << med << endl;

            // Cria novo nó
            int zId = createNode(node_get_folha(tempChildVals));
            vector<int> zVals;
            readNode(zId, zVals);

            // Nó original (child) fica com as primeiras t-1 chaves
            node_set_n(childVals, medianaIndex);  // t-1 chaves
            for (int i = 0; i < medianaIndex; ++i) {
                node_set_chave(childVals, i, node_get_chave(tempChildVals, i));
            }
            // Zera chaves restantes
            for (int i = medianaIndex; i < M - 1; ++i) {
                node_set_chave(childVals, i, 0);
            }

            // *** CORREÇÃO: Novo nó (z) fica com as chaves APÓS a mediana ***
            int chavesRestantes = (temp_n + 1) - medianaIndex - 1;  // Total - mediana - já copiadas
            node_set_n(zVals, chavesRestantes);
            for (int j = 0; j < chavesRestantes; ++j) {
                node_set_chave(zVals, j, node_get_chave(tempChildVals, j + medianaIndex + 1));
            }

            // Move filhos se não for folha
            if (!node_get_folha(tempChildVals)) {
                for (int j = 0; j <= chavesRestantes; ++j) {
                    node_set_filho(zVals, j, node_get_filho(tempChildVals, j + medianaIndex + 1));
                }
            }

            // *** DEBUG: Mostrar estado antes de inserir no parent ***
            cout << "DEBUG ANTES DE INSERIR NO PARENT:" << endl;
            cout << "Parent (nó " << nodeId << ") n atual: " << node_get_n(nodeVals) << endl;
            cout << "Child (nó " << childId << "): ";
            for (int i = 0; i < node_get_n(childVals); i++) {
                cout << node_get_chave(childVals, i) << " ";
            }
            cout << endl;
            cout << "Novo nó (nó " << zId << "): ";
            for (int i = 0; i < node_get_n(zVals); i++) {
                cout << node_get_chave(zVals, i) << " ";
            }
            cout << endl;
            cout << "Mediana a ser promovida: " << med << endl;

            // Insere mediana no parent
            int parent_n = node_get_n(nodeVals);

            // Shift para direita no parent
            for (int j = parent_n; j > childIndex; --j) {
                node_set_chave(nodeVals, j, node_get_chave(nodeVals, j - 1));
            }
            for (int j = parent_n + 1; j > childIndex + 1; --j) {
                node_set_filho(nodeVals, j, node_get_filho(nodeVals, j - 1));
            }

            node_set_chave(nodeVals, childIndex, med);
            node_set_filho(nodeVals, childIndex + 1, zId);
            node_set_n(nodeVals, parent_n + 1);

            // Write back
            writeNode(childId, childVals);
            writeNode(zId, zVals);
            writeNode(nodeId, nodeVals);

            cout << "Split especial concluído. Mediana " << med << " promovida." << endl;
            cout << "Nó " << childId << " agora tem: ";
            for (int i = 0; i < node_get_n(childVals); i++) {
                cout << node_get_chave(childVals, i) << " ";
            }
            cout << endl;
            cout << "Nó " << zId << " agora tem: ";
            for (int i = 0; i < node_get_n(zVals); i++) {
                cout << node_get_chave(zVals, i) << " ";
            }
            cout << endl;

        } else {
            // Filho não está cheio, insere normalmente
            insertNonFull(childId, chave);
        }
    }
}

// Requisito D: InsertB
void ArvoreMVias::insertB(int chave, const string& dadosElemento) {
    // Zera contadores para contar apenas a insercao
    // leituraDisco = 0;
    // escritaDisco = 0;

    // 1. Verifica se a chave ja existe no indice
    Resultado r = mSearch(chave);
    if (r.encontrou) {
        cout << "Aviso: Chave " << chave << " ja existe no indice. Insercao cancelada.\n";
        return;
    }

    // 2. Escreve no arquivo principal
    ofstream foutDados(arquivoDados, ios::binary | ios::app);
    if (!foutDados) {
        cerr << "Erro ao abrir " << arquivoDados << " para escrita.\n";
        return;
    }

    Registro novoReg;
    novoReg.chave = chave;
    size_t len = min(dadosElemento.length(), (size_t)MAX_DATA_SIZE - 1);
    dadosElemento.copy(novoReg.dados, len);
    novoReg.dados[len] = '\0';
    novoReg.deletado = false;

    foutDados.write(reinterpret_cast<const char*>(&novoReg), Registro::getSize());
    foutDados.close();

    // 3. Insere na arvore B (arquivo bin)
    if (!readHeader()) {
        cerr << "Erro ao ler header antes de inserir.\n";
        return;
    }

    vector<int> rootVals;
    readNode(raiz, rootVals);

    cout << "=== INICIANDO INSERÇÃO DA CHAVE " << chave << " ===" << endl;
    cout << "Raiz atual: " << raiz << ", n=" << node_get_n(rootVals) << endl;

    // Se raiz está cheia
    if (node_get_n(rootVals) == M - 1) {
        cout << "Raiz cheia. Criando nova raiz..." << endl;

        // Cria nova raiz
        int newRootId = createNode(false);
        vector<int> newRootVals;
        readNode(newRootId, newRootVals);

        // Faz a raiz antiga ser filho da nova raiz
        node_set_filho(newRootVals, 0, raiz);
        writeNode(newRootId, newRootVals);

        // Atualiza raiz
        raiz = newRootId;
        writeHeader();

        cout << "Nova raiz criada: " << raiz << endl;

        // *** AGORA chama insertNonFull que fará o split especial ***
        insertNonFull(raiz, chave);
    } else {
        insertNonFull(raiz, chave);
    }

    cout << "Insercao da chave " << chave << " realizada com sucesso no indice e no arquivo principal.\n";

    // Mostra estrutura final
    cout << "=== ESTRUTURA FINAL APÓS INSERÇÃO ===" << endl;
    print();
}

// Requisito A: Imprime o indice (e contadores)
void ArvoreMVias::imprimirIndice() {
    print(); // Imprime o indice
    // cout << "\n--- Contadores de Acesso a Disco ---\n";
    // cout << "Numero de leituras de disco: " << leituraDisco << endl;
    // cout << "Numero de escritas de disco: " << escritaDisco << endl;
}

// Requisito B: Imprime todo o arquivo principal
void ArvoreMVias::imprimirArquivoPrincipal() {
    ifstream fin(arquivoDados, ios::binary);
    if (!fin) {
        cout << "Arquivo de dados vazio ou nao existe (" << arquivoDados << ").\n";
        return;
    }
    Registro reg;
    cout << "\n--- Conteudo COMPLETO do Arquivo Principal BINARIO (" << arquivoDados << ") ---\n";
    cout << "Chave | Deletado | Dados\n";
    cout << "------------------------------------------------------------------\n";

    while (fin.read(reinterpret_cast<char*>(&reg), reg.getSize())) {
        // Exibe apenas se não estiver deletado (ou exibe todos, para debug)
        cout << setw(5) << reg.chave
             << " | " << setw(8) << (reg.deletado ? "TRUE" : "FALSE")
             << " | " << reg.dados << endl;
    }
    fin.close();
}

// Requisito C: Imprime o conteudo do arquivo principal associado a chave
void ArvoreMVias::imprimirArquivoPrincipal(int chave) {
    ifstream fin(arquivoDados, ios::binary);
    if (!fin) {
        cerr << "Erro: Arquivo de dados nao existe ou nao pode ser aberto.\n";
        return;
    }
    Registro reg;
    bool found = false;

    cout << "Chave | Deletado | Dados\n";
    cout << "------------------------------------------------------------------\n";

    while (fin.read(reinterpret_cast<char*>(&reg), reg.getSize())) {
        if (reg.chave == chave && !reg.deletado) {
            cout << setw(5) << reg.chave
                 << " | " << setw(8) << (reg.deletado ? "TRUE" : "FALSE")
                 << " | " << reg.dados << endl;
            found = true;
        }
    }
    if (!found) cout << "Nenhum registro ativo com a chave " << chave << " encontrado no arquivo principal.\n";
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
        cout << "Registro com chave " << chave << " marcado como deletado no arquivo principal (acesso direto a disco).\n";
    } else {
        cerr << "Aviso: Registro ATIVO para a chave " << chave << " nao foi encontrado no arquivo principal.\n";
    }
    // escritaDisco++; // Conta o acesso de escrita
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
    // opcional: deixar folha = 1
    if (empty.size() >= 2) empty[1] = 1;
    writeNode(nodeId, empty);
    cout << "[DIAG] deleteNode: limpo nó " << nodeId << endl;
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
        cerr << "Erro ao ler header antes da deleção.\n";
        return;
    }

    // Verifica se a chave existe no índice
    Resultado r = mSearch(chave);
    if (!r.encontrou) {
        cout << "Aviso: Chave " << chave << " não existe no índice. Deleção cancelada.\n";
        return;
    }

    cout << "=== INICIANDO DELEÇÃO da chave " << chave << " ===\n";
    cout << "Encontrada no nó " << r.indice_no << ", posição " << r.posicao << "\n";

    // Chama a função recursiva de deleção
    deleteFromNode(raiz, chave);

    // Verifica se a raiz ficou vazia
    vector<int> rootVals;
    if (readNode(raiz, rootVals)) {
        if (node_get_n(rootVals) == 0 && !node_get_folha(rootVals)) {
            int newRootId = node_get_filho(rootVals, 0);
            cout << "Raiz " << raiz << " ficou vazia. Substituindo por filho " << newRootId << "\n";
            deleteNode(raiz);
            raiz = newRootId;
            writeHeader();
        }
    }

    removeDataFromFile(chave);
    cout << "Deleção da chave " << chave << " concluída.\n";
    cout << "=== ESTRUTURA FINAL ===\n";
    print();
}

void ArvoreMVias::deleteFromNode(int nodeId, int chave) {
    cout << "DEBUG: M=" << M << ", T=" << T << ", T-1=" << T-1 << endl;

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
            cout << "Removendo chave " << chave << " da folha " << nodeId << endl;

            // *** GUARDA O ESTADO ANTES DA REMOÇÃO ***
            int nBefore = node_get_n(nodeVals);

            removeFromLeaf(nodeVals, i);
            writeNode(nodeId, nodeVals);

            // *** CORREÇÃO: VERIFICAR UNDERFLOW NA FOLHA APÓS REMOÇÃO ***
            int nAfter = node_get_n(nodeVals);
            cout << "Folha " << nodeId << ": nBefore=" << nBefore << ", nAfter=" << nAfter << ", T-1=" << T-1 << endl;

            if (nAfter < T - 1) {
                cout << "=== UNDERFLOW DETECTADO na folha " << nodeId << " ===" << endl;
                // Precisamos encontrar o pai desta folha para corrigir o underflow
                int parentId = findParent(raiz, nodeId, 0);
                if (parentId != 0) {
                    int childIndex = findChildIndex(parentId, nodeId);
                    if (childIndex != -1) {
                        cout << "Pai encontrado: " << parentId << ", childIndex: " << childIndex << endl;
                        cout << "Chamando fillChild..." << endl;
                        fillChild(parentId, childIndex);
                    } else {
                        cout << "ERRO: Índice do filho não encontrado" << endl;
                    }
                } else {
                    cout << "Folha " << nodeId << " é a raiz ou pai não encontrado" << endl;
                    if (nodeId == raiz) {
                        cout << "Folha É a raiz. Nada a fazer." << endl;
                    }
                }
            } else {
                cout << "Sem underflow na folha " << nodeId << endl;
            }
        } else {
            // Caso 1B: Remoção em nó interno
            cout << "Removendo chave " << chave << " do nó interno " << nodeId << endl;
            removeFromInternalNode(nodeId, i);
        }
    } else {
        // CASO 2: Chave não está neste nó - descer recursivamente
        if (node_get_folha(nodeVals)) {
            cout << "Erro: Chave " << chave << " não encontrada na folha " << nodeId << "\n";
            return;
        }

        int childId = node_get_filho(nodeVals, i);
        vector<int> childVals;
        readNode(childId, childVals);

        // Caso 2A: Filho tem número mínimo de chaves - garantir antes de descer
        if (node_get_n(childVals) == T - 1) {
            cout << "Filho " << childId << " tem mínimo de chaves. Executando fillChild...\n";
            fillChild(nodeId, i);
            // Re-ler o nó após possíveis mudanças
            readNode(nodeId, nodeVals);
            // Recalcular childId após fill (pode ter mudado)
            childId = node_get_filho(nodeVals, i);
        }

        if (childId != 0) {
            cout << "Descendo para filho " << childId << " na busca pela chave " << chave << "\n";
            deleteFromNode(childId, chave);
        }
    }
}

void ArvoreMVias::removeFromLeaf(vector<int>& vals, int idx) {
    int n = node_get_n(vals);
    cout << "removeFromLeaf: n=" << n << ", idx=" << idx << endl;

    // Desloca chaves para a esquerda
    for (int i = idx; i < n - 1; i++) {
        node_set_chave(vals, i, node_get_chave(vals, i + 1));
    }
    node_set_chave(vals, n - 1, 0);
    node_set_n(vals, n - 1);

    cout << "removeFromLeaf: novo n=" << node_get_n(vals) << endl;
}

void ArvoreMVias::removeFromInternalNode(int nodeId, int idx) {
    vector<int> nodeVals;
    readNode(nodeId, nodeVals);

    int chaveParaDeletar = node_get_chave(nodeVals, idx);
    cout << "DEBUG removeFromInternalNode: nó=" << nodeId << ", idx=" << idx
         << ", chave=" << chaveParaDeletar << endl;

    int leftChildId = node_get_filho(nodeVals, idx);
    int rightChildId = node_get_filho(nodeVals, idx + 1);

    cout << "DEBUG: leftChild=" << leftChildId << ", rightChild=" << rightChildId << endl;

    vector<int> leftVals, rightVals;
    readNode(leftChildId, leftVals);
    readNode(rightChildId, rightVals);

    int nLeft = node_get_n(leftVals);
    int nRight = node_get_n(rightVals);

    cout << "DEBUG: left.n=" << nLeft << ", right.n=" << nRight << ", T=" << T << endl;

    // Caso 2A: Filho esquerdo tem pelo menos T chaves
    if (nLeft >= T) {
        int predecessor = getPredecessor(leftChildId);
        cout << "Caso 2A: Predecessor = " << predecessor << endl;
        node_set_chave(nodeVals, idx, predecessor);
        writeNode(nodeId, nodeVals);
        deleteFromNode(leftChildId, predecessor);
    }
    // Caso 2B: Filho direito tem pelo menos T chaves
    else if (nRight >= T) {
        int successor = getSuccessor(rightChildId);
        cout << "Caso 2B: Successor = " << successor << endl;
        node_set_chave(nodeVals, idx, successor);
        writeNode(nodeId, nodeVals);
        deleteFromNode(rightChildId, successor);
    }
    // Caso 2C: Ambos os filhos têm T-1 chaves
    else {
        cout << "Caso 2C: Fazendo merge dos filhos..." << endl;
        mergeNodes(nodeId, idx);

        // Após merge, re-ler o nó (pode ter mudado drasticamente)
        if (readNode(nodeId, nodeVals)) {
            // Agora a chave foi movida para o filho esquerdo
            int mergedChildId = node_get_filho(nodeVals, idx);
            if (mergedChildId != 0) {
                cout << "Deletando recursivamente do filho merged " << mergedChildId << endl;
                deleteFromNode(mergedChildId, chaveParaDeletar);
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

    cout << "fillChild: parent=" << parentId << ", idx=" << idx
         << ", child=" << childId << ", child.n=" << node_get_n(childVals) << endl;

    // Tenta emprestar do irmão esquerdo
    if (idx > 0) {
        int leftSiblingId = node_get_filho(parentVals, idx - 1);
        vector<int> leftVals;
        readNode(leftSiblingId, leftVals);

        if (node_get_n(leftVals) >= T) {
            cout << "Emprestando do irmão esquerdo " << leftSiblingId << endl;
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
            cout << "Emprestando do irmão direito " << rightSiblingId << endl;
            borrowFromRight(parentId, idx);
            return;
        }
    }

    // Se não pode emprestar, faz merge
    cout << "Não pode emprestar. Fazendo merge..." << endl;
    if (idx > 0) {
        mergeNodes(parentId, idx - 1);
    } else if (idx < node_get_n(parentVals)) {
        mergeNodes(parentId, idx);
    } else {
        cout << "ERRO: Não foi possível fazer merge - índices inválidos" << endl;
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

    cout << "DEBUG MERGE: left=" << leftId << "(" << nLeft << "), right=" << rightId
         << "(" << nRight << "), parentKey=" << parentKey << endl;

    // PARA M=3: máximo 2 chaves por nó
    if (nLeft + nRight + 1 > M - 1) {
        cerr << "ERRO: Merge excederia capacidade máxima do nó!\n";
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

    // Remove chave e ponteiro do pai
    removeKeyAndPointer(parentVals, idx);

    cout << "DEBUG: Após merge - left.n=" << node_get_n(leftVals)
         << ", parent.n=" << node_get_n(parentVals) << endl;

    // Atualiza nós
    writeNode(leftId, leftVals);
    writeNode(parentId, parentVals);

    // Remove nó direito
    deleteNode(rightId);

    // VERIFICAÇÃO CRÍTICA: Se o pai é a raiz e ficou vazio
    if (parentId == raiz && node_get_n(parentVals) == 0) {
        cout << "Raiz " << parentId << " ficou vazia após merge. Promovendo filho " << leftId << " como nova raiz.\n";
        raiz = leftId;
        writeHeader();
        deleteNode(parentId);
    }
    // Se não é raiz e ficou com underflow
    else if (parentId != raiz && node_get_n(parentVals) < T - 1) {
        cout << "Pai " << parentId << " com underflow. Buscando avô...\n";
        int grandParentId = findParent(raiz, parentId, 0);
        if (grandParentId != 0) {
            int parentIndex = findChildIndex(grandParentId, parentId);
            if (parentIndex != -1) {
                cout << "Chamando fillChild no avô " << grandParentId << " para índice " << parentIndex << endl;
                fillChild(grandParentId, parentIndex);
            }
        }
    }
}

int ArvoreMVias::findParent(int currentNode, int targetId, int parentId) {
    cout << "findParent: current=" << currentNode << ", target=" << targetId << ", parent=" << parentId << endl;

    if (currentNode == targetId) {
        cout << "Encontrado! Pai é: " << parentId << endl;
        return parentId;
    }
    if (currentNode == 0) return 0;

    vector<int> vals;
    if (!readNode(currentNode, vals)) return 0;

    if (node_get_folha(vals)) return 0;

    int n = node_get_n(vals);
    cout << "Buscando nos " << n+1 << " filhos do nó " << currentNode << endl;

    for (int i = 0; i <= n; i++) {
        int childId = node_get_filho(vals, i);
        cout << "Filho[" << i << "] = " << childId << endl;
        if (childId == targetId) {
            cout << "Encontrado pai direto: " << currentNode << " para filho " << targetId << endl;
            return currentNode;
        }
        if (childId != 0) {
            int result = findParent(childId, targetId, currentNode);
            if (result != 0) return result;
        }
    }
    return 0;
}

int ArvoreMVias::findChildIndex(int parentId, int childId) {
    cout << "findChildIndex: parent=" << parentId << ", child=" << childId << endl;

    vector<int> parentVals;
    if (!readNode(parentId, parentVals)) return -1;

    int n = node_get_n(parentVals);
    cout << "Procurando em " << n+1 << " filhos do pai " << parentId << endl;

    for (int i = 0; i <= n; i++) {
        int currentChild = node_get_filho(parentVals, i);
        cout << "Filho[" << i << "] = " << currentChild << endl;
        if (currentChild == childId) {
            cout << "Encontrado no índice: " << i << endl;
            return i;
        }
    }
    cout << "Filho não encontrado!" << endl;
    return -1;
}
