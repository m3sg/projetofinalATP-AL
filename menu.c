/* =============================================================================
   menu.c
       Fluxo de navegação do menu principal: impressão de opções, leitura de
       escolha, despacho para cada funcionalidade e coleta de entrada do usuário.
   ============================================================================= */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "algebra.h"

/* -----------------------------------------------------------------------------
   printarOpcoes / lerOpcao / processarOpcao

   Formam o ciclo principal do menu. printarOpcoes exibe as seis opções
   disponíveis; lerOpcao lê e valida a escolha (1–6); processarOpcao despacha
   para a função de entrada e processamento correspondente, retornando 1 em
   sucesso e 0 se o usuário cancelar ou a entrada falhar.
   ----------------------------------------------------------------------------- */

void printarOpcoes() {

    int largura = (int)strlen("[4] - calcular autovalores e autovetores");
    printarCabecalho("SISTEMA DE ÁLGEBRA LINEAR", largura);

    printf("[1] - resolver sistema linear\n");
    printf("[2] - analisar transformação linear\n");
    printf("[3] - verificar base de espaço vetorial\n");
    printf("[4] - calcular autovalores e autovetores\n");
    printf("[5] - verificar diagonalização\n");
    printf("[6] - SAIR");
    separadores(largura);

}

int lerOpcao() {
    int op = 0;

    printf("* ESCOLHA A OPÇÃO: ");
    op = validarOpcao(1, 6);

    return op;
}

int processarOpcao(matriz *a, int opcao) {

    int dimensao = 0;

    switch (opcao) {
        case 1:
            if (entradaSistema(a)) {
                resolverSistema(a);
                return 1;
            } else {
                return 0;
            }
            break;
        case 2:
            if (entradaExpressão(a)) {
                transformaçãoLinear(a);
                return  1;
            } else {
                return 0;
            }
            break;
        case 3:
            dimensao = entradaVetores(a);
            if (dimensao > 0) {
                avaliarBase(a, dimensao);
                return 1;
            } else {
                return 0;
            }
            break;
        case 4:
            if (entradaAutovaloresAutovetores(a)) {
                avaliarAutovaloresAutovetores(a);
                return 1;
            } else {
                return 0;
            }
            break;
        case 5:
            if (entradaDiagonalizacao(a)) {
                diagonalizacao(a);
                return 1;
            } else {
                return 0;
            }
            break;
        default:
            return 0;
    }
}

/* -----------------------------------------------------------------------------
   entradaSistema / entradaExpressão / entradaVetores

   entradaAutovaloresAutovetores / processarDiagonalização
   Cada função gerencia a coleta de dados para sua respectiva funcionalidade.
   Exibem um submenu de origem (teclado / arquivo / retornar), leem a entrada
   pelo caminho escolhido e retornam 1 se a entrada foi concluída ou 0 se o
   usuário solicitou retorno ao menu principal. entradaVetores retorna
   a dimensão do espaço (> 0) em vez de 1, pois o chamador precisa desse valor.
   ----------------------------------------------------------------------------- */

