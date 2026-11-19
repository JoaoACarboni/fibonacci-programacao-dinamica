/*
 * Árvore de recursão do Fibonacci
 * -------------------------------
 * Desenha no terminal todas as chamadas feitas para calcular F(n):
 *   - primeiro na versão ingênua, marcando os subproblemas REPETIDOS;
 *   - depois na versão com memoização, mostrando onde o cache "poda" a árvore.
 *
 * Compilar:  gcc -Wall -o arvore arvore.c
 * Executar:  ./arvore 5        (n entre 1 e 8 para caber na tela)
 *
 * No Windows, rode "chcp 65001" antes para exibir os caracteres corretamente.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_N 8

static int vezes[MAX_N + 1];      /* quantas vezes cada fib(k) foi chamado */
static long long memo[MAX_N + 1];
static int calculado[MAX_N + 1];
static int total_chamadas;

/* Imprime o nó com o prefixo de "galhos" adequado */
static void imprime_no(const char *prefixo, int ultimo, int raiz, int n) {
    if (raiz) printf("fib(%d)", n);
    else      printf("%s%s fib(%d)", prefixo, ultimo ? "└──" : "├──", n);
}

static void monta_prefixo(char *destino, size_t tam, const char *prefixo, int ultimo, int raiz) {
    if (raiz) destino[0] = '\0';
    else snprintf(destino, tam, "%s%s", prefixo, ultimo ? "    " : "│   ");
}

/* ---------------- versão ingênua ---------------- */
static long long arvore_ingenua(int n, const char *prefixo, int ultimo, int raiz) {
    total_chamadas++;
    vezes[n]++;
    imprime_no(prefixo, ultimo, raiz, n);
    if (vezes[n] > 1 && n > 1) printf("   <-- REPETIDO (%dª vez)", vezes[n]);
    printf("\n");

    if (n <= 1) return n;

    char novo[512];
    monta_prefixo(novo, sizeof novo, prefixo, ultimo, raiz);
    long long a = arvore_ingenua(n - 1, novo, 0, 0);
    long long b = arvore_ingenua(n - 2, novo, 1, 0);
    return a + b;
}

/* ---------------- versão memoizada ---------------- */
static long long arvore_memo(int n, const char *prefixo, int ultimo, int raiz) {
    total_chamadas++;
    imprime_no(prefixo, ultimo, raiz, n);

    if (n <= 1) { printf("\n"); return n; }
    if (calculado[n]) {
        printf(" = %lld   [veio do cache]\n", memo[n]);
        return memo[n];                       /* não expande: a árvore é podada aqui */
    }
    printf("\n");

    char novo[512];
    monta_prefixo(novo, sizeof novo, prefixo, ultimo, raiz);
    long long a = arvore_memo(n - 1, novo, 0, 0);
    long long b = arvore_memo(n - 2, novo, 1, 0);
    memo[n] = a + b;
    calculado[n] = 1;
    return memo[n];
}

int main(int argc, char *argv[]) {
    int n = (argc > 1) ? atoi(argv[1]) : 5;
    if (n < 1 || n > MAX_N) {
        printf("Use n entre 1 e %d.\n", MAX_N);
        return 1;
    }

    printf("\n===== VERSÃO INGÊNUA: fib(%d) =====\n\n", n);
    total_chamadas = 0;
    long long r = arvore_ingenua(n, "", 1, 1);
    int chamadas_ingenua = total_chamadas;

    printf("\nResultado: %lld | Total de chamadas: %d\n", r, chamadas_ingenua);
    printf("Quantas vezes cada subproblema foi resolvido:\n");
    for (int k = n; k >= 0; k--) {
        printf("  fib(%d): %2dx  ", k, vezes[k]);
        for (int i = 0; i < vezes[k]; i++) printf("█");
        printf("\n");
    }

    printf("\n===== COM MEMOIZAÇÃO: fib(%d) =====\n\n", n);
    total_chamadas = 0;
    r = arvore_memo(n, "", 1, 1);

    printf("\nResultado: %lld | Total de chamadas: %d (antes eram %d)\n\n",
           r, total_chamadas, chamadas_ingenua);
    return 0;
}