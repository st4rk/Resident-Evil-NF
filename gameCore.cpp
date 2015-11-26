/***************************************************************************
** gameCore é utilizada para renderização, pegar todas as informações e   **
** gerencialas para que o jogo funcione de forma correta                  **
** Criado por Lucas P. Stark                                              **
** Nightmare Fiction Engine - NFE                                         **
***************************************************************************/

#include "gameCore.h"

// All global variable here is used to some importants functions on engine
// like which menu we are and bla bla

// It will handle all possibles menus, ie: "inGame, MainMenu, Credits [...]"
int gameState = 0;

// Grande maioria aqui é temporário para teste
// Mas alguns realmente vão ficar, acredito que a camera
// realmente vai ficar aqui por enquanto até eu achar um lugar melhor
// para colocar
float projectionScale = 90.0f;
unsigned int animCount = 0;
unsigned int animSelect = 0;
unsigned int camSelect = 0;


// Main Menu
unsigned char mainMenu = 0x0;
// In Game
unsigned char inGame   = 0x0;

// Variaveis utilizadas para fadeblack effect, to pensando em criar um
// engineEffect ou engineEvent ou algo do tipo, ou até mesmo um struct
// para não fica esse aglomerado aqui
float fadeBlackEffect = 1.0;
bool fadeBlackNormal  = false;
bool fadeBlackReverse = false;
bool inFadeBlack      = false;


// System Time, you can do a delay with it
int timer1_start = 0;
int delay_sys  = 0;

EMD modelList[MAX_MODEL];
RDT playerRDT;
PLD modelList_2[MAX_MODEL];
EMD_SEC2_DATA_T emdFrameAnimation;

// engineBackground é utilizado para carregar os backgrounds de cada .RDT
// que o player se encontra.
// engineFont é a fonte utilizad para desenhar textos, como so dedos
// de debug
// engineThisGame é a primeira imagem que aparece quando você abre o jogo
// dizendo que o jogo contem cenas bla bla e o mainMenu é o menu de start
bmp_loader_24bpp engineBackground;
bmp_loader_24bpp engineFont;
bmp_loader_24bpp engineThisGame;
bmp_loader_24bpp engineMainMenu;
bmp_loader_24bpp engineCreditos;

playerClass mainPlayer;
gameMath  mathEngine;
gameSound soundEngine;

gameCore::gameCore(int argc, char** argv) {
    // Inicializa a GLUT
    glutInit(&argc, argv);

    // Coloca o nome da Engine + Revisão
    memset(GAME_NAME, 0x0, 20);
    strcpy(GAME_NAME, "Nightmare Fiction Engine Rev 0.08a");

}

gameCore::~gameCore() {

}


// Tava pensando aqui 06/05/2015 as 01:50 da manhã, assim que fiz o teste
// e vi isso funcionando, o por que de tanta função espalhada nessa merda
// sinceramente eu quero organizar esse código, vou começar a criar uma
// parte toda estruturada só pra isso e deixar rolar.
void background_Loader(int bgNum) {
    char background[50];
    if (bgNum < 10)
        sprintf(background, "resource/stages/re1/ROOM10A00.bmp", bgNum);
    else
        sprintf(background, "resource/stages/re1/ROOM10A00.bmp", bgNum);

    engineBackground.bmpLoaderFile(background);
    mainPlayer.setPlayerCam(bgNum);
}


void gameCore::renderLoadResource() {

    // Clear Timer
    timer1_start = 0;


    // The game should start on Main Menu
    gameState       = STATE_MAIN_MENU;
    mainMenu        = MAIN_MENU_LOGO;
    fadeBlackNormal = true;

    // The game starts with fadeblack enable
    inFadeBlack = true;

    // RDT do Início do Player
    playerRDT.rdtLoadFile("background/ROOM1030.RDT");
    
    // Música que vai ser tocada no menu
    if (soundEngine.engineSoundInit()) {
        soundEngine.engineLoadSound("musicas/menu.ogg");
    }

    // Alguns gráficos que são carregados na memória
    modelList[0].emdLoadFile("modelos/EM058.EMD");
    modelList[1].emdLoadFile("modelos/EM050.EMD");
    modelList[2].emdLoadFile("modelos/BetaLeon.EMD");

    // Algumas imagens carregadas na memória
    engineThisGame.bmpLoaderFile("resource/intro_01.bmp");
    engineMainMenu.bmpLoaderFile("resource/intro_00.bmp");
    engineCreditos.bmpLoaderFile("creditos.bmp");

    // Carrega o background com número 5
    background_Loader(10);

    engineFont.bmpLoaderFile("fonte/1.bmp");

    // HardCode, modelo inicial, X,Y,Z e Número da câmera
    mainPlayer.setPlayerEMD(1);
    mainPlayer.setPlayerX(-17287);
    mainPlayer.setPlayerZ(-11394);
    mainPlayer.setPlayerY(0);
    mainPlayer.setPlayerCam(5);

    // Player Item HardCode
    mainPlayer.setPlayerItem(0, -1); // NO ITEM
    mainPlayer.setPlayerItem(1, -1); // NO ITEM
    mainPlayer.setPlayerItem(2, -1); // NO ITEM
    mainPlayer.setPlayerItem(3, -1); // NO ITEM
    mainPlayer.setPlayerItem(4, -1); // NO ITEM
    mainPlayer.setPlayerItem(5, -1); // NO ITEM
    mainPlayer.setPlayerItem(6, -1); // NO ITEM
    mainPlayer.setPlayerItem(7, -1); // NO ITEM


}



