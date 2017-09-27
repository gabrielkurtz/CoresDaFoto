#include <stdio.h>
#include <stdlib.h>
#include <string.h>		// Para usar strings
#include <math.h>

#ifdef WIN32
#include <windows.h>    // Apenas para Windows
#endif

#ifdef __APPLE__
#include <OpenGL/gl.h>
#include <OpenGL/glu.h>
#include <GLUT/glut.h>
#else
#include <GL/gl.h>     // Funções da OpenGL
#include <GL/glu.h>    // Funções da GLU
#include <GL/glut.h>   // Funções da FreeGLUT
#endif

// SOIL é a biblioteca para leitura das imagens
#include "SOIL.h"

// Um pixel RGB (24 bits)
typedef struct {
    unsigned char r, g, b;
} RGB;

// Uma imagem RGB
typedef struct {
    int width, height;
    RGB* img;
} Img;

// Protótipos
void load(char* name, Img* pic);
void valida();

// Funções da interface gráfica e OpenGL
void init();
void draw();
void keyboard(unsigned char key, int x, int y);

// Largura e altura da janela
int width, height;

// Identificadores de textura
GLuint tex[3];

// As 3 imagens
Img pic[3];

// Imagem selecionada (0,1,2)
int sel;

// Carrega uma imagem para a struct Img
void load(char* name, Img* pic)
{
    int chan;
    pic->img = (RGB*) SOIL_load_image(name, &pic->width, &pic->height, &chan, SOIL_LOAD_RGB);
    if(!pic->img)
    {
        printf( "SOIL loading error: '%s'\n", SOIL_last_result() );
        exit(1);
    }
    printf("Load: %d x %d x %d\n", pic->width, pic->height, chan);
}

int main(int argc, char** argv)
{
    if(argc < 2) {
        printf("transition [origem] [destino]\n");
        printf("Origem e' a fonte das cores, destino e' a imagem desejada\n");
        exit(1);
    }
	glutInit(&argc,argv);

	// Define do modo de operacao da GLUT
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);

	// pic[0] -> imagem com as cores
	// pic[1] -> imagem desejada
	// pic[2] -> resultado do algoritmo

	// Carrega as duas imagens
    load(argv[1], &pic[0]);
    load(argv[2], &pic[1]);

    // A largura e altura da janela são calculadas de acordo com a maior
    // dimensão de cada imagem
    width = pic[0].width > pic[1].width ? pic[0].width : pic[1].width;
    height = pic[0].height > pic[1].height ? pic[0].height : pic[1].height;

    // A largura e altura da imagem de saída são iguais às da imagem desejada (1)
    pic[2].width  = pic[1].width;
    pic[2].height = pic[1].height;

	// Especifica o tamanho inicial em pixels da janela GLUT
	glutInitWindowSize(width, height);

	// Cria a janela passando como argumento o titulo da mesma
	glutCreateWindow("Quebra-Cabeca digital");

	// Registra a funcao callback de redesenho da janela de visualizacao
	glutDisplayFunc(draw);

	// Registra a funcao callback para tratamento das teclas ASCII
	glutKeyboardFunc (keyboard);

	// Cria texturas em memória a partir dos pixels das imagens
    tex[0] = SOIL_create_OGL_texture((unsigned char*) pic[0].img, pic[0].width, pic[0].height, SOIL_LOAD_RGB, SOIL_CREATE_NEW_ID, 0);
    tex[1] = SOIL_create_OGL_texture((unsigned char*) pic[1].img, pic[1].width, pic[1].height, SOIL_LOAD_RGB, SOIL_CREATE_NEW_ID, 0);

    // Exibe as dimensões na tela, para conferência
    printf("Origem  : %s %d x %d\n", argv[1], pic[0].width, pic[0].height);
    printf("Destino : %s %d x %d\n", argv[2], pic[1].width, pic[0].height);
    sel = 0; // pic1

	// Define a janela de visualizacao 2D
	glMatrixMode(GL_PROJECTION);
	gluOrtho2D(0.0,width,height,0.0);
	glMatrixMode(GL_MODELVIEW);

	// Aplica o algoritmo e gera a saida em pic[2].img...
	// ...
	// ...
// Comente a linha abaixo para testar o seu algoritmo
// (Caso contrário, o programa tenta ler o resultado como um TERCEIRO parâmetro)
//#define DEMO
#ifdef DEMO
	load(argv[3], &pic[2]);
#else
    // Para valer, só aloca memória para a imagem de saída
	pic[2].img = malloc(pic[1].width * pic[1].height * 3); // W x H x 3 bytes (RGB)


///----------É aqui que faz o trabalho:

    int j=0;
    for(int i=0; i<pic[1].width * pic[1].height; i++) {
        j = pxProximo(i);
        pic[2].img[i].r = pic[0].img[j].r;
        pic[2].img[i].g = pic[0].img[j].g;
        pic[2].img[i].b = pic[0].img[j].b;

    }

#endif // DEMO

    // Cria textura para a imagem de saída
	tex[2] = SOIL_create_OGL_texture((unsigned char*) pic[2].img, pic[2].width, pic[2].height, SOIL_LOAD_RGB, SOIL_CREATE_NEW_ID, 0);

	// Entra no loop de eventos, não retorna
    glutMainLoop();
}

