#include "algebra.h"

/*bloco condicional de pré-processamento (prévia a compilação), para verificar
  se está compilando em sistema Windows. Se estiver inclui a biblioteca
  windows.h */

#ifdef _WIN32
#include <windows.h>
#endif

int main () {

    /*verifica mais uma vez se está no windows, e se estiver chama duas funções
      que resolvem o problema de caracteres especiais no terminal windows. */

    #ifdef _WIN32
        SetConsoleOutputCP(CP_UTF8);
        SetConsoleCP(CP_UTF8);
    #endif

    matriz a = {0};
    zerarMatriz(&a);

    int op = 0;

    printarOpcoes();

    op = lerOpcao();
    while (op != 6) {
        int processou = processarOpcao(&a, op);
        if (processou) aguardar("\nPressione ENTER para voltar ao menu...");
        limparTela();
        printarOpcoes();
        op = lerOpcao();
    }

    return 0;
}