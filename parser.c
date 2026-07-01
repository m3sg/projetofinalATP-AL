/* =============================================================================
   parser.c
   Leitura e interpretação de equações textuais, expressões e vetores, com a
   tradução para a struct matriz.
   ============================================================================= */

#include <stdio.h>
#include "algebra.h"
#include <string.h>
#include <ctype.h>
#include <math.h>
#include <stdlib.h>

/* descarta caracteres pendentes no stdin até '\n' ou EOF */
void limpezadeBuffer () {
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}

/* substitui o '\n' do fgets pelo terminador nulo */
void tirarEspaco (char *s) {
    s[strcspn(s, "\n")] = '\0';
}

/* -----------------------------------------------------------------------------
   tratarIncognita

   O que faz:
     verifica se uma incógnita já está registrada na matriz. Se estiver,
     retorna o índice da coluna onde ela foi registrada. Se não estiver,
     registra e retorna o novo índice.

   Como funciona:
     Percorre incognitas[] comparando cada entrada com a string recebida via
     strcmp. Se encontrar correspondência, retorna o índice imediatamente.
     Se não encontrar, copia a string para incognitas[n_incognitas], incrementa
     n_incognitas e retorna o índice recém-atribuído.

   Parâmetros:
     - matriz *a       : ponteiro para a matriz. Modificada quando a incógnita
                         for nova (escreve em incognitas[] e n_incognitas).
     - char *incognita : string com o nome da incógnita a buscar ou registrar.

   Retorno:
     índice (coluna) da incógnita em dados[][].
   ----------------------------------------------------------------------------- */

int tratarIncognita (matriz *a, char *incognita) {
    for (int i = 0; i < a->n_incognitas; i++) {
        if (strcmp(a->incognitas[i], incognita) == 0) {
            return i;
        }
    }
    if (a->n_incognitas >= TAM_MAX) return -1;
    strcpy(a->incognitas[a->n_incognitas], incognita);
    a->n_incognitas++;
    return a->n_incognitas - 1;
}

/* -----------------------------------------------------------------------------
   larguraFormatacao

   O que faz:
     percorre todos os elementos da matriz e retorna a largura em caracteres
     do maior elemento encontrado. Serve como base para qualquer função de
     impressão que precise alinhar colunas uniformemente.

   Como funciona:
     Formata cada elemento como string via snprintf (sem imprimir de fato)
     e mede seu comprimento em caracteres. A cada elemento maior encontrado,
     a largura armazenada é substituída.

   Parâmetros:
     - const matriz *a : ponteiro para a matriz percorrida. Não modificada.
     - int sinal       : 1 considera o sinal do número na medição (útil para
                         impressão de grades brutas); 0 usa o valor absoluto
                         (útil quando o sinal é impresso separadamente, como
                         em printarSistema).

   Retorno:
     largura em caracteres do maior elemento, para uso no formatador %*.
   ----------------------------------------------------------------------------- */

int larguraFormatacao(matriz *a, int sinal) {
    int largura_print = 0;
    int tam = 0;
    char auxiliar[50];

    for (int i = 0; i < a->linhas; i++) {
        for (int j = 0; j < a->n_incognitas; j++) {
            if (sinal == 1) {
                tam = snprintf(auxiliar, sizeof(auxiliar), "%.2lf", a->dados[i][j]);
            } else {
                tam = snprintf(auxiliar, sizeof(auxiliar), "%.2lf", fabs(a->dados[i][j]));
            }
            if (tam > largura_print) largura_print = tam;
        }

        if (sinal == 1) {
            tam = snprintf(auxiliar, sizeof(auxiliar), "%.2lf", a->independentes[i]);
        } else {
            tam = snprintf(auxiliar, sizeof(auxiliar), "%.2lf", fabs(a->independentes[i]));
        }
        if (tam > largura_print) largura_print = tam;
    }
    return largura_print;
}

