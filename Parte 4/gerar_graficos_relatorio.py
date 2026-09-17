import pandas as pd
import matplotlib.pyplot as plt
import os

valores_N = [1, 10, 100, 1000]
combinacoes = [(1, 1), (1, 2), (1, 4), (1, 8), (2, 1), (4, 1), (8, 1)]

print("Gerando gráficos de ocupação aglomerados para o relatório...")

for N in valores_N:
    # Cria uma figura grande com 7 minigráficos (1 linha, 7 colunas)
    fig, axes = plt.subplots(nrows=1, ncols=7, figsize=(20, 4), sharey=True)
    fig.suptitle(f"Ocupação do Buffer ao Longo do Tempo (Buffer N={N}) - Primeiras 5000 operações", fontsize=14)
    
    for idx, (Np, Nc) in enumerate(combinacoes):
        arquivo_csv = f"ocupacao_Np{Np}_Nc{Nc}_N{N}.csv"
        ax = axes[idx]
        
        if os.path.exists(arquivo_csv):
            df = pd.read_csv(arquivo_csv)
            df_zoom = df.head(5000) # Pega apenas as primeiras 5000 operações para limitar um pouco o gráfico
            
            ax.plot(df_zoom['Operacao'], df_zoom['Ocupacao'], color='blue', linewidth=1)
            ax.set_title(f"Np={Np}, Nc={Nc}")
            ax.set_xlabel("Operação")
            ax.grid(True)
            
            # Limita o eixo Y para o tamanho exato do buffer
            ax.set_ylim(0, N if N > 1 else 2) 
        else:
            ax.text(0.5, 0.5, 'Sem dados', ha='center', va='center')
            ax.set_title(f"Np={Np}, Nc={Nc}")

    axes[0].set_ylabel("Itens no Buffer")
    
    plt.tight_layout()
    # Ajusta o espaçamento para o título não sobrepor os gráficos
    plt.subplots_adjust(top=0.85) 
    
    nome_imagem = f"ocupacao_consolidada_N{N}.png"
    plt.savefig(nome_imagem, dpi=300)
    plt.close()
    
    print(f"[{nome_imagem}] gerado com sucesso!")

print("\nConcluído!")