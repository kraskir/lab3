#include <windows.h>
#include <gl/gl.h>
#include <string.h>
#define STB_IMAGE_IMPLEMENTATION
#include "stb-master/stb_easy_font.h"
#include "stb-master/stb_image.h"



LRESULT CALLBACK WindowProc(HWND, UINT, WPARAM, LPARAM);
void EnableOpenGL(HWND hwnd, HDC*, HGLRC*);
void DisableOpenGL(HWND, HDC, HGLRC);

int width, height;
int currentFrame = 0;
int frameCount = 8;
int frameDelay = 150;
float speed = 0.3f;
float gravity = 0.2f;
BOOL gameStarted = FALSE;

unsigned int textureST, textureJP, textureMVR, textureMVL;


typedef struct {
    float x, y, dx, dy;
} Hero;


Hero hero = {0, 0, 0, 0};



enum ActionType { IDLE, WALK_LEFT, WALK_RIGHT, JUMP };
enum ActionType currentAction = IDLE;

void HandleInput(Hero *obj) {
    if (GetKeyState(VK_LEFT) & 0x8000) {
        obj->dx = -speed;
        currentAction = WALK_LEFT;
        currentFrame = (currentFrame + 1) % frameCount;
    } else if (GetKeyState(VK_RIGHT) & 0x8000) {
        obj->dx = speed;
        currentAction = WALK_RIGHT;
        currentFrame = (currentFrame + 1) % frameCount;
    } else {
        obj->dx = 0;
        currentAction = IDLE;
    }

    if (GetKeyState(VK_UP) & 0x8000 && obj->y == 0) {
        obj->dy = speed * 1.5;
        currentAction = JUMP;
        currentFrame = (currentFrame + 1) % frameCount;
}
}

void Hero_Move(Hero *obj) {

    obj->x += obj->dx;
    obj->y += obj->dy;


    if (obj->y > 0) {
        obj->dy -= gravity;
    } else {
        obj->y = 0;
        obj->dy = 0;
        }
}



float textCoordWalkRight[10][8];
float textCoordWalkLeft[10][8];
float textCoordStatic[10][8];
float textCoordJump[10][8];



void GenerateTextureCoords(int frameWidth, int frameHeight, int spriteSheetWidth, int spriteSheetHeight, float textCoordArray[10][8], int frameCount)
{
    float startX = 0.0f;
    float startY = 0.0f;
    float stepX = (float)frameWidth / spriteSheetWidth;
    float stepY = (float)frameHeight / spriteSheetHeight;

    for (int i = 0; i < frameCount; ++i)
    {
        // ¬ычисл€ем текстурные координаты дл€ текущего кадра анимации
        textCoordArray[i][0] = startX;
        textCoordArray[i][1] = startY + stepY;
        textCoordArray[i][2] = startX + stepX;
        textCoordArray[i][3] = startY + stepY;
        textCoordArray[i][4] = startX + stepX;
        textCoordArray[i][5] = startY;
        textCoordArray[i][6] = startX;
        textCoordArray[i][7] = startY;

        startX += stepX; // ѕереходим к следующему кадру анимации
    }
}

unsigned int textureBackground;