void eventSystem_downKey(int key, int x, int y) {
    switch (gameState) {
        case STATE_MAIN_MENU:

        break;

        case STATE_IN_GAME:
            // Todo o movimento do personagem(principal) será controlado por aqui
            // Andar, subir, pegar objetos etc.
            switch (key) {
                case GLUT_KEY_UP:
                if (mainPlayer.getPlayerRunning()) {
                    mainPlayer.setPlayerInMove(true);
                    mainPlayer.setPlayerAnimSection(1);
                    mainPlayer.setPlayerAnim(1);
                } else {
                    mainPlayer.setPlayerInMove(true);
                    mainPlayer.setPlayerAnimSection(1);
                    mainPlayer.setPlayerAnim(0);           
                }
                break;

                case GLUT_KEY_DOWN:

                break;

                case GLUT_KEY_LEFT:
                if (mainPlayer.getPlayerRunning()) {
                    mainPlayer.setPlayerInRotatePos(1);
                    mainPlayer.setPlayerInRotate(true);
                    mainPlayer.setPlayerAnimSection(1);
                    mainPlayer.setPlayerAnim(1);
                } else {
                    mainPlayer.setPlayerInRotatePos(1);
                    mainPlayer.setPlayerInRotate(true);
                    mainPlayer.setPlayerAnimSection(1);
                    mainPlayer.setPlayerAnim(0);
                }
                break;

                case GLUT_KEY_RIGHT:
                if (mainPlayer.getPlayerRunning()) {
                    mainPlayer.setPlayerInRotatePos(0);
                    mainPlayer.setPlayerInRotate(true);
                    mainPlayer.setPlayerAnimSection(1);
                    mainPlayer.setPlayerAnim(1);
                } else {
                    mainPlayer.setPlayerInRotatePos(0);
                    mainPlayer.setPlayerInRotate(true);
                    mainPlayer.setPlayerAnimSection(1);
                    mainPlayer.setPlayerAnim(0);
                }
                break;

            }
        break;


        default:

        break;
    }
}

void eventSystem_upKey(int key, int x, int y) {
    switch (gameState) {
        case STATE_MAIN_MENU:

        break;

        case STATE_IN_GAME:
           switch (key) {
                case GLUT_KEY_UP:
                    mainPlayer.setPlayerInMove(false);
                break;

                case GLUT_KEY_DOWN:

                break;

                case GLUT_KEY_LEFT:
                    mainPlayer.setPlayerInRotatePos(2); // 2 = DISABLE
                    mainPlayer.setPlayerInRotate(false);
                  
                break;

                case GLUT_KEY_RIGHT:
                    mainPlayer.setPlayerInRotatePos(2); // 2 = DISABLE
                    mainPlayer.setPlayerInRotate(false);
                break;

            }
        break;

        default:

        break;
    }


}

void eventSystem_keyboardDown(unsigned char key, int x, int y) {
    switch (gameState) {
        case STATE_MAIN_MENU:
            if (mainMenu == MAIN_MENU_START) {
                if (key == 'x') {
                    mainMenu    = MAIN_MENU_GAME;
                    inFadeBlack = true;
                    glutIgnoreKeyRepeat(1);    
                }
            }
        break;

        case STATE_IN_GAME:
            switch (key) {
                case 'x':
                    if (!mainPlayer.getPlayerRunning()) {
                        mainPlayer.setPlayerRunning(true);
                        mainPlayer.setPlayerAnimSection(1);
                        mainPlayer.setPlayerAnim(1);
                    }
                break;

                default:

                break;
            }
        break;

        default:

        break;
    }
}


void eventSystem_keyboardUp(unsigned char key, int x, int y) {
    switch (key) {
        case 'z':

        break;

        case 'x':
            if (mainPlayer.getPlayerRunning()) {
                mainPlayer.setPlayerRunning(false);     
                mainPlayer.setPlayerAnimSection(1);
                mainPlayer.setPlayerAnim(0);       
            }
        break;

        default:

        break;
    }
}


