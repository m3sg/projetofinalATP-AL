/* =============================================================================
   transformacoes.c
   Cálculo do posto de uma matriz, classificação de sistemas lineares com
   base na relação entre o posto da matriz de coeficientes e o posto da
   matriz aumentada, cálculo de dimensões do núcleo e da imagem.

   OBS: incorpora também as funções relacionadas a base e diagonalização,
   devido à proximidade destes conteúdos.
   ============================================================================= */

#include <stdio.h>
#include <string.h>
#include <tgmath.h>
#include "algebra.h"

/* -----------------------------------------------------------------------------
   postos

   O que faz:
     conta o número de linhas não nulas da matriz escalonada segundo o critério
     selecionado pelo parâmetro "aumentada", retornando o posto correspondente.

   Como funciona:
     Percorre cada linha da matriz. Para cada linha, verifica se é nula
     segundo o critério escolhido:
       aumentada == 0: a linha é não nula se ao menos um coeficiente em
                       dados[i][j] (j < n_incognitas) for maior que 1e-9.
                       Conta o posto da matriz de coeficientes.
       aumentada == 1: a linha é não nula se ao menos um coeficiente for
                       maior que 1e-9 OU se independentes[i] for maior que
                       1e-9. Conta o posto da matriz aumentada.

   Parâmetros:
     - const matriz *a : ponteiro para a matriz.
     - int aumentada   : 0 para posto da matriz de coeficientes.
                         1 para posto da matriz aumentada.

   Retorno:
     inteiro com o número de linhas não nulas segundo o critério escolhido.
   ----------------------------------------------------------------------------- */

int posto(matriz *a, int aumentada) {

    int posto_coeficie = 0;
    int posto_aumentada  = 0;
    int linha_coef_nula = 1;
    int linha_aum_nula  = 1;

    for (int i = 0; i < a->linhas; i++) {

        linha_coef_nula = 1;
        linha_aum_nula  = 1;

        if (aumentada == 0) {
            for (int j = 0; j < a->n_incognitas; j++) {
                if (fabs(a->dados[i][j]) > 1e-9) {
                    linha_coef_nula = 0;
                    break;
                }
            }
        }

        if (aumentada == 1) {
            for (int j = 0; j < a->n_incognitas; j++) {
                if (fabs(a->dados[i][j]) > 1e-9) {
                    linha_coef_nula = 0;
                    break;
                }
            }
            if (fabs(a->independentes[i]) > 1e-9 ) {
                linha_aum_nula = 0;
            }
        }
        if (aumentada == 0) {
            if (linha_coef_nula == 0)posto_coeficie++;
        } else {
            if (linha_coef_nula == 0 || linha_aum_nula  == 0)posto_aumentada++;
        }
    }

    if (aumentada == 0) {
        return posto_coeficie;
    } else {
        return posto_aumentada;
    }
}

  /* -----------------------------------------------------------------------------
     injetividade

     -----------------------------------------------------------------------------
     O que faz:
       verifica se a transformação linear representada pela matriz é injetiva.

     Como funciona:
       Calcula o posto da matriz de coeficientes. Se o posto for igual ao número
       de incógnitas (colunas), não há variáveis livres e a transformação é
       injetiva. Retorna 0 também se o posto for zero (matriz nula).

     Parâmetros:
       - matriz *a: ponteiro para a matriz a ser verificada.

     Retorno:
       1 se injetiva, 0 caso contrário.
     ----------------------------------------------------------------------------- */

int injetividade(matriz *a) {

    int posto_coeficiente = posto(a, 0);

    if (posto_coeficiente == 0) return 0;

    if (posto_coeficiente == a->n_incognitas) {
        return 1;
    } else {
        return 0;
    }
}

  /* -----------------------------------------------------------------------------
     sobrejetividade

     -----------------------------------------------------------------------------
     O que faz:
       verifica se a transformação linear representada pela matriz é sobrejetiva.

     Como funciona:
       Calcula o posto da matriz de coeficientes. Se o posto for igual ao número
       de linhas (dimensão do contradomínio), a transformação é sobrejetiva.

     Parâmetros:
       - matriz *a: ponteiro para a matriz a ser verificada.

     Retorno:
       1 se sobrejetiva, 0 caso contrário.
     ----------------------------------------------------------------------------- */

