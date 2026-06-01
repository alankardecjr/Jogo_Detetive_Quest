#include "raylib.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#define MAX_STR 100
#define TAM_HASH 10

// Dimensões da Janela
#define LARGURA_TELA 1024
#define ALTURA_TELA 720

// ---------------------------------------------------------------
// ESTRUTURAS DE DADOS (Originais Mantidas)
// ---------------------------------------------------------------

typedef struct Sala {
    char nome[MAX_STR];
    char pista[MAX_STR];
    struct Sala* esquerda;
    struct Sala* direita;
    struct Sala* pai;
} Sala;

typedef struct PistaNode {
    char pista[MAX_STR];
    struct PistaNode* esquerda;
    struct PistaNode* direita;
} PistaNode;

typedef struct HashNode {
    char pista[MAX_STR];
    char suspeito[MAX_STR];
    struct HashNode* prox;
} HashNode;

// Estados do Jogo para a Interface
typedef enum { ESTADO_EXPLORACAO, ESTADO_LISTA_PISTAS, ESTADO_JULGAMENTO, ESTADO_FIM } EstadoJogo;

// ---------------------------------------------------------------
// FUNÇÕES DE LÓGICA (Árvores e Hash)
// ---------------------------------------------------------------

Sala* criarSala(const char* nome, const char* pista) {
    Sala* nova = (Sala*) malloc(sizeof(Sala));
    if (!nova) exit(1);
    strcpy(nova->nome, nome);
    if (pista != NULL) strcpy(nova->pista, pista);
    else nova->pista[0] = '\0';
    nova->esquerda = nova->direita = nova->pai = NULL;
    return nova;
}

PistaNode* criarPistaNode(const char* pista) {
    PistaNode* novo = (PistaNode*) malloc(sizeof(PistaNode));
    if (!novo) exit(1);
    strcpy(novo->pista, pista);
    novo->esquerda = novo->direita = NULL;
    return novo;
}

PistaNode* inserirPista(PistaNode* raiz, const char* pista) {
    if (raiz == NULL) return criarPistaNode(pista);
    if (strcmp(pista, raiz->pista) < 0)
        raiz->esquerda = inserirPista(raiz->esquerda, pista);
    else if (strcmp(pista, raiz->pista) > 0)
        raiz->direita = inserirPista(raiz->direita, pista);
    return raiz;
}

// Armazena as pistas da BST em uma string formatada para desenhar na tela
void obterPistasTexto(PistaNode* raiz, char* buffer) {
    if (raiz != NULL) {
        obterPistasTexto(raiz->esquerda, buffer);
        strcat(buffer, "- ");
        strcat(buffer, raiz->pista);
        strcat(buffer, "\n");
        obterPistasTexto(raiz->direita, buffer);
    }
}

int hash(const char* str) {
    int soma = 0;
    for (int i = 0; str[i] != '\0'; i++) soma += str[i];
    return soma % TAM_HASH;
}

void inserirNaHash(HashNode* tabela[], const char* pista, const char* suspeito) {
    int idx = hash(pista);
    HashNode* novo = (HashNode*) malloc(sizeof(HashNode));
    strcpy(novo->pista, pista);
    strcpy(novo->suspeito, suspeito);
    novo->prox = tabela[idx];
    tabela[idx] = novo;
}

const char* encontrarSuspeito(HashNode* tabela[], const char* pista) {
    int idx = hash(pista);
    HashNode* atual = tabela[idx];
    while (atual != NULL) {
        if (strcmp(atual->pista, pista) == 0) return atual->suspeito;
        atual = atual->prox;
    }
    return NULL;
}

int contarPistasPorSuspeito(PistaNode* raiz, HashNode* tabela[], const char* suspeito) {
    if (raiz == NULL) return 0;
    int conta = 0;
    const char* suspeitoAtual = encontrarSuspeito(tabela, raiz->pista);
    if (suspeitoAtual && strcmp(suspeitoAtual, suspeito) == 0) conta = 1;
    return conta + contarPistasPorSuspeito(raiz->esquerda, tabela, suspeito) +
                   contarPistasPorSuspeito(raiz->direita, tabela, suspeito);
}

