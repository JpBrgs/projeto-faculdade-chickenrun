#include "menu.h"
#include "jogo.h"
#include "hud.h"
#include <GL/glut.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>

static BotaoMenu botaoIniciar = {0.0f, 0.40f, 0.40f, 0.12f, "", 0};
static BotaoMenu botaoSair = {0.0f, 0.23f, 0.58f, 0.14f, "SAIR", 0};
static BotaoMenu botaoContinuar = {0.0f, 0.42f, 0.62f, 0.14f, "CONTINUAR", 0};
static BotaoMenu botaoMenu = {0.0f, 0.23f, 0.58f, 0.14f, "MENU", 0};
static BotaoMenu botaoReiniciar = {0.0f, 0.30f, 0.62f, 0.14f, "REINICIAR", 0};
static BotaoMenu botaoMenuFinal = {0.0f, 0.11f, 0.58f, 0.14f, "MENU", 0};

typedef struct {
    const char *arquivo;
    GLuint textura;
    int largura;
    int altura;
    int carregada;
    int tentouCarregar;
} TelaRaw;

static TelaRaw telaInicial = {"tela_inicial.raw", 0, 0, 0, 0, 0};
static TelaRaw telaVitoria = {"tela_vitoria.raw", 0, 0, 0, 0, 0};
static TelaRaw telaDerrota = {"tela_derrota.raw", 0, 0, 0, 0, 0};

static void desenharRetanguloMenu(float x, float y, float largura, float altura)
{
    glBegin(GL_QUADS);
    glVertex2f(x, y);
    glVertex2f(x + largura, y);
    glVertex2f(x + largura, y + altura);
    glVertex2f(x, y + altura);
    glEnd();
}

static void desenharTextoGrandeCentralizado(float x, float y, const char *texto)
{
    float ajuste = (float)strlen(texto) * 0.011f;
    desenharTextoGrande(x - ajuste, y, texto);
}

