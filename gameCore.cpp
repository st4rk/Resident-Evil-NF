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
int timer2_start = 0;
int delay_sys  = 0;

EMD modelList[MAX_MODEL];
RDT playerRDT;
PLD modelList_2[MAX_MODEL];


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
gameMath    mathEngine;
gameSound   soundEngine;
enemyClass  gameEnemy;

SDL_Surface *bg[0xFF];


gameCore::gameCore(int argc, char** argv) {
    /* start glut */
    glutInit(&argc, argv);

    /* game name */
    memset(GAME_NAME, 0x0, 20);
    strcpy(GAME_NAME, "Resident Evil : Nightmare Fiction");


    /* set all bacgrkoudns to null ptr */
    for (int i = 0; i < 0xFF; i++) 
        bg[i] = NULL;

}

gameCore::~gameCore() {
    for (int i = 0; i < 0xFF; i++)
        SDL_FreeSurface(bg[i]);
}


/* Okay, I want to remove you soon */
void background_Loader(std::string roomName) {
    // TESTE NOW ROOM109.BSS


    // Clear all Surface allocated in memory

    for (int i = 0; i < 0xFF; i++) {
        if (bg[i] != NULL)
            SDL_FreeSurface(bg[i]);
    }

    SDL_RWops *src;
    Uint8 *dstBuffer;
    int dstBufLen;

    src = SDL_RWFromFile(roomName.c_str(), "rb");
    if (!src) {
        fprintf(stderr, "Can not open %s for reading\n", "ROOM109.bss");
        exit (0);
    }

     int i = 0;
     int length = SDL_RWseek(src, 0, RW_SEEK_END);
     SDL_RWseek(src, 0, RW_SEEK_SET);

     while (SDL_RWseek(src, 0, RW_SEEK_CUR) < length) {
      vlc_depack(src, &dstBuffer, &dstBufLen);
      
      int pos = SDL_RWseek(src, 0, RW_SEEK_CUR);
      if ((pos & 0x7fff) != 0) SDL_RWseek(src, (pos & ~0x7fff) + 0x8000, RW_SEEK_SET);
      
      if (dstBuffer && dstBufLen) {
       SDL_RWops *mdec_src;

       mdec_src = SDL_RWFromMem(dstBuffer, dstBufLen);
       
       if (mdec_src) {
        Uint8 *dstMdecBuf;
        int dstMdecLen;

        mdec_depack(mdec_src, &dstMdecBuf, &dstMdecLen, 320,240);
        SDL_RWclose(mdec_src);

        if (dstMdecBuf && dstMdecLen) {
          SDL_Surface *image = mdec_surface(dstMdecBuf,320,240,0);
         
         if (image) {
              bg[i] = image;
              i++;
         }
         free(dstMdecBuf);
       }
      }
        free(dstBuffer);
      }
    }

}




/************************
** ENEMY Render and AI **
*************************/
void enemyAI_followPlayer() {

    /* Crappy AI, it's only follow the player, without path find obvious */
    if (mainPlayer.getPlayerX() > gameEnemy.getX()) {
        gameEnemy.setX(gameEnemy.getX() + 50);
    } else {
        gameEnemy.setX(gameEnemy.getX() - 50);
    }

    if (mainPlayer.getPlayerZ() > gameEnemy.getZ()) {
        gameEnemy.setZ(gameEnemy.getZ() + 50);
    } else {
        gameEnemy.setZ(gameEnemy.getZ() - 50);        
    }


    /* AI Animation */
    if (gameEnemy.getAnimCount() < (modelList[gameEnemy.getEMD()].emdSec2AnimInfo[1].animCount-1))
        gameEnemy.setAnimCount(gameEnemy.getAnimCount() + 1);
    else {
        gameEnemy.setAnimCount(0);
    }

    gameEnemy.setAnim(modelList[gameEnemy.getEMD()].emdSec2Data[gameEnemy.getAnimCount()+modelList[gameEnemy.getEMD()].emdSec2AnimInfo[1].animStart]);
    

    /* Angle between two vect, to know the player position */
    float angle = (atan2(((mainPlayer.getPlayerX() - gameEnemy.getX())), (mainPlayer.getPlayerZ() - gameEnemy.getZ())) / (M_PI / 180));
    gameEnemy.setAngle((angle-90)); 
}