// Funções de liberação de memória
void liberarPistas(PistaNode* raiz) {
    if (raiz != NULL) {
        liberarPistas(raiz->esquerda); liberarPistas(raiz->direita); free(raiz);
    }
}
void liberarSalas(Sala* raiz) {
    if (raiz != NULL) {
        liberarSalas(raiz->esquerda); liberarSalas(raiz->direita); free(raiz);
    }
}

// ---------------------------------------------------------------
// AUXILIARES DE INTERFACE INTERATIVA (BOTÕES)
// ---------------------------------------------------------------

bool DesenharBotao(Rectangle ret, const char* texto, Color corPadrao, Color corTexto) {
    Vector2 posicaoMouse = GetMousePosition();
    bool colisao = CheckCollisionPointRec(posicaoMouse, ret);
    
    // Efeito de hover (mudar cor quando mouse passa por cima)
    Color corAtual = colisao ? ColorAlpha(corPadrao, 0.8f) : corPadrao;
    
    DrawRectangleRec(ret, corAtual);
    DrawRectangleLinesEx(ret, 2, DARKGRAY);
    
    int tamanhoTexto = MeasureText(texto, 20);
    DrawText(texto, ret.x + (ret.width/2) - (tamanhoTexto/2), ret.y + (ret.height/2) - 10, 20, corTexto);
    
    return (colisao && IsMouseButtonPressed(MOUSE_BUTTON_LEFT));
}

// ---------------------------------------------------------------
// FUNÇÃO PRINCIPAL GUI
// ---------------------------------------------------------------

