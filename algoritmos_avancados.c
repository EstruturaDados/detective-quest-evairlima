#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define TAM_HASH 7
#define MAX_NOME 50
#define MAX_PISTA 100

/*
 * DETECTIVE QUEST - VERSÃO COMPLETA
 * Junta os 3 níveis da Enigma Studios
 */

// ========== ESTRUTURAS ==========
typedef struct Sala {
    char nome[MAX_NOME];
    struct Sala *esq;
    struct Sala *dir;
} Sala;

typedef struct PistaNode {
    char pista[MAX_PISTA];
    struct PistaNode *esq;
    struct PistaNode *dir;
} PistaNode;

typedef struct HashNode {
    char pista[MAX_PISTA];
    char suspeito[MAX_NOME];
    struct HashNode *prox;
} HashNode;

// ========== NÍVEL NOVATO ==========
// criarSala() – cria, de forma dinâmica, uma sala com nome.
Sala* criarSala(const char *nome) {
    Sala *nova = (Sala*) malloc(sizeof(Sala));
    if (!nova) { printf("Erro de alocacao!\n"); exit(1); }
    strcpy(nova->nome, nome);
    nova->esq = NULL;
    nova->dir = NULL;
    return nova;
}

// ========== NÍVEL AVENTUREIRO ==========
// inserirPista() – insere na BST em ordem alfabética
PistaNode* inserirPista(PistaNode *raiz, const char *pista) {
    if (!pista || pista[0] == '\0') return raiz;
    if (raiz == NULL) {
        PistaNode *novo = (PistaNode*) malloc(sizeof(PistaNode));
        strcpy(novo->pista, pista);
        novo->esq = novo->dir = NULL;
        return novo;
    }
    int cmp = strcmp(pista, raiz->pista);
    if (cmp < 0) raiz->esq = inserirPista(raiz->esq, pista);
    else if (cmp > 0) raiz->dir = inserirPista(raiz->dir, pista);
    return raiz;
}

// exibirPistas() – imprime em ordem alfabética
void exibirPistas(PistaNode *raiz) {
    if (raiz == NULL) return;
    exibirPistas(raiz->esq);
    printf(" - %s\n", raiz->pista);
    exibirPistas(raiz->dir);
}

// ========== NÍVEL MESTRE ==========
int hashFunc(const char *str) {
    int soma = 0;
    for (int i = 0; str[i]; i++) soma += (unsigned char)str[i];
    return soma % TAM_HASH;
}

// inserirNaHash() – insere associação pista/suspeito
void inserirNaHash(HashNode *tabela[], const char *pista, const char *suspeito) {
    int idx = hashFunc(pista);
    HashNode *novo = (HashNode*) malloc(sizeof(HashNode));
    strcpy(novo->pista, pista);
    strcpy(novo->suspeito, suspeito);
    novo->prox = tabela[idx];
    tabela[idx] = novo;
}

// encontrarSuspeito() – consulta o suspeito
const char* encontrarSuspeito(HashNode *tabela[], const char *pista) {
    int idx = hashFunc(pista);
    HashNode *atual = tabela[idx];
    while (atual) {
        if (strcmp(atual->pista, pista) == 0) return atual->suspeito;
        atual = atual->prox;
    }
    return NULL;
}

// obterPistaDaSala() – pistas definidas no código
const char* obterPistaDaSala(const char *nomeSala) {
    if (strcmp(nomeSala, "Hall de Entrada") == 0) return "Pegadas molhadas";
    if (strcmp(nomeSala, "Sala de Estar") == 0) return "Carta rasgada";
    if (strcmp(nomeSala, "Biblioteca") == 0) return "Livro com anotacoes";
    if (strcmp(nomeSala, "Jardim de Inverno") == 0) return "Luvas de jardinagem";
    if (strcmp(nomeSala, "Cozinha") == 0) return "";
    if (strcmp(nomeSala, "Despensa") == 0) return "Frasco vazio";
    if (strcmp(nomeSala, "Sala de Jantar") == 0) return "Copo quebrado";
    return "";
}