int entradaSistema(matriz *a) {

    zerarMatriz(a);

    limparTela();

    int escolha = 0;
    int continuar = 0;
    char equacao[100] = {0};
    char *cabecalho = "RESOLUÇÃO DE SISTEMAS LINEARES";
    int largura = (int)strlen("[1] - inserir equacoes por teclado");

    printarCabecalho(cabecalho, largura);
    printf("[1] - inserir equações por teclado\n");
    printf("[2] - inserir sistema por arquivo\n");
    printf("[3] - retornar ao menu principal");
    separadores(largura);
    printf("* ESCOLHA A OPÇÃO: ");
    escolha = validarOpcao(1, 3);
    if (escolha == 3) return 0;

    if (escolha == 1) {
        limparTela();
        do {
            printarCabecalho(cabecalho, largura);
            printf("\n");
            if (a->linhas > 0) printarSistema(a);
            printf("\n");
            leituraEquacao(a, equacao, sizeof(equacao));
            limparTela();
            printarCabecalho(cabecalho, largura);
            printf("\n");
            printarSistema(a);
            printf("\ndeseja ler outra equação?\n");
            printf("\n[1] - sim\n");
            printf("[2] - não\n");
            printf("[3] - retornar ao menu principal");
            separadores(largura);
            printf("* ESCOLHA A OPÇÃO: ");
            continuar = validarOpcao(1, 3);
            if (continuar == 3) return 0;
            limparTela();
        } while (continuar == 1);
    } else if (escolha == 2)  {
        limparTela();
        printarCabecalho(cabecalho, largura);
        printf("\n");
        leituraEquacaoArquivo(a, equacao, sizeof(equacao));
        limparTela();
        printarCabecalho(cabecalho, largura);
        printf("\n");
        printarSistema(a);
        aguardar("\nPressione ENTER para resolver o sistema...");
        limparTela();
    }

    return 1;

}

int entradaExpressão(matriz *a) {

    zerarMatriz(a);

    limparTela();

    int dim_dominio = 0;
    int dim_contradominio = 0;
    int escolha = 0;
    char expressao[100] = {0};
    char *cabecalho = " ANÁLISE DE TRANSFORMAÇÃO LINEAR";
    int largura = (int)strlen("[1] - inserir expressão por teclado");

    printarCabecalho(cabecalho, largura);
    printf("[1] - inserir expressão por teclado\n");
    printf("[2] - inserir expressão por arquivo\n");
    printf("[3] - retornar ao menu principal");
    separadores(largura);
    printf("* ESCOLHA A OPÇÃO: ");
    escolha = validarOpcao(1, 3);
    if (escolha == 3) return 0;

    if (escolha == 1) {
        limparTela();
        printarCabecalho(cabecalho, largura);
        printf("\n");
        printf("* DOMINIO E CONTRADOMÍNIO, RESPECTIVAMENTE: ");
        scanf("%d %d", &dim_dominio, &dim_contradominio);
        limpezadeBuffer();
        limparTela();
        printarCabecalho(cabecalho, largura);
        printf("\n");
        leituraExpressao(a, expressao, sizeof(expressao), dim_dominio, dim_contradominio);
        limparTela();
        printarCabecalho(cabecalho, largura);
        printf("\n");
        printarExpressao(a);
        printf("\n");
        aguardar("Pressione ENTER para verificar a transformação...");
        limparTela();
        printarCabecalho(cabecalho, largura);
        printf("\n");
    } else if (escolha == 2)  {
        limparTela();
        printarCabecalho(cabecalho, largura);
        printf("\n");
        printf("* DOMINIO E CONTRADOMÍNIO, RESPECTIVAMENTE: ");
        scanf("%d %d", &dim_dominio, &dim_contradominio);
        limpezadeBuffer();
        limparTela();
        printarCabecalho(cabecalho, largura);
        printf("\n");
        leituraExpressaoArquivo(a, expressao, sizeof(expressao), dim_dominio, dim_contradominio);
        limparTela();
        printarCabecalho(cabecalho, largura);
        printf("\n");
        printarExpressao(a);
        printf("\n");
        aguardar("Pressione ENTER para verificar a transformação...");
        limparTela();
        printarCabecalho(cabecalho, largura);
        printf("\n");
    }

    return 1;

}

