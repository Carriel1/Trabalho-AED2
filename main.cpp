// Integrantes do grupo:
// Caio Monteiro Sartori   N° 15444598
// Mateus Henrique Carriel   N° 15698362
// Murilo Augusto Jorge   N° 15552251

// Arquivo main do trabalho, rodando o mSearch e insertB

#include <iostream>
#include <string>
#include "ArvoreMVias.h"

using namespace std;

int main() {
    int ordem;
    cout << "Digite a ordem da arvore B (M >= 3): ";
    cin >> ordem;
    if (ordem < 3) {
        cout << "Ordem precisa ser >= 3. Encerrando.\n";
        return 1;
    }

    // nomes de arquivo (podem ser alterados)
    ArvoreMVias arvore("mvias.txt", "mvias.bin", "dados.bin", ordem);

    // cria/garante estrutura binaria inicial (arquivo .bin com header e raiz vazia)
    arvore.geradorBinario();

    char opcao = 's';
    while (opcao == 's' || opcao == 'S') {
        cout << "\nMenu:\n";
        cout << "1 - Imprimir indice\n";
        cout << "2 - Imprimir arquivo principal\n";
        cout << "3 - Buscar elemento\n";
        cout << "4 - Inserir elemento\n";
        cout << "5 - Remover elemento\n"; // Nova opção de remoção
        cout << "0 - Sair\n";
        cout << "Escolha: ";
        int escolha;

        // Garante que a entrada numérica seja lida corretamente
        if (!(cin >> escolha)) {
            cout << "Entrada invalida. Encerrando.\n";
            break;
        }

        if (escolha == 1) arvore.imprimirIndice();
        else if (escolha == 2) arvore.imprimirArquivoPrincipal();
        else if (escolha == 3) {
            int chave;
            cout << "Digite a chave para buscar: ";
            if (!(cin >> chave)) continue;

            Resultado res = arvore.mSearch(chave);
            cout << "Resultado da busca (chave, no, pos, encontrou): ";
            cout << chave << " (" << res.indice_no << "," << res.posicao << "," << (res.encontrou ? "true" : "false") << ")\n";

            if (res.encontrou) { // mostrar conteúdo associado no arquivo de dados
                cout << "Conteudo(s) encontrado(s) no arquivo principal (inclui registros deletados):\n";
                arvore.imprimirArquivoPrincipal(chave); // imprime apenas linhas com a chave
            }
        }
        else if (escolha == 4) {
            int chave;
            string dados;
            cout << "Digite a chave para inserir: ";
            if (!(cin >> chave)) continue;

            cout << "Digite os dados (limite " << MAX_DATA_SIZE << " chars): ";
            cin.ignore(); // Limpa o buffer antes de usar getline
            getline(cin, dados);

            // Trunca os dados se excederem o tamanho máximo
            if (dados.length() > MAX_DATA_SIZE) {
                dados = dados.substr(0, MAX_DATA_SIZE);
            }
            arvore.insertB(chave, dados);
        }
        else if (escolha == 5) {
            int chave;
            cout << "Digite a chave para remover: ";
            if (!(cin >> chave)) continue;

            arvore.deleteB(chave);
        }
        else if (escolha == 0) break;
        else cout << "Opcao invalida.\n";

        cout << "\nDeseja continuar? (s/n): ";
        cin >> opcao;
    }

    return 0;
}
