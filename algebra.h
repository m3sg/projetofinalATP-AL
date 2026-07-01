#ifndef PROJETOFINALATPAL_ALGEBRA_H
#define PROJETOFINALATPAL_ALGEBRA_H

#define TAM_MAX    10  //constante que define dimensão máxima da matriz.
#define NOME_MAX    2  //constante que define dimensão máxima do nome de uma incógnita em caracteres.
#define CAMINHO_MAX 100 //constante que define o tamanho máximo do caminho de arquivo.

typedef struct {
    int linhas; //guarda quantas linhas a matriz utiliza efetivamente.
    double independentes[TAM_MAX]; //guarda apenas os termos independentes.
    double dados[TAM_MAX][TAM_MAX]; //é a matriz em si, valores numéricos arranjados [linha]x[coluna]
    char incognitas[TAM_MAX][NOME_MAX]; //array de strings: 1ª dimensão: qt. de incógnitas; 2ª dimensão: composição da incógnita.
    int n_incognitas; //quantidade de incógnitas no sistema, equivalente a qt. de colunas para a matriz dos coeficientes.
} matriz;

//funções da matriz.c
void zerarMatriz (matriz *a);
void printarMatriz(matriz *a, int modo);
void trocarLinhas (matriz *a, int l1, int l2);
void combinarLinhas (matriz *a, int l1, int l2, double escalar);

//funções da parser.c
void limpezadeBuffer (void);
void tirarEspaco (char *s);
int tratarIncognita (matriz *a, char *incognita);
int larguraFormatacao (matriz *a, int sinal);
void parserEquacao (matriz *a, char *equacao);
void leituraEquacao (matriz *a, char *equacao, int buf_tamanho);
void leituraVetor (matriz *a, char *vetor, int buf_tamanho, int tamanho);
void leituraVetorArquivos (matriz *a, char *vetor, int buf_tamanho, int tamanho);
int parserVetor (matriz *a, char *vetor, int tamanho);
void printarVetor (matriz *a, int tamanho);
void leituraEquacaoArquivo (matriz *a, char *equacao, int buf_tamanho);
void leituraExpressao (matriz *a, char *expressao, int tam_buffer, int dim_dominio, int dim_contradominio);
void leituraExpressaoArquivo (matriz *a, char *expressao, int tam_buffer, int dim_dominio, int dim_contradominio);
void parserExpressao (matriz *a, char *expressao);
void printarExpressao (matriz *a);
void printarSistema (matriz *a);

//funções da sistema.c
matriz eliminacaoGauss(matriz *a, int v);
void resolverSistema(matriz *a);
void resolverSistemaSPI (matriz *a);
int avaliarSistema(matriz *a);

//funções da transformacao.c
int posto(matriz *a, int aumentada);
int injetividade (matriz *a);
int sobrejetividade (matriz *a);
int bijetividade (matriz *a);
int testeBase (matriz *a, int dimensao, int v);
int avaliarBase (matriz *a, int dimensao);
matriz dimensaoNucleo (matriz *a);
matriz dimensaoImagem (matriz *original, matriz *escalonada);
void transformaçãoLinear (matriz *a);
int diagonalizacao(matriz *a);

//funções da autovaloresAutovetores.c
int autovalores (matriz *a, double *lambda1, double *lambda2);
void autovetores (matriz *a, double lambda, double *autovetor_saida, int indice);
void resolverAutovetor (matriz *a, double *autovetor);
void avaliarAutovaloresAutovetores (matriz *a);

//funções da menu.c
void printarOpcoes ();
int lerOpcao ();
int processarOpcao (matriz *a, int opcao);
int entradaSistema(matriz *a);
int entradaExpressão(matriz *a);
int entradaVetores(matriz *a);
int entradaAutovaloresAutovetores (matriz *a);
int entradaDiagonalizacao (matriz *a);
void separadores (int tamanho_frase);
void espacadores (int quantidade_espacos);
void aguardar(char *mensagem);
void limparTela();
void printarCabecalho(char *titulo, int largura);
int validarOpcao(int min, int max);

#endif //PROJETOFINALATPAL_ALGEBRA_H