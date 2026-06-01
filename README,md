# 🕵️‍♂️ Detetive RPG - Mansão Hash & Árvores

Um jogo interativo de investigação em turnos com interface gráfica (GUI), desenvolvido na linguagem **C** utilizando a biblioteca **Raylib**. O projeto aplica conceitos avançados de **Estruturas de Dados** para gerenciar a exploração do cenário, coleta de pistas e a lógica de acusação dos suspeitos.

---

## 🧠 Estruturas de Dados Utilizadas

O motor do jogo foi construído utilizando dois pilares fundamentais da ciência da computação:

1. **Árvore Binária de Busca (BST - Binary Search Tree):** * **O Mapa da Mansão:** Os cômodos estão interconectados em uma estrutura de árvore binária com ponteiros bidirecionais (esquerda, direita e pai), permitindo que o jogador navegue de forma dinâmica e consiga retornar pelos caminhos já explorados.
   * **Organização de Pistas:** À medida que o jogador explora os cenários, as pistas encontradas são inseridas em uma BST de pistas coletadas. Isso garante que a listagem das evidências seja feita de forma organizada e em **ordem alfabética (Percurso Em-Ordem)**.

2. **Tabela Hash com Encadeamento:**
   * **Dicionário de Evidências:** Utilizada para fazer o mapeamento imediato ($O(1)$) entre uma pista encontrada e o respectivo suspeito daquele rastro. 
   * Quando o jogador faz uma acusação no Julgamento Final, o sistema varre a árvore de pistas e consulta a Tabela Hash para validar se o suspeito possui consistência de provas (mínimo de 2 pistas vinculadas) para ser considerado CULPADO.

---

## 🎮 Como Jogar

1. **Exploração:** Você começa no *Hall de Entrada*. Use os botões da interface para mover-se para a **Esquerda**, **Direita** ou **Voltar** para o cômodo anterior.
2. **Coleta Automática:** Ao entrar em uma sala que contém uma evidência, o jogo coleta a pista automaticamente e a armazena na sua Árvore de Pistas.
3. **Menu Inferior:**
   * **Ver Pistas Coletadas:** Abre a tela que exibe suas pistas em ordem alfabética através do percurso da árvore.
   * **Ir para Julgamento:** Leva você para o tribunal.
   * **Sair do Jogo:** Fecha o jogo liberando a memória com segurança.
4. **O Veredito:** No julgamento, acuse um dos 4 suspeitos (*Mordomo, Secretária, Jardineiro ou Babá*). Se você tiver coletado pelo menos 2 pistas corretas ligadas a ele na Tabela Hash, você vence o caso!

---

## 🛠️ Pré-requisitos e Configuração do Ambiente

O projeto foi atualizado para ambientes modernos de **64-bits**.

### Requisitos:
* **Compilador:** GCC moderno (Recomendado: [w64devkit](https://github.com/skeeto/w64devkit/releases)) instalado em `C:\w64devkit`.
* **Biblioteca:** [Raylib 5.0 (MinGW-w64 64-bits)](https://www.raylib.com/).
* **Editor:** Visual Studio Code com a extensão **C/C++** da Microsoft instalada.

### Estrutura de Pastas Recomendada:
```text
Detetive_Quest/
│
├── .vscode/
│   └── tasks.json      # Configuração de build do VS Code
├── include/
│   └── raylib.h        # Header da Raylib
├── lib/
│   └── libraylib.a     # Binário da Raylib (64-bits)
└── main.c              # Código fonte do jogo