/* -----------------------------------------------------------------------------
   parserEquacao

   O que faz:
     interpreta uma equação linear em formato textual (ex.: "3x + 2y = 5") e
     preenche uma linha da matriz com os coeficientes e o termo independente
     correspondentes.

   Como funciona:
     Percorre a string caractere por caractere. Cada tipo de caractere dispara
     uma ação:
       '+' ou '-' : atualiza o sinal corrente do próximo termo.
       '='        : marca que os próximos dígitos são o termo independente.
       dígito     : usa strtod para ler o número completo (incluindo decimais)
                    e avança o índice para além do número lido. Se antes do
                    '=' é coeficiente; se após, é termo independente.
       letra      : lê o nome da incógnita (um caractere), chama tratarIncognita
                    para obter a coluna e escreve coeficiente * sinal em
                    dados[linhas][coluna]. Reseta coeficiente para 1.0 e sinal
                    para 1.0 após o uso.
       espaço     : ignorado.
     Ao final da string, incrementa a->linhas.

   Parâmetros:
     - matriz *a     : ponteiro para a matriz. Modificada: escreve em dados[][],
                       independentes[] e incrementa linhas.
     - char *equacao : string com a equação a interpretar.
   ----------------------------------------------------------------------------- */

void parserEquacao (matriz *a, char *equacao) {
    if (a->linhas >= TAM_MAX) return;
    double coeficiente = 1.0;
    double sinal = 1.0;
    char nome_incognita[NOME_MAX] = {0};
    char *coeficiente_fim = NULL;
    int i = 0;
    int igual = 0;
    while (equacao[i] != '\0') {
        if (equacao[i] == '+') {
            sinal = 1.0;
        } else if (equacao[i] == '-') {
            sinal = -1.0;
        } else if (equacao[i] == '=') {
            igual = 1;
            sinal = 1.0;
        } else if (isdigit(equacao[i])) {
            if (igual == 1) {
                a->independentes[a->linhas] = strtod(&equacao[i], &coeficiente_fim) * sinal;
            } else if (igual != 1)
                coeficiente = strtod(&equacao[i], &coeficiente_fim);
            i = coeficiente_fim - equacao;
            continue;
        } else if (isalpha(equacao[i])) {
            nome_incognita[0] = equacao[i];
            nome_incognita[1] = '\0';
            int col = tratarIncognita(a, nome_incognita);
            if (col == -1) return;
            a->dados[a->linhas][col] = coeficiente * sinal;
            coeficiente = 1.0;
            sinal = 1.0;
        } else if (isspace(equacao[i])) {
            i++;
            continue;
        }
        i++;
    }
    a->linhas++;
}

/* -----------------------------------------------------------------------------
   leituraEquacao

   O que faz:
     solicita ao usuário que digite uma equação e a repassa para parserEquacao,
     que a interpreta e registra na matriz.

   Parâmetros:
     - matriz *a     : ponteiro para a matriz.
     - char *equacao : buffer externo onde a equação lida será armazenada.
   ----------------------------------------------------------------------------- */

void leituraEquacao (matriz *a, char *equacao, int buf_tamanho) {
    printf("* DIGITE UMA EQUAÇÃO: ");
    fgets(equacao, buf_tamanho, stdin);
    tirarEspaco(equacao);
    parserEquacao(a, equacao);
}

/* -----------------------------------------------------------------------------
   leituraVetor

   O que faz:
     solicita ao usuário que digite um vetor e o repassa para parserVetor,
     que o interpreta e registra na matriz. Repete a solicitação enquanto
     a dimensão do vetor digitado não corresponder a tamanho.

   Parâmetros:
     - matriz *a       : ponteiro para a matriz.
     - char *vetor     : buffer externo onde o vetor lido será armazenado.
     - int buf_tamanho : capacidade do buffer vetor, usada no fgets.
     - int tamanho     : número de elementos esperado em cada vetor.
   ----------------------------------------------------------------------------- */