static int carregarTelaRaw(TelaRaw *tela)
{
    FILE *arquivo;
    unsigned char *arquivoBytes;
    unsigned char *pixels;
    long tamanhoArquivo;
    unsigned int largura = 0;
    unsigned int altura = 0;
    int temCabecalho = 0;
    int bytesPorPixel = 4;
    size_t totalPixels;
    size_t totalEntrada;
    size_t i;

    if (tela->tentouCarregar) {
        return tela->carregada;
    }
    tela->tentouCarregar = 1;

    arquivo = fopen(tela->arquivo, "rb");
    if (arquivo == NULL) {
        fprintf(stderr, "Erro abrindo arquivo de tela '%s': %s\n", tela->arquivo, strerror(errno));
        return 0;
    }

    fseek(arquivo, 0, SEEK_END);
    tamanhoArquivo = ftell(arquivo);
    rewind(arquivo);

    if (tamanhoArquivo <= 0) {
        fclose(arquivo);
        return 0;
    }

    arquivoBytes = (unsigned char *)malloc((size_t)tamanhoArquivo);
    if (arquivoBytes == NULL) {
        fclose(arquivo);
        return 0;
    }

    if (fread(arquivoBytes, 1, (size_t)tamanhoArquivo, arquivo) !=
        (size_t)tamanhoArquivo) {
        free(arquivoBytes);
        fclose(arquivo);
        return 0;
    }
    fclose(arquivo);

    if (tamanhoArquivo >= 8) {
        largura = (unsigned int)arquivoBytes[0] |
                  ((unsigned int)arquivoBytes[1] << 8) |
                  ((unsigned int)arquivoBytes[2] << 16) |
                  ((unsigned int)arquivoBytes[3] << 24);
        altura = (unsigned int)arquivoBytes[4] |
                 ((unsigned int)arquivoBytes[5] << 8) |
                 ((unsigned int)arquivoBytes[6] << 16) |
                 ((unsigned int)arquivoBytes[7] << 24);
        totalEntrada = (size_t)largura * (size_t)altura * 4u;
        if (largura > 0 && altura > 0 && largura <= 4096 && altura <= 4096 &&
            (size_t)tamanhoArquivo == totalEntrada + 8u) {
            temCabecalho = 1;
        }
    }

    if (!temCabecalho) {
        if ((size_t)tamanhoArquivo == 1024u * 512u * 4u) {
            largura = 1024;
            altura = 512;
            bytesPorPixel = 4;
        } else if ((size_t)tamanhoArquivo == 1024u * 512u * 3u) {
            largura = 1024;
            altura = 512;
            bytesPorPixel = 3;
        } else if ((size_t)tamanhoArquivo == 512u * 512u * 4u) {
            largura = 512;
            altura = 512;
            bytesPorPixel = 4;
        } else if ((size_t)tamanhoArquivo == 512u * 512u * 3u) {
            largura = 512;
            altura = 512;
            bytesPorPixel = 3;
        } else if ((size_t)tamanhoArquivo == 2048u * 1024u * 4u) {
            largura = 2048;
            altura = 1024;
            bytesPorPixel = 4;
        } else if ((size_t)tamanhoArquivo == 2048u * 1024u * 3u) {
            largura = 2048;
            altura = 1024;
            bytesPorPixel = 3;
        } else {
            fprintf(stderr, "Formato raw nao reconhecido em '%s' (%ld bytes)\n",
                    tela->arquivo, tamanhoArquivo);
            free(arquivoBytes);
            return 0;
        }
    }

    totalPixels = (size_t)largura * (size_t)altura * 4u;
    pixels = (unsigned char *)malloc(totalPixels);
    if (pixels == NULL) {
        free(arquivoBytes);
        return 0;
    }

    if (bytesPorPixel == 4) {
        memcpy(pixels, arquivoBytes + (temCabecalho ? 8 : 0), totalPixels);
    } else {
        const unsigned char *entrada = arquivoBytes + (temCabecalho ? 8 : 0);
        for (i = 0; i < (size_t)largura * (size_t)altura; i++) {
            pixels[i * 4u] = entrada[i * 3u];
            pixels[i * 4u + 1u] = entrada[i * 3u + 1u];
            pixels[i * 4u + 2u] = entrada[i * 3u + 2u];
            pixels[i * 4u + 3u] = 255;
        }
    }
    free(arquivoBytes);

    glGenTextures(1, &tela->textura);
    glBindTexture(GL_TEXTURE_2D, tela->textura);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    gluBuild2DMipmaps(GL_TEXTURE_2D, GL_RGBA, (GLsizei)largura,
                      (GLsizei)altura, GL_RGBA, GL_UNSIGNED_BYTE, pixels);
    glBindTexture(GL_TEXTURE_2D, 0);

    free(pixels);
    tela->largura = (int)largura;
    tela->altura = (int)altura;
    tela->carregada = 1;
    return 1;
}

static int desenharTelaRaw(TelaRaw *tela)
{
    if (!carregarTelaRaw(tela)) {
        return 0;
    }

    glColor3f(1.0f, 1.0f, 1.0f);
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, tela->textura);
    glBegin(GL_QUADS);
    glTexCoord2f(0.0f, 1.0f);
    glVertex2f(-1.0f, -0.15f);
    glTexCoord2f(1.0f, 1.0f);
    glVertex2f(1.0f, -0.15f);
    glTexCoord2f(1.0f, 0.0f);
    glVertex2f(1.0f, 1.15f);
    glTexCoord2f(0.0f, 0.0f);
    glVertex2f(-1.0f, 1.15f);
    glEnd();
    glBindTexture(GL_TEXTURE_2D, 0);
    glDisable(GL_TEXTURE_2D);
    return 1;
}