/* Hardcode game init */
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
    playerRDT.rdtRE1LoadFile("resource/stages/re1/ROOM1060.RDT");
    
    // Música que vai ser tocada no menu
    if (soundEngine.engineSoundInit()) {
        soundEngine.engineLoadSound("musicas/menu.ogg");
    }

    // Alguns gráficos que são carregados na memória
    modelList[0].emdLoadFile("modelos/EM058.EMD");
    modelList[1].emdLoadFile("modelos/EMD21.EMD");
    modelList[2].emdLoadFile("modelos/EMD04.EMD");

    // Algumas imagens carregadas na memória
    engineThisGame.bmpLoaderFile("resource/intro_01.bmp");
    engineMainMenu.bmpLoaderFile("resource/intro_00.bmp");
    engineCreditos.bmpLoaderFile("creditos.bmp");

    // Carrega o background com número 5
    background_Loader("resource/stages/re1/ROOM106.BSS");

    engineFont.bmpLoaderFile("fonte/1.bmp");

    // HardCode, modelo inicial, X,Y,Z e Número da câmera
    mainPlayer.setPlayerEMD(0);

    mainPlayer.setPlayerX(18391);
    mainPlayer.setPlayerZ(12901);
    mainPlayer.setPlayerY(0);
    mainPlayer.setPlayerCam(1);
    mainPlayer.setPlayerAngle(90.0);

    // Player Item HardCode
    mainPlayer.setPlayerItem(0, -1); // NO ITEM
    mainPlayer.setPlayerItem(1, -1); // NO ITEM
    mainPlayer.setPlayerItem(2, -1); // NO ITEM
    mainPlayer.setPlayerItem(3, -1); // NO ITEM
    mainPlayer.setPlayerItem(4, -1); // NO ITEM
    mainPlayer.setPlayerItem(5, -1); // NO ITEM
    mainPlayer.setPlayerItem(6, -1); // NO ITEM
    mainPlayer.setPlayerItem(7, -1); // NO ITEM

    /* Init teste enemy */

    gameEnemy.setX(18400);
    gameEnemy.setZ(13000);
    gameEnemy.setY(0);
    gameEnemy.setEMD(1);
    gameEnemy.setAngle(0);

}

