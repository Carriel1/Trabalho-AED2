// ====================================================================================================================

// INTEGRANTES DO GRUPO:
// Caio Monteiro Sartori            N° 15444598
// Mateus Henrique J. S. Carriel    N° 15698362
// Murilo Augusto Jorge             N° 15552251

// ====================================================================================================================

// DEFINICAO DO ARVOREMVIAS_H:
#ifndef ARVOREMVIAS_H
#define ARVOREMVIAS_H

// ====================================================================================================================

// INCLUSAO DE BIBLIOTECAS (+ using namespace):
#include <iostream>    // Operacoes de Entradas e Saidas
#include <fstream>     // Manipulacao de Arquivos
#include <iomanip>     // Formatacao de Saida
#include <string>      // Utilizacao da Classe String
#include <vector>      // Vetores para representacao de Nohs em Memoria
#include <cstring>     // Funcoes de Manipulacao de Memoria (memset)
using namespace std;   // Simplificacao do Codigo

// ====================================================================================================================
// ESTRUTURA DOS REGISTROS: Inseridos pelo Usuario
#pragma pack(push, 1)
struct Registro {

    // ================================================================================================================
    // DECLARACAO DOS CAMPOS:
    int chave;                    // Chave Primaria do Registro (usada no Indice)
    char titulo[100];
    char autor[50];
    int ano;
    char genero[30];
    bool deletado;                // Flag para Remocao Logica
    // ================================================================================================================
    // CONSTRUTOR: Inicializa os Campos
    Registro() {
        chave = 0;
        ano = 0;
        deletado = false;
        memset(titulo, 0, 100);
        memset(autor, 0, 50);
        memset(genero, 0, 30);
    } // Fim do Construtor
    // ================================================================================================================
    // TAMANHO DO REGISTRO: Retorna o tamanho TOTAL em Bytes (Tamanho Fixo)
    static size_t getSize() {
        return sizeof(Registro);
    } // Fim do Tamanho do Registro
    // ================================================================================================================

}; // Fim da STRUCT
#pragma pack(pop)
// ====================================================================================================================

// ====================================================================================================================
// ESTRUTURA DOS RESULTADOS: Resultado das Buscas a serem realizadas
struct Resultado {

    // DECLARACAO DOS CAMPOS:
    int indice_no;    // ID do Noh onde a BUSCA TERMINOU (ou onde DEVERIA ESTAR)
    int posicao;      // Posição dentro do Noh (1-based) ou Posição de Inserção
    bool encontrou;   // Indica se a Chave foi Localizada

}; // Fim da STRUCT
// ====================================================================================================================

// ====================================================================================================================

// DECLARACAO DA CLASSE ArvoreMVias:
class ArvoreMVias {

// ====================================================================================================================
// METODOS PRIVADOS:
private:

    // ================================================================================================================
    // Nomeacao de Arquivos:
    string arquivoTxt;        // Nome do Arquivo nao usado (Legado)
    string arquivoBin;        // Arquivo de Indice da Arvore B
    string arquivoDados;      // Arquivo de Dados Principal
    // ================================================================================================================

    // ================================================================================================================
    // Variaveis Essenciais:
    static const int M = 3;                    // Ordem da Arvore (Maximo de Filhos)
    int T;                    // Grau Minimo (Minimo de Chaves eh T-1)
    int raiz;                 // ID do Noh Raiz
    int nextNodeId;           // Proximo ID Disponivel (1-based)
    // ================================================================================================================

    // ================================================================================================================
    // Metodos de Sequenciamento e I/O de Disco:
    int nodeInts() const;                              // Calcula o Numero de Inteiros que compoem um Noh no Disco.
    void writeHeader();                                // Escreve o Cabecalho da Arvore no Disco (M, raiz, nextNodeId).
    bool readHeader();                                 // Leh o Cabecalho da Arvore do Disco.
    void writeNode(int id, const vector<int>& vals);   // Escreve um Noh (Vetor de Ints) no Disco.
    bool readNode(int id, vector<int>& vals);          // Leh um Noh (Vetor de Ints) do Disco.
    // ================================================================================================================