void leituraVetor(matriz *a, char *vetor, int buf_tamanho, int tamanho) {
    int resultado;
    do {
        printf("* DIGITE UM VETOR: ");
        fgets(vetor, buf_tamanho, stdin);
        tirarEspaco(vetor);
        resultado = parserVetor(a, vetor, tamanho);
        if (resultado == -1) printf("\nerro: dimensão inválida!\n");
    } while (resultado == -1);
}

/* -----------------------------------------------------------------------------
   leituraVetorArquivos

   O que faz:
     solicita um caminho de arquivo ao usuário, abre o arquivo e interpreta
     cada linha como um vetor, chamando parserVetor para cada uma.
     Linhas com dimensão incompatível com tamanho são ignoradas com aviso.

   Parâmetros:
     - matriz *a       : ponteiro para a matriz.
     - char *vetor     : buffer externo usado como intermediário para cada linha lida.
     - int buf_tamanho : capacidade do buffer vetor, usada no fgets.
     - int tamanho     : número de elementos esperado em cada vetor.
   ----------------------------------------------------------------------------- */

void leituraVetorArquivos (matriz *a, char *vetor, int buf_tamanho, int tamanho) {
    int erro;
    do {
        erro = 0;
        zerarMatriz(a);

        FILE *arquivo;
        do {
            char caminho[CAMINHO_MAX] = {0};
            printf("* ESCREVA O CAMINHO DO ARQUIVO: ");
            fgets(caminho, sizeof(caminho), stdin);
            tirarEspaco(caminho);
            arquivo = fopen(caminho, "r");
            if (arquivo == NULL)
                printf("\nArquivo vazio ou não localizado!\n");
        } while (arquivo == NULL);

        while (fgets(vetor, buf_tamanho, arquivo)) {
            tirarEspaco(vetor);
            if (parserVetor(a, vetor, tamanho) == -1) {
                printf("\nerro: dimensão inválida! Corrija o arquivo e insira o caminho novamente.\n");
                erro = 1;
                break;
            }
        }
        fclose(arquivo);
    } while (erro == 1);
}

/* -----------------------------------------------------------------------------
   parserVetor

   O que faz:
     interpreta uma string contendo valores numéricos separados por espaço
     ou vírgula e preenche uma linha da matriz com esses valores como
     componentes de um vetor. Suporta valores negativos via prefixo '-'.

   Como funciona:
     Percorre a string caractere a caractere. Ao encontrar um dígito, usa
     strtod para ler o número completo (incluindo decimais) e avança o índice
     para além do número lido. O sinal acumulado (positivo ou negativo) é
     aplicado ao valor antes de armazená-lo. Letras e espaços são ignorados.
     Se o número de valores lidos for exatamente tamanho, incrementa a->linhas
     e retorna 0. Caso contrário, retorna -1 sem modificar a->linhas.

   Parâmetros:
     - matriz *a    : ponteiro para a matriz. Modificada em caso de sucesso:
                      escreve em dados[linhas][] e incrementa linhas.
     - char *vetor  : string com os valores a interpretar.
     - int tamanho  : número exato de elementos esperado no vetor.

   Retorno:
      0 se dimensão correta, -1 caso contrário.
   ----------------------------------------------------------------------------- */

int parserVetor (matriz *a, char *vetor, int tamanho) {
    if (a->linhas >= TAM_MAX) return -1;
    int j = 0;
    double sinal = 1.0;
    int tamanho_lido = 0;
    char *numero_fim = NULL;
    int len = (int)strlen(vetor);
    for (int i = 0; i < len; i++) {
        if (isdigit(vetor[i])) {
            if (j >= TAM_MAX) break;
            if (sinal < 0) {
                a->dados[a->linhas][j] = strtod(&vetor[i], &numero_fim) * sinal;
            } else {
                a->dados[a->linhas][j] = strtod(&vetor[i], &numero_fim);
            }
            i = (int)(numero_fim - vetor) - 1;
            tamanho_lido++;
            sinal = 1.0;
            j++;
        } else if (isalpha(vetor[i]) || isspace(vetor[i])) {
            continue;
        } else if (vetor[i] == '-') {
            sinal = -1.0;
        }
    }

    if (tamanho_lido == tamanho) {
        a->linhas++;
        a->n_incognitas = tamanho;
        return 0;
    } else {
        return -1;
    }
}