int entradaVetores(matriz *a) {

    zerarMatriz(a);

    limparTela();

    int dimensao= 0;
    int escolha = 0;
    int continuar = 0;
    char vetor[25] = {0};
    char *cabecalho = " VERIFICAÇÃO DE BASE DO ESPAÇO VETORIAL";
    int largura = (int)strlen("VERIFICAÇÃO DE BASE DO ESPAÇO VETORIAL");

    printarCabecalho(cabecalho, largura);
    printf("[1] - inserir vetores por teclado\n");
    printf("[2] - inserir vetores por arquivo\n");
    printf("[3] - retornar ao menu principal");
    separadores(largura);
    printf("* ESCOLHA A OPÇÃO: ");
    escolha = validarOpcao(1, 3);
    if (escolha == 3) return 0;

    if (escolha == 1) {
        limparTela();
        printarCabecalho(cabecalho, largura);
        printf("\n");
        printf("\n");
        printf("* INSIRA A DIMENSÃO DO ESPAÇO: ");
        scanf("%d", &dimensao);
        limpezadeBuffer();
        do {
            limparTela();
            printarCabecalho(cabecalho, largura);
            printf("\n");
            if (a->linhas > 0) {
                printarVetor(a, dimensao);
                printf("\n");
            }
            printf("\n");
            leituraVetor(a, vetor, sizeof(vetor), dimensao);
            limparTela();
            printarCabecalho(cabecalho, largura);
            printf("\n");
            printarVetor(a, dimensao);
            printf("\n\ndeseja ler outro vetor?\n");
            printf("\n[1] - sim\n");
            printf("[2] - não\n");
            printf("[3] - retornar ao menu principal");
            separadores(largura);
            printf("* ESCOLHA A OPÇÃO: ");
            continuar = validarOpcao(1, 3);
            if (continuar == 3) return 0;
            limparTela();
            printarCabecalho(cabecalho, largura);
        } while (continuar == 1);
    } else if (escolha == 2)  {
        limparTela();
        printarCabecalho(cabecalho, largura);
        printf("\n");
        printf("* INSIRA A DIMENSÃO DO ESPAÇO: ");
        scanf("%d", &dimensao);
        limpezadeBuffer();
        limparTela();
        printarCabecalho(cabecalho, largura);
        printf("\n");
        leituraVetorArquivos(a, vetor, sizeof(vetor), dimensao);
        limparTela();
        printarCabecalho(cabecalho, largura);
        printf("\n");
        printarVetor(a, dimensao);
        printf("\n\n");
        aguardar("Pressione ENTER para resolver o sistema...");
        limparTela();
        printarCabecalho(cabecalho, largura);
    }

    return dimensao;

}

int entradaAutovaloresAutovetores (matriz *a) {
    zerarMatriz(a);

    limparTela();

    int escolha = 0;
    char expressao[100] = {0};
    char *cabecalho = " CÁLCULO DE AUTOVALORES E AUTOVETORES";
    int largura = (int)strlen(cabecalho);

    printarCabecalho(cabecalho, largura);
    printf("[1] - inserir expressão por teclado\n");
    printf("[2] - inserir expressão por arquivo\n");
    printf("[3] - retornar ao menu principal");
    separadores(largura);
    printf("* ESCOLHA A OPÇÃO: ");
    escolha = validarOpcao(1, 3);
    if (escolha == 3) return 0;

    if (escolha == 1) {
        limparTela();
        printarCabecalho(cabecalho, largura);
        printf("\n* ATENÇÃO: restrito a transformações R² → R²\n\n");
        leituraExpressao(a, expressao, sizeof(expressao), 2, 2);
        limparTela();
        printarCabecalho(cabecalho, largura);
        printf("\n");
        printarExpressao(a);
        printf("\n\n");
        aguardar("Pressione ENTER para calcular autovalores e autovetores...");
        limparTela();
        printarCabecalho(cabecalho, largura);
        printf("\n");
    } else if (escolha == 2)  {
        limparTela();
        printarCabecalho(cabecalho, largura);
        printf("\n* ATENÇÃO: restrito a transformações R² → R²\n\n");
        leituraExpressaoArquivo(a, expressao, sizeof(expressao), 2, 2);
        limparTela();
        printarCabecalho(cabecalho, largura);
        printf("\n");
        printarExpressao(a);
        printf("\n\n");
        aguardar("Pressione ENTER para calcular autovalores e autovetores...");
        limparTela();
        printarCabecalho(cabecalho, largura);
        printf("\n");
    }

    return 1;
}

