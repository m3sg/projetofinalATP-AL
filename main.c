#include "algebra.h"

#ifdef _WIN32
#include <windows.h>
#endif

int main () {

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