void Game_Init()
{

    int w0, h0, cnt0;
    unsigned char *data_background = stbi_load("fon.png", &w0, &h0, &cnt0, 0);
    glGenTextures(1, &textureBackground);
    glBindTexture(GL_TEXTURE_2D, textureBackground);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w0, h0, 0, cnt0 == 4 ? GL_RGBA : GL_RGB, GL_UNSIGNED_BYTE, data_background);
    glBindTexture(GL_TEXTURE_2D, 0);
    stbi_image_free(data_background);

    int w, h, cnt;
    unsigned char *data = stbi_load("Harry stoit.png", &w, &h, &cnt, 0);
    GenerateTextureCoords(100, 80, w, h, textCoordStatic, frameCount);
    glGenTextures(1, &textureST);
    glBindTexture(GL_TEXTURE_2D, textureST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, cnt == 4 ? GL_RGBA : GL_RGB, GL_UNSIGNED_BYTE, data);
    glBindTexture(GL_TEXTURE_2D, 0);
    stbi_image_free(data);

    int w1, h1, cnt1;
    unsigned char *data_walkR = stbi_load("Harry hodit.png", &w1, &h1, &cnt1, 0);
    GenerateTextureCoords(100, 80, w1, h1, textCoordWalkRight, frameCount);
    glGenTextures(1, &textureMVR);
    glBindTexture(GL_TEXTURE_2D, textureMVR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w1, h1, 0, cnt1 == 4 ? GL_RGBA : GL_RGB, GL_UNSIGNED_BYTE, data_walkR);
    glBindTexture(GL_TEXTURE_2D, 0);
    stbi_image_free(data_walkR);

    int w2, h2, cnt2;
    unsigned char *data_walkL = stbi_load("Harry hodit2.png", &w2, &h2, &cnt2, 0);
    GenerateTextureCoords(100, 80, w2, h2, textCoordWalkLeft, frameCount);
    glGenTextures(1, &textureMVL);
    glBindTexture(GL_TEXTURE_2D, textureMVL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w2, h2, 0, cnt2 == 4 ? GL_RGBA : GL_RGB, GL_UNSIGNED_BYTE, data_walkL);
    glBindTexture(GL_TEXTURE_2D, 0);
    stbi_image_free(data_walkL);

    int w3, h3, cnt3;
    unsigned char *data_jump = stbi_load("Harry prignul.png", &w3, &h3, &cnt3, 0);
    GenerateTextureCoords(100, 80, w3, h3, textCoordJump, frameCount);
    glGenTextures(1, &textureJP);
    glBindTexture(GL_TEXTURE_2D, textureJP);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w3, h3, 0, cnt3 == 4 ? GL_RGBA : GL_RGB, GL_UNSIGNED_BYTE, data_jump);
    glBindTexture(GL_TEXTURE_2D, 0);
    stbi_image_free(data_jump);

}

void ShowBackground()
{
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, textureBackground);

    glBegin(GL_QUADS);
    glTexCoord2f(0.0f, 1.0f); glVertex2f(-2.0f, -2.0f);
    glTexCoord2f(1.0f, 1.0f); glVertex2f(2.0f, -2.0f);
    glTexCoord2f(1.0f, 0.0f); glVertex2f(2.0f, 2.0f);
    glTexCoord2f(0.0f, 0.0f); glVertex2f(-2.0f, 2.0f);
    glEnd();

    glBindTexture(GL_TEXTURE_2D, 0);
    glDisable(GL_TEXTURE_2D);
}

float vertex[] = {-0.3,-0.3, 0.3,-0.3, 0.3,0.3, -0.3,0.3};



void UpdateAnimation()
{
    currentFrame = (currentFrame + 1) % frameCount;
    Sleep(frameDelay);
}


void Game_Show()
{
    if (gameStarted) {
        glEnable(GL_TEXTURE_2D);
        glEnable(GL_ALPHA_TEST);
        glAlphaFunc(GL_GREATER, 0.5f);

        glColor3f(1, 1, 1);
        glPushMatrix();
        glEnableClientState(GL_VERTEX_ARRAY);
        glEnableClientState(GL_TEXTURE_COORD_ARRAY);


        switch (currentAction) {
            case WALK_RIGHT:
                glBindTexture(GL_TEXTURE_2D, textureMVR);
                glTexCoordPointer(2, GL_FLOAT, 0, textCoordWalkRight[currentFrame]);
                break;
            case WALK_LEFT:
                glBindTexture(GL_TEXTURE_2D, textureMVL);
                glTexCoordPointer(2, GL_FLOAT, 0, textCoordWalkLeft[currentFrame]);
                break;
            case JUMP:
                glBindTexture(GL_TEXTURE_2D, textureJP);
                glTexCoordPointer(2, GL_FLOAT, 0, textCoordJump[currentFrame]);
                break;
            default:
                glBindTexture(GL_TEXTURE_2D, textureST);
                glTexCoordPointer(2, GL_FLOAT, 0, textCoordStatic[currentFrame]);
        }


        float translatedVertex[8];
        for (int i = 0; i < 4; ++i) {
            translatedVertex[i * 2] = vertex[i * 2] + hero.x;
            translatedVertex[i * 2 + 1] = vertex[i * 2 + 1] + hero.y;
        }
        glVertexPointer(2, GL_FLOAT, 0, translatedVertex);
        glDrawArrays(GL_TRIANGLE_FAN, 0 , 4);

        glDisableClientState(GL_TEXTURE_COORD_ARRAY);
        glDisableClientState(GL_VERTEX_ARRAY);

        glDisable(GL_ALPHA_TEST);
        glPopMatrix();
    }
}

