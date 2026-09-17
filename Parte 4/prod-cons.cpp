#include <iostream>
#include <vector>
#include <thread>
#include <mutex>
#include <semaphore.h>
#include <chrono>
#include <fstream>
#include <cstdlib>
#include <string>

using namespace std;
using namespace std::chrono;

// Variáveis Globais
int N, Np, Nc;
const int M = 100000;
int itens_processados = 0;
int ocupacao_atual = 0;
bool silencioso = false;

vector<int> buffer;
int pos_in = 0, pos_out = 0;
vector<int> historico_ocupacao;

// Sincronização
sem_t espacos_livres;
sem_t itens_disponiveis;
mutex mtx_buffer;
mutex mtx_saida;


// Função de verificação de primalidade
bool is_prime(int n) {
    if (n <= 1) return false;
    if (n == 2 || n == 3) return true;
    if (n % 2 == 0) return false;
    for (int i = 3; i * i <= n; i += 2) {
        if (n % i == 0) return false;
    }
    return true;
}

void thread_produtora(int id) {
    while (true) {
        int numero = (rand() % 10000000) + 1; 

        sem_wait(&espacos_livres); 
        mtx_buffer.lock(); 
        
        if (itens_processados >= M) {
            mtx_buffer.unlock();
            sem_post(&itens_disponiveis);
            sem_post(&espacos_livres); // Evita deadlock no encerramento
            break;
        }
        
        // Insere no buffer circular
        buffer[pos_in] = numero;
        pos_in = (pos_in + 1) % N;
        ocupacao_atual++;
        
        // Registra ocupação para o gráfico
        historico_ocupacao.push_back(ocupacao_atual);
        
        mtx_buffer.unlock(); 
        sem_post(&itens_disponiveis); 
    }
}

void thread_consumidora(int id) {
    while (true) {
        sem_wait(&itens_disponiveis); 
        mtx_buffer.lock(); 
        
        if (itens_processados >= M) {
            mtx_buffer.unlock();
            sem_post(&espacos_livres);
            sem_post(&itens_disponiveis); // Evita deadlock no encerramento
            break;
        }
        
        // Retira do buffer circular
        int numero = buffer[pos_out];
        pos_out = (pos_out + 1) % N;
        ocupacao_atual--;
        itens_processados++;
        
        historico_ocupacao.push_back(ocupacao_atual);
        
        mtx_buffer.unlock(); 
        sem_post(&espacos_livres); 
        
        // A verificação e impressão ocorrem FORA da zona crítica para não travar as outras threads
        bool primo = is_prime(numero);

        if (!silencioso) {
            lock_guard<mutex> lock(mtx_saida);
            cout << numero << " é " << (primo ? "primo" : "não primo") << "\n";
        }
    }
}

int main(int argc, char* argv[]) {
    if (argc < 4 || argc > 5) {
        cerr << "Uso: " << argv[0] << " <Np> <Nc> <N (Tamanho do Buffer)> [silencioso]\n";
        return 1;
    }

    Np = stoi(argv[1]);
    Nc = stoi(argv[2]);
    N = stoi(argv[3]);

    if (argc == 5) {
        silencioso = (stoi(argv[4]) == 1); 
    }

    buffer.resize(N);
    historico_ocupacao.reserve(M * 2); // Pré-aloca espaço para evitar realocações na memória

    // Inicialização dos semáforos POSIX
    sem_init(&espacos_livres, 0, N);
    sem_init(&itens_disponiveis, 0, 0);

    vector<thread> produtoras, consumidoras;

    srand(time(NULL));

    auto inicio_tempo = high_resolution_clock::now();

    // Lançamento das threads
    for (int i = 0; i < Np; i++) produtoras.push_back(thread(thread_produtora, i));
    for (int i = 0; i < Nc; i++) consumidoras.push_back(thread(thread_consumidora, i));

    // Aguarda finalização
    for (auto& t : produtoras) t.join();
    for (auto& t : consumidoras) t.join();

    auto fim_tempo = high_resolution_clock::now();
    duration<double, milli> duracao = fim_tempo - inicio_tempo;

    cout << "Tempo de Execução (Np=" << Np << ", Nc=" << Nc << ", N=" << N << "): " 
         << duracao.count() << " ms\n";

    // Geração do arquivo para o gráfico
    string filename = "ocupacao_Np"+ to_string(Np) + "_Nc" + to_string(Nc) + "_N" + to_string(N) + ".csv";
    ofstream arquivo_csv(filename);
    arquivo_csv << "Operacao,Ocupacao\n";
    for (size_t i = 0; i < historico_ocupacao.size(); i++) {
        arquivo_csv << i << "," << historico_ocupacao[i] << "\n";
    }
    arquivo_csv.close();

    // Limpeza
    sem_destroy(&espacos_livres);
    sem_destroy(&itens_disponiveis);

    return 0;
}