void MainLoop(int t) {
    float x = 0, z = 0;
    bool canMove = true;
    if (gameState == STATE_IN_GAME) {
        if (((mainPlayer.getPlayerInMove()) == false) && (mainPlayer.getPlayerInShoot() == false) && (mainPlayer.getPlayerInRotate() == false)) {
            mainPlayer.setPlayerAnimSection(1);
            mainPlayer.setPlayerAnim(2);
        }

        if (mainPlayer.getPlayerInShoot() == true) {
                mainPlayer.setPlayerAnim(10);
        } 

        if (mainPlayer.getPlayerInRotate() && (mainPlayer.getPlayerInRotatePos() == 1)) {
            projectionScale = fmod((projectionScale - 5), 360.0);
        }

        if (mainPlayer.getPlayerInRotate() && (mainPlayer.getPlayerInRotatePos() == 0)) {
            projectionScale = fmod((projectionScale + 5), 360.0);
        }

        if (((mainPlayer.getPlayerInMove() == true) && (mainPlayer.getPlayerInRotate() == false)) 
            || ((mainPlayer.getPlayerInMove() == true) && (mainPlayer.getPlayerInRotate() == true))) {
            if (mainPlayer.getPlayerRunning()) {
                x = cos((projectionScale * PI/180)) * 180.0;
                z = sin((projectionScale * PI/180)) * 180.0;
            } else {
                x = cos((projectionScale * PI/180)) * 80.0;
                z = sin((projectionScale * PI/180)) * 80.0;
            }
            for (unsigned int i = 0; i < (playerRDT.rdtColisionHeader.arraySum -1); i++) {
                if (mathEngine.collisionDetect(0, playerRDT.rdtColissionArray[i].X, playerRDT.rdtColissionArray[i].Z, 
                                         playerRDT.rdtColissionArray[i].W, playerRDT.rdtColissionArray[i].D, mainPlayer.getPlayerX() + x, mainPlayer.getPlayerZ()-z) == true) {
                    canMove = false;
                } 
            }
        
            for (unsigned int i = 0; i < playerRDT.rdtCameraSwitch.size(); i++) {
                if (mathEngine.mapSwitch(mainPlayer.getPlayerX(), mainPlayer.getPlayerZ(), playerRDT.rdtCameraSwitch[i].x1, playerRDT.rdtCameraSwitch[i].z1, playerRDT.rdtCameraSwitch[i].x2, playerRDT.rdtCameraSwitch[i].z2, 
                                         playerRDT.rdtCameraSwitch[i].x3, playerRDT.rdtCameraSwitch[i].z3, playerRDT.rdtCameraSwitch[i].x4, playerRDT.rdtCameraSwitch[i].z4)) {
                    
                    if (playerRDT.rdtCameraSwitch[i].cam1 != 0) //|| ((playerRDT.rdtCameraSwitch[i].cam1 == 0) && (playerRDT.rdtCameraSwitch[i].cam0 == 1)))
                            background_Loader(playerRDT.rdtCameraSwitch[i].cam1);

                
                } 
            }


            if (canMove == true) {
                mainPlayer.setPlayerX(mainPlayer.getPlayerX() + x);
                mainPlayer.setPlayerZ(mainPlayer.getPlayerZ() - z);
            }
        }



        if ((mainPlayer.getPlayerAnimSection()) == 0) {
            if (animCount < (modelList[mainPlayer.getPlayerEMD()].emdSec2AnimInfo[animSelect].animCount-1))
                animCount++;
            else {
                animCount = 0;
            }
            emdFrameAnimation = modelList[mainPlayer.getPlayerEMD()].emdSec2Data[animCount+modelList[mainPlayer.getPlayerEMD()].emdSec2AnimInfo[animSelect].animStart];
        } else {
            if (animCount < (modelList[mainPlayer.getPlayerEMD()].emdSec4AnimInfo[animSelect].animCount-1))
                animCount++;
            else {
                animCount = 0;
            }

            emdFrameAnimation = modelList[mainPlayer.getPlayerEMD()].emdSec4Data[animCount+modelList[mainPlayer.getPlayerEMD()].emdSec4AnimInfo[animSelect].animStart];
        }

        animSelect = mainPlayer.getPlayerAnim(); 
    }

    glutPostRedisplay();   
    glutTimerFunc(33, MainLoop, 0);
}



void gameCore::renderInit() {    
    GLuint textureTest;

    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);

    // Resolução da Tela
    glutInitWindowSize(RES_HEIGHT, RES_WIDTH);
    // Posição onde a tela vai inicializar
    glutInitWindowPosition(100, 100);
    // Título do Projeto
    glutCreateWindow(GAME_NAME);
    // Limpa a tela
    glClearColor(0.0, 1.0, 0.0, 1.0);

    // Luz para normal dos modelos
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glEnable(GL_LIGHT1);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_NORMALIZE);
    glEnable(GL_BLEND); 
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    // Permite utilização de textura 2D
    glEnable(GL_TEXTURE_2D);
    glActiveTexture(GL_TEXTURE0);
    glGenTextures(1, &textureTest);
    glBindTexture(GL_TEXTURE_2D, textureTest);
    glDepthMask(GL_TRUE);



    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(60.0, ((double)RES_HEIGHT/(double)RES_WIDTH), 1,  65535);
    glViewport(0,0, RES_WIDTH, RES_HEIGHT);
    /* Inicializa a matriz de Model View */
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();


    if (glGetError() != GL_NO_ERROR) {
        std::cout << "Houve um erro na inicialização do opengl!" << std::endl;
        exit(0);
    }

    // Função que vai ser chamada cada vez que for fazer a renderização

    glutDisplayFunc(renderScene);
    glutSpecialFunc( eventSystem_downKey );
    glutSpecialUpFunc ( eventSystem_upKey );
    glutKeyboardFunc( eventSystem_keyboardDown );
    glutKeyboardUpFunc( eventSystem_keyboardUp );

    glutTimerFunc(1.0, MainLoop, 0);

    // Evento de Loop
    glutMainLoop();

}