typedef struct {
    char name[20];
    float vert[8];
    BOOL hover;
}TButton;

TButton btn[] = {
    {"start", {10,10, 110,10, 110,40, 10,40}, FALSE},
    {"stop", {10,50, 110,50, 110,80, 10,80}, FALSE},
    {"quit", {10,90, 110,90, 110,120, 10,120}, FALSE}

};
int btnCnt = sizeof(btn) / sizeof(btn[0]);

void print_string(float x, float y, char *text, float r, float g, float b)
{
  static char buffer[99999]; // ~500 chars
  int num_quads;

  num_quads = stb_easy_font_print(x, y, text, NULL, buffer, sizeof(buffer));

  glColor3f(r,g,b);
  glEnableClientState(GL_VERTEX_ARRAY);
  glVertexPointer(2, GL_FLOAT, 16, buffer);
  glDrawArrays(GL_QUADS, 0, num_quads*4);
  glDisableClientState(GL_VERTEX_ARRAY);
}


void TButton_Show(TButton btn)
{
    glEnableClientState(GL_VERTEX_ARRAY);
        if (btn.hover) glColor3f(0, 1, 0);
        else glColor3f(0, 0.8, 1);
        glVertexPointer(2, GL_FLOAT, 0, btn.vert);
        glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
    glDisableClientState(GL_VERTEX_ARRAY);

    glColor3f(0,0,0);
    glPushMatrix();
        glTranslatef(btn.vert[0], btn.vert[1], 0);
        glScalef(2,2,2);
        print_string(3,3, btn.name, 0, 0, 0);
    glPopMatrix();
}

BOOL PointInButton(int x, int y, TButton btn)
{
    return (x>btn.vert[0]) && (x<btn.vert[4]) &&
    (y>btn.vert[1]) && (y<btn.vert[5]);
}

void Quad(float x, float y, float dx, float dy)
{
    glBegin(GL_TRIANGLE_FAN);
        glVertex2f(x,       y);
        glVertex2f(x+dx,    y);
        glVertex2f(x+dx, y+dy);
        glVertex2f(x,    y+dy);
    glEnd();
}


void ShowMenu()
{

    glPushMatrix();
        glLoadIdentity();
        glOrtho(0, width, height, 0, -1,1);
        for(int i = 0; i < btnCnt; i++)
            TButton_Show(btn[i]);

    glPopMatrix();

}

