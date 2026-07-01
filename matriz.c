/* =============================================================================
   matriz.c
   Operações essenciais sobre a struct: inicialização, impressão e
   manipulação de linhas.
   ============================================================================= */

   #include <stdio.h>
   #include "algebra.h"

/* -----------------------------------------------------------------------------
   zerarMatriz

   Zera todos os campos da struct. Deve ser chamada antes do primeiro uso.
   ----------------------------------------------------------------------------- */

    void zerarMatriz(matriz *a) {

        a->linhas = 0;
        a->n_incognitas = 0;

        for (int i = 0; i < TAM_MAX; i++) {
            a->independentes[i] = 0.0;
            a->incognitas[i][0] = '\0';
            for (int j = 0; j < TAM_MAX; j++) {
                a->dados[i][j] = 0.0;
            }
        }
    }


/* -----------------------------------------------------------------------------
   printarMatriz

   Imprime a matriz como grade com colchetes, colunas alinhadas por largura uniforme.
   modo == 1 inclui os termos independentes.
   ----------------------------------------------------------------------------- */

  void printarMatriz(matriz *a, int modo) {

      int largura_print = larguraFormatacao(a, 1);

      for (int i = 0; i < a->linhas; i++) {
          printf("[ ");
          for (int j = 0; j < a->n_incognitas; j++) {
              printf("%*.2lf ", largura_print, a->dados[i][j]);
          }
          if (modo == 1) {
              printf("%*.2lf ", largura_print, a->independentes[i]);
          }
          printf("]\n");
      }
  }

/* -----------------------------------------------------------------------------
   trocarLinhas

   Troca as linhas l1 e l2 de posição dentro da matriz.
   ----------------------------------------------------------------------------- */

    void trocarLinhas(matriz *a, int l1, int l2) {

        double aux;

        for (int j = 0; j < a->n_incognitas; j++) {
            aux = a->dados[l1][j];
            a->dados[l1][j] = a->dados[l2][j];
            a->dados[l2][j] = aux;
        }
        aux = a->independentes[l1];
        a->independentes[l1] = a->independentes[l2];
        a->independentes[l2] = aux;
    }

/* -----------------------------------------------------------------------------
   combinarLinhas

   Executa l1 = l1 + (escalar * l2) elemento a elemento. l2 não é modificada.
   ----------------------------------------------------------------------------- */

    void combinarLinhas(matriz *a, int l1, int l2, double escalar) {

        for (int j = 0; j < a->n_incognitas; j++) {
            a->dados[l1][j] += escalar * a->dados[l2][j];
        }
        a->independentes[l1] += escalar * a->independentes[l2];
    }
