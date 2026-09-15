#include <iostream>
#include <sys/types.h>
#include <signal.h>
#include <unistd.h>
#include <cstdlib>
#include <cerrno>

using namespace std;

int main(int argc, char* argv[]) {
    // Validação dos parâmetros de entrada
    if (argc != 3) {
        cerr << "Uso: " << argv[0] << " <PID> <Sinal>\n";
        return 1;
    }
    
    pid_t pid = stoi(argv[1]);
    int sig = stoi(argv[2]);

        // Envio do sinal e verificação de erros
    if (kill(pid, sig) == -1) {
        if (errno == ESRCH) {
            cerr << "Erro: O processo destino " << pid << " não existe.\n";
        } else {
            perror("Erro ao enviar sinal");
        }
        return 1;
    }

    cout << "Sinal " << sig << " enviado com sucesso ao processo " << pid << ".\n";
    return 0;
}