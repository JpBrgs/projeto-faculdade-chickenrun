CC = gcc
CFLAGS = -Wall -Wextra -std=c99

GLUT_DIR = C:/Users/João Borges Laptop/Downloads/glut-3.7.6-bin/glut-3.7.6-bin

ifeq ($(OS),Windows_NT)
TARGET = chicken_run.exe
CFLAGS += -I"$(GLUT_DIR)"
LIBS = -L"$(GLUT_DIR)" -lglut32 -lglu32 -lopengl32 -lOpenAL32 -lm
else
TARGET = chicken_run
LIBS = -lglut -lGLU -lGL -lopenal -lm
endif

SRCS = main.c jogo.c frango.c obstaculo.c cenario.c \
       colisao.c efeitos.c audio.c hud.c menu.c captura_tela.c

$(TARGET): $(SRCS)
	$(CC) $(CFLAGS) -o $(TARGET) $(SRCS) $(LIBS)

clean:
	$(RM) $(TARGET)