void gameCore::renderText(float x, float y, float z, std::string texto) {

}   

    
void modelRelPosAnimation(unsigned int objNum, unsigned int var, int var2) {
     if (var == objNum) {

        glTranslatef((float)modelList[mainPlayer.getPlayerEMD()].emdSec2RelPos[var2].x, (float)modelList[mainPlayer.getPlayerEMD()].emdSec2RelPos[var2].y, (float)modelList[mainPlayer.getPlayerEMD()].emdSec2RelPos[var2].z);
        glRotatef(emdFrameAnimation.vector[var2].x, 1.0f, 0.0f, 0.0f);
        glRotatef(emdFrameAnimation.vector[var2].y, 0.0f, 1.0f, 0.0f);
        glRotatef(emdFrameAnimation.vector[var2].z, 0.0f, 0.0f, 1.0f);

  }
     
     for (unsigned int c = 0; c < modelList[mainPlayer.getPlayerEMD()].emdSec2Armature[var].meshCount; c++) {
        modelRelPosAnimation(objNum, modelList[mainPlayer.getPlayerEMD()].emdSec2Mesh[var][c], var2);
     }
}



void drawMainPlayer() {
    float width_t  = (float) (modelList[mainPlayer.getPlayerEMD()].emdTimTexture.timTexture.imageWidth*2);
    float height_t = (float)  modelList[mainPlayer.getPlayerEMD()].emdTimTexture.timTexture.imageHeight;

    unsigned short uPage = 0;

    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
    glTexImage2D(GL_TEXTURE_2D, 0,GL_RGB, (modelList[mainPlayer.getPlayerEMD()].emdTimTexture.timTexture.imageWidth*2), modelList[mainPlayer.getPlayerEMD()].emdTimTexture.timTexture.imageHeight, 0,GL_RGB, GL_UNSIGNED_BYTE, modelList[mainPlayer.getPlayerEMD()].emdTimTexture.rawTexture);


    // Toda a renderização do personagem vai ficar aqui !
    for (unsigned int x = 0; x < modelList[mainPlayer.getPlayerEMD()].emdTotalObj; x++) {
        
        glLoadIdentity();


        
        // Aqui temos a camera do mapa que o personagem se encontra
        // o número de cameras é variado
        gluLookAt(     playerRDT.rdtCameraPos[mainPlayer.getPlayerCam()].positionX, playerRDT.rdtCameraPos[mainPlayer.getPlayerCam()].positionY, playerRDT.rdtCameraPos[mainPlayer.getPlayerCam()].positionZ,
                       playerRDT.rdtCameraPos[mainPlayer.getPlayerCam()].targetX, playerRDT.rdtCameraPos[mainPlayer.getPlayerCam()].targetY, playerRDT.rdtCameraPos[mainPlayer.getPlayerCam()].targetZ,   
                       0.0f,   -0.1f,   0.0f);
   
        glTranslatef((float)mainPlayer.getPlayerX(), mainPlayer.getPlayerY(), (float)mainPlayer.getPlayerZ());  

        
        glRotatef(projectionScale, 0.0f, 1.0f, 0.0f);



        for (unsigned int z = 0; z <  modelList[mainPlayer.getPlayerEMD()].emdTotalObj; z++) {
            modelRelPosAnimation(x, z, z);
        }

        for (unsigned int y = 0; y < modelList[mainPlayer.getPlayerEMD()].emdObjectBuffer[x].triangles.triCount; y++) {
            uPage = ((modelList[mainPlayer.getPlayerEMD()].emdTritexture[x][y].page & 0b00111111) * 128);

            glBegin(GL_TRIANGLES);
 
                glNormal3i(modelList[mainPlayer.getPlayerEMD()].emdNormal[x][modelList[mainPlayer.getPlayerEMD()].emdTriangle[x][y].n0].x,
                    (modelList[mainPlayer.getPlayerEMD()].emdNormal[x][modelList[mainPlayer.getPlayerEMD()].emdTriangle[x][y].n0].y)*-1,
                    modelList[mainPlayer.getPlayerEMD()].emdNormal[x][modelList[mainPlayer.getPlayerEMD()].emdTriangle[x][y].n0].z);
                
                glTexCoord2f((float)(modelList[mainPlayer.getPlayerEMD()].emdTritexture[x][y].u0 + uPage) / width_t , (float)modelList[mainPlayer.getPlayerEMD()].emdTritexture[x][y].v0/height_t);

                glVertex3i(modelList[mainPlayer.getPlayerEMD()].emdVertex[x][modelList[mainPlayer.getPlayerEMD()].emdTriangle[x][y].v0].x,
                    modelList[mainPlayer.getPlayerEMD()].emdVertex[x][modelList[mainPlayer.getPlayerEMD()].emdTriangle[x][y].v0].y ,
                    modelList[mainPlayer.getPlayerEMD()].emdVertex[x][modelList[mainPlayer.getPlayerEMD()].emdTriangle[x][y].v0].z);

                glNormal3i(modelList[mainPlayer.getPlayerEMD()].emdNormal[x][modelList[mainPlayer.getPlayerEMD()].emdTriangle[x][y].n1].x,
                    (modelList[mainPlayer.getPlayerEMD()].emdNormal[x][modelList[mainPlayer.getPlayerEMD()].emdTriangle[x][y].n1].y)*-1,
                    modelList[mainPlayer.getPlayerEMD()].emdNormal[x][modelList[mainPlayer.getPlayerEMD()].emdTriangle[x][y].n1].z);
                
                glTexCoord2f((float)(modelList[mainPlayer.getPlayerEMD()].emdTritexture[x][y].u1 + uPage) / width_t , (float)modelList[mainPlayer.getPlayerEMD()].emdTritexture[x][y].v1/height_t);
                
                glVertex3i(modelList[mainPlayer.getPlayerEMD()].emdVertex[x][modelList[mainPlayer.getPlayerEMD()].emdTriangle[x][y].v1].x,
                    modelList[mainPlayer.getPlayerEMD()].emdVertex[x][modelList[mainPlayer.getPlayerEMD()].emdTriangle[x][y].v1].y ,
                    modelList[mainPlayer.getPlayerEMD()].emdVertex[x][modelList[mainPlayer.getPlayerEMD()].emdTriangle[x][y].v1].z );

                glNormal3i(modelList[mainPlayer.getPlayerEMD()].emdNormal[x][modelList[mainPlayer.getPlayerEMD()].emdTriangle[x][y].n2].x,
                    (modelList[mainPlayer.getPlayerEMD()].emdNormal[x][modelList[mainPlayer.getPlayerEMD()].emdTriangle[x][y].n2].y)*-1,
                    modelList[mainPlayer.getPlayerEMD()].emdNormal[x][modelList[mainPlayer.getPlayerEMD()].emdTriangle[x][y].n2].z);
                
                glTexCoord2f((float)(modelList[mainPlayer.getPlayerEMD()].emdTritexture[x][y].u2 + uPage)  / width_t ,(float) modelList[mainPlayer.getPlayerEMD()].emdTritexture[x][y].v2/height_t);

                glVertex3i(modelList[mainPlayer.getPlayerEMD()].emdVertex[x][modelList[mainPlayer.getPlayerEMD()].emdTriangle[x][y].v2].x ,
                    modelList[mainPlayer.getPlayerEMD()].emdVertex[x][modelList[mainPlayer.getPlayerEMD()].emdTriangle[x][y].v2].y ,
                    modelList[mainPlayer.getPlayerEMD()].emdVertex[x][modelList[mainPlayer.getPlayerEMD()].emdTriangle[x][y].v2].z );
                
            glEnd();
        }

        for (unsigned int y = 0; y < modelList[mainPlayer.getPlayerEMD()].emdObjectBuffer[x].quads.quadCount; y++) {
            uPage = ((modelList[mainPlayer.getPlayerEMD()].emdQuadTexture[x][y].page & 0b00111111) * 128);
            glBegin(GL_QUADS);

                glNormal3i(modelList[mainPlayer.getPlayerEMD()].emdquadNormal[x][modelList[mainPlayer.getPlayerEMD()].emdQuad[x][y].n0].x,
                    (modelList[mainPlayer.getPlayerEMD()].emdquadNormal[x][modelList[mainPlayer.getPlayerEMD()].emdQuad[x][y].n0].y)*-1,
                    modelList[mainPlayer.getPlayerEMD()].emdquadNormal[x][modelList[mainPlayer.getPlayerEMD()].emdQuad[x][y].n0].z);

                glTexCoord2f((float)(modelList[mainPlayer.getPlayerEMD()].emdQuadTexture[x][y].u0 + uPage) / width_t, (float)modelList[mainPlayer.getPlayerEMD()].emdQuadTexture[x][y].v0/height_t);

                glVertex3i(modelList[mainPlayer.getPlayerEMD()].emdquadVertex[x][modelList[mainPlayer.getPlayerEMD()].emdQuad[x][y].v0].x , 
                    modelList[mainPlayer.getPlayerEMD()].emdquadVertex[x][modelList[mainPlayer.getPlayerEMD()].emdQuad[x][y].v0].y ,
                    modelList[mainPlayer.getPlayerEMD()].emdquadVertex[x][modelList[mainPlayer.getPlayerEMD()].emdQuad[x][y].v0].z );

                glNormal3i(modelList[mainPlayer.getPlayerEMD()].emdquadNormal[x][modelList[mainPlayer.getPlayerEMD()].emdQuad[x][y].n1].x,
                    (modelList[mainPlayer.getPlayerEMD()].emdquadNormal[x][modelList[mainPlayer.getPlayerEMD()].emdQuad[x][y].n1].y) * -1,
                    modelList[mainPlayer.getPlayerEMD()].emdquadNormal[x][modelList[mainPlayer.getPlayerEMD()].emdQuad[x][y].n1].z);
               glTexCoord2f((float)(modelList[mainPlayer.getPlayerEMD()].emdQuadTexture[x][y].u1 + uPage)  / width_t, (float)modelList[mainPlayer.getPlayerEMD()].emdQuadTexture[x][y].v1/height_t);
                
                glVertex3i(modelList[mainPlayer.getPlayerEMD()].emdquadVertex[x][modelList[mainPlayer.getPlayerEMD()].emdQuad[x][y].v1].x ,
                    modelList[mainPlayer.getPlayerEMD()].emdquadVertex[x][modelList[mainPlayer.getPlayerEMD()].emdQuad[x][y].v1].y ,
                    modelList[mainPlayer.getPlayerEMD()].emdquadVertex[x][modelList[mainPlayer.getPlayerEMD()].emdQuad[x][y].v1].z );


                glNormal3i(modelList[mainPlayer.getPlayerEMD()].emdquadNormal[x][modelList[mainPlayer.getPlayerEMD()].emdQuad[x][y].n3].x ,
                    (modelList[mainPlayer.getPlayerEMD()].emdquadNormal[x][modelList[mainPlayer.getPlayerEMD()].emdQuad[x][y].n3].y)*-1,
                    modelList[mainPlayer.getPlayerEMD()].emdquadNormal[x][modelList[mainPlayer.getPlayerEMD()].emdQuad[x][y].n3].z);
                glTexCoord2f((float)(modelList[mainPlayer.getPlayerEMD()].emdQuadTexture[x][y].u3 + uPage)  / width_t, (float)modelList[mainPlayer.getPlayerEMD()].emdQuadTexture[x][y].v3/height_t);
               
                glVertex3i(modelList[mainPlayer.getPlayerEMD()].emdquadVertex[x][modelList[mainPlayer.getPlayerEMD()].emdQuad[x][y].v3].x,
                    modelList[mainPlayer.getPlayerEMD()].emdquadVertex[x][modelList[mainPlayer.getPlayerEMD()].emdQuad[x][y].v3].y ,
                    modelList[mainPlayer.getPlayerEMD()].emdquadVertex[x][modelList[mainPlayer.getPlayerEMD()].emdQuad[x][y].v3].z);


               glNormal3i(modelList[mainPlayer.getPlayerEMD()].emdquadNormal[x][modelList[mainPlayer.getPlayerEMD()].emdQuad[x][y].n2].x,
                    (modelList[mainPlayer.getPlayerEMD()].emdquadNormal[x][modelList[mainPlayer.getPlayerEMD()].emdQuad[x][y].n2].y)*-1,
                    modelList[mainPlayer.getPlayerEMD()].emdquadNormal[x][modelList[mainPlayer.getPlayerEMD()].emdQuad[x][y].n2].z);
               glTexCoord2f((float)(modelList[mainPlayer.getPlayerEMD()].emdQuadTexture[x][y].u2 + uPage)  / width_t, (float)modelList[mainPlayer.getPlayerEMD()].emdQuadTexture[x][y].v2/height_t);
               
                glVertex3i(modelList[mainPlayer.getPlayerEMD()].emdquadVertex[x][modelList[mainPlayer.getPlayerEMD()].emdQuad[x][y].v2].x,
                    modelList[mainPlayer.getPlayerEMD()].emdquadVertex[x][modelList[mainPlayer.getPlayerEMD()].emdQuad[x][y].v2].y ,
                    modelList[mainPlayer.getPlayerEMD()].emdquadVertex[x][modelList[mainPlayer.getPlayerEMD()].emdQuad[x][y].v2].z);


            glEnd();
           }
    } 


}

