#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <errno.h>
#include <time.h>

#define TAM 20 //Definindo o tamanho da string para 20 bytes

//Função para determinar se é primo ou não
int is_prime(int n) {
    if (n <= 1) return 0;
    for (int i = 2; i*i <= n; i++) {
        if (n % i == 0) return 0;
    }
    return 1;
}

int main(int argc, char const *argv[])
{
    //Conferindo a quantidade de argumentos passados para o programa está correta
    if (argc != 2) {
        fprintf(stderr, "Uso: %s <quantidade>\n", argv[0]);
        return 1;
    }

    //Convertendo a string passada pelo usuário para inteiro
    int qtd = atoi(argv[1]);

    //Criando um pipe
    int fd[2];
    if (pipe(fd) == -1)
    {
        perror("Erro ao abrir pipe");
        return 2;
    }

    //Fazendo um fork para duplicar os processos
    pid_t id = fork();
    if(id == -1)
    {
        perror("Erro ao executar fork");
        return 3;
    }
    
    if (id==0) //PRODUTOR
    {
        close(fd[0]);   //Fechando a ponta de leitura
        srand(time(NULL));
        int n=1;

        for (int i = 0; i < qtd; i++)
        {
            //Calculando N
            int delta = (rand()%100)+1;
            n+=delta;

            //Convertendo o inteiro para uma string de TAManho fixo
            char str[TAM];
            snprintf(str, TAM, "%d", n);

            //Produtor enviando a mensagem N no formato de string 
            if( write(fd[1], str, TAM) ==-1)
            {
                perror("Erro ao escrever no pipe");
                return 4;
            }
            printf("Produtor: enviou %d\n", n);
        }
        //Envia 0 para encerrar
        char str[TAM];
        snprintf(str, TAM, "%d", 0);
        if( write(fd[1], str, TAM) ==-1)
        {
            perror("Erro ao escrever no pipe");
            return 4;
        }
        printf("Produtor: enviou 0 (fim)\n");

        close(fd[1]);   //Fechando a ponta de escrita depois de terminar de escrever
    }
    else    //CONSUMIDOR
    {
        close(fd[1]);   //Fechando a ponta de escrita
        char str[TAM];
        while (1)
        {
            int nbytes = read(fd[0], str, TAM);
            if (nbytes == 0) {
                // Pipe fechado normalmente
                break;
            }
            if (nbytes == -1) {
                perror("Erro ao ler do pipe");
                return 5;
            }
            int num = atoi(str);
            if (num == 0)   //Avaliando se encerrou o programa
            {
                printf("Consumidor: Fim da execução\n");
                break;
            }
            if (is_prime(num))  //Avaliando se o número é primo ou não
            {
                printf("Consumidor: %d é primo\n", num);
            }
            else
            {
                printf("Consumidor: %d não é primo\n", num);
            }
            
        }
        close(fd[0]);   //Fechando a ponta de leitura depois de terminar de escrever
            
        wait(NULL);//Garantindo que o produtor finalize antes do consumidor encerrar completamente
    }
    
    

    return 0;
}