// explorarSalas() – navegação e coleta
void explorarSalas(Sala *atual, PistaNode **arvorePistas) {
    char escolha;
    printf("\n=== DETECTIVE QUEST ===\n");
    while (atual!= NULL) {
        printf("\n>> Voce esta em: %s\n", atual->nome);
        const char *pista = obterPistaDaSala(atual->nome);
        if (pista[0]!= '\0') {
            printf(" [Pista] %s\n", pista);
            *arvorePistas = inserirPista(*arvorePistas, pista);
        }
        if (atual->esq == NULL && atual->dir == NULL) {
            printf(" Este comodo nao tem mais saidas.\n");
        }
        printf(" Caminhos: ");
        if (atual->esq) printf("[e] %s ", atual->esq->nome);
        if (atual->dir) printf("[d] %s ", atual->dir->nome);
        printf("[s] Sair\nEscolha: ");
        scanf(" %c", &escolha);

        if (escolha == 's') break;
        else if (escolha == 'e' && atual->esq) atual = atual->esq;
        else if (escolha == 'd' && atual->dir) atual = atual->dir;
        else printf("Caminho invalido!\n");
    }
}

int contarPistasDoSuspeito(PistaNode *raiz, HashNode *tabela[], const char *suspeitoAlvo) {
    if (!raiz) return 0;
    int conta = 0;
    const char *sus = encontrarSuspeito(tabela, raiz->pista);
    if (sus && strcmp(sus, suspeitoAlvo) == 0) conta = 1;
    return conta + contarPistasDoSuspeito(raiz->esq, tabela, suspeitoAlvo)
                 + contarPistasDoSuspeito(raiz->dir, tabela, suspeitoAlvo);
}

// verificarSuspeitoFinal() – julgamento
void verificarSuspeitoFinal(PistaNode *pistas, HashNode *tabela[]) {
    char acusado[MAX_NOME];
    printf("\n=== FASE FINAL ===\nPistas coletadas:\n");
    exibirPistas(pistas);
    printf("\nSuspeitos: Sr. Verde, Dona Violeta, Coronel Mostarda\n");
    printf("Quem voce acusa? ");
    scanf(" %[^\n]", acusado);

    int total = contarPistasDoSuspeito(pistas, tabela, acusado);
    printf("\nPistas que apontam para %s: %d\n", acusado, total);
    if (total >= 2)
        printf("VEREDITO: Acusacao SUSTENTADA! %s e o culpado.\n", acusado);
    else
        printf("VEREDITO: Provas insuficientes.\n");
}

int main() {
    // Monta mapa
    Sala *hall = criarSala("Hall de Entrada");
    hall->esq = criarSala("Sala de Estar");
    hall->dir = criarSala("Cozinha");
    hall->esq->esq = criarSala("Biblioteca");
    hall->esq->dir = criarSala("Jardim de Inverno");
    hall->dir->esq = criarSala("Despensa");
    hall->dir->dir = criarSala("Sala de Jantar");

    // Hash pista -> suspeito
    HashNode *tabela[TAM_HASH] = {NULL};
    inserirNaHash(tabela, "Pegadas molhadas", "Sr. Verde");
    inserirNaHash(tabela, "Luvas de jardinagem", "Sr. Verde");
    inserirNaHash(tabela, "Carta rasgada", "Dona Violeta");
    inserirNaHash(tabela, "Livro com anotacoes", "Dona Violeta");
    inserirNaHash(tabela, "Frasco vazio", "Coronel Mostarda");
    inserirNaHash(tabela, "Copo quebrado", "Coronel Mostarda");

    PistaNode *pistas = NULL;
    explorarSalas(hall, &pistas);
    verificarSuspeitoFinal(pistas, tabela);
    return 0;
}