/* Here all stuff related with new room/stage goes here */
void eventSystem_newRoom(int roomNum) {
    char bss[0xFF];
    char rdt[0xFF];

    sprintf(rdt, "resource/stages/re1/ROOM%x%02x0.RDT", (STAGE(playerRDT.door_set_re1[roomNum].next_stage_and_room) > 0 ? STAGE(playerRDT.door_set_re1[roomNum].next_stage_and_room) : 1),
        ROOM(playerRDT.door_set_re1[roomNum].next_stage_and_room));

    sprintf(bss, "resource/stages/re1/ROOM%x%02x.BSS", (STAGE(playerRDT.door_set_re1[roomNum].next_stage_and_room) > 0 ? STAGE(playerRDT.door_set_re1[roomNum].next_stage_and_room) : 1),
        ROOM(playerRDT.door_set_re1[roomNum].next_stage_and_room));


    std::cout << bss << std::endl;
    std::cout << rdt << std::endl;

    /* Load BSS and RDT */
    background_Loader(bss);
    playerRDT.rdtRE1LoadFile(rdt);

    /* Set Player new X,Y,Z */
    mainPlayer.setPlayerX(playerRDT.door_set_re1[roomNum].next_x);
    mainPlayer.setPlayerY(playerRDT.door_set_re1[roomNum].next_y);
    mainPlayer.setPlayerZ(playerRDT.door_set_re1[roomNum].next_z);
    mainPlayer.setPlayerCam(playerRDT.rdtRE1CameraSwitch[1].from);
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

                case 'z':
                for(unsigned int i = 0; i < playerRDT.door_set_len; i++) {
                    if (mathEngine.collisionDetect(1, playerRDT.door_set_re1[i].x, playerRDT.door_set_re1[i].y, 
                               playerRDT.door_set_re1[i].h, playerRDT.door_set_re1[i].w, 
                               mainPlayer.getPlayerX(), mainPlayer.getPlayerZ())) {
                        eventSystem_newRoom(i);
                    }
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

        /* Enemy AI Stuff */
        //enemyAI_followPlayer();

        if (((mainPlayer.getPlayerInMove()) == false) && (mainPlayer.getPlayerInShoot() == false) && (mainPlayer.getPlayerInRotate() == false)) {
            mainPlayer.setPlayerAnimSection(1);
            mainPlayer.setPlayerAnim(2);
        }

        if (mainPlayer.getPlayerInShoot() == true) {
                mainPlayer.setPlayerAnim(10);
        } 

        if (mainPlayer.getPlayerInRotate() && (mainPlayer.getPlayerInRotatePos() == 1)) {
            mainPlayer.setPlayerAngle(fmod((mainPlayer.getPlayerAngle() - 5), 360.0));
        }

        if (mainPlayer.getPlayerInRotate() && (mainPlayer.getPlayerInRotatePos() == 0)) {
            mainPlayer.setPlayerAngle(fmod((mainPlayer.getPlayerAngle() + 5), 360.0));
        }

        if (((mainPlayer.getPlayerInMove() == true) && (mainPlayer.getPlayerInRotate() == false)) 
            || ((mainPlayer.getPlayerInMove() == true) && (mainPlayer.getPlayerInRotate() == true))) {
            if (mainPlayer.getPlayerRunning()) {
                x = cos((mainPlayer.getPlayerAngle() * PI/180)) * 180.0;
                z = sin((mainPlayer.getPlayerAngle() * PI/180)) * 180.0;
            } else {
                x = cos((mainPlayer.getPlayerAngle() * PI/180)) * 80.0;
                z = sin((mainPlayer.getPlayerAngle() * PI/180)) * 80.0;
            }

            /* Collision Detection RE 1 */
            for (unsigned int i = 0; i < (playerRDT.rdtRE1ColisionHeader.counts -1); i++) {
                if (playerRDT.rdtRE1ColissionArray[i].floor == 768) {
                    if (mathEngine.collisionDetect(playerRDT.rdtRE1ColissionArray[i].type, playerRDT.rdtRE1ColissionArray[i].x2, playerRDT.rdtRE1ColissionArray[i].z2, 
                                             playerRDT.rdtRE1ColissionArray[i].x1, playerRDT.rdtRE1ColissionArray[i].z1, mainPlayer.getPlayerX() + x, mainPlayer.getPlayerZ()-z) == true) {
                      //  canMove = false;
                    } 
                }
            }

            /* Colision Detection RDT 1.5 and 2.0
            for (unsigned int i = 0; i < (playerRDT.rdtColisionHeader.arraySum -1); i++) {
                if (mathEngine.collisionDetect(0, playerRDT.rdtColissionArray[i].X, playerRDT.rdtColissionArray[i].Z, 
                                         playerRDT.rdtColissionArray[i].W, playerRDT.rdtColissionArray[i].D, mainPlayer.getPlayerX() + x, mainPlayer.getPlayerZ()-z) == true) {
                    canMove = false;
                } 
            }
            */
            
            /* Camera Switch RDT 1.5 and 2.0
            for (unsigned int i = 0; i < playerRDT.rdtCameraSwitch.size(); i++) {
                if (mathEngine.mapSwitch(mainPlayer.getPlayerX(), mainPlayer.getPlayerZ(), playerRDT.rdtCameraSwitch[i].x1, playerRDT.rdtCameraSwitch[i].z1, playerRDT.rdtCameraSwitch[i].x2, playerRDT.rdtCameraSwitch[i].z2, 
                                         playerRDT.rdtCameraSwitch[i].x3, playerRDT.rdtCameraSwitch[i].z3, playerRDT.rdtCameraSwitch[i].x4, playerRDT.rdtCameraSwitch[i].z4)) {
                    
                    if (playerRDT.rdtCameraSwitch[i].cam1 != 0) {//|| ((playerRDT.rdtCameraSwitch[i].cam1 == 0) && (playerRDT.rdtCameraSwitch[i].cam0 == 1)))
                           backgroundNow = playerRDT.rdtCameraSwitch[i].cam1;// background_Loader(playerRDT.rdtCameraSwitch[i].cam1);
                           mainPlayer.setPlayerCam(backgroundNow);
                    }
                
                } 
            }
            */

            /* Camera Switch Zone/Camera Position RE 1.0 */
            for (unsigned int i = 0; i < playerRDT.rdtRE1CameraSwitch.size(); i++) {
                if (mathEngine.mapSwitch(mainPlayer.getPlayerX(), mainPlayer.getPlayerZ(), playerRDT.rdtRE1CameraSwitch[i].x1, playerRDT.rdtRE1CameraSwitch[i].z1, playerRDT.rdtRE1CameraSwitch[i].x2, playerRDT.rdtRE1CameraSwitch[i].z2, 
                                         playerRDT.rdtRE1CameraSwitch[i].x3, playerRDT.rdtRE1CameraSwitch[i].z3, playerRDT.rdtRE1CameraSwitch[i].x4, playerRDT.rdtRE1CameraSwitch[i].z4)) {
                    
                        if (playerRDT.rdtRE1CameraSwitch[i].to != 9) {
                             mainPlayer.setPlayerCam(playerRDT.rdtRE1CameraSwitch[i].to);
                        }

                } 
            }                    



            if (canMove == true) {

                mainPlayer.setPlayerX(mainPlayer.getPlayerX() + x);
                mainPlayer.setPlayerZ(mainPlayer.getPlayerZ() - z);
            }

        }



        /* Here is the animation stuff */
        if ((mainPlayer.getPlayerAnimSection()) == 0) {
            if (mainPlayer.getPlayerAnimCount() < (modelList[mainPlayer.getPlayerEMD()].emdSec2AnimInfo[mainPlayer.getPlayerAnim()].animCount-1))
                mainPlayer.setPlayerAnimCount(mainPlayer.getPlayerAnimCount() + 1);
            else {
                mainPlayer.setPlayerAnimCount(0);
            }

            mainPlayer.setPlayerEMDAnim(modelList[mainPlayer.getPlayerEMD()].emdSec2Data[mainPlayer.getPlayerAnimCount()+modelList[mainPlayer.getPlayerEMD()].emdSec2AnimInfo[mainPlayer.getPlayerAnim()].animStart]);
        } else {
            
            if (mainPlayer.getPlayerAnimCount() < (modelList[mainPlayer.getPlayerEMD()].emdSec4AnimInfo[mainPlayer.getPlayerAnim()].animCount-1))
                mainPlayer.setPlayerAnimCount(mainPlayer.getPlayerAnimCount() + 1);
            else {
                mainPlayer.setPlayerAnimCount(0);
            }

            mainPlayer.setPlayerEMDAnim(modelList[mainPlayer.getPlayerEMD()].emdSec4Data[mainPlayer.getPlayerAnimCount()+modelList[mainPlayer.getPlayerEMD()].emdSec4AnimInfo[mainPlayer.getPlayerAnim()].animStart]);
        }
    }


    glutPostRedisplay();   
    glutTimerFunc(33, MainLoop, 0);
}