/* -----------------------------------------------------------------------------
   printarVetor

   O que faz:
     exibe cada vetor armazenado na matriz no formato "(x, y, z)", um por linha,
     precedido de seu índice.

   Parâmetros:
     - matriz *a  : ponteiro para a matriz. Não modificada.
     - int tamanho: número de componentes de cada vetor (colunas a imprimir).
   ----------------------------------------------------------------------------- */

void printarVetor (matriz *a, int tamanho) {
    for (int i = 0; i < a->linhas; i++) {
        if (i != 0) printf(", ");
        printf("vetor %d ", i + 1);
        for (int j = 0; j < tamanho; j++) {
            if (j == 0) {
                printf("(");
                printf("%.2lf", a->dados[i][j]);
            } else if (j == tamanho - 1) {
                printf(", %.2lf", a->dados[i][j]);
                printf(")");
            } else {
                printf(", %.2lf", a->dados[i][j]);
            }
        }
    }
}

/* -----------------------------------------------------------------------------
   leituraEquacaoArquivo

   O que faz:
     solicita um caminho de arquivo ao usuário, abre o arquivo e interpreta
     cada linha como uma equação, chamando parserEquacao para cada uma.

   Parâmetros:
     - matriz *a     : ponteiro para a matriz.
     - char *equacao : buffer externo usado como intermediário para cada linha lida.
   ----------------------------------------------------------------------------- */

void leituraEquacaoArquivo (matriz *a, char *equacao, int buf_tamanho) {
    char caminho[CAMINHO_MAX] = {0};
    FILE *arquivo = NULL;
    do {
        printf("* ESCREVA O CAMINHO DO ARQUIVO: ");
        fgets(caminho, sizeof(caminho), stdin);
        tirarEspaco(caminho);
        arquivo = fopen(caminho, "r");
        if (arquivo == NULL) printf("\nArquivo vazio ou não localizado!\n");
    } while (arquivo == NULL);
    while (fgets(equacao, buf_tamanho, arquivo)) {
        parserEquacao(a, equacao);
    }
    fclose(arquivo);
}

/* -----------------------------------------------------------------------------
   leituraExpressao

   O que faz:
     solicita ao usuário que digite uma expressão de transformação linear no
     formato "ax + by, cx + dy" e preenche a matriz com os coeficientes de
     cada componente do contradomínio. Repete a solicitação enquanto o número
     de incógnitas na expressão exceder dim_dominio.

   Como funciona:
     1. Solicita as variáveis do domínio por teclado e pré-registra sua ordem.
     2. Lê a expressão completa com fgets e remove o '\n' via tirarEspaco.
     3. Divide a string nas vírgulas: cada segmento corresponde a uma linha
        do contradomínio e é repassado para parserExpressao.
     4. O último segmento (sem vírgula à direita) é tratado separadamente.
     5. Se a->n_incognitas > dim_dominio após o parse, exibe erro e reinicia.
     6. Ao final, força a->linhas = dim_contradominio.

   Parâmetros:
     - matriz *a              : ponteiro para a matriz. Reinicializada a cada
                                tentativa e modificada via parserExpressao.
     - char *expressao        : buffer externo onde a expressão lida é armazenada.
     - int tam_buffer         : capacidade do buffer expressao, usada no fgets.
     - int dim_dominio        : número máximo de incógnitas permitido (dimensão
                                do domínio da transformação).
     - int dim_contradominio  : número de componentes esperado na expressão
                                (dimensão do contradomínio).
   ----------------------------------------------------------------------------- */