int entradaDiagonalizacao (matriz *a) {

    zerarMatriz(a);

    limparTela();

    int escolha = 0;
    char expressao[100] = {0};
    char *cabecalho = " VERIFICAR POSSIBILIDADE DE DIAGONALIZAÇÃO";
    int largura = (int)strlen(cabecalho);

    printarCabecalho(cabecalho, largura);
    printf("[1] - inserir expressão por teclado\n");
    printf("[2] - inserir expressão por arquivo\n");
    printf("[3] - retornar ao menu principal");
    separadores(largura);
    printf("* ESCOLHA A OPÇÃO: ");
    escolha = validarOpcao(1, 3);
    if (escolha == 3) return 0;

    if (escolha == 1) {
        limparTela();
        printarCabecalho(cabecalho, largura);
        printf("\n* ATENÇÃO: restrito a transformações R² → R²\n\n");
        leituraExpressao(a, expressao, sizeof(expressao), 2, 2);
        limparTela();
        printarCabecalho(cabecalho, largura);
        printf("\n");
        printarExpressao(a);
        printf("\n");
        aguardar("Pressione ENTER para verificar diagonalização...");
        limparTela();
        printarCabecalho(cabecalho, largura);
        printf("\n");
    } else if (escolha == 2)  {
        limparTela();
        printarCabecalho(cabecalho, largura);
        printf("\n* ATENÇÃO: restrito a transformações R² → R²\n\n");
        leituraExpressaoArquivo(a, expressao, sizeof(expressao), 2, 2);
        limparTela();
        printarCabecalho(cabecalho, largura);
        printf("\n");
        printarExpressao(a);
        printf("\n");
        aguardar("Pressione ENTER para verificar diagonalização...");
        limparTela();
        printarCabecalho(cabecalho, largura);
        printf("\n");
    }

    return 1;
}

/* -----------------------------------------------------------------------------
   separadores / espacadores / cabecalho

   Auxiliares de formatação visual. separadores imprime uma linha de
   hifens de comprimento dado; espacadores imprime n espaços;
   cabecalho centraliza o título entre dois separadores.
   ----------------------------------------------------------------------------- */

void separadores (int tamanho_frase) {
    printf("\n");
    for (int i = 0; i < tamanho_frase; i++) printf("-");
    printf("\n");
}

void espacadores (int quantidade_espacos) {
    for (int i = 0; i < quantidade_espacos; i++) {
        printf(" ");
    }
}

/* -----------------------------------------------------------------------------
   aguardar / limparTela

   aguardar exibe uma mensagem e bloqueia até o usuário pressionar ENTER.
   limparTela limpa o terminal via sequência ANSI no Linux ou "cls" no Windows.
   ----------------------------------------------------------------------------- */

void aguardar(char *mensagem) {
    printf("%s", mensagem);
    getchar();
}

void limparTela() {
#ifdef _WIN32
    system("cls");
#else
    printf("\033[H\033[J");
#endif
}

void printarCabecalho(char *titulo, int largura) {
    int espacos = (largura - (int)strlen(titulo)) / 2;
    separadores(largura);
    espacadores(espacos);
    printf("%s", titulo);
    separadores(largura);
}

/* -----------------------------------------------------------------------------
   validarOpcao

   Lê um inteiro e rejeita em loop qualquer valor fora do intervalo [min, max],
   retornando apenas quando a entrada for válida.

   Parâmetros:
     - int min: menor opção aceita.
     - int max: maior opção aceita.

   Retorno:
     inteiro válido dentro do intervalo.
   ----------------------------------------------------------------------------- */

int validarOpcao(int min, int max) {
    int op = 0;
    scanf("%d", &op);
    limpezadeBuffer();
    while (op < min || op > max) {
        printf("\nopção inválida, tente novamente: ");
        scanf("%d", &op);
        limpezadeBuffer();
    }
    return op;
}
