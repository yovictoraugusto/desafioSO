// Descrição: Desenvolva um programa que simule os seguintes algoritmos de escalonamento de processos: First-Come,
// First-Served (FCFS); Round Robin (RR) e; Shortest Job First (SJF). Implemente cada um desses algoritmos de forma a poder compará-los facilmente. Seu programa deve aceitar uma lista de processos, cada um com um tempo de chegada e uma duração específica.
// First-Come, First-Served (FCFS): Neste algoritmo, os processos são atendidos na ordem em que chegam, ou seja, o primeiro a chegar é o primeiro a ser processado.
// Round Robin (RR): Neste algoritmo, cada processo é atribuído um quantum de tempo fixo. Os processos são atendidos em uma ordem circular, com cada processo sendo interrompido após o término de seu quantum para dar lugar ao próximo na fila, retornando ao final da fila se necessário.
// Shortest Job First (SJF): Neste algoritmo, os processos com as menores durações de execução são atendidos primeiro, priorizando aqueles que podem ser concluídos mais rapidamente. Considere a possibilidade de ativar o modo preemptivo, passando por parâmetro.
// Ao simular esses algoritmos, analise o desempenho do sistema em relação a métricas como tempo de espera (quanto tempo os processos ficam aguardando antes de serem processados), tempo de resposta (tempo entre a submissão do processo e a primeira resposta), tempo de processamento (tempo total necessário para concluir o processo) e o tempo de turnaround (tempo transcorrido desde o momento em que o processo entra e o instante em que termina sua execução).
// Objetivo: O objetivo desta atividade é ajudá-lo a entender como diferentes algoritmos de escalonamento funcionam e como eles influenciam o desempenho do sistema operacional.

#include <stdio.h>
#include <stdlib.h>
#include<time.h>

#define TEMPO_QUANTUM 30

typedef struct tempos{
    float tempoEspera;
    float tempoResposta;
    float tempoProcessamento;
    float tempoTurnaround;
}tempos;

typedef struct Processo{
    int idProcesso;
    int tempoChegada;
    int duracao;
    tempos tempos;
}Processo;

typedef struct Node {
    Processo data;           
    struct Node* prev;  // Ponteiro para o nó anterior
    struct Node* next;  // Ponteiro para o próximo nó
} Node;

Node* createNode(Processo data) {
    Node* newNode = (Node*)malloc(sizeof(Node));
    if (newNode == NULL) {
        printf("Erro: Falha na alocação de memória para o nó.\n");
        exit(EXIT_FAILURE);
    }
    newNode->data = data;
    newNode->prev = NULL;
    newNode->next = NULL;
    return newNode;
}

// Função para inserir um nó no final da lista
void insertAtEnd(Node** head, Processo data) {
    Node* newNode = createNode(data);
    if (*head == NULL) {
        *head = newNode;
    } else {
        Node* temp = *head;
        while (temp->next != NULL) {
            temp = temp->next;
        }
        temp->next = newNode;
        newNode->prev = temp;
    }
}

void calculaMedias(int quant, tempos tempoMedia) {
    tempoMedia.tempoEspera = tempoMedia.tempoEspera / quant;
    tempoMedia.tempoResposta = tempoMedia.tempoResposta / quant;
    tempoMedia.tempoProcessamento = tempoMedia.tempoProcessamento / quant;
    tempoMedia.tempoTurnaround = tempoMedia.tempoTurnaround / quant;

    printf("\t Tempo de Espera: %.2f\n", tempoMedia.tempoEspera);
    printf("\t Tempo de Resposta: %.2f\n", tempoMedia.tempoResposta);
    printf("\t Tempo de Processamento: %.2f\n", tempoMedia.tempoProcessamento);
    printf("\t Tempo de Turnaround: %.2f\n\n", tempoMedia.tempoTurnaround);
}

tempos inicializaTempos(tempos tempo){
    tempo.tempoEspera = 0;
    tempo.tempoResposta = 0;
    tempo.tempoProcessamento = 0;
    tempo.tempoTurnaround = 0;
    return tempo;
}

void FCFS(Node* head, int quant) {
    int tempoSistema = 0;
    int tempoEsperaGlobal = 0; // Variável para armazenar o tempo de espera global

    Node* temp = head;
    tempos tempoMedia = inicializaTempos(tempoMedia);

    while (temp != NULL) {
        tempos tempTempos = temp->data.tempos;

        tempoSistema += temp->data.duracao; // tempo atual do sistema

        // Calcula o tempo de processamento
        tempTempos.tempoProcessamento = temp->data.duracao; 
        temp->data.duracao = 0;

        // Atualiza o tempo de espera para o nó atual
        tempTempos.tempoEspera = tempoEsperaGlobal; 

        // Atualiza o tempo de espera global
        tempoEsperaGlobal += tempTempos.tempoProcessamento;

        // Calcula o tempo de Resposta
        tempTempos.tempoResposta = tempTempos.tempoEspera;

        // Calculo do tempo de Turnaround
        tempTempos.tempoTurnaround = tempTempos.tempoEspera + tempTempos.tempoProcessamento;

        // Calcula a média dos tempos        
        tempoMedia.tempoEspera += tempTempos.tempoEspera;
        tempoMedia.tempoResposta += tempTempos.tempoResposta;
        tempoMedia.tempoProcessamento += tempTempos.tempoProcessamento;
        tempoMedia.tempoTurnaround += tempTempos.tempoTurnaround;

        temp = temp->next;
    }
    
    printf("Tempos do Escalonador FCFS\n");
    calculaMedias(quant, tempoMedia);
    printf("####################################################\n");
}