void leituraExpressao (matriz *a, char *expressao, int tam_buffer, int dim_dominio, int dim_contradominio) {
    int erro;
    do {
        erro = 0;
        zerarMatriz(a);

        char variaveis [dim_dominio][NOME_MAX];
        printf("* DIGITE AS VARIÁVEIS DO DOMÍNIO:\n");
        for (int i = 0; i < dim_dominio; i++) {
            printf("* VARIÁVEL %d: ", i + 1);
            scanf(" %c", variaveis[i]);
            variaveis[i][NOME_MAX - 1] = '\0';
            tratarIncognita(a, variaveis[i]);
        }
        limpezadeBuffer();

        printf("* DIGITE A EXPRESSÃO: ");
        fgets(expressao, tam_buffer, stdin);
        tirarEspaco(expressao);

        size_t tamanho2;
        char aux_expressao [tam_buffer];
        char *inicio = expressao;
        char *virgula;
        int contador = 0;

        while ((virgula = strchr(inicio, ',')) != NULL) {
            if (contador == dim_contradominio) break;
            tamanho2 = virgula - inicio;
            strncpy(aux_expressao, inicio, tamanho2);
            aux_expressao[tamanho2] = '\0';
            inicio = virgula + 1;
            parserExpressao(a, aux_expressao);
            contador++;
        }

        if (contador < dim_contradominio) {
            memcpy(aux_expressao, inicio, strlen(inicio));
            aux_expressao[strlen(inicio)] = '\0';
            parserExpressao(a, aux_expressao);
        }

        if (a->n_incognitas > dim_dominio) {
            printf("\nerro: quantidade inválida de incógnitas!\n");
            erro = 1;
        }

        a->linhas = dim_contradominio;
    } while (erro == 1);
}

/* -----------------------------------------------------------------------------
   leituraExpressaoArquivo

   O que faz:
     lê uma expressão de transformação linear a partir de um arquivo de texto
     e preenche a matriz com os coeficientes de cada componente do contradomínio.
     Repete a solicitação de caminho enquanto o número de incógnitas exceder
     dim_dominio ou o arquivo não puder ser aberto.

   Como funciona:
     1. Solicita as variáveis do domínio por teclado e pré-registra sua ordem.
     2. Solicita o caminho do arquivo em loop até fopen ter sucesso.
     3. Lê a primeira linha do arquivo como a expressão completa.
     4. Divide a string nas vírgulas e repassa cada segmento para
        parserExpressao, da mesma forma que leituraExpressao.
     5. Se a->n_incognitas > dim_dominio, exibe erro e reinicia do passo 1.
     6. Ao final, força a->linhas = dim_contradominio.

   Parâmetros:
     - matriz *a              : ponteiro para a matriz. Reinicializada a cada
                                tentativa e modificada via parserExpressao.
     - char *expressao        : buffer externo usado para armazenar a linha lida.
     - int tam_buffer         : capacidade do buffer expressao, usada no fgets.
     - int dim_dominio        : número máximo de incógnitas permitido.
     - int dim_contradominio  : número de componentes esperado na expressão.
   ----------------------------------------------------------------------------- */

void leituraExpressaoArquivo (matriz *a, char *expressao, int tam_buffer, int dim_dominio, int dim_contradominio) {
    int erro;
    do {
        erro = 0;
        zerarMatriz(a);

        char variaveis [dim_dominio][NOME_MAX];
        printf("* DIGITE AS VARIÁVEIS DO DOMÍNIO:\n");
        for (int i = 0; i < dim_dominio; i++) {
            printf("* VARIÁVEL %d: ", i + 1);
            scanf(" %c", variaveis[i]);
            variaveis[i][NOME_MAX - 1] = '\0';
            tratarIncognita(a, variaveis[i]);
        }
        limpezadeBuffer();

        FILE *arquivo;

        do {
            char caminho[CAMINHO_MAX] = {0};
            printf("* ESCREVA O CAMINHO DO ARQUIVO: ");
            fgets(caminho, sizeof(caminho), stdin);
            tirarEspaco(caminho);
            arquivo = fopen(caminho, "r");
            if (arquivo == NULL) {
                printf("\nArquivo vazio ou não localizado!\n");
            }
        } while (arquivo == NULL);

        fgets(expressao, tam_buffer, arquivo);
        tirarEspaco(expressao);
        fclose(arquivo);

        size_t tamanho2;
        char aux_expressao [tam_buffer];
        char *inicio = expressao;
        char *virgula;
        int contador = 0;

        while ((virgula = strchr(inicio, ',')) != NULL) {
            if (contador == dim_contradominio) break;
            tamanho2 = virgula - inicio;
            strncpy(aux_expressao, inicio, tamanho2);
            aux_expressao[tamanho2] = '\0';
            inicio = virgula + 1;
            parserExpressao(a, aux_expressao);
            contador++;
        }

        if (contador < dim_contradominio) {
            memcpy(aux_expressao, inicio, strlen(inicio));
            aux_expressao[strlen(inicio)] = '\0';
            parserExpressao(a, aux_expressao);
        }

        if (a->n_incognitas > dim_dominio) {
            printf("\nerro: quantidade inválida de incógnitas! Corrija o arquivo e insira o caminho novamente.\n");
            erro = 1;
        }

        a->linhas = dim_contradominio;
    } while (erro == 1);
}