int main() {
    // Inicialização do grafo/árvore de salas
    Sala* hall        = criarSala("Hall de Entrada", "Pegadas de lama fresca");
    Sala* salaEstar   = criarSala("Sala de Estar", "Bilhete amassado");
    Sala* biblioteca  = criarSala("Biblioteca", "Agenda com pagina faltando");
    Sala* suite1      = criarSala("Suite 1", "Comoda revirada");
    Sala* sacada1     = criarSala("Sacada 1", "");
    Sala* suite2      = criarSala("Suite 2", "");
    Sala* sacada2     = criarSala("Sacada 2", "Luvas de couro");
    Sala* escritorio  = criarSala("Escritorio", "Lencos com manchas de sangue");
    Sala* copa        = criarSala("Copa", "Xicaras quebradas");
    Sala* porao       = criarSala("Porao", "Arma do crime");

    hall->esquerda = salaEstar; hall->direita = biblioteca;
    salaEstar->esquerda = suite1; suite1->esquerda = sacada1;
    salaEstar->direita = suite2; suite2->direita = sacada2;
    biblioteca->esquerda = copa; biblioteca->direita = escritorio;
    escritorio->direita = porao;

    salaEstar->pai = hall; biblioteca->pai = hall;
    suite1->pai = salaEstar; sacada1->pai = suite1;
    suite2->pai = salaEstar; sacada2->pai = suite2;
    copa->pai = biblioteca; escritorio->pai = biblioteca; porao->pai = escritorio;

    // Inicialização da Tabela Hash
    HashNode* tabelaHash[TAM_HASH] = {NULL};
    inserirNaHash(tabelaHash, "Pegadas de lama fresca", "Jardineiro");
    inserirNaHash(tabelaHash, "Bilhete amassado", "Baba");
    inserirNaHash(tabelaHash, "Agenda com pagina faltando", "Secretaria");
    inserirNaHash(tabelaHash, "Comoda revirada", "Mordomo");
    inserirNaHash(tabelaHash, "Luvas de couro", "Jardineiro");
    inserirNaHash(tabelaHash, "Lencos com manchas de sangue", "Baba");
    inserirNaHash(tabelaHash, "Xicaras quebradas", "Secretaria");
    inserirNaHash(tabelaHash, "Arma do crime", "Baba");

    PistaNode* pistasColetadas = NULL;
    Sala* salaAtual = hall;
    
    // Variáveis de controle de estado da GUI
    EstadoJogo estadoAtual = ESTADO_EXPLORACAO;
    char textoUltimaPista[MAX_STR] = "Nenhuma pista recente.";
    char resultadoJulgamento[MAX_STR] = "";
    char listaPistasStr[2000] = "";

    // Inicialização da Janela Raylib
    InitWindow(LARGURA_TELA, ALTURA_TELA, "Detetive RPG - Mansao Hash & Arvores");
    SetTargetFPS(60);

    // Loop Principal do Jogo Graphic
    while (!WindowShouldClose()) {
        
        // -----------------------------------------------------------
        // LÓGICA DE ATUALIZAÇÃO (Captura de Coleta Automática)
        // -----------------------------------------------------------
        if (estadoAtual == ESTADO_EXPLORACAO && salaAtual != NULL) {
            if (strlen(salaAtual->pista) > 0) {
                strcpy(textoUltimaPista, salaAtual->pista);
                pistasColetadas = inserirPista(pistasColetadas, salaAtual->pista);
                salaAtual->pista[0] = '\0'; // Evita recoleta
            }
        }

        // -----------------------------------------------------------
        // DESENHO DA INTERFACE (Renderização)
        // -----------------------------------------------------------
        BeginDrawing();
        ClearBackground(RAYWHITE);

        // Topo da tela comum
        DrawRectangle(0, 0, LARGURA_TELA, 80, MAROON);
        DrawText("INVESTIGACAO NA MANSAO", 30, 25, 32, WHITE);

        switch (estadoAtual) {
            
            case ESTADO_EXPLORACAO: {
                // Painel da Sala Atual
                DrawRectangle(40, 110, 944, 140, LIGHTGRAY);
                DrawText("Voce esta no(a):", 60, 130, 20, DARKGRAY);
                DrawText(salaAtual->nome, 60, 160, 36, BLACK);

                // Notificação de Pistas
                DrawText("Ultima pista coletada:", 60, 270, 18, GRAY);
                DrawText(textoUltimaPista, 60, 295, 22, DARKGREEN);

                // --- BOTÕES DE NAVEGAÇÃO ---
                DrawText("Escolha uma direcao para se mover:", 40, 370, 20, BLACK);
                
                // Botão Esquerda
                if (salaAtual->esquerda != NULL) {
                    char label[150]; sprintf(label, "Ir para: %s (Esquerda)", salaAtual->esquerda->nome);
                    if (DesenharBotao((Rectangle){40, 410, 440, 60}, label, LIGHTGRAY, BLACK)) {
                        salaAtual = salaAtual->esquerda;
                    }
                } else {
                    DrawRectangleRec((Rectangle){40, 410, 440, 60}, ColorAlpha(LIGHTGRAY, 0.3f));
                    DrawText("[Caminho Esquerdo Bloqueado]", 100, 430, 18, GRAY);
                }

                // Botão Direita
                if (salaAtual->direita != NULL) {
                    char label[150]; sprintf(label, "Ir para: %s (Direita)", salaAtual->direita->nome);
                    if (DesenharBotao((Rectangle){544, 410, 440, 60}, label, LIGHTGRAY, BLACK)) {
                        salaAtual = salaAtual->direita;
                    }
                } else {
                    DrawRectangleRec((Rectangle){544, 410, 440, 60}, ColorAlpha(LIGHTGRAY, 0.3f));
                    DrawText("[Caminho Direito Bloqueado]", 610, 430, 18, GRAY);
                }

                // Botão Voltar (Pai)
                if (salaAtual->pai != NULL) {
                    char label[150]; sprintf(label, "Voltar para: %s", salaAtual->pai->nome);
                    if (DesenharBotao((Rectangle){292, 490, 440, 60}, label, GRAY, BLACK)) {
                        salaAtual = salaAtual->pai;
                    }
                }

                // --- BOTÕES DE MENU INFERIOR ---
                if (DesenharBotao((Rectangle){40, 610, 280, 50}, "Ver Pistas Coletadas", DARKGRAY, WHITE)) {
                    listaPistasStr[0] = '\0';
                    if (pistasColetadas != NULL) obterPistasTexto(pistasColetadas, listaPistasStr);
                    else strcpy(listaPistasStr, "Nenhuma pista coletada ainda.");
                    estadoAtual = ESTADO_LISTA_PISTAS;
                }

                if (DesenharBotao((Rectangle){372, 610, 280, 50}, "Ir para Julgamento", ORANGE, WHITE)) {
                    estadoAtual = ESTADO_JULGAMENTO;
                }

                if (DesenharBotao((Rectangle){704, 610, 280, 50}, "Sair do Jogo", RED, WHITE)) {
                    CloseWindow(); 
                    liberarSalas(hall);
                    liberarPistas(pistasColetadas);
                    return 0;      
                }
            } break; // <- Chave e break reposicionados perfeitamente aqui!

            case ESTADO_LISTA_PISTAS: {
                DrawText("PISTAS COLETADAS (Ordem alfabetica da BST):", 40, 120, 24, BLACK);
                
                // Exibe a string montada a partir da caminhada em-ordem
                DrawRectangle(40, 160, 944, 380, BLACK);
                DrawText(listaPistasStr, 60, 180, 20, GREEN);

                if (DesenharBotao((Rectangle){40, 580, 200, 50}, "< Voltar", DARKGRAY, WHITE)) {
                    estadoAtual = ESTADO_EXPLORACAO;
                }
            } break;

            case ESTADO_JULGAMENTO: {
                DrawText(">>> JULGAMENTO FINAL <<<", 40, 120, 26, BLACK);
                DrawText("Escolha com cuidado. Uma acusacao consistente exige no minimo 2 pistas corretas.", 40, 160, 18, DARKGRAY);

                // Lista de Suspeitos como Botões Interactivos
                const char* suspeitos[] = {"Mordomo", "Secretaria", "Jardineiro", "Baba"};
                int posYSuspeito = 220;

                for (int i = 0; i < 4; i++) {
                    if (DesenharBotao((Rectangle){40, posYSuspeito, 400, 55}, suspeitos[i], BLUE, WHITE)) {
                        int totalPistas = contarPistasPorSuspeito(pistasColetadas, tabelaHash, suspeitos[i]);
                        
                        if (totalPistas >= 2) {
                            sprintf(resultadoJulgamento, "Acusacao consistente!\n\n%s foi considerado CULPADO com %d pistas vinculadas pela Tabela Hash!", suspeitos[i], totalPistas);
                        } else {
                            sprintf(resultadoJulgamento, "Acusacao fraca!\n\n%s foi considerado INOCENTE. Faltaram provas concretas (Apenas %d pista(s) encontrada(s)).", suspeitos[i], totalPistas);
                        }
                        estadoAtual = ESTADO_FIM;
                    }
                    posYSuspeito += 75;
                }

                if (DesenharBotao((Rectangle){40, 580, 200, 50}, "< Voltar", DARKGRAY, WHITE)) {
                    estadoAtual = ESTADO_EXPLORACAO;
                }
            } break;

            case ESTADO_FIM: {
                DrawRectangle(40, 130, 944, 300, LIGHTGRAY);
                DrawText("VEREDITO DO JUIZ", 60, 150, 24, MAROON);
                DrawText(resultadoJulgamento, 60, 200, 22, BLACK);

                DrawText("Obrigado por jogar! O mapa e as estruturas foram limpos da memoria.", 40, 480, 20, DARKGRAY);

                if (DesenharBotao((Rectangle){40, 550, 300, 60}, "Fechar Jogo", RED, WHITE)) {
                    CloseWindow();
                    liberarSalas(hall);
                    liberarPistas(pistasColetadas);
                    return 0;
                }
            } break;
        }

        EndDrawing();
    }

    // Limpeza de memória final preventiva
    CloseWindow();
    liberarSalas(hall);
    liberarPistas(pistasColetadas);

    return 0;
}