/* OpenGL stuff start */
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

/* This function is responsible for all emd animation */
void renderEMD_modelAnimation(unsigned int objNum, unsigned int var, int var2, EMD_SEC2_DATA_T animFrame, unsigned int emdNum) {
    if (var == objNum) {
        glTranslatef((float)modelList[emdNum].emdSec2RelPos[var2].x, (float)modelList[emdNum].emdSec2RelPos[var2].y, (float)modelList[emdNum].emdSec2RelPos[var2].z);
        glRotatef(animFrame.vector[var2].x, 1.0f, 0.0f, 0.0f);
        glRotatef(animFrame.vector[var2].y, 0.0f, 1.0f, 0.0f);
        glRotatef(animFrame.vector[var2].z, 0.0f, 0.0f, 1.0f);
    }

     for (unsigned int c = 0; c < modelList[emdNum].emdSec2Armature[var].meshCount; c++) {
        renderEMD_modelAnimation(objNum, modelList[emdNum].emdSec2Mesh[var][c], var2, animFrame, emdNum);
     }
}

/* This functions is responsible for all emd rendering used by the game */
void renderEMD(float m_x, float m_y, float m_z, float angle, unsigned int emdNum, EMD_SEC2_DATA_T animFrame) {
    float width_t  = (float) (modelList[emdNum].emdTimTexture.timTexture.imageWidth*2);
    float height_t = (float)  modelList[emdNum].emdTimTexture.timTexture.imageHeight;

    unsigned short uPage = 0;

    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
    glTexImage2D(GL_TEXTURE_2D, 0,GL_RGB, (modelList[emdNum].emdTimTexture.timTexture.imageWidth*2), modelList[emdNum].emdTimTexture.timTexture.imageHeight, 0,GL_RGB, GL_UNSIGNED_BYTE, modelList[emdNum].emdTimTexture.rawTexture);


    // Toda a renderização do personagem vai ficar aqui !
    for (unsigned int x = 0; x < modelList[emdNum].emdTotalObj; x++) {
        
        glLoadIdentity();        
        
        /* Camera Position from RDT 1.5 and 2.0
        gluLookAt(     playerRDT.rdtCameraPos[mainPlayer.getPlayerCam()].positionX, playerRDT.rdtCameraPos[mainPlayer.getPlayerCam()].positionY, playerRDT.rdtCameraPos[mainPlayer.getPlayerCam()].positionZ,
                       playerRDT.rdtCameraPos[mainPlayer.getPlayerCam()].targetX, playerRDT.rdtCameraPos[mainPlayer.getPlayerCam()].targetY, playerRDT.rdtCameraPos[mainPlayer.getPlayerCam()].targetZ,   
                       0.0f,   -0.1f,   0.0f);
        */

        gluLookAt(     playerRDT.rdtRE1CameraPos[mainPlayer.getPlayerCam()].positionX, playerRDT.rdtRE1CameraPos[mainPlayer.getPlayerCam()].positionY, playerRDT.rdtRE1CameraPos[mainPlayer.getPlayerCam()].positionZ,
                       playerRDT.rdtRE1CameraPos[mainPlayer.getPlayerCam()].targetX, playerRDT.rdtRE1CameraPos[mainPlayer.getPlayerCam()].targetY, playerRDT.rdtRE1CameraPos[mainPlayer.getPlayerCam()].targetZ,   
                       0.0f,   -0.1f,   0.0f);
   
        glTranslatef(m_x, m_y, m_z);  

        glRotatef(angle, 0.0f, 1.0f, 0.0f);


        for (unsigned int z = 0; z <  modelList[emdNum].emdTotalObj; z++) {
            renderEMD_modelAnimation(x, z, z, animFrame, emdNum);
        }

        for (unsigned int y = 0; y < modelList[emdNum].emdObjectBuffer[x].triangles.triCount; y++) {
            uPage = ((modelList[emdNum].emdTritexture[x][y].page & 0b00111111) * 128);

            glBegin(GL_TRIANGLES);
 
                glNormal3i(modelList[emdNum].emdNormal[x][modelList[emdNum].emdTriangle[x][y].n0].x,
                    (modelList[emdNum].emdNormal[x][modelList[emdNum].emdTriangle[x][y].n0].y)*-1,
                    modelList[emdNum].emdNormal[x][modelList[emdNum].emdTriangle[x][y].n0].z);
                
                glTexCoord2f((float)(modelList[emdNum].emdTritexture[x][y].u0 + uPage) / width_t , (float)modelList[emdNum].emdTritexture[x][y].v0/height_t);

                glVertex3i(modelList[emdNum].emdVertex[x][modelList[emdNum].emdTriangle[x][y].v0].x,
                    modelList[emdNum].emdVertex[x][modelList[emdNum].emdTriangle[x][y].v0].y ,
                    modelList[emdNum].emdVertex[x][modelList[emdNum].emdTriangle[x][y].v0].z);

                glNormal3i(modelList[emdNum].emdNormal[x][modelList[emdNum].emdTriangle[x][y].n1].x,
                    (modelList[emdNum].emdNormal[x][modelList[emdNum].emdTriangle[x][y].n1].y)*-1,
                    modelList[emdNum].emdNormal[x][modelList[emdNum].emdTriangle[x][y].n1].z);
                
                glTexCoord2f((float)(modelList[emdNum].emdTritexture[x][y].u1 + uPage) / width_t , (float)modelList[emdNum].emdTritexture[x][y].v1/height_t);
                
                glVertex3i(modelList[emdNum].emdVertex[x][modelList[emdNum].emdTriangle[x][y].v1].x,
                    modelList[emdNum].emdVertex[x][modelList[emdNum].emdTriangle[x][y].v1].y ,
                    modelList[emdNum].emdVertex[x][modelList[emdNum].emdTriangle[x][y].v1].z );

                glNormal3i(modelList[emdNum].emdNormal[x][modelList[emdNum].emdTriangle[x][y].n2].x,
                    (modelList[emdNum].emdNormal[x][modelList[emdNum].emdTriangle[x][y].n2].y)*-1,
                    modelList[emdNum].emdNormal[x][modelList[emdNum].emdTriangle[x][y].n2].z);
                
                glTexCoord2f((float)(modelList[emdNum].emdTritexture[x][y].u2 + uPage)  / width_t ,(float) modelList[emdNum].emdTritexture[x][y].v2/height_t);

                glVertex3i(modelList[emdNum].emdVertex[x][modelList[emdNum].emdTriangle[x][y].v2].x ,
                    modelList[emdNum].emdVertex[x][modelList[emdNum].emdTriangle[x][y].v2].y ,
                    modelList[emdNum].emdVertex[x][modelList[emdNum].emdTriangle[x][y].v2].z );
                
            glEnd();
        }

        for (unsigned int y = 0; y < modelList[emdNum].emdObjectBuffer[x].quads.quadCount; y++) {
            uPage = ((modelList[emdNum].emdQuadTexture[x][y].page & 0b00111111) * 128);
            glBegin(GL_QUADS);

                glNormal3i(modelList[emdNum].emdquadNormal[x][modelList[emdNum].emdQuad[x][y].n0].x,
                    (modelList[emdNum].emdquadNormal[x][modelList[emdNum].emdQuad[x][y].n0].y)*-1,
                    modelList[emdNum].emdquadNormal[x][modelList[emdNum].emdQuad[x][y].n0].z);

                glTexCoord2f((float)(modelList[emdNum].emdQuadTexture[x][y].u0 + uPage) / width_t, (float)modelList[emdNum].emdQuadTexture[x][y].v0/height_t);

                glVertex3i(modelList[emdNum].emdquadVertex[x][modelList[emdNum].emdQuad[x][y].v0].x , 
                    modelList[emdNum].emdquadVertex[x][modelList[emdNum].emdQuad[x][y].v0].y ,
                    modelList[emdNum].emdquadVertex[x][modelList[emdNum].emdQuad[x][y].v0].z );

                glNormal3i(modelList[emdNum].emdquadNormal[x][modelList[emdNum].emdQuad[x][y].n1].x,
                    (modelList[emdNum].emdquadNormal[x][modelList[emdNum].emdQuad[x][y].n1].y) * -1,
                    modelList[emdNum].emdquadNormal[x][modelList[emdNum].emdQuad[x][y].n1].z);
               glTexCoord2f((float)(modelList[emdNum].emdQuadTexture[x][y].u1 + uPage)  / width_t, (float)modelList[emdNum].emdQuadTexture[x][y].v1/height_t);
                
                glVertex3i(modelList[emdNum].emdquadVertex[x][modelList[emdNum].emdQuad[x][y].v1].x ,
                    modelList[emdNum].emdquadVertex[x][modelList[emdNum].emdQuad[x][y].v1].y ,
                    modelList[emdNum].emdquadVertex[x][modelList[emdNum].emdQuad[x][y].v1].z );


                glNormal3i(modelList[emdNum].emdquadNormal[x][modelList[emdNum].emdQuad[x][y].n3].x ,
                    (modelList[emdNum].emdquadNormal[x][modelList[emdNum].emdQuad[x][y].n3].y)*-1,
                    modelList[emdNum].emdquadNormal[x][modelList[emdNum].emdQuad[x][y].n3].z);
                glTexCoord2f((float)(modelList[emdNum].emdQuadTexture[x][y].u3 + uPage)  / width_t, (float)modelList[emdNum].emdQuadTexture[x][y].v3/height_t);
               
                glVertex3i(modelList[emdNum].emdquadVertex[x][modelList[emdNum].emdQuad[x][y].v3].x,
                    modelList[emdNum].emdquadVertex[x][modelList[emdNum].emdQuad[x][y].v3].y ,
                    modelList[emdNum].emdquadVertex[x][modelList[emdNum].emdQuad[x][y].v3].z);


               glNormal3i(modelList[emdNum].emdquadNormal[x][modelList[emdNum].emdQuad[x][y].n2].x,
                    (modelList[emdNum].emdquadNormal[x][modelList[emdNum].emdQuad[x][y].n2].y)*-1,
                    modelList[emdNum].emdquadNormal[x][modelList[emdNum].emdQuad[x][y].n2].z);
               glTexCoord2f((float)(modelList[emdNum].emdQuadTexture[x][y].u2 + uPage)  / width_t, (float)modelList[emdNum].emdQuadTexture[x][y].v2/height_t);
               
                glVertex3i(modelList[emdNum].emdquadVertex[x][modelList[emdNum].emdQuad[x][y].v2].x,
                    modelList[emdNum].emdquadVertex[x][modelList[emdNum].emdQuad[x][y].v2].y ,
                    modelList[emdNum].emdquadVertex[x][modelList[emdNum].emdQuad[x][y].v2].z);


            glEnd();
           }
    } 
}

