/*
 * ============================================================
 *  JOGO DA MEMORIA - Terminal
 * ============================================================
 *  Autor  : IgorASB
 *  Data   : 20/05/2026
 *
 *  Conceitos praticados:
 *   - Matriz 2D alocada dinamicamente (ponteiro para ponteiro)
 *   - Structs para celulas do tabuleiro
 *   - Funcoes com ponteiros como parametro
 *   - Embaralhamento com array de ponteiros (Fisher-Yates)
 *   - Arquivo texto para salvar recordes
 *   - Loops aninhados e logica de jogo
 * ============================================================
 *
 *  Compilar: gcc -Wall -o jogo_memoria src/jogo_memoria.c
 *  Rodar   : ./jogo_memoria
 * ============================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

/* ============================================================
 *  CONSTANTES
 * ============================================================ */
#define ARQUIVO_RECORDES "recordes.txt"
#define MAX_RECORDES     5
#define SIMBOLOS         "ABCDEFGHIJKLMNOPQRSTUVWXYZ"

/* ============================================================
 *  ESTRUTURAS
 * ============================================================ */

/* Representa uma celula do tabuleiro */
typedef struct {
    char simbolo;   /* par que a celula representa (ex: 'A', 'B' ...) */
    int  revelada;  /* 1 = encontrada/permanentemente visivel, 0 = oculta */
} Celula;

/* Tabuleiro com matriz alocada dinamicamente */
typedef struct {
    Celula **grid;  /* ponteiro para ponteiro: matriz 2D dinamica */
    int     linhas;
    int     colunas;
    int     pares_total;
    int     pares_encontrados;
    int     tentativas;
} Tabuleiro;

/* Recorde de uma partida */
typedef struct {
    int tentativas;
    int linhas;
    int colunas;
} Recorde;

/* ============================================================
 *  SECAO 1 - ALOCACAO E LIBERACAO DO TABULEIRO
 * ============================================================ */

/*
 * Aloca a matriz 2D dinamicamente usando ponteiro para ponteiro.
 * grid[i] e um ponteiro para um array de Celula (cada linha).
 */
Tabuleiro *criar_tabuleiro(int linhas, int colunas) {
    Tabuleiro *t = (Tabuleiro *) malloc(sizeof(Tabuleiro));
    if (!t) { fprintf(stderr, "Erro ao alocar tabuleiro.\n"); exit(1); }

    /* Aloca array de ponteiros (um por linha) */
    t->grid = (Celula **) malloc(linhas * sizeof(Celula *));
    if (!t->grid) { fprintf(stderr, "Erro ao alocar linhas.\n"); exit(1); }

    /* Aloca cada linha individualmente */
    for (int i = 0; i < linhas; i++) {
        t->grid[i] = (Celula *) malloc(colunas * sizeof(Celula));
        if (!t->grid[i]) { fprintf(stderr, "Erro ao alocar coluna %d.\n", i); exit(1); }
    }

    t->linhas             = linhas;
    t->colunas            = colunas;
    t->pares_total        = (linhas * colunas) / 2;
    t->pares_encontrados  = 0;
    t->tentativas         = 0;

    return t;
}

/* Libera toda a memoria da matriz linha por linha */
void destruir_tabuleiro(Tabuleiro *t) {
    for (int i = 0; i < t->linhas; i++)
        free(t->grid[i]);
    free(t->grid);
    free(t);
}

/* ============================================================
 *  SECAO 2 - INICIALIZACAO E EMBARALHAMENTO
 * ============================================================ */

/*
 * Preenche o tabuleiro com pares de simbolos e embaralha.
 * Usa Fisher-Yates sobre um array de ponteiros para Celula:
 * nenhum dado e copiado, apenas os ponteiros sao trocados.
 */
void inicializar_tabuleiro(Tabuleiro *t) {
    int total = t->linhas * t->colunas;
    int pares = total / 2;

    /* Array auxiliar de ponteiros para todas as celulas */
    Celula **celulas = (Celula **) malloc(total * sizeof(Celula *));
    if (!celulas) { fprintf(stderr, "Erro ao alocar auxiliar.\n"); exit(1); }

    /* Mapeia cada ponteiro para sua posicao na matriz */
    int k = 0;
    for (int i = 0; i < t->linhas; i++)
        for (int j = 0; j < t->colunas; j++)
            celulas[k++] = &t->grid[i][j];

    /* Preenche pares de simbolos nas celulas apontadas */
    for (int p = 0; p < pares; p++) {
        char s = SIMBOLOS[p % 26];
        celulas[p * 2]->simbolo     = s;
        celulas[p * 2 + 1]->simbolo = s;
        celulas[p * 2]->revelada     = 0;
        celulas[p * 2 + 1]->revelada = 0;
    }

    /* Se total e impar, celula extra recebe '?' visivel */
    if (total % 2 != 0) {
        celulas[total - 1]->simbolo  = '?';
        celulas[total - 1]->revelada = 1;
    }

    /* Fisher-Yates: embaralha trocando o conteudo das celulas */
    srand((unsigned int) time(NULL));
    for (int i = total - 1; i > 0; i--) {
        int j = rand() % (i + 1);
        Celula tmp    = *celulas[i];
        *celulas[i]   = *celulas[j];
        *celulas[j]   = tmp;
    }

    free(celulas);
}

