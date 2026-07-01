/* =============================================================================
   autovaloresAutovetores.c
       Cálculo de autovalores via polinômio característico, e autovetores por
       escalonamento do sistema (A - λI)x = 0. Contém também a função de entrada
       que orquestra o fluxo completo de exibição.
   ============================================================================= */

#include "algebra.h"
#include <math.h>
#include <stdio.h>
#include <string.h>

/* -----------------------------------------------------------------------------
   autovalores

   O que faz:
     calcula os autovalores reais de uma matriz 2x2 resolvendo o polinômio
     característico det(A - λI) = 0 e os armazena nos ponteiros recebidos.
     Imprime o polinômio e o discriminante durante o processo.

   Como funciona:
     Monta os coeficientes da equação quadrada λ² - tr(A)λ + det(A) = 0 e
     calcula o discriminante, que se não negativo, resolve pela fórmula de Bhaskara,
     atribuindo as raízes a lambda1 e lambda2. Se o discriminante for negativo, as
     raízes são complexas e a função retorna 0 sem atribuir valores.

   Parâmetros:
     - matriz *a      : ponteiro para a matriz 2x2.
     - double *lambda1: ponteiro de saída para o primeiro autovalor.
     - double *lambda2: ponteiro de saída para o segundo autovalor.

   Retorno:
     1 se os autovalores são reais, 0 se o discriminante for negativo.
   ----------------------------------------------------------------------------- */

int autovalores(matriz *a, double *lambda1, double *lambda2) {

    double a1, b2, c3, discriminante;

    a1 = 1;
    b2 = -(a->dados[0][0] + a->dados[1][1]);
    c3 = ((a->dados[0][0] * a->dados[1][1]) - (a->dados[0][1] * a->dados[1][0]));
    discriminante = (b2 * b2) - 4 * (a1 * c3);

    printf("* POLINÔMIO CARACTERÍSTICO: λ² - (%.2lf)λ + (%.2lf) = 0\n", -b2, c3);
    printf("* DISCRIMINANTE: %.2lf\n", discriminante);

    if (discriminante < 0) {
        return 0;
    } else {
        double x1 = (-b2 + sqrt(discriminante))/(2 * a1);
        double x2 = (-b2 - sqrt(discriminante))/(2 * a1);
        *lambda1 = x1;
        *lambda2 = x2;
        return 1;
    }
}

/* -----------------------------------------------------------------------------
   autovetores

   O que faz:
     calcula o autovetor associado a um autovalor dado, resolvendo o sistema
     (A - λI)x = 0 por escalonamento, e imprime o resultado.

   Como funciona:
     1. Constrói a matriz (A - λI) subtraindo lambda da diagonal principal.
     2. Escalona essa matriz via eliminacaoGauss(resultado, 0).
     3. Delega a extração do autovetor a resolverAutovetor, que resolve o
        sistema escalonado com uma variável livre fixada em 1.
     4. Copia o autovetor para autovetor_saida e imprime seus componentes.

   Parâmetros:
     - matriz *a              : ponteiro para a matriz original A.
     - double lambda          : autovalor cujo autovetor será calculado.
     - double *autovetor_saida: ponteiro de saída para o autovetor (tamanho 2).
     - int indice             : índice do autovalor (1 ou 2), usado apenas na
                                impressão.
   ----------------------------------------------------------------------------- */

void autovetores(matriz *a, double lambda, double *autovetor_saida, int indice) {

    double autovetor[2] = {0};
    matriz resultado;
    zerarMatriz(&resultado);

    for (int i = 0; i < a->linhas; i++) {
        for (int j = 0; j < a->n_incognitas; j++) {
            if (i == j) {
                resultado.dados[i][j] = (a->dados[i][j] - lambda);
            } else {
                resultado.dados[i][j] = a->dados[i][j];
            }
        }
    }

    resultado.linhas = a->linhas;
    resultado.n_incognitas = a->n_incognitas;

    for (int i = 0; i < a->n_incognitas; i++) {
        strcpy(resultado.incognitas[i], a->incognitas[i]);
    }

    resultado = eliminacaoGauss(&resultado, 0);
    resolverAutovetor(&resultado, autovetor);

    autovetor_saida[0] = autovetor[0];
    autovetor_saida[1] = autovetor[1];

    printf("* AUTOVETOR DE λ%d: (%.2lf, %.2lf)\n", indice, autovetor[0], autovetor[1]);
}

/* -----------------------------------------------------------------------------
   resolverAutovetor

   O que faz:
     extrai um autovetor de uma matriz 2x2 já escalonada, atribuindo t=1 à
     variável livre e resolvendo a variável pivô por substituição retroativa.

   Como funciona:
     Percorre a primeira linha para identificar o pivô (primeiro elemento não
     nulo). A variável oposta ao pivô é a livre e recebe t=1. O valor do pivô
     é calculado isolando-o na equação da linha pivô.

   Parâmetros:
     - matriz *a        : ponteiro para a matriz já escalonada de (A - λI).
     - double *autovetor: ponteiro de saída para o autovetor extraído (tamanho 2).
   ----------------------------------------------------------------------------- */

void resolverAutovetor(matriz *a, double *autovetor) {
    double resultado[2] = {0};
    int pivo = -1, livre = -1;

    for (int i = 0; i < a->linhas; i++) {
        for (int j = 0; j < a->n_incognitas; j++) {
            if (fabs(a->dados[i][j]) > 1e-9){
                pivo = j;
                break;
            }
        }
    }

    if (pivo == 0) {
        livre = 1;
    } else {
        livre = 0;
    }

    resultado[livre] = 1.0;
    resultado[pivo] = (a->independentes[0] - a->dados[0][livre] * resultado[livre])/a->dados[0][pivo];

    autovetor[0] = resultado[0];
    autovetor[1] = resultado[1];
}

/* -----------------------------------------------------------------------------
   avaliarAutovaloresAutovetores

   O que faz:
     ponto de entrada para o cálculo e exibição completos de autovalores e
     autovetores de uma matriz 2x2.

   Como funciona:
     Chama autovalores para obter λ1 e λ2. Se reais, chama
     autovetores para cada um, imprimindo o autovalor e seu autovetor
     associado. Se o discriminante for negativo, exibe que não há autovalores
     reais.

   Parâmetros:
     - matriz *a: ponteiro para a matriz 2x2 de entrada.
   ----------------------------------------------------------------------------- */

void avaliarAutovaloresAutovetores(matriz *a) {
    double lambda1, lambda2;

    int flag = autovalores(a, &lambda1, &lambda2);

    if (flag != 0) {
        double v1[2] = {0}, v2[2] = {0};
        printf("* λ1 = %.2lf\n", lambda1);
        autovetores(a, lambda1, v1, 1);
        printf("* λ2 = %.2lf\n", lambda2);
        autovetores(a, lambda2, v2, 2);
    } else {
        printf("* SEM AUTOVALORES REAIS\n");
    }
}