int WINAPI WinMain(HINSTANCE hInstance,
                   HINSTANCE hPrevInstance,
                   LPSTR lpCmdLine,
                   int nCmdShow)
{
    WNDCLASSEX wcex;
    HWND hwnd;
    HDC hDC;
    HGLRC hRC;
    MSG msg;
    BOOL bQuit = FALSE;
    float theta = 0.0f;

    /* register window class */
    wcex.cbSize = sizeof(WNDCLASSEX);
    wcex.style = CS_OWNDC;
    wcex.lpfnWndProc = WindowProc;
    wcex.cbClsExtra = 0;
    wcex.cbWndExtra = 0;
    wcex.hInstance = hInstance;
    wcex.hIcon = LoadIcon(NULL, IDI_APPLICATION);
    wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
    wcex.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
    wcex.lpszMenuName = NULL;
    wcex.lpszClassName = "GLSample";
    wcex.hIconSm = LoadIcon(NULL, IDI_APPLICATION);;


    if (!RegisterClassEx(&wcex))
        return 0;

    /* create main window */
    hwnd = CreateWindowEx(0,
                          "GLSample",
                          "OpenGL Sample",
                          WS_OVERLAPPEDWINDOW,
                          CW_USEDEFAULT,
                          CW_USEDEFAULT,
                          700,
                          700,
                          NULL,
                          NULL,
                          hInstance,
                          NULL);

    ShowWindow(hwnd, nCmdShow);

    /* enable OpenGL for the window */
    EnableOpenGL(hwnd, &hDC, &hRC);



    Game_Init();

    /* program main loop */
    while (!bQuit)
    {
        /* check for messages */
        if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
        {
            /* handle or dispatch messages */
            if (msg.message == WM_QUIT)
            {
                bQuit = TRUE;
            }
            else
            {
                TranslateMessage(&msg);
                DispatchMessage(&msg);
            }
        }
        else
        {


            if(gameStarted){
                HandleInput(&hero);
                Hero_Move(&hero);
            }

            glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
            glClear(GL_COLOR_BUFFER_BIT);

            ShowBackground();

            ShowMenu();

            UpdateAnimation();

            Game_Show();

            SwapBuffers(hDC);

            theta += 1.0f;
            Sleep (1);
        }
    }

    /* shutdown OpenGL */
    DisableOpenGL(hwnd, hDC, hRC);

    /* destroy the window explicitly */
    DestroyWindow(hwnd);

    return msg.wParam;
}

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch (uMsg)
    {
        case WM_CLOSE:
            PostQuitMessage(0);
        break;

        case WM_LBUTTONDOWN:
            for(int i = 0; i < btnCnt; i++)
                if (PointInButton(LOWORD(lParam), HIWORD(lParam), btn[i]))
                {
                    printf("%s\n", btn[i].name);
                    if(strcmp(btn[i].name, "quit") == 0)
                        PostQuitMessage(0);
                    else if (strcmp(btn[i].name, "start") == 0) {
                        gameStarted = TRUE;
                    } else if (strcmp(btn[i].name, "stop") == 0) {
                        gameStarted = FALSE;
                    }

                }


        break;

        case WM_MOUSEMOVE:
            for(int i = 0; i < btnCnt; i++)
                btn[i].hover = (PointInButton(LOWORD(lParam), HIWORD(lParam), btn[i]));

        break;


        case WM_SIZE:
            width = LOWORD(lParam);
            height = HIWORD(lParam);
            glViewport(0, 0, width, height);
            glLoadIdentity();
            float k = width/(float)height;
            glOrtho(-k, k, -1,1, -1,1);
        break;



        case WM_DESTROY:
            return 0;

        case WM_KEYDOWN:
        {

            switch (wParam)
            {
                case VK_ESCAPE:
                    PostQuitMessage(0);
                break;
            }
        }
        break;

        default:
            return DefWindowProc(hwnd, uMsg, wParam, lParam);
    }

    return 0;
}

void EnableOpenGL(HWND hwnd, HDC* hDC, HGLRC* hRC)
{
    PIXELFORMATDESCRIPTOR pfd;

    int iFormat;

    /* get the device context (DC) */
    *hDC = GetDC(hwnd);

    /* set the pixel format for the DC */
    ZeroMemory(&pfd, sizeof(pfd));

    pfd.nSize = sizeof(pfd);
    pfd.nVersion = 1;
    pfd.dwFlags = PFD_DRAW_TO_WINDOW |
                  PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
    pfd.iPixelType = PFD_TYPE_RGBA;
    pfd.cColorBits = 32;
    pfd.cDepthBits = 16;
    pfd.iLayerType = PFD_MAIN_PLANE;

    iFormat = ChoosePixelFormat(*hDC, &pfd);

    SetPixelFormat(*hDC, iFormat, &pfd);

    /* create and enable the render context (RC) */
    *hRC = wglCreateContext(*hDC);

    wglMakeCurrent(*hDC, *hRC);
}

void DisableOpenGL (HWND hwnd, HDC hDC, HGLRC hRC)
{
    wglMakeCurrent(NULL, NULL);
    wglDeleteContext(hRC);
    ReleaseDC(hwnd, hDC);
}

