# Sistema de Álgebra Linear em C

## Visão Geral

Este programa implementa, em linguagem C, cinco operações fundamentais de Álgebra Linear, acessíveis por um menu interativo no terminal: resolução de sistemas lineares, análise de transformações lineares (dimensão do núcleo, dimensão da imagem, injetividade, sobrejetividade, bijetividade), verificação de bases de espaços vetoriais, cálculo de autovalores e autovetores, e verificação de diagonalização.

Toda a lógica matemática é implementada sem bibliotecas externas de álgebra linear. O programa aceita entrada tanto via teclado quanto por arquivo de texto, em todas as opções do menu.

---

## Como Compilar e Rodar

O projeto pode ser compilado de duas formas.

**Opção 1 — CMake**
```bash
mkdir build
cd build
cmake ..
cmake --build .
```
O executável é gerado dentro da pasta `build/`.

**Opção 2 — Make**
```bash
make
```
O executável é gerado diretamente na raiz do projeto.

**Pré-requisitos:** um compilador C. Nenhuma biblioteca externa precisa ser instalada.

**Para rodar**, basta executar:
```bash
./projetofinalATPAL        # Linux/Mac
projetofinalATPAL.exe      # Windows
```

---

## Como Usar

Ao iniciar, o programa exibe um menu com seis opções. As cinco primeiras levam a um submenu de entrada (teclado ou arquivo), e a sexta encerra o programa.

### [1] Resolver sistema linear
Recebe um conjunto de equações lineares e retorna a classificação do sistema (determinado, indeterminado ou impossível) e a solução, quando existir. Cada equação é digitada com todos os termos espaçados entre si:
```
x + y + z = 1
```

### [2] Analisar transformação linear
Recebe a dimensão do domínio e do contradomínio, variáveis esperadas, seguida da expressão da transformação, e retorna a dimensão do núcleo, a dimensão da imagem, e se a transformação é injetora, sobrejetora e/ou bijetora. A expressão é escrita entre parênteses, com vírgula e espaço separando cada componente:
```
(x + y + z, x + y + z)
```

### [3] Verificar base de espaço vetorial
Recebe a dimensão do espaço e um conjunto de vetores, e retorna se o conjunto forma uma base. Cada vetor é escrito entre parênteses, com vírgula e espaço entre os componentes:
```
(2, 1)
```

### [4] Calcular autovalores e autovetores
Recebe a expressão de um operador linear, bem como as variáveis esperadas (restrito a R² → R², dimensão fixa), e retorna os autovalores e seus respectivos autovetores. Caso os autovalores sejam complexos, o programa informa claramente que não há autovalores reais, em vez de exibir resultado numérico inválido. Formato de expressão igual ao da opção 2.

### [5] Verificar diagonalização
Recebe a expressão de um operador linear, bem como as variáveis esperadas (também restrito a R² → R²), e retorna se ele é diagonalizável em R, exibindo a matriz diagonal D quando aplicável.

Em todas as opções com entrada por teclado, o programa guia o usuário por um fluxo fixo de perguntas, sendo a primeira sobre a dimensão (quando for preciso), e depois os dados de entrada necessários. Recomendam-se as entradas como descritas acima (parênteses, vírgulas, sinal de igual), mas o programa tolera pequenas variações, como espaçamento livre entre os termos, coeficiente implícito (x equivale a 1x), e não diferencia maiúsculas de minúsculas.

A entrada por arquivo segue o mesmo formato da entrada por teclado, uma equação/vetor/linha de expressão por linha do arquivo. Em qualquer opção do menu, ao escolher entrada por arquivo o programa solicita o caminho do arquivo, repetindo o pedido até que um caminho válido seja informado. Dentro da pasta do projeto, o diretório exemplos/ contém arquivos de texto prontos para uso com cada uma das cinco opções, servindo tanto para teste rápido quanto como referência de formatação.

---

## Estrutura do Projeto

```
main.c                    → entrada do programa, que chama todos os outro menus e funções
algebra.h                 → struct + cabeçalho das funções
matriz.c                  → operações elementares sobre a struct matriz
parser.c                  → leitura e interpretação de texto (equações, vetores, expressões)
sistema.c                 → escalonamento, classificação e resolução de sistemas
transformacao.c           → núcleo, imagem, injetividade/sobrejetividade/bijetividade, bases, diagonalização
autovaloresAutovetores.c  → polinômio característico, autovalores e autovetores
menu.c                    → interface de navegação, submenus e entrada de dados
```

Cada arquivo `.c` concentra um tema da álgebra linear, ou funções de menu/leitura, enquanto que `algebra.h` é a biblioteca que dá acesso a struct, constantes e funções definidas.

### A struct `matriz`

Toda a representação de dados do programa acontece em uma única struct:

```c
#define TAM_MAX     10
#define NOME_MAX     2

typedef struct {
    int    linhas;
    double independentes[TAM_MAX];
    double dados[TAM_MAX][TAM_MAX];
    char   incognitas[TAM_MAX][NOME_MAX];
    int    n_incognitas;
} matriz;
```