/* ============================================================
 *  SECAO 3 - EXIBICAO DO TABULEIRO
 * ============================================================ */

void imprimir_cabecalho_colunas(int colunas) {
    printf("    ");
    for (int j = 0; j < colunas; j++)
        printf(" %2d ", j + 1);
    printf("\n    ");
    for (int j = 0; j < colunas; j++)
        printf("----");
    printf("\n");
}

/*
 * Exibe o tabuleiro. Celulas reveladas mostram o simbolo;
 * ocultas mostram '[ ]'. dl/dc e sl/sc sao as posicoes
 * temporariamente destacadas na rodada atual (-1 = nenhuma).
 */
void exibir_tabuleiro(Tabuleiro *t, int dl, int dc, int sl, int sc) {
    printf("\n");
    imprimir_cabecalho_colunas(t->colunas);

    for (int i = 0; i < t->linhas; i++) {
        printf(" %2d| ", i + 1);
        for (int j = 0; j < t->colunas; j++) {
            Celula *c = &t->grid[i][j];
            int destaque = (i == dl && j == dc) || (i == sl && j == sc);
            if (c->revelada || destaque)
                printf("[%c] ", c->simbolo);
            else
                printf("[ ] ");
        }
        printf("\n");
    }
    printf("\nTentativas: %d | Pares encontrados: %d/%d\n",
           t->tentativas, t->pares_encontrados, t->pares_total);
}

/* ============================================================
 *  SECAO 4 - LOGICA DE JOGADA
 * ============================================================ */

/* Le e valida uma coordenada do jogador */
void ler_coordenada(Tabuleiro *t, int *linha, int *coluna,
                    int bloq_l, int bloq_c) {
    while (1) {
        printf("  Linha (1-%d): ", t->linhas);
        scanf("%d", linha);
        printf("  Coluna (1-%d): ", t->colunas);
        scanf("%d", coluna);

        (*linha)--;
        (*coluna)--;

        if (*linha < 0 || *linha >= t->linhas ||
            *coluna < 0 || *coluna >= t->colunas) {
            printf("  Posicao invalida. Tente novamente.\n");
            continue;
        }
        if (t->grid[*linha][*coluna].revelada) {
            printf("  Essa celula ja foi encontrada. Escolha outra.\n");
            continue;
        }
        if (*linha == bloq_l && *coluna == bloq_c) {
            printf("  Voce ja escolheu essa celula. Escolha outra.\n");
            continue;
        }
        break;
    }
}

/* Processa uma rodada completa (duas escolhas) */
void jogar_rodada(Tabuleiro *t) {
    int l1, c1, l2, c2;

    printf("\n--- Escolha a 1a carta ---\n");
    ler_coordenada(t, &l1, &c1, -1, -1);

    exibir_tabuleiro(t, l1, c1, -1, -1);

    printf("\n--- Escolha a 2a carta ---\n");
    ler_coordenada(t, &l2, &c2, l1, c1);

    t->tentativas++;

    exibir_tabuleiro(t, l1, c1, l2, c2);

    if (t->grid[l1][c1].simbolo == t->grid[l2][c2].simbolo) {
        printf("*** PAR ENCONTRADO! '%c' ***\n", t->grid[l1][c1].simbolo);
        t->grid[l1][c1].revelada = 1;
        t->grid[l2][c2].revelada = 1;
        t->pares_encontrados++;
    } else {
        printf("--- Nao e par. As cartas serao ocultadas novamente. ---\n");
        printf("(pressione ENTER para continuar)\n");
        getchar(); getchar();
    }
}

/* ============================================================
 *  SECAO 5 - RECORDES (ARQUIVO TEXTO)
 * ============================================================ */

int carregar_recordes(Recorde recordes[], int max) {
    FILE *fp = fopen(ARQUIVO_RECORDES, "r");
    if (!fp) return 0;

    int n = 0;
    while (n < max &&
           fscanf(fp, "%d %d %d",
                  &recordes[n].tentativas,
                  &recordes[n].linhas,
                  &recordes[n].colunas) == 3)
        n++;

    fclose(fp);
    return n;
}