static void desenharClassificacaoFinal(float x, float y)
{
    int total = obterTotalClassificacao();
    int i;
    char texto[64];

    glColor3f(0.05f, 0.05f, 0.05f);
    desenharTexto(x, y, "Historico de pontos");

    if (total == 0) {
        desenharTexto(x, y - 0.07f, "Sem pontos");
        return;
    }

    for (i = 0; i < total; i++) {
        snprintf(texto, sizeof(texto), "%d. %d pontos",
                 i + 1, obterPontuacaoClassificacao(i));
        desenharTexto(x, y - 0.07f - (float)i * 0.065f, texto);
    }
}

static void converterMouse(int mouseX, int mouseY, float *x, float *y)
{
    float largura = (float)jogo.larguraJanela;
    float altura = (float)jogo.alturaJanela;

    if (largura <= 0.0f || altura <= 0.0f) {
        *x = 0.0f;
        *y = 0.0f;
        return;
    }
    *x = -1.0f + 2.0f * ((float)mouseX / largura);
    *y = 1.15f - 1.30f * ((float)mouseY / altura);
}

static void desenharFundoMenu(void)
{
    glColor3f(0.06f, 0.18f, 0.25f);
    desenharRetanguloMenu(-1.0f, -0.15f, 2.0f, 1.30f);

    glColor3f(0.16f, 0.55f, 0.23f);
    desenharRetanguloMenu(-1.0f, -0.15f, 2.0f, 0.22f);

    glColor3f(0.16f, 0.16f, 0.17f);
    desenharRetanguloMenu(-1.0f, 0.07f, 2.0f, 0.13f);

    glColor3f(0.06f, 0.34f, 0.78f);
    desenharRetanguloMenu(-1.0f, 0.20f, 2.0f, 0.12f);

    glColor3f(0.95f, 0.92f, 0.30f);
    glBegin(GL_TRIANGLES);
    glVertex2f(-0.75f, 0.92f);
    glVertex2f(-0.67f, 0.92f);
    glVertex2f(-0.71f, 1.00f);
    glVertex2f(0.72f, 0.82f);
    glVertex2f(0.80f, 0.82f);
    glVertex2f(0.76f, 0.90f);
    glEnd();
}

static void desenharBotao(BotaoMenu *botao)
{
    float e = botao->x - botao->largura * 0.5f;
    float d = botao->x + botao->largura * 0.5f;
    float b = botao->y - botao->altura * 0.5f;
    float c = botao->y + botao->altura * 0.5f;

    if (botao->destacado) {
        glColor3f(0.98f, 0.76f, 0.22f);
    } else {
        glColor3f(0.22f, 0.52f, 0.42f);
    }

    glBegin(GL_QUADS);
    glVertex2f(e, b);
    glVertex2f(d, b);
    glVertex2f(d, c);
    glVertex2f(e, c);
    glEnd();

    glColor3f(0.04f, 0.12f, 0.14f);
    glLineWidth(2.0f);
    glBegin(GL_LINE_LOOP);
    glVertex2f(e, b);
    glVertex2f(d, b);
    glVertex2f(d, c);
    glVertex2f(e, c);
    glEnd();
    glLineWidth(1.0f);

    glColor3f(1.0f, 1.0f, 1.0f);
    desenharTextoGrandeCentralizado(botao->x, botao->y - 0.02f, botao->rotulo);
}

void desenharMenuInicial(void)
{
    if (!desenharTelaRaw(&telaInicial)) {
        desenharFundoMenu();
        glColor3f(1.0f, 0.94f, 0.20f);
        desenharTextoGrandeCentralizado(0.0f, 0.78f, "CHICKEN RUN");
        glColor3f(0.88f, 0.94f, 1.0f);
        desenharTexto(-0.47f, 0.66f, "Atravesse pistas e rios sem perder as vidas.");
    }
}

void desenharTelaPausa(void)
{
    glColor4f(0.0f, 0.0f, 0.0f, 0.55f);
    glBegin(GL_QUADS);
    glVertex2f(-1.0f, -0.15f);
    glVertex2f(1.0f, -0.15f);
    glVertex2f(1.0f, 1.15f);
    glVertex2f(-1.0f, 1.15f);
    glEnd();

    glColor3f(1.0f, 1.0f, 1.0f);
    desenharTextoGrande(-0.15f, 0.66f, "PAUSA");
    desenharBotao(&botaoContinuar);
    desenharBotao(&botaoMenu);
}

