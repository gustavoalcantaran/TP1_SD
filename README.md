# TP1 - Sistemas Distribuídos

Implementação das atividades do Trabalho Prático 1 de Sistemas Distribuídos.

## Estrutura do projeto

```text
TP1/
├── Parte 1 e 2/
│   ├── emissor.cpp
│   └── receptor.cpp
├── Parte 4/
│   ├── prod-cons.cpp
│   ├── automacao.py
│   ├── gerar_graficos_relatorio.py
│   ├── *.csv
│   └── *.png
└── SD_TP1_2_2026.pdf
```

As fontes usam APIs POSIX, portanto a execução deve ser feita em Linux, WSL ou outro ambiente compatível. O PDF contém o enunciado da atividade.

## Parte 1 e 2 - Comunicação por sinais

Esta parte é composta por dois programas:

- `receptor.cpp`: registra handlers para `SIGUSR1`, `SIGUSR2` e `SIGINT`, exibe o próprio PID e aguarda sinais em um dos dois modos disponíveis.
- `emissor.cpp`: recebe um PID e um número de sinal e envia o sinal ao processo receptor usando `kill`.

### Compilação

Na pasta `Parte 1 e 2`:

```bash
g++ receptor.cpp -o receptor
g++ emissor.cpp -o emissor
```

### Execução

Em um terminal, inicie o receptor em modo de espera bloqueante:

```bash
./receptor 0
```

Ou inicie-o em modo de espera ocupada:

```bash
./receptor 1
```

O programa exibirá o PID do processo. Em outro terminal, envie sinais usando esse PID:

```bash
./emissor <PID_DO_RECEPTOR> 10   # SIGUSR1
./emissor <PID_DO_RECEPTOR> 12   # SIGUSR2
./emissor <PID_DO_RECEPTOR> 2    # SIGINT, encerra o receptor
```

Os valores numéricos dos sinais podem variar entre sistemas. Em Linux, `SIGUSR1`, `SIGUSR2` e `SIGINT` normalmente correspondem a 10, 12 e 2, respectivamente.

### Comparação dos modos de espera

- **Blocking Wait:** o receptor usa `pause()` e fica suspenso até a chegada de um sinal. Esse modo evita consumo desnecessário de CPU enquanto não há eventos.
- **Busy Wait:** o receptor permanece em um loop ativo. A resposta aos sinais continua funcionando, mas o processo consome ciclos de CPU continuamente.

## Parte 4 - Problema produtor-consumidor

O programa `prod-cons.cpp` implementa um buffer circular compartilhado entre threads produtoras e consumidoras.

- `Np`: quantidade de threads produtoras.
- `Nc`: quantidade de threads consumidoras.
- `N`: capacidade do buffer.
- `M = 100000`: quantidade total de itens processados.

Cada produtora gera números aleatórios e os insere no buffer. Cada consumidora retira um número e verifica se ele é primo. O acesso às posições e aos contadores compartilhados é protegido por mutex, enquanto os semáforos controlam a quantidade de espaços livres e itens disponíveis.

### Compilação e execução

Na pasta `Parte 4`:

```bash
g++ -std=c++17 -pthread prod-cons.cpp -o prod-cons
./prod-cons <Np> <Nc> <N>
```

Exemplo:

```bash
./prod-cons 2 4 100
```

Ao terminar, o programa imprime o tempo de execução e cria um arquivo CSV com o histórico de ocupação do buffer, por exemplo:

```text
ocupacao_Np2_Nc4_N100.csv
```

### Automação dos experimentos

O script `automacao.py` executa 10 repetições para cada combinação de:

- buffers `N = 1, 10, 100 e 1000`;
- configurações `(Np, Nc) = (1,1), (1,2), (1,4), (1,8), (2,1), (4,1) e (8,1)`.

Isso totaliza 280 execuções. O script coleta os tempos impressos pelo programa e gera o gráfico de tempos médios.

Com Python e as dependências instaladas, execute:

```bash
python3 automacao.py
python3 gerar_graficos_relatorio.py
```

Dependências:

```bash
pip install matplotlib pandas
```

O segundo script lê os CSVs e gera um gráfico consolidado para cada tamanho de buffer. Os gráficos de ocupação mostram as primeiras 5000 operações para manter a visualização legível.

## Resultados

### Tempo médio de execução

![Tempo médio de execução](Parte%204/grafico_tempos_medios.png)

O gráfico compara o tempo médio para diferentes quantidades de produtoras, consumidoras e tamanhos de buffer. Buffers muito pequenos tendem a aumentar a disputa por sincronização, enquanto o aumento do número de threads não garante melhoria monotônica: depois de certo ponto, o custo de coordenação pode superar o paralelismo obtido.

### Ocupação do buffer

#### Buffer `N = 1`

![Ocupação do buffer N=1](Parte%204/ocupacao_consolidada_N1.png)

Com capacidade unitária, produtores e consumidores precisam alternar praticamente a cada operação. A ocupação fica restrita aos valores 0 e 1, evidenciando a forte influência dos semáforos.

#### Buffer `N = 10`

![Ocupação do buffer N=10](Parte%204/ocupacao_consolidada_N10.png)

O buffer passa a absorver pequenas diferenças entre as velocidades de produção e consumo. Ainda assim, configurações desbalanceadas podem levar a períodos de buffer vazio ou próximo da capacidade máxima.

#### Buffer `N = 100`

![Ocupação do buffer N=100](Parte%204/ocupacao_consolidada_N100.png)

A capacidade maior reduz a necessidade de bloqueios imediatos e permite que as threads trabalhem com mais independência durante parte da execução.

#### Buffer `N = 1000`

![Ocupação do buffer N=1000](Parte%204/ocupacao_consolidada_N1000.png)

Com um buffer grande, a ocupação tende a apresentar maior espaço para absorver variações de escalonamento. O comportamento observado ainda depende da relação entre o número de produtoras, consumidoras e o custo da verificação de primalidade.