void salvar_recordes(Recorde recordes[], int n) {
    FILE *fp = fopen(ARQUIVO_RECORDES, "w");
    if (!fp) { perror("Erro ao salvar recordes"); return; }
    for (int i = 0; i < n; i++)
        fprintf(fp, "%d %d %d\n",
                recordes[i].tentativas,
                recordes[i].linhas,
                recordes[i].colunas);
    fclose(fp);
}

void registrar_recorde(int tentativas, int linhas, int colunas) {
    Recorde recordes[MAX_RECORDES + 1];
    int n = carregar_recordes(recordes, MAX_RECORDES);

    recordes[n].tentativas = tentativas;
    recordes[n].linhas     = linhas;
    recordes[n].colunas    = colunas;
    n++;

    /* Bubble sort por tentativas crescente */
    for (int i = 0; i < n - 1; i++)
        for (int j = 0; j < n - 1 - i; j++)
            if (recordes[j].tentativas > recordes[j+1].tentativas) {
                Recorde tmp   = recordes[j];
                recordes[j]   = recordes[j+1];
                recordes[j+1] = tmp;
            }

    if (n > MAX_RECORDES) n = MAX_RECORDES;
    salvar_recordes(recordes, n);

    printf("\n===== TOP %d RECORDES =====\n", MAX_RECORDES);
    for (int i = 0; i < n; i++)
        printf("  %do: %d tentativas (%dx%d)\n",
               i + 1, recordes[i].tentativas,
               recordes[i].linhas, recordes[i].colunas);
}

/* ============================================================
 *  SECAO 6 - MENU E MAIN
 * ============================================================ */

int jogo_acabou(Tabuleiro *t) {
    return t->pares_encontrados == t->pares_total;
}

void configurar_jogo(int *linhas, int *colunas) {
    printf("\n=== CONFIGURACAO DO TABULEIRO ===\n");
    printf("Dimensoes sugeridas: 2x2 (facil), 2x4, 4x4, 4x6\n");
    do {
        printf("Linhas  (2-6): ");  scanf("%d", linhas);
        printf("Colunas (2-8): "); scanf("%d", colunas);
        if (*linhas < 2 || *linhas > 6 || *colunas < 2 || *colunas > 8)
            printf("Intervalo invalido.\n");
        else if ((*linhas * *colunas) % 2 != 0)
            printf("O total de celulas (%d) deve ser par.\n", *linhas * *colunas);
        else
            break;
    } while (1);
}

int main(void) {
    int jogar_novamente;

    printf("\u2554\u2550\u2550\u2550\u2550\u2550\u2550\u2550\u2550\u2550\u2550\u2550\u2550\u2550\u2550\u2550\u2550\u2550\u2550\u2550\u2550\u2550\u2550\u2550\u2550\u2550\u2550\u2550\u2550\u2550\u2550\u2557\n");
    printf("\u2551     JOGO DA MEMORIA - C      \u2551\n");
    printf("\u2551   Encontre todos os pares!   \u2551\n");
    printf("\u255a\u2550\u2550\u2550\u2550\u2550\u2550\u2550\u2550\u2550\u2550\u2550\u2550\u2550\u2550\u2550\u2550\u2550\u2550\u2550\u2550\u2550\u2550\u2550\u2550\u2550\u2550\u2550\u2550\u2550\u2550\u255d\n");

    do {
        int linhas, colunas;
        configurar_jogo(&linhas, &colunas);

        Tabuleiro *t = criar_tabuleiro(linhas, colunas);
        inicializar_tabuleiro(t);

        printf("\nTabuleiro %dx%d criado com %d pares. Boa sorte!\n",
               linhas, colunas, t->pares_total);

        exibir_tabuleiro(t, -1, -1, -1, -1);

        while (!jogo_acabou(t))
            jogar_rodada(t);

        printf("\n\u2554\u2550\u2550\u2550\u2550\u2550\u2550\u2550\u2550\u2550\u2550\u2550\u2550\u2550\u2550\u2550\u2550\u2550\u2550\u2550\u2550\u2550\u2550\u2550\u2550\u2550\u2550\u2550\u2550\u2550\u2550\u2557\n");
        printf("\u2551   PARABENS! Voce venceu!     \u2551\n");
        printf("\u2551   Tentativas: %-14d\u2551\n", t->tentativas);
        printf("\u255a\u2550\u2550\u2550\u2550\u2550\u2550\u2550\u2550\u2550\u2550\u2550\u2550\u2550\u2550\u2550\u2550\u2550\u2550\u2550\u2550\u2550\u2550\u2550\u2550\u2550\u2550\u2550\u2550\u2550\u2550\u255d\n");

        registrar_recorde(t->tentativas, linhas, colunas);
        destruir_tabuleiro(t);

        printf("\nJogar novamente? (1 = Sim / 0 = Nao): ");
        scanf("%d", &jogar_novamente);

    } while (jogar_novamente == 1);

    printf("Obrigado por jogar! Ate a proxima.\n");
    return 0;
}