    // ================================================================================================================
    // Metodos de Acesso/Manipulacao a Campos do Noh em Memoria:
    // Getters e Setters - para N, Folha, Chave e Filho.
    int node_get_n(const vector<int>& vals) const;
    void node_set_n(vector<int>& vals, int n);
    bool node_get_folha(const vector<int>& vals) const;
    void node_set_folha(vector<int>& vals, bool folha);
    int node_get_chave(const vector<int>& vals, int idx) const;
    void node_set_chave(vector<int>& vals, int idx, int chave);
    int node_get_filho(const vector<int>& vals, int idx) const;
    void node_set_filho(vector<int>& vals, int idx, int filho);
    // ================================================================================================================

    // ================================================================================================================
    // Metodos de Insercao (InsertB):
    int createNode(bool folha);                                 // Cria e inicializa um novo Noh no Disco.
    void insertNonFull(int nodeId, int chave);                  // Insercao Principal - lida com Split de Filho Cheio.
    // ================================================================================================================

    // ================================================================================================================
    // Metodos de Remocao no ARQUIVO DE DADOS:
    void removeDataFromFile(int chave);        // Encontra e marca o Registro como Deletado.
    void markAsDeleted(int chave);             // Implementacao da Remocao Logica.
    // ================================================================================================================

    // ================================================================================================================
    // Metodos de Remocao no INDICE (DeleteB):
    void removeKeyAndPointer(vector<int>& vals, int idx);        // Remove Chave K_idx e Ponteiro A_{idx+1} de um Noh na Memoria.
    void deleteFromNode(int nodeId, int chave);                  // Funcao Recursiva Principal para Remocao no Indice.
    void removeFromLeaf(vector<int>& vals, int idx);             // Remove Chave de um Noh Folha na Memoria.
    void removeFromInternalNode(int nodeId, int idx);            // Substitui a Chave por Predecessor/Sucessor ou faz Merge.
    void fillChild(int parentId, int idx);                       // Faz o Filho em Underflow (T-1) receber uma Chave (Empresta ou Funde).
    void mergeNodes(int parentId, int idx);                      // Funde 2 Nohs Irmaos e a Chave do Pai entre eles.
    int getPredecessor(int nodeId);                              // Encontra a MAIOR Chave na Subarvore a ESQUERDA.
    int getSuccessor(int nodeId);                                // Encontra a MENOR Chave na Subarvore a DIREITA.
    void borrowFromLeft(int parentId, int childIndex);           // Move Chave do Irmao ESQUERDO para o Noh em Underflow.
    void borrowFromRight(int parentId, int childIndex);          // Move Chave do Irmao DIREITO para o Noh em Underflow.
    void deleteNode(int nodeId);                                 // Limpa o conteudo de um Noh no Disco.
    int findParent(int currentNode, int targetId, int parentId); // Busca recursivamente o Pai de um Noh.
    int findChildIndex(int parentId, int childId);               // Encontra a Posicao (indice) do Ponteiro para o Filho.

// ====================================================================================================================

// ====================================================================================================================
// METODOS PUBLICOS:
public:

    // ================================================================================================================
    // CONSTRUTOR: Configura os Nomes de Arquivo e a Ordem (M).
    ArvoreMVias(const string& txt, const string& bin, const string& dados);
    // ================================================================================================================

    // ================================================================================================================
    // METODOS PRINCIPAIS [mSearch(1), insertB(2), deleteB(3)]:
    Resultado mSearch(int chave);                            // Busca uma Chave na Arvore B.
    void insertB(int chave, const string& titulo, const string& autor, int ano, const string& genero);    // Insere Chave e Dados.
    void deleteB(int chave);                                 // Remove Chave do Indice e Marca Dados como Deletados.
    // ================================================================================================================

    // ================================================================================================================
    // METODOS DE GERADOR DE ARQUIVO BINARIO E IMPRESSOES:
    void geradorBinario();                        // Inicializa ou Carrega a Estrutura do Indice Binario.
    void print();                                 // Imprime o Conteudo de Todos os Nohs do Indice.
    void imprimirIndice();                        // Imprime o Indice.
    void imprimirArquivoPrincipal();              // Imprime o Conteudo Completo do Arquivo de Dados.
    void imprimirArquivoPrincipal(int chave);     // Imprime Registros Ativos Associados a uma Chave.

// ====================================================================================================================

}; // Fim da Declaracao da Classe ArvoreMVias

#endif // Fim da Definicao da ARVOREMVIAS_H
