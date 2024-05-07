#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>


#define MAX_PROCESSOS 2
#define MAX_RECURSOS 2

// processo
typedef struct {
    int id;
    int recursos_necessarios[MAX_RECURSOS];
    bool em_execucao;
} Processo;

// recurso
typedef struct {
    int id;
    int quantidade_disponivel;
} Recurso;

// estado seguro
bool estado_seguro(Processo processos[], int processo_id, Recurso recursos[], int num_recursos, int recursos_disponiveis[]) {
    bool alocacao_possivel = true;
    for (int i = 0; i < num_recursos; i++) {
        if (processos[processo_id].recursos_necessarios[i] > recursos_disponiveis[i]) {
            alocacao_possivel = false;
            break;
        }
    }

    if (alocacao_possivel) {
        // Simula a alocação de recursos
        for (int i = 0; i < num_recursos; i++) {
            recursos_disponiveis[i] -= processos[processo_id].recursos_necessarios[i];
            processos[processo_id].em_execucao = true;
        }

        // Verifica se o estado resultante é seguro
        bool processos_executaveis[MAX_PROCESSOS] = {false};
        bool todos_processos_executaveis = true;

        // Inicializa os processos como executáveis
        for (int i = 0; i < MAX_PROCESSOS; i++) {
            if (!processos[i].em_execucao) {
                for (int j = 0; j < num_recursos; j++) {
                    if (processos[i].recursos_necessarios[j] > recursos_disponiveis[j]) {
                        processos_executaveis[i] = false;
                        todos_processos_executaveis = false;
                        break;
                    }
                }
                processos_executaveis[i] = true;
            }
        }

        // Verifica se todos os processos podem ser executados
        if (todos_processos_executaveis) {
            return true;
        } else {
            // Libera os recursos alocados para o processo
            for (int i = 0; i < num_recursos; i++) {
                recursos_disponiveis[i] += processos[processo_id].recursos_necessarios[i];
                processos[processo_id].em_execucao = false;
            }
            return false;
        }
    } else {
        return false;
    }
}

//prevenir deadlock (Banqueiro)
void prevenir_deadlock(Processo processos[], int num_processos, Recurso recursos[], int num_recursos) {
    int recursos_disponiveis[MAX_RECURSOS];
    for (int i = 0; i < num_recursos; i++) {
        recursos_disponiveis[i] = recursos[i].quantidade_disponivel;
    }

    // Verifica se alocar recursos para cada processo levará a um estado seguro
    for (int i = 0; i < num_processos; i++) {
        if (!estado_seguro(processos, i, recursos, num_recursos, recursos_disponiveis)) {
            printf("A alocação de recursos para o processo %d pode levar a um deadlock. Recursos não alocados.\n", processos[i].id);
        }
    }
}

// verifica se há ciclo no grafo de espera
bool detectar_ciclo(int processo_id, Processo processos[], bool visitados[], bool pilha_recursao[]) {
    if (!visitados[processo_id]) {
        visitados[processo_id] = true;
        pilha_recursao[processo_id] = true;

        for (int i = 0; i < MAX_RECURSOS; i++) {
            int recurso_necessario = processos[processo_id].recursos_necessarios[i];
            if (recurso_necessario > 0) {
                // Verifica se o recurso está em uso por algum outro processo
                for (int j = 0; j < MAX_PROCESSOS; j++) {
                    if (processos[j].em_execucao && j != processo_id && processos[j].recursos_necessarios[i] > 0) {
                        if (!visitados[j] && detectar_ciclo(j, processos, visitados, pilha_recursao)) {
                            return true;
                        } else if (pilha_recursao[j]) {
                            return true;
                        }
                    }
                }
            }
        }
    }
    pilha_recursao[processo_id] = false;
    return false;
}


// deadlock
bool detectar_deadlock(Processo processos[], int num_processos, Recurso recursos[], int num_recursos) {
    bool visitados[MAX_PROCESSOS] = {false};
    bool pilha_recursao[MAX_PROCESSOS] = {false};

    for (int i = 0; i < num_processos; i++) {
        if (detectar_ciclo(i, processos, visitados, pilha_recursao)) {
            printf("Deadlock detectado!\n");
            return true;
        }
    }
    printf("Não há deadlock.\n");
    return false;
}