void engineLight() {
    // Toda a iluminação de cada mapa, toda camera tem três iluminações, onde duas são lidas
    // do arquivo RDT e a outra fica no *SCD* que eu não vou pegar por enquanto
    // Tirando isso eu tenho que converter para float :p 
    GLfloat ambientColor[] = {(float)((playerRDT.rdtLight[mainPlayer.getPlayerCam()].colorAmbient.R/255.0f)*1.0f + 1.0),
        (float)((playerRDT.rdtLight[mainPlayer.getPlayerCam()].colorAmbient.G/255.0f)*1.0f + 1.0),
        (float)((playerRDT.rdtLight[mainPlayer.getPlayerCam()].colorAmbient.B/255.0f)*1.0f + 1.0),  1.0 };
    GLfloat lightPos1[] = {(float)(playerRDT.rdtLight[mainPlayer.getPlayerCam()].lightPos[0].X), 
        (float)(playerRDT.rdtLight[mainPlayer.getPlayerCam()].lightPos[0].Y), 
        (float)(playerRDT.rdtLight[mainPlayer.getPlayerCam()].lightPos[0].Z), 1.0};
    GLfloat lightPos2[] = {(float)(playerRDT.rdtLight[mainPlayer.getPlayerCam()].lightPos[1].X), 
        (float)(playerRDT.rdtLight[mainPlayer.getPlayerCam()].lightPos[1].Y), 
        (float)(playerRDT.rdtLight[mainPlayer.getPlayerCam()].lightPos[1].Z), 1.0};
    GLfloat lightColor1[] = {(float)((playerRDT.rdtLight[mainPlayer.getPlayerCam()].colorLight[0].R/255.0f)*1.0f),
        (float)((playerRDT.rdtLight[mainPlayer.getPlayerCam()].colorLight[0].G/255.0f)*1.0f),
        (float)((playerRDT.rdtLight[mainPlayer.getPlayerCam()].colorLight[0].B/255.0f)*1.0f), 1.0};
    GLfloat lightColor2[] = {(float)((playerRDT.rdtLight[mainPlayer.getPlayerCam()].colorLight[1].R/255.0f)*1.0f),
        (float)((playerRDT.rdtLight[mainPlayer.getPlayerCam()].colorLight[1].G/255.0f)*1.0f),
        (float)((playerRDT.rdtLight[mainPlayer.getPlayerCam()].colorLight[1].B/255.0f)*1.0f), 1.0};

    glLoadIdentity();
    // Cor do ambiente, modifica de acordo com a camera
    glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, ambientColor);
    // Primeira Luz
    glLightfv(GL_LIGHT0, GL_POSITION, lightPos1);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, lightColor1);
    // Segunda Luz
    glLightfv(GL_LIGHT1, GL_POSITION, lightPos2);   
    glLightfv(GL_LIGHT1, GL_DIFFUSE, lightColor2);
}

