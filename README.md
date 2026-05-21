# 🃏 Jogo-da-Memoria-C

Jogo da Memória no terminal em C com matriz dinâmica 2D, embaralhamento Fisher-Yates, placar de tentativas e arquivo de recordes. Tabuleiro totalmente configurável pelo jogador.

---

## 🎮 Como Jogar

1. Escolha as dimensões do tabuleiro (ex: `4x4`)
2. Todas as cartas começam ocultas `[ ]`
3. Em cada rodada, escolha **duas posições** (linha e coluna)
4. Se os símbolos forem iguais → **par encontrado!** `[A]`
5. Se forem diferentes → as cartas voltam a ficar ocultas
6. Vence quem encontrar **todos os pares** com menos tentativas

---

## 🧠 Conceitos Praticados

- **Matriz 2D alocada dinamicamente** (`Celula **grid` — ponteiro para ponteiro)
- **Structs** com campos `simbolo` e `revelada`
- **Embaralhamento Fisher-Yates** usando array de ponteiros
- **Funções modulares** com ponteiros como parâmetro
- **Arquivo texto** para salvar e carregar ranking de recordes
- **Loops aninhados** para percorrer e exibir a matriz
- **Leitura e validação** de entrada do usuário

---

## 🚀 Como Compilar e Rodar

### Usando o Makefile

```bash
make
./jogo_memoria
```

### Manualmente com GCC

```bash
gcc -Wall -o jogo_memoria src/jogo_memoria.c
./jogo_memoria
```

### Limpar binários

```bash
make clean
```

---

## 📁 Estrutura do Projeto

```
Jogo-da-Memoria-C/
├── src/
│   └── jogo_memoria.c    # Código-fonte principal
├── Makefile              # Compilação automatizada
├── .gitignore            # Ignora binários e recordes gerados
└── README.md             # Documentação do projeto
```

---

## 🏆 Sistema de Recordes

Ao final de cada partida, o resultado é salvo em `recordes.txt`.
O jogo mantém automaticamente o **Top 5** de melhores resultados,
ordenados pelo **menor número de tentativas**.

---

## 📐 Tamanhos Sugeridos

| Dificuldade | Tabuleiro | Pares |
|---|---|---|
| Fácil | 2 × 2 | 2 |
| Médio | 2 × 4 | 4 |
| Difícil | 4 × 4 | 8 |
| Extremo | 4 × 6 | 12 |

---

## 🛠️ Tecnologias

![C](https://img.shields.io/badge/C-00599C?style=for-the-badge&logo=c&logoColor=white)
![GCC](https://img.shields.io/badge/GCC-333333?style=for-the-badge&logo=gnu&logoColor=white)

---

## 👤 Autor

**IgorASB** — [github.com/IgorASB](https://github.com/IgorASB)
