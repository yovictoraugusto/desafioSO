#include<stdio.h>
#include<stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

int main(int argc, char *argv[]){
    int quantFilhos, intervaloInicio, intervaloFinal;

    // Verifica se foi respeitado os requisitos de entrada
    if(argc != 4){
        fprintf(stderr, "\nFormato errado de entrada!\nFormato certo: quantidadeFilhos intervaloInicio intervalorFinal\n\n");
        return 1;
    }

    // Pega as entradas e atribui a variáveis
    quantFilhos = atoi(argv[1]);
    intervaloInicio = atoi(argv[2]);
    intervaloFinal = atoi(argv[3]);

    // Calcula a quantidade de números por procesos, resto (caso não seja possível
    // dividir igualmente por todos os filhos)
    int quantPorProcesso = (intervaloFinal - intervaloInicio +1) / quantFilhos;
    int restoDoIntervalo = (intervaloFinal - intervaloInicio + 1) % quantFilhos;

    // printf("Quantidade de numeros por processo: %d\n", quantProcesso);
    // printf("Resto: %d", restoDoIntervalo);

    int somaTotal = 0;

    // Criar pipe
    int pipe_fd[2];
    if (pipe(pipe_fd) == -1) {
        perror("Erro ao criar o pipe");
        exit(EXIT_FAILURE);
    }

    // Filhos
    for(int i = 0; i < quantFilhos; i++){
        pid_t pid = fork();

        if(pid<0){
            perror("Erro no fork");
            return 1;
        }else if(pid == 0){
            // Calcular o inicio e fim de cada processo
            int auxInicio = intervaloInicio + i * quantPorProcesso;
            int auxFinal = auxInicio + quantPorProcesso - 1;
            
            // Caso não dê para dividir igualmente todos os processos,
            // os numeros faltantes vão para o ultimo filho
            if(i == quantFilhos -1) auxFinal = auxFinal + restoDoIntervalo;

            int auxSoma = 0;
            for(int j = auxInicio; j <= auxFinal; j++){
                auxSoma = auxSoma + j;
            }

            // printf("Filho %d, inicio %d, fim %d, soma do intervalo %d\n", i, auxInicio, auxFinal, auxSoma);
            close(pipe_fd[0]); // Fechar o lado de leitura do pipe no filho
            write(pipe_fd[1], &auxSoma, sizeof(auxSoma)); // Escreve o valor retornado no lado de escrita do pipe filho
            close(pipe_fd[1]); // Fechar o lado de escrita do pipe no filho

            exit(EXIT_SUCCESS); // "Retorna" o valor que o filho calculou
        }
    }

    // Pai
    for (int i = 0; i < quantFilhos; i++) {
        int valorFilho;
        close(pipe_fd[1]); // Fecha o lado de escrita do pipe no pai
        read(pipe_fd[0], &valorFilho, sizeof(valorFilho));
        // printf("\n%d\n\n", valorFilho);
        somaTotal += valorFilho;
    }

    close(pipe_fd[0]); // Fechar o lado de leitura do pipe no pai após ler todos os valores

    printf("Soma do intervalor %d a %d é: %d\n", intervaloInicio, intervaloFinal, somaTotal);
    
    return 0;
}