void drawMapBackground() {
    glDepthMask(0);
    glDisable(GL_LIGHTING);
    
    glLoadIdentity();
    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
    glTexImage2D(GL_TEXTURE_2D, 0,GL_RGB, engineBackground.bmpWidth, engineBackground.bmpHeight, 0,GL_BGR, GL_UNSIGNED_BYTE, engineBackground.bmpBuffer);

    glBegin(GL_QUADS);                     
        glColor4f(1.0, 1.0, 1.0, 1.0);
        glTexCoord2i(0,1);
        glVertex3f(-0.77f, 0.58f, -1.0f);              // Top Left
        glTexCoord2i(1,1);
        glVertex3f( 0.77f, 0.58f, -1.0f);              // Top Right
        glTexCoord2i(1,0);
        glVertex3f( 0.77f,-0.58f, -1.0f);              // Bottom Right
        glTexCoord2i(0,0);
        glVertex3f(-0.77f,-0.58f, -1.0f);              // Bottom Left        
    glEnd();

    glEnable(GL_LIGHTING);
    glDepthMask(1);
    
}


void drawCreditos() {
    glDepthMask(0);
        glPixelZoom(1,1);
    glDrawPixels(engineCreditos.bmpWidth, engineCreditos.bmpHeight, GL_BGR, GL_UNSIGNED_BYTE, engineCreditos.bmpBuffer );
    glDepthMask(1);
}

