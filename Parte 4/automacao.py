import subprocess
import matplotlib.pyplot as plt
import re
import os

# Parâmetros exigidos pela especificação
valores_N = [1, 10, 100, 1000]
combinacoes = [(1, 1), (1, 2), (1, 4), (1, 8), (2, 1), (4, 1), (8, 1)]
repeticoes = 10

# Dicionário para guardar os resultados: { N: [medias...] }
tempos_medios = {N: [] for N in valores_N}
labels_eixo_x = [f"({Np},{Nc})" for Np, Nc in combinacoes]

print("Iniciando bateria de testes (280 execuções).\n")

for N in valores_N:
    for Np, Nc in combinacoes:
        tempos = []
        print(f"Rodando cenário Np={Np}, Nc={Nc}, N={N}...")
        
        for i in range(repeticoes):
            # Executa o programa C++ em background
            resultado = subprocess.run(["./prod-cons", str(Np), str(Nc), str(N), "1"], capture_output=True, text=True)
            
            # Puxa o tempo (ms) da frase que o C++ imprimiu usando Regex
            match = re.search(r": ([\d.]+) ms", resultado.stdout)
            if match:
                tempos.append(float(match.group(1)))
            else:
                print(f"Erro ao ler a saída na repetição {i}")

        qtd_sucessos = len(tempos)
        if qtd_sucessos > 0:
            media = sum(tempos) / qtd_sucessos
        else:
            media = 0
            print(f"Aviso: Nenhuma execução funcionou para o cenário Np={Np}, Nc={Nc}.")
            
        tempos_medios[N].append(media)

print("\nTestes finalizados! Gerando o gráfico de Tempo Médio...")

# --- 1. GERAÇÃO DO GRÁFICO DE TEMPO MÉDIO ---
plt.figure(figsize=(10, 6))

# Plota uma curva (linha) para cada valor de N
for N in valores_N:
    plt.plot(labels_eixo_x, tempos_medios[N], marker='o', label=f'Buffer N={N}')

plt.title("Tempo Médio de Execução vs Número de Threads")
plt.xlabel("Combinações de Threads (Np, Nc)")
plt.ylabel("Tempo Médio de Execução (ms)")
plt.legend()
plt.grid(True)
plt.savefig("grafico_tempos_medios.png", dpi=300)
print("Gráfico principal salvo como 'grafico_tempos_medios.png'.")