int sobrejetividade(matriz *a) {

    int posto_coeficiente = posto(a, 0);

    if (posto_coeficiente == a->linhas) {
        return 1;
    } else {
        return 0;
    }
}

  /* -----------------------------------------------------------------------------
     bijetividade

     -----------------------------------------------------------------------------
     O que faz:
       verifica se a transformação linear é bijetiva, combinando os testes de
       injetividade e sobrejetividade.

     Como funciona:
       Chama injetividade e sobrejetividade. Retorna 1 apenas
       se ambas retornarem 1.

     Parâmetros:
       - matriz *a: ponteiro para a matriz a ser verificada.

     Retorno:
       1 se bijetiva, 0 caso contrário.
     ----------------------------------------------------------------------------- */

int bijetividade(matriz *a) {

    int injetiva = injetividade(a);
    int sobrejetiva = sobrejetividade(a);

    if (injetiva == 1 && sobrejetiva == 1) {
        return 1;
    } else {
        return 0;
    }
}

/* -----------------------------------------------------------------------------
   testeBase

   O que faz:
     verifica se os vetores da matriz formam uma base do espaço de dimensão
     dada, checando duas condições: posto igual à dimensão e número de linhas
     igual à dimensão.

   Como funciona:
     Calcula o posto da matriz de coeficientes. Se posto == dimensao e
     a->linhas == dimensao, as duas condições são satisfeitas e a função
     retorna 1. Caso contrário retorna 0. Quando v == 1, imprime qual
     condição foi satisfeita ou violada antes de retornar.

   Parâmetros:
     - matriz *a    : ponteiro para a matriz (já escalonada) a ser verificada.
     - int dimensao : dimensão do espaço vetorial em questão.
     - int v        : 1 imprime o diagnóstico de cada condição, 0 não imprime.

   Retorno:
     1 se forma base, 0 caso contrário.
   ----------------------------------------------------------------------------- */

int testeBase(matriz *a, int dimensao, int v) {

    int valor_posto = posto(a, 0);
    if (valor_posto == dimensao && a->linhas == dimensao) {
        if (v == 1) {
            printf("\n");
            printf("* condição 1 [✓]: posto [%d] é igual a dimensão [%d]\n", valor_posto, dimensao);
            printf("* condição 2 [✓]: quantidade de linhas [%d] é igual a dimensão [%d]\n", a->linhas, dimensao);
        }
        return 1;
    } else {
        if (v == 1) {
            if (valor_posto != dimensao) {
                printf("\n");
                printf("* condição 1 [X]: posto [%d] é diferente da dimensão [%d]\n", valor_posto, dimensao);
            } else if (a->linhas != dimensao) {
                printf("\n");
                printf("* condição 2 [X]: quantidade de linhas [%d] é diferente da dimensão [%d]\n", a->linhas, dimensao);
            } else if (valor_posto != dimensao && a->linhas != dimensao) {
                printf("\n");
                printf("* condição 1 [X]: posto [%d] é diferente da dimensão [%d]\n", valor_posto, dimensao);
                printf("* condição 2 [X]: quantidade de linhas [%d] é diferente da dimensão [%d]\n", a->linhas, dimensao);
            }
        }
        return 0;
    }
}

/* -----------------------------------------------------------------------------
   avaliarBase

   O que faz:
     ponto de entrada para verificar se os vetores fornecidos formam uma base,
     escalona a matriz internamente e imprime o veredito final.

   Como funciona:
     1. Escalona uma cópia da matriz via eliminacaoGauss(a, 0), sem impressão
        do passo a passo.
     2. Chama testeBase(&escalonada, dimensao, 1) para imprimir o diagnóstico
        de cada condição.
     3. Imprime "FORMA BASE" ou "NÃO FORMA BASE" conforme o resultado.
     4. Chama testeBase novamente com v == 0 para obter o valor de retorno
        sem efeito colateral.

   Parâmetros:
     - matriz *a    : ponteiro para a matriz original.
     - int dimensao : dimensão do espaço vetorial em questão.

   Retorno:
     1 se forma base, 0 caso contrário.
   ----------------------------------------------------------------------------- */

int avaliarBase(matriz *a, int dimensao) {

    matriz escalonada = eliminacaoGauss(a, 0);

    if (testeBase(&escalonada, dimensao, 1)) {
        printf("\n* FORMA BASE DO ESPAÇO\n");
    } else {
        printf("\n* NÃO FORMA BASE DO ESPAÇO\n");
    }
    return testeBase(&escalonada, dimensao, 0);
}

/* -----------------------------------------------------------------------------
   dimensaoNucleo

   O que faz:
     calcula uma base do núcleo da transformação linear representada
     pela matriz A. O núcleo é o conjunto de vetores x tal que Ax = 0.

   Como funciona:
     1. Escalona uma cópia da matriz.
     2. Identifica quais colunas têm pivô e quais são livres.
     3. Para cada variável livre, monta um vetor da base do núcleo atribuindo
        t=1 para ela e resolvendo as demais por substituição retroativa.

   Parâmetros:
     - const matriz *a : matriz de coeficientes.
     - matriz *nucleo  : matriz de saída onde cada linha é um vetor da base
                         do núcleo.

   Retorno:
     número de vetores na base do núcleo (dimensão do núcleo = n - posto).
   ----------------------------------------------------------------------------- */