// aloca recursos a um processo
void alocar_recursos(Processo *processo, Recurso recursos[], int num_recursos) {
    // Verifica se os recursos necessários estão disponíveis
    bool recursos_suficientes = true;
    for (int i = 0; i < num_recursos; i++) {
        // printf("R: %d\tQuant: %d\tRN: %d\n",recursos[i].id, recursos[i].quantidade_disponivel, processo->recursos_necessarios[i]);
        if (recursos[i].quantidade_disponivel < processo->recursos_necessarios[i]) {
            recursos_suficientes = false;
            break;
        }
    }

    // Se houver recursos suficientes, aloca-os ao processo
    if (recursos_suficientes) {
        for (int i = 0; i < num_recursos; i++) {
            recursos[i].quantidade_disponivel -= processo->recursos_necessarios[i];
        }
        processo->em_execucao = true;
        printf("Recursos alocados para o processo %d\n", processo->id);
        printf("Executando o processo %d...\n", processo->id);
    } else {
        printf("Recursos insuficientes para o processo %d\n", processo->id);
    }
}

// libera recursos de um processo
void liberar_recursos(Processo *processo, Recurso recursos[], int num_recursos) {
    // Libera os recursos alocados pelo processo
    for (int i = 0; i < num_recursos; i++) {
        recursos[i].quantidade_disponivel += processo->recursos_necessarios[i];
    }
    processo->em_execucao = false;
    printf("Recursos liberados pelo processo %d\n", processo->id);
}

int main(int argc, char *argv[]) {
    int op = atoi(argv[1]);

    Processo processos[MAX_PROCESSOS];
    Recurso recursos[MAX_RECURSOS];

    if(op == 0){
        // Inicialização dos processos
        for (int i = 0; i < MAX_PROCESSOS; i++) {
            processos[i].id = i;
            processos[i].em_execucao = false;
            // Inicializa os recursos necessários de cada processo
            for (int j = 0; j < MAX_RECURSOS; j++) {
                processos[i].recursos_necessarios[j] = 0;
            }
        }
        // Inicialização dos recursos
        for (int i = 0; i < MAX_RECURSOS; i++) {
            recursos[i].id = i;
            recursos[i].quantidade_disponivel = 1; // Quantidade disponível de recursos
        }

        // Simulação da alocação de recursos e execução dos processos
        for (int i = 0; i < MAX_PROCESSOS; i++) {
            // Aloca recursos para o processo
            alocar_recursos(&processos[i], recursos, MAX_RECURSOS);
            // Libera recursos ao final da execução do processo
            liberar_recursos(&processos[i], recursos, MAX_RECURSOS);
        }
        // Verificação de deadlock
        detectar_deadlock(processos, MAX_PROCESSOS, recursos, MAX_RECURSOS);

    }else if (op == 1) {
        // Inicialização dos processos
        processos[0].id = 0;
        processos[0].recursos_necessarios[0] = 0;
        processos[0].recursos_necessarios[1] = 1;
        processos[0].em_execucao = false;

        processos[1].id = 1;
        processos[1].recursos_necessarios[0] = 0;
        processos[1].recursos_necessarios[1] = 1;
        processos[1].em_execucao = false;

        // Inicialização dos recursos
        recursos[0].id = 0;
        recursos[0].quantidade_disponivel = 1;

        recursos[1].id = 1;
        recursos[1].quantidade_disponivel = 1;

        // Simulação da alocação de recursos e execução dos processos
        // Aloca recursos para o processo
        processos[0].em_execucao = true;
        processos[1].em_execucao = true;
        alocar_recursos(&processos[0], recursos, MAX_RECURSOS);
        alocar_recursos(&processos[1], recursos, MAX_RECURSOS);
        // Verificação de deadlock
        detectar_deadlock(processos, MAX_PROCESSOS, recursos, MAX_RECURSOS);

        // Libera recursos ao final da execução do processo
        liberar_recursos(&processos[0], recursos, MAX_RECURSOS);
        liberar_recursos(&processos[1], recursos, MAX_RECURSOS);
    }

    return 0;
}