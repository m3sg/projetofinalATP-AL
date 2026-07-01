#include <stdio.h>
#include <string.h>
#include "algebra.h"

int main () {

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