// RR pela ordem de chegada
void RoundRobin(Node* headRR, int quant){
    int tempoSistema = 0;
    int tempoEsperaGlobal = 0;
    int quantum = TEMPO_QUANTUM;

    Node* tempRR = headRR;
    tempos tempoMediaRR = inicializaTempos(tempoMediaRR);

    while (tempRR != NULL && tempRR->data.duracao > 0) {
        tempos tempTempos = tempRR->data.tempos;
        // Processo vai ser finalizado nesse quantum
        if (tempRR->data.duracao <= quantum) {
            tempoSistema += tempRR->data.duracao; // Atualiza o tempo do sistema
            tempTempos.tempoProcessamento += tempRR->data.duracao; // Calcula o tempo de processamento
            tempRR->data.duracao = 0; // Processo finalizado
        } // Processo vai preceisar de ao menos mais um quantum
        else {
            tempoSistema += quantum; // Atualiza o tempo do sistema
            tempTempos.tempoProcessamento += quantum; // Calcula o tempo de processamento
            tempRR->data.duracao -= quantum; // Atualiza o tempo restante do processo
        }
        tempTempos.tempoEspera = tempoEsperaGlobal; // Atualiza o tempo de espera
        tempoEsperaGlobal += tempTempos.tempoProcessamento; // Atualiza o tempo de espera global
        tempTempos.tempoResposta = tempTempos.tempoEspera;  // Calcula o tempo de resposta
        tempTempos.tempoTurnaround = tempTempos.tempoEspera + tempTempos.tempoProcessamento; // Calcula o tempo de turnaround
        
        // Calcula a média dos tempos
        tempoMediaRR.tempoEspera += tempTempos.tempoEspera;
        tempoMediaRR.tempoResposta += tempTempos.tempoResposta;
        tempoMediaRR.tempoProcessamento += tempTempos.tempoProcessamento;
        tempoMediaRR.tempoTurnaround += tempTempos.tempoTurnaround;
        
         if(tempRR->next != NULL){
            tempRR = tempRR->next;
        }else{
            tempRR = headRR;
        }
    }
    printf("\nTempos do Escalonador RR\n");
    calculaMedias(quant, tempoMediaRR);
    printf("####################################################\n");
}

void ordenacaoPorDuracao(Node* headSJF, int quant) {
    Node* tempSJF = headSJF;
    Node* tempSJF2 = NULL;
    for(int i = 0; i<quant; i++){ // Ordena a lista de processos pelo tempo de duração
        tempSJF2 = tempSJF->next;
        while(tempSJF2 != NULL){
            if(tempSJF->data.duracao > tempSJF2->data.duracao){
                Processo tempProcesso = tempSJF->data;
                tempSJF->data = tempSJF2->data;
                tempSJF2->data = tempProcesso;
            }
            tempSJF2 = tempSJF2->next;
        }
        tempSJF = tempSJF->next;
    }
}

void SJF(Node* headSJF, int quant){
        int tempoSistema = 0;
        int tempoEsperaGlobal = 0;

        Node* tempSJF = headSJF;
        // Ordenacao dos processos pelo tempo de duração
        ordenacaoPorDuracao(headSJF, quant);

        tempSJF = headSJF;
        tempos tempoMediaSJF = inicializaTempos(tempoMediaSJF);

        while (tempSJF != NULL && tempSJF->data.duracao > 0) {
            tempos tempTempos = tempSJF->data.tempos;

            tempoSistema += tempSJF->data.duracao; // Atualiza o tempo do sistema
            tempTempos.tempoProcessamento += tempSJF->data.duracao; // Calcula o tempo de processamento
            tempSJF->data.duracao = 0; // Processo finalizado
            tempTempos.tempoEspera = tempoEsperaGlobal; // Atualiza o tempo de espera
            tempoEsperaGlobal += tempTempos.tempoProcessamento; // Atualiza o tempo de espera global
            tempTempos.tempoResposta = tempTempos.tempoEspera;  // Calcula o tempo de resposta
            tempTempos.tempoTurnaround = tempTempos.tempoEspera + tempTempos.tempoProcessamento; // Calcula o tempo de turnaround
            
            // Calcula a média dos tempos
            tempoMediaSJF.tempoEspera += tempTempos.tempoEspera;
            tempoMediaSJF.tempoResposta += tempTempos.tempoResposta;
            tempoMediaSJF.tempoProcessamento += tempTempos.tempoProcessamento;
            tempoMediaSJF.tempoTurnaround += tempTempos.tempoTurnaround;
            
            tempSJF = tempSJF->next;
        }
        
        printf("\nTempos do Escalonador SJF\n");
        calculaMedias(quant, tempoMediaSJF);
}

int main(int argc, char *argv[]) {
    int quant = atoi(argv[1]);

    Node* headFCFS = NULL;
    Node* headRR = NULL;
    Node* headSJF = NULL;

    // Processo processosFCFS;
    // Processo processosRR;
    // Processo processosSJF;

    Processo processos;
    srand(time(NULL));

    // Inserir elementos na lista
    for(int i = 0; i<quant; i++){
        tempos tempTempos = processos.tempos;
        processos.idProcesso = i;
        processos.tempoChegada = i;

        int num = rand() % 41 + 10;
        processos.duracao = num;

        tempTempos = inicializaTempos(tempTempos);
        printf("%d: Tempo Chegada: %d\t Tempo de duração: %d\n",processos.idProcesso, processos.tempoChegada, processos.duracao);
        insertAtEnd(&headFCFS, processos);
        insertAtEnd(&headRR, processos);
        insertAtEnd(&headSJF, processos);
    }
    FCFS(headFCFS, quant);
    RoundRobin(headRR, quant);
    SJF(headSJF, quant);
    return 0;
}