matriz dimensaoNucleo(matriz *a) {

    int tem_pivo[TAM_MAX] = {0};

    int n = a->n_incognitas;


    int pivo_da_coluna[TAM_MAX];
    for (int j = 0; j < n; j++) {
        pivo_da_coluna[j] = -1;
    }

    int linha_atual = 0;
    for (int i = 0; i < n && linha_atual < a->linhas; i++) {
        if (fabs(a->dados[linha_atual][i]) > 1e-9) {
            pivo_da_coluna[i] = linha_atual;
            tem_pivo[i] = 1;
            linha_atual++;
        }
    }

    matriz nucleo;
    zerarMatriz(&nucleo);
    nucleo.n_incognitas = n;
    for (int i = 0; i < n; i++) {
        strcpy(nucleo.incognitas[i], a->incognitas[i]);
    }

    for (int livre = 0; livre < n; livre++) {
        if (tem_pivo[livre]) continue;

        double vetor[TAM_MAX] = {0};
        vetor[livre] = 1.0;

        for (int j = a->n_incognitas - 1; j >= 0; j--) {
            if (pivo_da_coluna[j] == -1) continue;
            int linha = pivo_da_coluna[j];
            double soma = 0.0;
            for (int k = j + 1; k < a->n_incognitas; k++) {
                soma += a->dados[linha][k] * vetor[k];
            }
            vetor[j] =  -soma / a->dados[linha][j];
        }

        for (int j = 0; j < a->n_incognitas; j++) {
            nucleo.dados[nucleo.linhas][j] = vetor[j];
        }
        nucleo.linhas++;
    }

    return nucleo;
}

/* -----------------------------------------------------------------------------
   dimensaoImagem

   O que faz:
     extrai uma base da imagem da transformação linear, coletando as colunas
     pivô da matriz original que correspondem às linhas não nulas da forma
     escalonada.

   Como funciona:
     1. Percorre a matriz escalonada para identificar quais colunas possuem
        pivô, armazenando o índice da coluna em pivo_da_coluna[].
     2. Para cada coluna pivô j, copia a coluna j inteira da matriz original
        como uma nova linha da matriz imagem (transpõe coluna → linha).

   Parâmetros:
     - matriz *original   : matriz de coeficientes original, da qual as
                            colunas pivô são extraídas.
     - matriz *escalonada : forma escalonada de original, usada apenas para
                            identificar as posições dos pivôs.

   Retorno:
     nova struct matriz onde cada linha é um vetor da base da imagem.
   ----------------------------------------------------------------------------- */

matriz dimensaoImagem(matriz *original, matriz *escalonada) {

    int n = escalonada->n_incognitas;

    int pivo_da_coluna[TAM_MAX];
    for (int j = 0; j < n; j++) pivo_da_coluna[j] = -1;

    int linha_atual = 0;
    for (int j = 0; j < n && linha_atual < escalonada->linhas; j++) {
        if (fabs(escalonada->dados[linha_atual][j]) > 1e-9) {
            pivo_da_coluna[j] = linha_atual;
            linha_atual++;
        }
    }

    matriz imagem;
    zerarMatriz(&imagem);
    imagem.n_incognitas = original->linhas;

    for (int j = 0; j < n; j++) {
        if (pivo_da_coluna[j] == -1) continue;

        for (int i = 0; i < original->linhas; i++) {
            imagem.dados[imagem.linhas][i] = original->dados[i][j];
        }
        imagem.linhas++;
    }

    return imagem;
}

/* -----------------------------------------------------------------------------
   transformaçãoLinear

   O que faz:
     ponto de entrada para análise completa de uma transformação linear:
     calcula e exibe núcleo, imagem e classificação (injetiva, sobrejetiva
     ou bijetiva).

   Como funciona:
     1. Escalona uma cópia da matriz via eliminacaoGauss(a, 0).
     2. Calcula o núcleo via dimensaoNucleo e a base da imagem via
        dimensaoImagem, imprimindo as dimensões e as bases de cada um.
     3. Chama injetividade, sobrejetividade e
        bijetividade sobre a forma escalonada e imprime a
        classificação resultante. Bijetiva tem prioridade na exibição,
        seguida de injetiva e sobrejetiva.

   Parâmetros:
     - matriz *a: ponteiro para a matriz de coeficientes da transformação.
   ----------------------------------------------------------------------------- */