void engineLight() {
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
    glTexImage2D(GL_TEXTURE_2D, 0,GL_RGB, bg[mainPlayer.getPlayerCam()]->w, bg[mainPlayer.getPlayerCam()]->h, 0,GL_RGB, GL_UNSIGNED_BYTE, bg[mainPlayer.getPlayerCam()]->pixels);


  /* RE 1.5 and RE 2
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

    */

    /* RE 1 */
    glBegin(GL_QUADS);                     
        glColor4f(1.0, 1.0, 1.0, 1.0);
        glTexCoord2i(0,0);
        glVertex3f(-0.77f, 0.58f, -1.0f);              // Top Left
        glTexCoord2i(1,0);
        glVertex3f( 0.77f, 0.58f, -1.0f);              // Top Right
        glTexCoord2i(1,1);
        glVertex3f( 0.77f,-0.58f, -1.0f);              // Bottom Right
        glTexCoord2i(0,1);
        glVertex3f(-0.77f,-0.58f, -1.0f);              // Bottom Left        
    glEnd();

    glEnable(GL_LIGHTING);
    glDepthMask(1);
    
}


/* I WILL REMOVE THIS SHIT RENDERING URRGG */
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
 

    /* .BSS Background stuff */
    drawMapBackground();

    /* All model rendering is done by renderEMD Function */
    /* Player Rendering */
    renderEMD(mainPlayer.getPlayerX(), mainPlayer.getPlayerY(), mainPlayer.getPlayerZ(), 
              mainPlayer.getPlayerAngle(), mainPlayer.getPlayerEMD(), mainPlayer.getPlayerEMDAnim());
    /* Enemy Render 
    renderEMD(gameEnemy.getX(), gameEnemy.getY(), gameEnemy.getZ(), 
              gameEnemy.getAngle(), gameEnemy.getEMD(), gameEnemy.getAnim());*/
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


