/* =============================================================================
   sistema.c
   Resolução de sistemas lineares via eliminação de Gauss e substituição
   retroativa. Contém o escalonamento por pivoteamento parcial e a lógica
   de ramificação entre sistemas determinados, indeterminados e impossíveis.
   ============================================================================= */

#include <stdio.h>
#include "algebra.h"
#include <tgmath.h>
#include <string.h>

/* -----------------------------------------------------------------------------
   eliminacaoGauss

   O que faz:
     aplica a eliminação de Gauss com pivoteamento parcial sobre uma cópia
     da matriz recebida, reduzindo ela para a forma escalonada. A matriz original
     não é modificada. O resultado é retornado por valor como uma nova struct.

   Como funciona:
     Para cada coluna i (de 0 até n_incognitas - 1):
       1. Busca a linha com o maior valor absoluto na coluna i (pivoteamento
       parcial), para reduzir erros numéricos com pivôs pequenos.
       2. Troca essa linha com a linha i via trocarLinhas.
       3. Se o pivô resultante for menor que 1e-9 (usado para comparação segura de
       valores double com zero, evitando falsos negativos causados por erro de
       arredondamento de ponto flutuante acumulado durante o escalonamento.), pula a
       coluna, pois ela não tem pivô utilizável.
       4. Para cada linha k abaixo de i, calcula o multiplicador de eliminação
       e chama combinarLinhas para zerar dados[k][i].
       5. Quando v == 1, imprime o estado da matriz após cada troca e cada operação
       de linha, rotulando cada passo com um número sequencial.

   Parâmetros:
     - matriz *a : ponteiro para a matriz de entrada. Lida mas não modificada —
                   todas as operações acontecem sobre uma cópia local.
     - int v     : sinal para impressão. 1 imprime o passo a passo, e 0 não.

   Retorno:
     nova struct matriz contendo a forma escalonada. Retornada por valor, para que o
     chamador receba uma cópia independente, segura para modificar ou descartar.
   ----------------------------------------------------------------------------- */

matriz eliminacaoGauss(matriz *a, int v) {

    matriz copia = *a;
    char titulo[100] = {0};
    int tamanho_frase = 0;
    int passo = 0;

    if (v == 1) {
        strcpy(titulo, "* MATRIZ INICIAL");
        tamanho_frase = strlen(titulo);
        printf("%s\n", titulo);
        printarMatriz(a, 1);
        printf("\n");
    }
    int coluna_pivo = 0;
    for (int i = 0; i < copia.n_incognitas; i++) {
        if (coluna_pivo >= copia.linhas) break;
        int maior = coluna_pivo;
        for (int k = coluna_pivo + 1; k < a->linhas; k++) {
            if (fabs(copia.dados[maior][i]) < fabs(copia.dados[k][i])) {
                maior = k;
            }
        }
        if (maior != coluna_pivo) {
           trocarLinhas(&copia, maior, coluna_pivo);
            if (v == 1) {
                passo++;
                snprintf(titulo, sizeof(titulo), "* PASSO %d: trocar linha %d com linha %d", passo, coluna_pivo + 1, maior + 1);
                tamanho_frase = strlen(titulo);
                printf("%s\n", titulo);
                printarMatriz(&copia, 1);
                printf("\n");
            }
        }
        if (fabs(copia.dados[coluna_pivo][i]) < 1e-9) {
            continue;
        }
        for (int k = coluna_pivo + 1; k < copia.linhas; k++) {
            double escalar = -(copia.dados[k][i] / copia.dados[coluna_pivo][i]);
            combinarLinhas(&copia, k, coluna_pivo, escalar);
            if (v == 1) {
                passo++;
                snprintf(titulo, sizeof(titulo), "* PASSO %d: somar linha %d com linha %d", passo, coluna_pivo + 1, k + 1);
                tamanho_frase = strlen(titulo);
                printf("%s\n", titulo);
                printarMatriz(&copia, 1);
                printf("\n");
            }
        }
        coluna_pivo++;
    }
    if (v == 1) {
        strcpy(titulo, "* MATRIZ ESCALONADA");
        tamanho_frase = strlen(titulo);
        printf("%s\n", titulo);
        printarMatriz(&copia, 1);
        printf("\n");
    }
    return copia;
}

/* -----------------------------------------------------------------------------
   resolverSistema

   O que faz:
     é a função principal para resolução de um sistema linear. Escala a matriz,
     classifica o sistema e, se determinado ou indeterminado, calcula e exibe
     as soluções por substituição retroativa. Caso contrário (impossível), exibe
     o diagnóstico textual.

   Como funciona:
     1. Chama eliminacaoGauss(a, 1) para obter a forma escalonada com passo a passo,
     armazenando o resultado numa cópia local.
     2. Chama avaliarSistema(&copia) para determinar o tipo do sistema:
           1 → possível e determinado
           0 → impossível
          -1 → possível indeterminado
     3. Se determinado ou indeterminado: realiza substituição retroativa sobre a cópia
     escalonada (determinado ocorre dentro da própria função, mas indeterminado possui
     uma função própria para isso. Ao fim, imprime cada incógnita com seu valor.
     4. Se impossível: exibe apenas o diagnóstico textual.

   Parâmetros:
     - matriz *a: ponteiro para a matriz original do sistema. Lida mas não
                  modificadam pois o escalonamento opera sobre uma cópia interna.
   ----------------------------------------------------------------------------- */