/* -----------------------------------------------------------------------------
   parserExpressao

   O que faz:
     interpreta um único componente de uma expressão de transformação linear
     (ex.: "3x + 2y") e preenche a linha corrente da matriz com os coeficientes
     encontrados. Análoga a parserEquacao, mas sem suporte ao lado direito do '='.

   Como funciona:
     Percorre a string caractere a caractere:
       '+' ou '-' : atualiza o sinal do próximo termo.
       dígito     : usa strtod para ler o coeficiente completo e avança o índice.
       letra      : lê o nome da incógnita, chama tratarIncognita para obter a
                    coluna e escreve coeficiente * sinal em dados[linhas][coluna].
                    Reseta coeficiente e sinal para 1.0 após o uso.
       espaço     : ignorado.
     Ao final, incrementa a->linhas.

   Parâmetros:
     - matriz *a       : ponteiro para a matriz. Modificada: escreve em dados[][]
                         e incrementa linhas.
     - char *expressao : string com um componente da expressão a interpretar.
   ----------------------------------------------------------------------------- */

void parserExpressao (matriz *a, char *expressao) {
    if (a->linhas >= TAM_MAX) return;
    double coeficiente = 1.0;
    double sinal = 1.0;
    char nome_incognita[NOME_MAX] = {0};
    char *coeficiente_fim = NULL;
    size_t i = 0;
    while (expressao[i] != '\0') {
        if (expressao[i] == '+') {
            sinal = 1.0;
        } else if (expressao[i] == '-') {
            sinal = -1.0;
        } else if (isdigit(expressao[i])) {
            coeficiente = strtod(&expressao[i], &coeficiente_fim);
            i = coeficiente_fim - expressao;
            continue;
        } else if (isalpha(expressao[i])) {
            nome_incognita[0] = expressao[i];
            nome_incognita[1] = '\0';
            int col = tratarIncognita(a, nome_incognita);
            if (col == -1) return;
            a->dados[a->linhas][col] = coeficiente * sinal;
            coeficiente = 1.0;
            sinal = 1.0;
        } else if (isspace(expressao[i])) {
            i++;
            continue;
        }
        i++;
    }
    a->linhas++;
}

/* -----------------------------------------------------------------------------
   printarExpressao

   O que faz:
     exibe a transformação linear armazenada na matriz no formato
     "(a1x + b1y, a2x + b2y)", com alinhamento uniforme entre os coeficientes.

   Como funciona:
     Para cada componente, percorre as colunas imprimindo o coeficiente com
     sinal (" - ", " + " ou espaço alinhado) seguido do nome da incógnita.
     Componentes separados por ", " dentro dos parênteses.

   Parâmetros:
     - matriz *a: ponteiro para a matriz. Não modificada.
   ----------------------------------------------------------------------------- */

