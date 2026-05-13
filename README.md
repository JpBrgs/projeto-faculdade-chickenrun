# Chicken Run

Jogo 2D feito em C com GLUT/OpenGL e OpenAL. O jogador controla uma galinha que precisa atravessar pistas e rios, desviando de carros e usando toras para chegar ate a meta.

## Como compilar

No Windows com MSYS2/UCRT64:

```powershell
cd caminho\para\projeto-chickenrun-main
$env:PATH="C:\msys64\ucrt64\bin;$env:PATH"
mingw32-make
.\chicken_run.exe
```

## Controles

- `W`, `A`, `S`, `D` ou setas: mover a galinha.
- `P`: pausar ou continuar.
- `R`: reiniciar.
- `ESC`: sair.
- Mouse: clicar nos botoes do menu, pausa, vitoria e derrota.

## Estrutura principal

- `main.c`: loop principal, estados, teclado, mouse, vitoria e derrota.
- `jogo.c`: estado geral, pontuacao, tempo e melhor pontuacao.
- `frango.c`: jogador e movimentacao.
- `cenario.c`: faixas de grama, pista, rio e chegada.
- `obstaculo.c`: carros e toras.
- `colisao.c`: colisao AABB e regras de rio/vitoria.
- `efeitos.c`: particulas, flash e trilha.
- `hud.c`: pontos, tempo, vidas e progresso.
- `menu.c`: menu inicial, pausa, tela de vitoria e tela de derrota.
- `audio.c`: sons WAV com OpenAL.

## Observacao sobre as telas

A tela inicial, a tela de vitoria e a tela de derrota foram feitas como uma solucao pratica para fechar o fluxo visual do jogo. Elas usam fundo simples, textos e botoes.

Os arquivos `.raw`, quando usados, servem apenas como imagem bruta de fundo. Eles nao controlam regra de jogo, nao calculam pontuacao e nao tratam cliques. A logica continua em `main.c`, `jogo.c` e `menu.c`.

A documentacao completa esta em `DOCUMENTACAO_CHICKEN_RUN.md`.