void desenharTelaVitoria(int pontuacao, int tempo)
{
    char texto[96];

    if (!desenharTelaRaw(&telaVitoria)) {
        desenharFundoMenu();
    }

    glColor3f(0.05f, 0.05f, 0.05f);
    snprintf(texto, sizeof(texto), "%d pontos", pontuacao);
    desenharTexto(-0.91f, 1.02f, texto);
    snprintf(texto, sizeof(texto), "%02d:%02d", (tempo / 1000) / 60,
             (tempo / 1000) % 60);
    desenharTexto(-0.91f, 0.895f, texto);
    desenharClassificacaoFinal(-0.22f, 0.66f);
}

void desenharTelaDerrota(int pontuacao, int tempo)
{
    char texto[64];

    if (!desenharTelaRaw(&telaDerrota)) {
        desenharFundoMenu();
    }

    glColor3f(0.05f, 0.05f, 0.05f);
    snprintf(texto, sizeof(texto), "%d pontos", pontuacao);
    desenharTexto(-0.91f, 1.02f, texto);
    snprintf(texto, sizeof(texto), "%02d:%02d", (tempo / 1000) / 60,
             (tempo / 1000) % 60);
    desenharTexto(-0.91f, 0.895f, texto);
    desenharClassificacaoFinal(-0.22f, 0.66f);
}

int verificarCliqueEmBotao(BotaoMenu *botao, int mouseX, int mouseY)
{
    float x;
    float y;

    converterMouse(mouseX, mouseY, &x, &y);
    return x >= botao->x - botao->largura * 0.5f &&
           x <= botao->x + botao->largura * 0.5f &&
           y >= botao->y - botao->altura * 0.5f &&
           y <= botao->y + botao->altura * 0.5f;
}

void processarCliqueMouse(int mouseX, int mouseY)
{
    if (jogo.estadoAtual == ESTADO_MENU) {
        if (verificarCliqueEmBotao(&botaoIniciar, mouseX, mouseY)) {
            jogo.estadoAtual = ESTADO_JOGANDO;
        } else if (verificarCliqueEmBotao(&botaoSair, mouseX, mouseY)) {
            exit(0);
        }
    } else if (jogo.estadoAtual == ESTADO_PAUSADO) {
        if (verificarCliqueEmBotao(&botaoContinuar, mouseX, mouseY)) {
            jogo.estadoAtual = ESTADO_JOGANDO;
        } else if (verificarCliqueEmBotao(&botaoMenu, mouseX, mouseY)) {
            jogo.estadoAtual = ESTADO_MENU;
        }
    } else if (jogo.estadoAtual == ESTADO_VITORIA ||
               jogo.estadoAtual == ESTADO_DERROTA) {
        if (verificarCliqueEmBotao(&botaoReiniciar, mouseX, mouseY)) {
            jogo.estadoAtual = ESTADO_JOGANDO;
        } else if (verificarCliqueEmBotao(&botaoMenuFinal, mouseX, mouseY)) {
            jogo.estadoAtual = ESTADO_MENU;
        }
    }
}

void atualizarDestaqueMenu(int mouseX, int mouseY)
{
    botaoIniciar.destacado = verificarCliqueEmBotao(&botaoIniciar, mouseX, mouseY);
    botaoSair.destacado = verificarCliqueEmBotao(&botaoSair, mouseX, mouseY);
    botaoContinuar.destacado = verificarCliqueEmBotao(&botaoContinuar, mouseX, mouseY);
    botaoMenu.destacado = verificarCliqueEmBotao(&botaoMenu, mouseX, mouseY);
    botaoReiniciar.destacado = verificarCliqueEmBotao(&botaoReiniciar, mouseX, mouseY);
    botaoMenuFinal.destacado = verificarCliqueEmBotao(&botaoMenuFinal, mouseX, mouseY);
}