void printarExpressao (matriz *a) {
    int largura_print = larguraFormatacao(a, 0);
    printf("(");
    for (int i = 0; i < a->linhas; i++) {
        if (i > 0) printf(", ");
        for (int j = 0; j < a->n_incognitas; j++) {
            if (j == 0) {
                if (a->dados[i][j] < 0) {
                    printf(" - ");
                    printf("%*.2lf%s", largura_print, fabs(a->dados[i][j]), a->incognitas[j]);
                } else if (a->dados[i][j] > 0) {
                    printf("%*.2lf%s", largura_print, a->dados[i][j], a->incognitas[j]);
                } else {
                    printf("%*.2lf", largura_print, 0.0);
                }
            } else {
                if (a->dados[i][j] < 0) {
                    printf(" - ");
                    printf("%*.2lf%s", largura_print, fabs(a->dados[i][j]), a->incognitas[j]);
                } else if (a->dados[i][j] > 0) {
                    printf(" + ");
                    printf("%*.2lf%s", largura_print, a->dados[i][j], a->incognitas[j]);
                } else {
                    printf("   ");
                    printf("%*.2lf", largura_print, 0.0);
                }
            }
        }
    }
    printf(")\n");
}

/* -----------------------------------------------------------------------------
   printarSistema

   O que faz:
     exibe a matriz como um sistema de equações legível, usando os nomes de
     incógnitas registrados em incognitas[]. É o caminho inverso do parser:
     lê dados numéricos da struct e reconstrói a representação textual.

   Como funciona:
     Para cada linha, imprime os coeficientes com sinal (" - ", " + " ou
     espaço alinhado) seguidos do nome da incógnita, terminando com " = "
     e o termo independente.

   Parâmetros:
     - const matriz *a: ponteiro para a matriz (não modificada).
   ----------------------------------------------------------------------------- */

void printarSistema (matriz *a) {
    int largura_print = larguraFormatacao(a, 0);
    for (int i = 0; i < a->linhas; i++) {
        for (int j = 0; j < a->n_incognitas; j++) {
           if (j == 0) {
               printf("{ ");
               if (a->dados[i][j] < 0) {
                   printf(" - ");
                   printf("%*.2lf%s", largura_print, fabs(a->dados[i][j]), a->incognitas[j]);
               } else if (a->dados[i][j] > 0){
                   printf("   ");
                   printf("%*.2lf%s", largura_print, a->dados[i][j], a->incognitas[j]);
               } else if (a->dados[i][j] == 0) {
                   printf("   ");
                   printf("%*.2lf", largura_print, 0.0);
               } if (a->n_incognitas == 1) printf(" = ");
           } else if (j == a->n_incognitas - 1) {
               if (a->dados[i][j] < 0) {
                   printf(" - ");
                   printf("%*.2lf%s", largura_print, fabs(a->dados[i][j]), a->incognitas[j]);
                   printf(" = ");
               } else if (a->dados[i][j] > 0){
                   printf(" + ");
                   printf("%*.2lf%s", largura_print, a->dados[i][j], a->incognitas[j]);
                   printf(" = ");
               } else if (a->dados[i][j] == 0) {
                   printf("   ");
                   printf("%*.2lf", largura_print, 0.0);
                   printf(" = ");
               }
           } else {
               if (a->dados[i][j] < 0) {
                   printf(" - ");
                   printf("%*.2lf%s", largura_print, fabs(a->dados[i][j]), a->incognitas[j]);
               } else if (a->dados[i][j] > 0){
                   printf(" + ");
                   printf("%*.2lf%s", largura_print, a->dados[i][j], a->incognitas[j]);
               } else if (a->dados[i][j] == 0) {
                   printf("   ");
                   printf("%*.2lf", largura_print, 0.0);
               }
           }
        }
        if (a->independentes[i] < 0) {
            printf(" - ");
            printf("%*.2lf", largura_print, fabs(a->independentes[i]));
        } else if (a->independentes[i] > 0){
            printf("   ");
            printf("%*.2lf", largura_print, a->independentes[i]);
        } else {
            printf("   ");
            printf("%*.2lf", largura_print, 0.0);
        }
        printf("\n");
    }
}


