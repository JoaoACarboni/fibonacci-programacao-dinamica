/*
 * Fibonacci com Programação Dinâmica
 * ----------------------------------
 * Quatro formas de calcular F(n), da pior para a melhor:
 *
 *   1) Recursão ingênua ........ O(2^n) tempo | O(n) memória (pilha)
 *   2) Memoização (top-down) ... O(n)   tempo | O(n) memória
 *   3) Tabulação (bottom-up) ... O(n)   tempo | O(n) memória
 *   4) Otimizada (2 variáveis) . O(n)   tempo | O(1) memória
 *
 * Compilar:  gcc -O2 -Wall -o fibonacci fibonacci.c
 * Executar:  ./fibonacci        -> tabela comparativa
 *            ./fibonacci 50     -> F(50) por todos os métodos possíveis
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

typedef unsigned long long u64;

#define MAX_N        93  /* F(93) é o maior Fibonacci que cabe em 64 bits sem sinal */
#define MAX_INGENUO  45  /* acima disso a versão ingênua demora demais */

static u64 chamadas; /* conta quantas vezes a função foi chamada */

/* ------------------------------------------------------------------ */
/* 1) Recursão ingênua: recalcula os mesmos subproblemas várias vezes  */
/* ------------------------------------------------------------------ */
u64 fib_ingenuo(int n) {
    chamadas++;
    if (n <= 1) return (u64)n;               /* casos base: F(0)=0, F(1)=1 */
    return fib_ingenuo(n - 1) + fib_ingenuo(n - 2);
}

/* ------------------------------------------------------------------ */
/* 2) Memoização (top-down): mesma recursão, mas guarda cada resultado */
/*    na primeira vez e reaproveita nas próximas                       */
/* ------------------------------------------------------------------ */
static u64 memo[MAX_N + 1];
static int calculado[MAX_N + 1];

static u64 fib_memo_rec(int n) {
    chamadas++;
    if (n <= 1) return (u64)n;
    if (calculado[n]) return memo[n];        /* já sei a resposta: só consulto */
    memo[n] = fib_memo_rec(n - 1) + fib_memo_rec(n - 2);
    calculado[n] = 1;
    return memo[n];
}

u64 fib_memo(int n) {
    memset(calculado, 0, sizeof calculado);  /* limpa o cache */
    return fib_memo_rec(n);
}

/* ------------------------------------------------------------------ */
/* 3) Tabulação (bottom-up): preenche uma tabela do menor para o maior */
/* ------------------------------------------------------------------ */
u64 fib_tabela(int n) {
    if (n <= 1) return (u64)n;
    u64 *t = malloc((size_t)(n + 1) * sizeof *t);
    if (t == NULL) { perror("malloc"); exit(1); }
    t[0] = 0;
    t[1] = 1;
    for (int i = 2; i <= n; i++)
        t[i] = t[i - 1] + t[i - 2];          /* cada valor depende só dos 2 anteriores */
    u64 r = t[n];
    free(t);
    return r;
}

/* ------------------------------------------------------------------ */
/* 4) Otimizada: se só preciso dos 2 últimos valores, não preciso da   */
/*    tabela inteira                                                   */
/* ------------------------------------------------------------------ */
u64 fib_otimizado(int n) {
    u64 a = 0, b = 1;                        /* a = F(i), b = F(i+1) */
    for (int i = 0; i < n; i++) {
        u64 prox = a + b;
        a = b;
        b = prox;
    }
    return a;
}

/* Mede o tempo (em ms) de uma função, contando as chamadas */
static double medir(u64 (*f)(int), int n, u64 *resultado, u64 *qtd_chamadas) {
    chamadas = 0;
    clock_t ini = clock();
    *resultado = f(n);
    clock_t fim = clock();
    *qtd_chamadas = chamadas;
    return 1000.0 * (double)(fim - ini) / CLOCKS_PER_SEC;
}

static void tabela_comparativa(void) {
    int ns[] = {5, 10, 20, 30, 35, 40};
    int qtd = (int)(sizeof ns / sizeof ns[0]);

    printf("\nPrimeiros termos: ");
    for (int i = 0; i <= 15; i++) printf("%llu ", fib_otimizado(i));
    printf("...\n\n");

    printf("+----+-------------+------------------+---------------+---------------+-------------+\n");
    printf("|  n |        F(n) | chamadas ingênua | tempo ingênua | chamadas memo |  tempo memo |\n");
    printf("+----+-------------+------------------+---------------+---------------+-------------+\n");
    for (int i = 0; i < qtd; i++) {
        int n = ns[i];
        u64 r1, r2, c1, c2;
        double t1 = medir(fib_ingenuo, n, &r1, &c1);
        double t2 = medir(fib_memo, n, &r2, &c2);
        printf("| %2d | %11llu | %16llu | %10.2f ms | %13llu | %8.4f ms |\n",
               n, r1, c1, t1, c2, t2);
    }
    printf("+----+-------------+------------------+---------------+---------------+-------------+\n");
    printf("\nRepare: a cada +5 no n, a versão ingênua faz ~11x mais chamadas (crescimento exponencial).\n");
    printf("A memoizada faz sempre 2n-1 chamadas (crescimento linear).\n\n");
}

static void calcular_um(int n) {
    u64 r, c;
    double t;

    printf("\nCalculando F(%d):\n\n", n);

    if (n <= MAX_INGENUO) {
        t = medir(fib_ingenuo, n, &r, &c);
        printf("  1) Ingênua ....... %llu  (%llu chamadas, %.3f ms)\n", r, c, t);
    } else {
        printf("  1) Ingênua ....... pulada (n > %d levaria tempo demais!)\n", MAX_INGENUO);
    }

    t = medir(fib_memo, n, &r, &c);
    printf("  2) Memoização .... %llu  (%llu chamadas, %.3f ms)\n", r, c, t);

    t = medir(fib_tabela, n, &r, &c);
    printf("  3) Tabulação ..... %llu  (%.3f ms)\n", r, t);

    t = medir(fib_otimizado, n, &r, &c);
    printf("  4) Otimizada ..... %llu  (%.3f ms)\n\n", r, t);
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        tabela_comparativa();
        return 0;
    }

    int n = atoi(argv[1]);
    if (n < 0 || n > MAX_N) {
        printf("Use 0 <= n <= %d (F(94) já não cabe em 64 bits).\n", MAX_N);
        return 1;
    }
    calcular_um(n);
    return 0;
}