void renderMainMenu() {

    // FadeBlack Reverse
   switch (mainMenu) {
        // Render the game logo (this game contains...)
        case MAIN_MENU_LOGO:
            if (inFadeBlack) {
                // First Game Logo
                glDepthMask(0);
                glDrawPixels(engineThisGame.bmpWidth, engineThisGame.bmpHeight, GL_BGR, GL_UNSIGNED_BYTE, engineThisGame.bmpBuffer );
                glPixelZoom(2.5,2.5);   
                glDepthMask(1);
                if (fadeBlackNormal) {
                    // Timer
                    if (timer1_start < glutGet(GLUT_ELAPSED_TIME)) {
                        if (fadeBlackEffect >  0.0) {
                            fadeBlackEffect -= 0.1;
                            delay_sys = 23;
                        } else {
                            fadeBlackEffect = 0.0;
                            delay_sys--;
                            if (delay_sys <= 0) {
                                fadeBlackReverse = true;
                                fadeBlackNormal  = false;
                            }
                        }

                        timer1_start = (glutGet(GLUT_ELAPSED_TIME) + 50);
                    }
                } else {
                    // Timer
                    if (timer1_start < glutGet(GLUT_ELAPSED_TIME)) {
                        if (fadeBlackEffect <= 1.0) {
                            fadeBlackEffect += 0.1;
                            delay_sys = 100;
                        } else {
                            fadeBlackNormal = true;
                            mainMenu = MAIN_MENU_ENGINE_LOGO;
                        }

                        timer1_start = (glutGet(GLUT_ELAPSED_TIME) + 50);
                    }                
                }
            }
        break;

        case MAIN_MENU_ENGINE_LOGO:
            if (inFadeBlack) {
                // First Game Logo
                glDepthMask(0);
                glDrawPixels(engineThisGame.bmpWidth, engineThisGame.bmpHeight, GL_BGR, GL_UNSIGNED_BYTE, engineThisGame.bmpBuffer );
                glPixelZoom(2.5,2.5);   
                glDepthMask(1);
                if (fadeBlackNormal) {
                    // Timer
                    if (timer1_start < glutGet(GLUT_ELAPSED_TIME)) {
                        if (fadeBlackEffect >  0.0) {
                            fadeBlackEffect -= 0.1;
                            delay_sys = 23;
                        } else {
                            fadeBlackEffect = 0.0;
                            delay_sys--;
                            if (delay_sys <= 0) {
                                fadeBlackReverse = true;
                                fadeBlackNormal  = false;
                            }
                        }

                        timer1_start = (glutGet(GLUT_ELAPSED_TIME) + 50);
                    }
                } else {
                    // Timer
                    if (timer1_start < glutGet(GLUT_ELAPSED_TIME)) {
                        if (fadeBlackEffect <= 1.0) {
                            fadeBlackEffect += 0.1;
                            delay_sys = 100;
                        } else {
                            fadeBlackNormal = true;
                            mainMenu = MAIN_MENU_START;
                        }

                        timer1_start = (glutGet(GLUT_ELAPSED_TIME) + 50);
                    }                
                }
            }
        break;

        case MAIN_MENU_START:
            if (inFadeBlack) {
                // MainMenu Background
                glDepthMask(0);
                glDrawPixels(engineMainMenu.bmpWidth, engineMainMenu.bmpHeight, GL_BGR, GL_UNSIGNED_BYTE, engineMainMenu.bmpBuffer );
                glPixelZoom(2.5,2.5);
                glDepthMask(1);
                if (fadeBlackNormal) {
                    // Timer
                    if (timer1_start < glutGet(GLUT_ELAPSED_TIME)) {
                        if (fadeBlackEffect >  0.0) {
                            fadeBlackEffect -= 0.1;
                            delay_sys = 23;
                        } else {
                            fadeBlackEffect = 0.0;
                            delay_sys--;
                            if (delay_sys <= 0) {
                                fadeBlackReverse = true;
                                fadeBlackNormal  = false;
                            }
                        }

                        timer1_start = (glutGet(GLUT_ELAPSED_TIME) + 50);
                    }
                }
            }
        break;


       case MAIN_MENU_GAME:
            if (inFadeBlack) {
                glDepthMask(0);
                glDrawPixels(engineMainMenu.bmpWidth, engineMainMenu.bmpHeight, GL_BGR, GL_UNSIGNED_BYTE, engineMainMenu.bmpBuffer );
                glPixelZoom(2.5,2.5);
                glDepthMask(1);

                if (fadeBlackNormal) {
                    // Timer
                    if (timer1_start < glutGet(GLUT_ELAPSED_TIME)) {
                        if (fadeBlackEffect <= 1.0) {
                            fadeBlackEffect += 0.1;
                            delay_sys = 23;
                        } else {
                            delay_sys--;
                            if (delay_sys <= 0) {
                                fadeBlackReverse = true;
                                fadeBlackNormal  = false;
                                gameState       = STATE_IN_GAME;
                                inGame          = IN_GAME_BEGIN;
                            }
                        }

                        timer1_start = (glutGet(GLUT_ELAPSED_TIME) + 50);
                    }
                }       
            }
       break;

       default:
          std::cout << "Main Menu System bug !" << std::endl;
       break;
   }

    
}