- **`linhas`** e **`n_incognitas`** guardam as dimensões reais em uso. A memória alocada é `TAM_MAX × TAM_MAX`, mas o código nunca itera além dessas dimensões, para não processar lixo de memória.
- **`dados`** é a matriz de coeficientes: `dados[i][j]` é o coeficiente da incógnita `j` na linha `i`.
- **`independentes`** guarda separadamente os termos à direita do `=` em cada equação (ajuda a separar matriz dos coeficientes de matriz expandida).
- **`incognitas`** guarda o nome de cada variável (`"x"`, `"y"`, `"z"`, etc.), permitindo que o programa imprima resultados com os nomes originais informados pelo usuário.

Usar `double` aqui é justificável pois a eliminação de Gauss pode acumular erros de arredondamento ao longo das operações, e `double` reduz esse acúmulo a um nível menor do que float faria.

Essa mesma struct é reaproveitada em todos os módulos. Um sistema linear, um conjunto de vetores e a matriz de uma transformação são, internamente, a mesma estrutura de dados, apenas interpretada de formas diferentes conforme o contexto.

---

## Decisões de Projeto

**Matriz quase sempre por referência.** Grande parte das funções recebe `matriz *a` (ponteiro) em vez de `matriz a` (cópia por valor). As únicas exceções são funções como `eliminacaoGauss`, `dimensaoNucleo`, e `dimensaoImagem`, que retornam uma nova `matriz` por valor, já que o objetivo ali é justamente produzir uma versão modificada, mas sem alterar a original, e o retorno por valor deixa isso mais claro.

**Sempre operar sobre cópia local, nunca sobre a entrada original.** Funções que transformam a matriz (como o escalonamento) começam copiando a struct recebida (`matriz copia = *a;`) e operam sobre essa cópia. Isso permite, por exemplo, que `autovetores` escalone a matriz `(A - λI)` sem afetar a matriz original `A`, que ainda será necessária para o cálculo do segundo autovalor.

**Comparações com 1e-9 ao invés de 0.** Sempre que o código precisa decidir se um valor `double` é "zero" (para identificar pivôs inválidos, colunas livres, ou linhas nulas no cálculo de posto) a comparação usa `1e-9` em vez de comparar com 0. Devido ao acúmulo de erros de arredondamento com ponto flutuante, e uma comparação direta com `0.0` pode classificar um valor "quase zero" como não-nulo (algo que aconteceu durante os testes de desenvolvimento). Ademais, há outra implementação que visa evitar erros de arrendodamento: o pivoteamento no escalonamento busca sempre o maior valor absoluto disponível na coluna, via função fabs.

**Parser com formato fixo, mas tolerante a pequenas variações.** Em vez de tentar interpretar múltiplos formatos de entrada, o parser exige um formato único para cada tipo de dado. Isso simplifica a lógica de interpretação, guiando o usuário pelo caminho correto de entrada. Mas existem tolerâncias a pequenas variações, como por exemplo: ignorar espaços, lidar com maiúsculas e minúsculas, saber inferir 0 na falta de coeficiente ou incógnita, e etc.

**Separação entre leitura, interpretação e organização de chamadas.** Para equações, vetores e expressões, existem sempre duas funções distintas: uma lerX, responsável por ler os dados (via teclado ou arquivo), e uma parsearX, responsável por interpretar o que foi lido e preencher a matriz. Essa separação é o que permite que a mesma lógica de interpretação seja reaproveitada tanto para entrada via teclado quanto via arquivo, sem duplicar a parte de parsing. Acima dessas duas camadas, existe sempre uma função organizadora por opção do menu, como a `entradaSistema` ou `entradaVetores`. Elas apresentam o submenu de escolha (teclado ou arquivo), chamam a função de leitura correta, e chamam a função de cálculo. O menu.c nunca lida diretamente com parsing ou matemática, ele apenas chama essas organizadora.

**Sistema de revalidação até entrada correta.** Em todo ponto do programa que depende de algo fornecido externamente a estratégia é sempre a mesma: validar a entrada e, se inválida, pedir novamente em loop, sem jamais prosseguir com um valor incorreto. Isso se repete identicamente em pontos completamente diferentes do código (leitura de opções de menu, leitura de equações, leitura de vetores, leitura de caminho de arquivo).

**Função de limpeza de tela portável entre sistemas operacionais.** A função `limparTela` usa (`#ifdef _WIN32`) para rodar o comando `cls` no Windows ou a sequência "\033[H\033[J" em sistemas Linux e Mac. O intuito é tentar que os menus se comportem de maneira parecida, independentemente do sistema.

**Exibição explícita da matriz de mudança de base na diagonalização.** Ao diagonalizar um operador, o programa não imprime apenas a matriz diagonal D, mas monta e exibe também P, a matriz de mudança de base formada pelos autovetores como colunas, em vez de apresentar só o resultado final sem o contexto de como ele se conecta à matriz original.

**Build duplo (CMake + Makefile).** O projeto fornece duas formas de compilação, pois o programa foi desenvolvido em Linux, com a IDE CLION, mas o intuito é que possa rodar em qualquer outro ambiente.
