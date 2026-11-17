# Gerador de Autômatos Finitos Determinísticos (AFD)

Aplicação em **C++17** desenvolvida para o componente curricular de **Linguagens Formais e Autômatos (LFA)** do curso de Ciência da Computação da **Universidade Federal da Fronteira Sul (UFFS) – Campus Chapecó**.

O sistema lê especificações léxicas contendo palavras reservadas/tokens literais e Gramáticas Regulares (GRs) em notação BNF, gerando a tabela de transições de um **Autômato Finito Determinístico (AFD)** completo, livre de estados inalcançáveis e mortos (sem aplicação de classes de equivalência) e totalizado com estado de erro.

---

## Arquitetura do Projeto

O processamento segue o estilo arquitetural **Pipes and Filters**, dividido em cinco etapas sequenciais:

| Etapa | Módulo (`src/`) | Responsabilidade |
| :--- | :--- | :--- |
| **1. Leitura** | `entrada/LeitorArquivo` | Realiza o *parsing* de tokens literais e produções BNF a partir do arquivo texto. |
| **2. Construção AFND** | `pipeline/ConstrutorAFND` | Unifica tokens e GRs compartilhando exclusivamente o estado inicial `S` (sem transições $\epsilon$). |
| **3. Determinização** | `pipeline/Determinizador` | Converte o AFND em AFD via **Construção de Subconjuntos** (Teorema 3.1), preservando a precedência léxica. |
| **4. Minimização** | `pipeline/Minimizador` | Remove estados inalcançáveis (BFS direta a partir de `S`) e estados mortos (BFS reversa a partir de $F$). |
| **5. Totalização** | `pipeline/TotalizadorErro` | Fecha a função de transição $\delta$ adicionando o estado absorvedor `ERRO` para todas as células indefinidas. |

---

## Pré-requisitos

* Compilador C++ com suporte ao padrão **C++17** (`g++` via MinGW/WinLibs no Windows ou GCC no Linux/macOS).

---

## Como Compilar e Executar

### Windows (PowerShell)

**Compilar e executar com o arquivo padrão (`entradas/exemplo_pdf.txt`):**
```powershell
g++ -std=c++17 -Wall -Wextra src/*.cpp src/*/*.cpp -o gerador_afd.exe; if ($?) { .\gerador_afd.exe }
```

**Apenas compilar:**
```powershell
g++ -std=c++17 -Wall -Wextra src/*.cpp src/*/*.cpp -o gerador_afd.exe
```

**Executar passando um arquivo de entrada específico:**
```powershell
.\gerador_afd.exe entradas/meu_teste.txt
```

### Linux / macOS

**Compilar e executar:**
```bash
g++ -std=c++17 -Wall -Wextra src/*.cpp src/*/*.cpp -o gerador_afd && ./gerador_afd
```

**Executar com arquivo personalizado:**
```bash
./gerador_afd entradas/meu_teste.txt
```

---

## Formato do Arquivo de Entrada

O arquivo fonte deve conter tokens literais (um por linha) e/ou regras de Gramáticas Regulares lineares à direita em notação BNF (utilizando o caractere `ε` ou `&` para representar a cadeia vazia $\epsilon$):

```text
se
entao
senao
<S> ::= a<A> | e<A> | i<A> | o<A> | u<A>
<A> ::= a<A> | e<A> | i<A> | o<A> | u<A> | ε
```

---

## Estrutura de Diretórios

```text
gerador-afd/
├── entradas/
│   └── exemplo_pdf.txt          # Caso de teste de referência da especificação
├── src/
│   ├── main.cpp                 # Orquestrador do pipeline
│   ├── modelo/
│   │   ├── AutomatoFinito.hpp   # Estrutura da quíntupla M = (K, Σ, δ, q0, F)
│   │   └── AutomatoFinito.cpp
│   ├── entrada/
│   │   ├── LeitorArquivo.hpp    # Parser de tokens e Gramáticas Regulares (BNF)
│   │   └── LeitorArquivo.cpp
│   ├── pipeline/
│   │   ├── ConstrutorAFND.*     # Etapa 2: Geração do AFND unificado
│   │   ├── Determinizador.*     # Etapa 3: Construção de Subconjuntos
│   │   ├── Minimizador.*        # Etapa 4: Poda de estados inalcançáveis e mortos
│   │   └── TotalizadorErro.*    # Etapa 5: Totalização com Estado de Erro
│   └── saida/
│       ├── ExibidorTabela.hpp   # Formatação das tabelas de transição no terminal
│       └── ExibidorTabela.cpp
└── README.md
```

---

## Autor

* **Abimael Pivetta Mendes** — Ciência da Computação (UFFS – Campus Chapecó)