void renderCredits() {

}

void renderInventary() {

}

// All in game stuff related with rendering is done here
void renderGame() {

    if (inGame == IN_GAME_BEGIN) {
        if (timer1_start < glutGet(GLUT_ELAPSED_TIME)) {
            if (fadeBlackEffect >  0.0) {
                fadeBlackEffect -= 0.1;
            } else {
                fadeBlackNormal = true;
                inFadeBlack     = false;
                inGame          = IN_GAME_NORMAL;
            }

            timer1_start = (glutGet(GLUT_ELAPSED_TIME) + 50);
        }          
    }


    // Filtros de textura, é necessário definilos para a textura ser apresentada
    // Eu utilizei até agora 2 filtros, o GL_NEAREST que deixa a qualidade da textura mais *original*
    // e o GL_LINEAR que é um filtro que deixa a textura mais bonita
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST ); 
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
    // Isso vai ser subistituido futuramente por um *sceneNum* ficará mais fácil
    // decidir o que vai ser renderizado no momento certo, como menu, menu de start
    // ou qualquer outro, mas por enquanto fica apenas esse bool =)
 

    // Todo .RDT tem vários fundos, toda a renderização fica nessa parte
    drawMapBackground();


    // Toda iluminação do motor gráfico é tratado aqui
    // No caso a iluminação é relativamente *simples*, tudo vai ser lido dos .RDT
    // só precisamos colocar de forma certa
    engineLight();

    // Toda a renderização do personagem é feita por essa função
    drawMainPlayer();
}


void renderScene( void ) {
    
    // Limpa a cor do buffer (background)
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Reseta a matriz ModelView
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    switch (gameState) {
        case STATE_MAIN_MENU:
            renderMainMenu();
        break;

        case STATE_INVENTARY:
            renderInventary();
        break;

        case STATE_IN_GAME:
            renderGame();
        break;

        case STATE_CREDITS:
            renderCredits();
        break;

        default:
            std::cout << "meh" << std::endl;
        break;
    }
  
        // Fade Black Effect
    if (inFadeBlack) {
        glDisable(GL_LIGHTING);
        glDisable(GL_TEXTURE_2D);
        
        glLoadIdentity();

        glBegin(GL_QUADS);                     
            glColor4f(0.0, 0.0, 0.0, fadeBlackEffect);
            glVertex3f(-3.0f, 2.0f, -3.0f);              // Top Left
            glVertex3f( 3.0f, 2.0f, -3.0f);              // Top Right
            glVertex3f( 3.0f,-2.0f, -3.0f);              // Bottom Right
            glVertex3f(-3.0f,-2.0f, -3.0f);              // Bottom Left        
        glEnd();

        glEnable(GL_LIGHTING);
        glEnable(GL_TEXTURE_2D);
    }

    // Buffers de renderização
    glutSwapBuffers();
}