void transformaçãoLinear (matriz *a) {

    matriz escalonada = eliminacaoGauss(a, 0);
    matriz nucleo, imagem;
    zerarMatriz(&nucleo);
    zerarMatriz(&imagem);

    nucleo = dimensaoNucleo(&escalonada);
    imagem = dimensaoImagem(a, &escalonada);

    int dim_nucleo  = nucleo.linhas;
    int dim_imagem  = imagem.linhas;

    printf("* DIMENSÃO DO NÚCLEO: %d\n", dim_nucleo);
    printf("* DIMENSÃO DA IMAGEM: %d\n", dim_imagem);

    if (dim_nucleo == 0) {
        printf("* BASE DO NÚCLEO: {0} (núcleo trivial)\n");
    } else {
        printf("* BASE DO NÚCLEO: ");
        printarVetor(&nucleo, nucleo.n_incognitas);
        printf("\n");
    }

    if (dim_imagem == 0) {
        printf("* BASE DA IMAGEM: {0} (imagem trivial)\n");
    } else {
        printf("* BASE DA IMAGEM: ");
        printarVetor(&imagem, imagem.n_incognitas);
        printf("\n");
    }

    int injetiva, sobrejetiva, bijetiva;

    injetiva = injetividade(&escalonada);
    sobrejetiva = sobrejetividade(&escalonada);
    bijetiva = bijetividade(&escalonada);

    if (bijetiva) {
        printf("* CLASSIFICAÇÃO DA TRANSFORMAÇÃO: BIJETIVA\n");
    } else if (injetiva) {
        printf("* CLASSIFICAÇÃO DA TRANSFORMAÇÃO: INJETIVA\n");
    } else if (sobrejetiva) {
        printf("* CLASSIFICAÇÃO DA TRANSFORMAÇÃO: SOBREJETIVA\n");
    } else {
        printf("* SEM CLASSIFICAÇÃO\n");
    }
}

/* -----------------------------------------------------------------------------
   diagonalizacao

   O que faz:
     tenta diagonalizar uma matriz 2x2, calculando autovalores e autovetores
     e montando as matrizes P (mudança de base) e D (diagonal), exibindo
     ambas ao final.

   Como funciona:
     1. Chama autovalores para obter λ1 e λ2. Se o discriminante for
        negativo (autovalores complexos), encerra com retorno 0.
     2. Para cada autovalor, chama autovetores para preencher v1 e v2.
     3. Monta P colocando v1 e v2 como colunas, e D com λ1 e λ2 na diagonal.
     4. Imprime P e D. Se λ1 == λ2 (autovalor repetido), emite um aviso de
        que a diagonalização pode não ser possível.

   Parâmetros:
     - matriz *a: ponteiro para a matriz 2x2 a ser diagonalizada.

   Retorno:
     1 se diagonalizável, 0 se os autovalores forem complexos.
   ----------------------------------------------------------------------------- */

int diagonalizacao(matriz *a) {

    double lambda1, lambda2;
    double v1[2] = {0}; // autovetor de lambda1
    double v2[2] = {0}; // autovetor de lambda2


    int flag = autovalores(a, &lambda1, &lambda2);

    if (flag == 0) {
        printf("* MATRIZ NÃO DIAGONALIZÁVEL: autovalores complexos\n");
        return 0;
    }

    printf("* λ1 = %.2lf\n", lambda1);
    autovetores(a, lambda1, v1, 1);
    printf("* λ2 = %.2lf\n", lambda2);
    autovetores(a, lambda2, v2, 2);

    if (fabs(v1[0]*v2[1] - v1[1]*v2[0]) < 1e-9) {
        printf("* MATRIZ NÃO DIAGONALIZÁVEL: autovetores linearmente dependentes\n");
        return 0;
    }

    matriz P;
    zerarMatriz(&P);
    P.linhas       = a->linhas;
    P.n_incognitas = a->n_incognitas;

    for (int i = 0; i < a->linhas; i++) {
        P.dados[i][0] = v1[i];
        P.dados[i][1] = v2[i];
    }

    matriz D;
    zerarMatriz(&D);
    D.linhas       = a->linhas;
    D.n_incognitas = a->n_incognitas;
    D.dados[0][0]  = lambda1;
    D.dados[1][1]  = lambda2;

    printf("\n* MATRIZ P (autovetores como colunas):\n");
    printarMatriz(&P, 0);
    printf("\n* MATRIZ D (autovalores na diagonal):\n");
    printarMatriz(&D, 0);

    return 1;
}

