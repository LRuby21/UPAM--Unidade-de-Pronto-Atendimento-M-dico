#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Paciente {
    int id;
    char nome[50];
    int gravidade;
    int prioridade;
    int hora_chegada;
    int hora_atendimento;

    struct Paciente *prox;
} Paciente;

/* fila de espera */
Paciente *fila = NULL;

/* pilha de atendidos */
Paciente *pilha = NULL;

/* criar paciente */
Paciente* criarPaciente(int id, char nome[],
                        int gravidade,
                        int prioridade,
                        int chegada) {

    Paciente *p = (Paciente*) malloc(sizeof(Paciente));

    if (p == NULL) {
        printf("Erro de memoria!\n");
        exit(1);
    }

    p->id = id;

    strcpy(p->nome, nome);

    p->gravidade = gravidade;
    p->prioridade = prioridade;

    p->hora_chegada = chegada;
    p->hora_atendimento = 0;

    p->prox = NULL;

    return p;
}

/* inserir na fila de prioridade */
void inserirFila(Paciente **fila, Paciente *novo) {

    /* fila vazia */
    if (*fila == NULL) {
        *fila = novo;
        return;
    }

    /* inserir no começo */
    if (novo->prioridade > (*fila)->prioridade ||

       (novo->prioridade == (*fila)->prioridade &&
        novo->hora_chegada < (*fila)->hora_chegada)) {

        novo->prox = *fila;
        *fila = novo;
        return;
    }

    /* procurar posição correta */
    Paciente *atual = *fila;

    while (atual->prox != NULL &&

          (atual->prox->prioridade > novo->prioridade ||

          (atual->prox->prioridade == novo->prioridade &&
           atual->prox->hora_chegada < novo->hora_chegada))) {

        atual = atual->prox;
    }

    novo->prox = atual->prox;
    atual->prox = novo;
}

/* mostrar fila */
void mostrarFila(Paciente *fila) {

    Paciente *atual = fila;

    printf("\n===== FILA DE PRIORIDADE =====\n\n");

    while (atual != NULL) {

        printf("ID: %d\n", atual->id);
        printf("Nome: %s\n", atual->nome);
        printf("Gravidade: %d\n", atual->gravidade);
        printf("Prioridade: %d\n", atual->prioridade);
        printf("Hora chegada: %d\n", atual->hora_chegada);

        printf("-------------------------\n");

        atual = atual->prox;
    }
}

/* atender paciente */
Paciente* atenderPaciente(Paciente **fila) {

    if (*fila == NULL) {
        return NULL;
    }

    Paciente *atendido = *fila;

    *fila = (*fila)->prox;

    atendido->prox = NULL;

    return atendido;
}

/* empilhar atendido */
void push(Paciente **pilha, Paciente *p) {

    p->prox = *pilha;

    *pilha = p;
}

/* remover da pilha */
Paciente* pop(Paciente **pilha) {

    if (*pilha == NULL) {
        return NULL;
    }

    Paciente *p = *pilha;

    *pilha = (*pilha)->prox;

    p->prox = NULL;

    return p;
}

/* desfazer atendimento */
void desfazerAtendimento(Paciente **fila,
                         Paciente **pilha) {

    Paciente *p;

    p = pop(pilha);

    if (p == NULL) {

        printf("\nNenhum atendimento para desfazer!\n");

        return;
    }

    /* prioridade máxima */
    p->prioridade = 999;

    inserirFila(fila, p);

    printf("\nAtendimento desfeito: %s\n", p->nome);
}

/* aumentar prioridade automática */
void aumentarPrioridade(Paciente **fila) {

    if (*fila == NULL) {
        return;
    }

    Paciente *novaFila = NULL;

    Paciente *atual = *fila;

    int posicao = 1;

    while (atual != NULL) {

        Paciente *proximo = atual->prox;

        atual->prox = NULL;

        /* aumenta prioridade a partir do 6º */
        if (posicao >= 6) {

            if (atual->prioridade < 10) {
                atual->prioridade++;
            }
        }

        inserirFila(&novaFila, atual);

        atual = proximo;

        posicao++;
    }

    *fila = novaFila;
}

/* mostrar pilha */
void mostrarPilha(Paciente *pilha) {

    Paciente *atual = pilha;

    printf("\n===== PILHA DE ATENDIDOS =====\n\n");

    while (atual != NULL) {

        printf("ID: %d\n", atual->id);
        printf("Nome: %s\n", atual->nome);
        printf("Prioridade: %d\n", atual->prioridade);

        printf("-------------------------\n");

        atual = atual->prox;
    }
}

/* processar arquivo */
void processarArquivo(char nomeArquivo[]) {

    FILE *arquivo;

    arquivo = fopen(nomeArquivo, "r");

    if (arquivo == NULL) {

        printf("Erro ao abrir arquivo!\n");

        return;
    }

    char linha[200];

    while (fgets(linha, sizeof(linha), arquivo)) {

        /* remover quebra de linha */
        linha[strcspn(linha, "\n")] = 0;

        /* comando ENTRADA */
        if (strncmp(linha, "ENTRADA", 8) == 0) {

            int id;
            char nome[50];
            int gravidade;
            int prioridade;
            int chegada;

            sscanf(linha,
                   "ENTRADA;%d;%49[^;];%d;%d;%d",
                   &id,
                   nome,
                   &gravidade,
                   &prioridade,
                   &chegada);

            Paciente *novo;

            novo = criarPaciente(id,
                                 nome,
                                 gravidade,
                                 prioridade,
                                 chegada);

            inserirFila(&fila, novo);

            printf("\nPaciente inserido: %s\n", nome);
        }

        /* comando ATENDER */
        else if (strcmp(linha, "ATENDER") == 0) {

            Paciente *atendido;

            atendido = atenderPaciente(&fila);

            if (atendido != NULL) {

                printf("\nPaciente atendido: %s\n",
                       atendido->nome);

                push(&pilha, atendido);
            }
        }

        /* comando DESFAZER */
        else if (strcmp(linha, "DESFAZER") == 0) {

            desfazerAtendimento(&fila, &pilha);
        }
    }

    fclose(arquivo);
}

/* liberar memória */
void liberarLista(Paciente *lista) {

    Paciente *temp;

    while (lista != NULL) {

        temp = lista;

        lista = lista->prox;

        free(temp);
    }
}

int main() {

    processarArquivo("entrada.txt");

    printf("\nFILA FINAL:\n");

    mostrarFila(fila);

    printf("\nPILHA FINAL:\n");

    mostrarPilha(pilha);

    liberarLista(fila);
    liberarLista(pilha);

    return 0;
}