void resolverSistema(matriz *a) {

    char titulo[100] = {0};
    double resultado[TAM_MAX] = {0};
    matriz copia = eliminacaoGauss(a, 1);
    int determinado = avaliarSistema(&copia);

     if (determinado == 1) {
        for (int i = copia.linhas - 1; i >= 0; i--) {
            resultado[i] = copia.independentes[i];
            for (int j = i + 1; j < copia.n_incognitas; j++) {
                resultado[i] -= copia.dados[i][j] * resultado[j];
            }
            if (fabs(copia.dados[i][i]) < 1e-9) {
                resultado[i] = 0.0;
            } else {
                resultado[i] /= copia.dados[i][i];
            }
        }
        strcpy(titulo, "* RESULTADO: SISTEMA POSSÍVEL DETERMINADO");
        printf("%s\n", titulo);
        for (int i = 0; i < a->n_incognitas; i++) {
            printf("%s = %.2f\n", a->incognitas[i], resultado[i]);
        }
    } else if (determinado == 0) {
        strcpy(titulo, "* RESULTADO: SISTEMA IMPOSSÍVEL");
        printf("%s\n", titulo);
    } else if (determinado == -1){
        strcpy(titulo, "* RESULTADO: SISTEMA POSSÍVEL INDETERMINADO");
        printf("%s\n", titulo);
        resolverSistemaSPI(&copia);
    }

}

  /* -----------------------------------------------------------------------------
     resolverSistemaSPI

     O que faz:
       realiza substituição retroativa sobre uma matriz já escalonada de um sistema
       possível indeterminado, expressando cada variável pivô em função das
       variáveis livres e imprimindo o resultado.

     Como funciona:
       1. Percorre cada linha da matriz para identificar a coluna do pivô de cada
          linha (primeiro elemento com valor absoluto > 1e-9), armazenando em
          pivo[i].
       2. Marca como livres (livre[j] = 1) todas as colunas que não são pivô de
          nenhuma linha.
       3. Para cada incógnita j:
            - Se j é livre: imprime "xj = xj".
            - Se j é pivô: localiza a linha correspondente e imprime a expressão
              "xj = (independente / pivô) ± coef·xk" para cada variável livre xk
              presente naquela linha, ajustando o sinal conforme o coeficiente.

     Parâmetros:
       - matriz *a: ponteiro para a matriz já escalonada do sistema.
     ----------------------------------------------------------------------------- */

void resolverSistemaSPI (matriz *a) {

    int pivo[TAM_MAX] = {0};
    int livre[TAM_MAX] = {0};

    for (int i = 0; i < a->linhas; i++) {
        for (int j = 0; j < a->n_incognitas; j++) {
            if (fabs(a->dados[i][j]) > 1e-9){
                pivo[i] = j;
                break;
            }
        }
    }

    for (int j = 0; j < a->n_incognitas; j++) {
        livre[j] = 1;
        for (int i = 0; i < a->linhas; i++) {
            if (pivo[i] == j) {
                livre[j] = 0;
                break;
            }
        }
    }

    for (int j = 0; j < a->n_incognitas; j++) {
        if (livre[j] == 1) {
            printf("%s = %s\n", a->incognitas[j], a->incognitas[j]);
        } else {
            for (int i = 0; i < a->linhas; i++) {
                if (pivo[i] == j) {
                    printf("%s", a->incognitas[j]);
                    printf(" = ");
                    if ((a->independentes[i]/a->dados[i][j]) < 0) printf("- ");
                    printf("%.2lf", fabs(a->independentes[i]/a->dados[i][j]));
                    for (int j2 = 0; j2 < a->n_incognitas; j2++) {
                        if (j2 == j) {
                            continue;
                        } else {
                            if (fabs(a->dados[i][j2]) > 1e-9) {
                                if ((a->dados[i][j2]/a->dados[i][j]) > 0) {
                                    printf(" - ");
                                    printf("%.2lf%s", fabs(a->dados[i][j2]/a->dados[i][j]), a->incognitas[j2]);
                                } else if ((a->dados[i][j2]/a->dados[i][j]) < 0) {
                                    printf(" + ");
                                    printf("%.2lf%s", fabs(a->dados[i][j2]/a->dados[i][j]), a->incognitas[j2]);
                                }
                            }
                        }
                    }
                    printf("\n");
                    break;
                }
            }
        }
    }
}

/* -----------------------------------------------------------------------------
   avaliarSistema

   O que faz:
     classifica um sistema linear em impossível, possível determinado ou
     possível indeterminado, com base na relação entre os postos da matriz
     (escalonada) de coeficientes e da matriz aumentada.

   Como funciona:
     Chama posto duas vezes sobre a mesma matriz, sendo uma para o posto
     dos coeficientes e outra para o posto da matriz aumentada:
       posto_aumentada > posto_coeficiente → sistema impossível     → retorna 0
       ambos iguais e == n_incognitas      → possível determinado   → retorna 1
       ambos iguais mas < n_incognitas     → possível indeterminado → retorna -1

   Parâmetros:
     - const matriz *a: ponteiro para a matriz escalonada.

   Retorno:
      1 → sistema possível e determinado (solução única)
      0 → sistema impossível (sem solução)
     -1 → sistema possível indeterminado (infinitas soluções)
   ----------------------------------------------------------------------------- */

int avaliarSistema(matriz *a) {

    int posto_coeficiente = posto(a, 0);
    int posto_aumentada = posto(a, 1);

    if (posto_aumentada > posto_coeficiente) {
        return 0;
    } else if ((posto_aumentada == posto_coeficiente) && posto_coeficiente == a->n_incognitas) {
        return 1;
    } else {
        return -1;
    }
}


