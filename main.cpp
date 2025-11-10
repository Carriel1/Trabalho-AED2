// ====================================================================================================================

// INTEGRANTES DO GRUPO:
// Caio Monteiro Sartori            N° 15444598
// Mateus Henrique J. S. Carriel    N° 15698362
// Murilo Augusto Jorge             N° 15552251

// ====================================================================================================================

// INCLUSAO DE BIBLIOTECAS E ARQUIVOS (+ using namespace):
#include <iostream>        // Operacoes de Entradas e Saidas
#include <string>          // Utilizacao da Classe String
#include "ArvoreMVias.h"   // Arquivo com Metodos Implementados
using namespace std;       // Simplificacao do Codigo

// ====================================================================================================================

// INICIO DA FUNCAO MAIN (PRINCIPAL):
int main() {

    cout << "Arvore B de ordem M = 3 (fixo)" << endl;

    // Instancia a classe ArvoreMVias com os nomes de arquivos
    // binários de índice e dados.
    ArvoreMVias arvore("mvias.txt", "mvias.bin", "dados.bin");
    // ================================================================================================================

    // ================================================================================================================

    // INICIALIZACAO DO INDICE BINARIO (mvias.bin):
    // Cria o Arquivo e a Raiz caso seja a primeira execucao,
    // ou leh o Cabecalho caso o Arquivo jah exista.

    arvore.geradorBinario();

    // ================================================================================================================

    // LOOP DO MENU PRINCIPAL:
    char opcao = 's';                        // Variavel de opcao para mantimento do loop
    while (opcao == 's' || opcao == 'S') {   // O loop se mantem enquanto opcao for s/S (sim)

        // Impressao do Menu:
        cout << "====================| MENU PRINCIPAL |====================\n" << endl;
        cout << "1 - Imprimir indice" << endl;
        cout << "2 - Imprimir arquivo principal" << endl;
        cout << "3 - Buscar livro" << endl;
        cout << "4 - Inserir livro" << endl;
        cout << "5 - Remover livro" << endl;
        cout << "0 - Sair" << endl;
        cout << "\nEscolha: ";
        int escolha;                         // Variavel de opcao para o que se deseja fazer do menu.

        // ============================================================================================================
        // Tratamento de Erro na Leitura
        if (!(cin >> escolha)) {
            cout << "ENTRADA INVALIDA. Encerrando programa...\n" << endl;
            cout << "========================================================" << endl;
            break;
        } // Fim do Tratamento de Erro
        // ============================================================================================================

        // ============================================================================================================
        // Entrada 1:
        // Exibe a estrutura do indice da Arvore B.
        if (escolha == 1){
            arvore.imprimirIndice();
        } // Fim da Entrada 1
        // ============================================================================================================

        // ============================================================================================================
        // Entrada 2:
        // Exibe todos os registros do arquivo de dados.
        else if (escolha == 2) {
            arvore.imprimirArquivoPrincipal();
        } // Fim da Entrada 2
        // ============================================================================================================

        // ============================================================================================================
        // Entrada 3:
        // Realiza uma busca na arvore.
        else if (escolha == 3) {
            // Solicita a chave a ser procurada.
            int chave;
            cout << "Digite o ID do livro para buscar: ";
            if (!(cin >> chave)) continue;

            // Realiza a busca com o mSearch.
            Resultado res = arvore.mSearch(chave);

            // Imprime o resultado obtido.
            cout << "Resultado da busca (chave, no, pos, encontrou): ";
            cout << chave << " (" << res.indice_no << "," << res.posicao << ","
                 << (res.encontrou ? "true" : "false") << ")\n";

            // Caso a chave tenha sido encontrada no indice, mostra o conteudo associado no arquivo
            // de dados, imprimindo apenas os registros do arquivo principal com a chave.
            if (res.encontrou) {
                cout << "Conteudo(s) encontrado(s) no arquivo principal:\n";
                arvore.imprimirArquivoPrincipal(chave);
            } // Fim do IF
        } // Fim da Entrada 3
        // ============================================================================================================

        // ============================================================================================================
        // Entrada 4:
        // Realiza a insercao de um item na arvore.
        else if (escolha == 4) {
            // Solicita a chave a ser inserida (ID):
            int chave, ano;
            string titulo, autor, genero;
            cout << "Digite o ID do livro a ser adicionado: ";
            if (!(cin >> chave)) continue;

            // Solicita o titulo do livro.
            cout << "Digite o titulo do livro: ";
            cin.ignore();            // Limpa o buffer para permitir a leitura de strings com
            getline(cin, titulo);    // espacos antes de usar o getline.

            // Solicita o autor do livro.
            cout << "Digite o autor do livro: ";
            cin.ignore();            // Limpa o buffer para permitir a leitura de strings com
            getline(cin, autor);     // espacos antes de usar o getline.

            // Solicita o ano de publicacao do livro.
            cout << "Digite o ano de publicacao do livro: ";
            if (!(cin >> ano)) continue;    // Caso a leitura falhe, volta ao inicio do loop.

            // Solicita o genero do livro.
            cout << "Digite o genero do livro: ";
            cin.ignore();            // Limpa o buffer para permitir a leitura de strings com
            getline(cin, genero);     // espacos antes de usar o getline.

            // Insere a Chave no Indice e o Registro no Arquivo de Dados
            arvore.insertB(chave, titulo, autor, ano, genero);
        } // Fim da Entrada 4
        // ============================================================================================================

        // ============================================================================================================
        // Entrada 5:
        // Realiza a remocao de um item na arvore.
        else if (escolha == 5) {
            // Solicita a chave a ser removida (ID):
            int chave;
            cout << "Digite o ID do livro para remover: ";
            if (!(cin >> chave)) continue;

            arvore.deleteB(chave);
        } // Fim da Entrada 5
        // ============================================================================================================

        // ============================================================================================================
        // Entrada 0:
        // Sai do loop e encerra o programa.
        else if (escolha == 0) {
            break;
        } // Fim da Entrada 0
        // ============================================================================================================

        // ============================================================================================================
        // Entrada Invalida:
        else {
            cout << "Opcao invalida.\n";
        } // Fim dos IF/ELSE (Variacoes de Entrada)
        // ============================================================================================================

        // ============================================================================================================
        // Mantimento do Loop:
        // Solicita a confirmacao quanto a continuacao.
        cout << "\nDeseja continuar? (s/n): ";
        cin >> opcao;    // Caso NAO seja digitado 's'/'S', sai do loop e encerra o programa.
        cout << endl;
        // ============================================================================================================

    } // Fim do WHILE

    return 0;    // Encerra a execucao

} // Fim da Funcao MAIN
