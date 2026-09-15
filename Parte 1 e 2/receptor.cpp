#include <iostream>
#include <csignal>
#include <cstdlib>
#include <unistd.h>

using namespace std;

// Signal handler para os três sinais escolhidos
void signal_handler(int sig) {
    if (sig == SIGUSR1) {
        cout << "\n[Handler] Recebeu SIGUSR1 (Sinal de usuário 1).\n";
    } else if (sig == SIGUSR2) {
        cout << "\n[Handler] Recebeu SIGUSR2 (Sinal de usuário 2).\n";
    } else if (sig == SIGINT) {
        cout << "\n[Handler] Recebeu SIGINT. Encerrando a execução do programa...\n";
        exit(0); 
    }
}

int main(int argc, char* argv[]) {
    if (argc != 2) {
        cerr << "Uso: " << argv[0] << " <0 para Blocking Wait | 1 para Busy Wait>\n";
        return 1;
    }
    
    int modo = stoi(argv[1]);

    // Registro dos handlers
    signal(SIGUSR1, signal_handler);
    signal(SIGUSR2, signal_handler);
    signal(SIGINT, signal_handler);

    cout << "Processo Receptor PID: " << getpid() << "\n";
    
    if (modo == 0) {
        cout << "Iniciando em modo Blocking Wait...\n";
        while (true) {
            pause(); // O SO suspende a thread até a chegada de um sinal
        }
    } else if (modo == 1) {
        cout << "Iniciando em modo Busy Wait...\n";
        while (true) {
            // Loop infinito ativo consumindo ciclos de CPU 
        }
    } else {
        cerr << "Modo inválido.\n";
        return 1;
    }

    return 0;
}