int pxProximo(int i) {
    double dist = 0;
    for(int j=0; j < pic[0].width * pic[0].height; j++) {
        if(pic[0].img[j].r != 0 && pic[0].img[j].g != 0 && pic[0].img[j].b != 0){
            dist =  sqrt(pow(pic[1].img[i].r - pic[0].img[j].r, 2) +
                         pow(pic[1].img[i].g - pic[0].img[j].g, 2) +
                         pow(pic[1].img[i].b - pic[0].img[j].b, 2)
                    );
            if(dist < 60) {
                return j;
            }
        }
    }
}

/// --------------- Aqui termina nosso código.


// Funcao de comparacao para qsort: ordena por R, G, B (desempate nessa ordem)
int cmp(const void* elem1, const void* elem2)
{
    RGB* ptr1 = (RGB*) elem1;
    RGB* ptr2 = (RGB*) elem2;
    unsigned char r1 = ptr1->r;
    unsigned char r2 = ptr2->r;
    unsigned char g1 = ptr1->g;
    unsigned char g2 = ptr2->g;
    unsigned char b1 = ptr1->b;
    unsigned char b2 = ptr2->b;
    int r = 0;
    if(r1 < r2) r = -1;
    else if(r1 > r2) r = 1;
    else if(g1 < g2) r = -1;
    else if(g1 > g2) r = 1;
    else if(b1 < b2) r = -1;
    else if(b1 > b2) r = 1;
    return r;
}

// Verifica se o algoritmo foi aplicado corretamente:
// Ordena os pixels da imagem origem e de saída por R, G e B;
// depois compara uma com a outra: devem ser iguais
void valida()
{
    int ok = 1;
    int size = width * height;
    // Aloca memória para os dois arrays
    RGB* aux1 = malloc(size*3);
    RGB* aux2 = malloc(size*3);
    // Copia os pixels originais
    memcpy(aux1, pic[0].img, size*3);
    memcpy(aux2, pic[2].img, size*3);
    // Mostra primeiros 8 pixels de ambas as imagens
    // antes de ordenar (teste)
    for(int i=0; i<8; i++)
        printf("[%02X %02X %02X] ", aux1[i].r, aux1[i].g, aux1[i].b);
    printf("\n");
    for(int i=0; i<8; i++)
        printf("[%02X %02X %02X] ", aux2[i].r, aux2[i].g, aux2[i].b);
    printf("\n");
    printf("Validando...\n");
    // Ordena ambos os arrays
    qsort(aux1, size, sizeof(RGB), cmp);
    qsort(aux2, size, sizeof(RGB), cmp);
    // Mostra primeiros 8 pixels de ambas as imagens
    // depois de ordenar
    for(int i=0; i<8; i++)
        printf("[%02X %02X %02X] ", aux1[i].r, aux1[i].g, aux1[i].b);
    printf("\n");
    for(int i=0; i<8; i++)
        printf("[%02X %02X %02X] ", aux2[i].r, aux2[i].g, aux2[i].b);
    printf("\n");
    for(int i=0; i<size; i++) {
        if(aux1[i].r != aux2[i].r ||
           aux1[i].g != aux2[i].g ||
           aux1[i].b != aux2[i].b) {
            // Se pelo menos um dos pixels for diferente, o algoritmo foi aplicado incorretamente
            printf("*** INVALIDO na posicao %d ***: %02X %02X %02X -> %02X %02X %02X\n",
                    i, aux1[i].r, aux1[i].g, aux1[i].b, aux2[i].r, aux2[i].g, aux2[i].b);
            ok = 0;
            break;
        }
    }
    // Libera memória dos arrays ordenados
    free(aux1);
    free(aux2);
    if(ok)
        printf(">>>> TRANSFORMACAO VALIDA <<<<<\n");
}

// Gerencia eventos de teclado
void keyboard(unsigned char key, int x, int y)
{
    if(key==27) {
      // ESC: libera memória e finaliza
      free(pic[0].img);
      free(pic[1].img);
      free(pic[2].img);
      exit(1);
    }
    if(key >= '1' && key <= '3')
        // 1-3: seleciona a imagem correspondente (origem, destino e resultado)
        sel = key - '1';
    // V para validar a solução
    if(key == 'v')
        valida();
    glutPostRedisplay();
}

// Callback de redesenho da tela
void draw()
{
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);  // Preto
    glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

    // Para outras cores, veja exemplos em /etc/X11/rgb.txt

    glColor3ub(255, 255, 255);  // branco

    // Ativa a textura corresponde à imagem desejada
    glBindTexture(GL_TEXTURE_2D, tex[sel]);
    // E desenha um retângulo que ocupa toda a tela
    glEnable(GL_TEXTURE_2D);
    glBegin(GL_QUADS);

    glTexCoord2f(0,0);
    glVertex2f(0,0);

    glTexCoord2f(1,0);
    glVertex2f(pic[sel].width,0);

    glTexCoord2f(1,1);
    glVertex2f(pic[sel].width, pic[sel].height);

    glTexCoord2f(0,1);
    glVertex2f(0,pic[sel].height);

    glEnd();
    glDisable(GL_TEXTURE_2D);

    // Exibe a imagem
    glutSwapBuffers();
}
