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

/* This is the main menu machine state */
unsigned char mainMenu      = 0x0;
/* This is used by Select Character Screen */
unsigned char selectState           = 0x0;
unsigned char charSelectNum         = 0x0;
unsigned char charMusicNum          = 0x0;
float         nextCharAngle         = 0.0f;
bool          nextCharTransition    = false;
bool          charNextStep          = false;

/* This is used by menu's */
unsigned char menuArrow  = 0x0;
unsigned char menuUpDown = 0x0;
// In Game
unsigned char inGame   = 0x0;

// System Time, you can do a delay with it
int timer1_start = 0;
int timer2_start = 0;
int delay_sys  = 0;

RDT playerRDT;
PLD modelList_2[MAX_MODEL];
EMD modelList[MAX_MODEL];



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
bmp_loader_24bpp engineLogo;
bmp_loader_24bpp engineSelectChar;
bmp_loader_24bpp engineCharMenu;

playerClass mainPlayer;
gameMath    mathEngine;
gameSound   soundEngine;
enemyClass  gameEnemy;
gameMisc    miscStuff;

/* Room transition */
unsigned int roomNum = -1; 

/* Debug Menu Features */
bool wireFrameMode = false;

SDL_Surface *bg[0xFF];

unsigned int tmr60FPS = 0;
    
/* Debug Room */
debugRoom debugR[0xFF];
bool jumpToRun = false;
int debug_jRoomNum = 0;

gameCore *core;

gameCore::gameCore(int argc, char** argv) {
    /* start glut */
    glutInit(&argc, argv);

    /* clear timer */
    coreTmr_anim = 0;

    /* game name */
    memset(GAME_NAME, 0x0, 20);
    strcpy(GAME_NAME, "Resident Evil : Nightmare Fiction");


    /* set all bacgrkoudns to null ptr */
    for (int i = 0; i < 0xFF; i++) 
        bg[i] = NULL;


    for (int i = 0; i < 0x10; i++)
        keyList[i] = false;

    oldAnim = -1;
    inInterpolation = false;

}

gameCore::~gameCore() {
    for (int i = 0; i < 0xFF; i++)
        SDL_FreeSurface(bg[i]);

    for (unsigned int i = 0; i < mixList.size(); i++) {
        Mix_FreeChunk(mixList[i]);
    }
}


/*
 * background_Loader
 * This function is a crapy .bss loader
 * thanks to EPSX Emulator
 */
void background_Loader(std::string roomName) {
    // TESTE NOW ROOM109.BSS


    SDL_RWops *src;
    Uint8 *dstBuffer;
    int dstBufLen;

    src = SDL_RWFromFile(roomName.c_str(), "rb");
    if (!src) {
        fprintf(stderr, "Can not open %s for reading\n", roomName.c_str());
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
             
             if (bg[i] != NULL)
                 SDL_FreeSurface(bg[i]);

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
        gameEnemy.setX(gameEnemy.getX() + 5);
    } else {
        gameEnemy.setX(gameEnemy.getX() - 5);
    }

    if (mainPlayer.getPlayerZ() > gameEnemy.getZ()) {
        gameEnemy.setZ(gameEnemy.getZ() + 5);
    } else {
        gameEnemy.setZ(gameEnemy.getZ() - 5);        
    }


    /* AI Animation */
    if (gameEnemy.getAnimCount() < (modelList[gameEnemy.getEMD()].emdSec2AnimInfo[0].animCount-1))
        gameEnemy.setAnimCount(gameEnemy.getAnimCount() + 1);
    else {
        gameEnemy.setAnimCount(0);
    }

    gameEnemy.setAnim(modelList[gameEnemy.getEMD()].emdSec2Data[gameEnemy.getAnimCount()+modelList[gameEnemy.getEMD()].emdSec2AnimInfo[0].animStart]);
    

    /* Angle between two vect, to know the player position */
    float angle = (atan2(((mainPlayer.getPlayerX() - gameEnemy.getX())), (mainPlayer.getPlayerZ() - gameEnemy.getZ())) / (M_PI / 180));
    gameEnemy.setAngle((angle-90)); 
}

/*
 * renderLoadResource
 * This function is the hardcoded game initialization
 */
void gameCore::renderLoadResource() {
    Mix_Chunk *node = NULL;
    // Clear Timer
    timer1_start = 0;

    // The game should start on Main Menu
    gameState       = STATE_MAIN_MENU;
    mainMenu        = MAIN_MENU_ENGINE_LOGO;

    // RDT do Início do Player
    playerRDT.rdtRE1LoadFile("resource/stages/re1/ROOM1060.RDT");
    
    
    /* 
     * Player Selection Sound
     */
    if (soundEngine.engineSoundInit()) {
        soundEngine.engineLoadSound("resource/ost/7.mp3");
    }

    // Alguns gráficos que são carregados na memória
    modelList[0].emdLoadFile("resource/models/Leon_sel_1.EMD");
    /* Leon S. Kennedy RE2 */
    modelList[1].emdLoadFile("resource/models/EM050.EMD");
    /* Chris Redfield  RE2 */
    modelList[2].emdLoadFile("resource/models/2.EMD");
    modelList[3].emdLoadFile("resource/models/PL0BCH.EMD");
    modelList[4].emdLoadFile("resource/models/EMD05.EMD");

    /*
     * Load all sound effects
     */
    // Som do click
    node = Mix_LoadWAV("resource/sfx/click.wav");
    mixList.push_back(node);
    node = Mix_LoadWAV("resource/sfx/title3.wav");
    mixList.push_back(node);
    node = Mix_LoadWAV("resource/sfx/next.wav");
    mixList.push_back(node);

    // Algumas imagens carregadas na memória
    engineThisGame.bmpLoaderFile  ("resource/menus/intro_2.bmp",0);
    engineLogo.bmpLoaderFile      ("resource/menus/intro_1.bmp",0);
    engineMainMenu.bmpLoaderFile  ("resource/menus/mainMenu.bmp",0);
    engineSelectChar.bmpLoaderFile("resource/menus/selectChar.bmp", 0);
    engineCharMenu.bmpLoaderFile  ("resource/menus/charMenu.bmp", 1);

    // Carrega o background com número 5
    background_Loader("resource/stages/re1/ROOM106.BSS");

    engineFont.bmpLoaderFile("resource/texture/1.bmp",1);

    // HardCode, modelo inicial, X,Y,Z e Número da câmera
    mainPlayer.setPlayerEMD(0);
    /*
    mainPlayer.setPlayerX(18391);
    mainPlayer.setPlayerZ(12901);
    mainPlayer.setPlayerY(0);
    */
    mainPlayer.setPlayerX(0);
    mainPlayer.setPlayerZ(0);
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

    gameEnemy.setX(0);
    gameEnemy.setZ(-10000.0f);
    gameEnemy.setY(0);
    gameEnemy.setEMD(4);
    gameEnemy.setAngle(0);


    /* Hardcoded Debug Room X,Y,Z,RoomNum and RoomName !*/

    debugR[0].roomName = "Main Hall";
    debugR[0].roomNum  = 6;
    debugR[0].x = 17060.0f;
    debugR[0].y = 0.0f;
    debugR[0].z = 11933.0f;

    debugR[1].roomName = "Testee ha!";
    debugR[1].roomNum  = 5;
    debugR[1].x        = 30900.0f;
    debugR[1].y        = 0.0f;
    debugR[1].z        = 8300.0f;



    miscStuff.setupFadeEffect(TYPE_FADE_IN, 0, 0, 0, 60);
}


/*
 * renderText
 * This function is used to draw text on Screen
 * There are two different charset
 * CHAR_SET_1 TEXT_TYPE_NORMAL
 * CHAR_SET_2 TEXT_TYPE_LITTLE
 */
void gameCore::renderText(float x, float y, float z, int type, std::string text, float r = 1.0, float g = 1.0, float b = 1.0, float a = 1.0) {
    glDisable(GL_LIGHTING);

    glLoadIdentity();
    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
    glTexImage2D(GL_TEXTURE_2D, 0,GL_RGBA, engineFont.bmpWidth, engineFont.bmpHeight, 0,GL_RGBA, GL_UNSIGNED_BYTE, engineFont.bmpBuffer);


    /* 
       Important information, character(big) Y(up) = 0.0532
       Next Character(right), 0.055 * char_num
       Note: what the hell is wrong with my coordinate system ? :|
       ASCII Starts in 0x24(36)

       There are 18(0x12) character per line
       To know which charater draw in line, character_num mod 18
       To know which line is, (character_num - 36) / 18

    */

    switch(type) {
        case TEXT_TYPE_NORMAL: {
            float tX = -0.77+x;
            float tY =  0.48-y;

            for (unsigned int i = 0; i < text.size(); i++, tX += 0.07) {
                if (text[i] != ' ') {
                    float Xo = 0.055 * (text[i] % 18);
                    float Yo = 0.8332 - (0.0532 * ((text[i]-36)/18));
                    glBegin(GL_QUADS);                     
                        glColor4f(r,g,b,a);
                        /* Texture Coord */
                        glTexCoord2f(Xo,Yo);
                        glVertex3f(tX, tY, -1.0f);            
                        /* Texture Coord */
                        glTexCoord2f(Xo+0.053,Yo);
                        glVertex3f(tX+0.08f, tY, -1.0f);              
                        /* Texture Coord */
                        glTexCoord2f(Xo+0.053,Yo+0.053);
                        glVertex3f(tX+0.08f,tY+0.08f, -1.0f);              
                        /* Texture Coord */
                        glTexCoord2f(Xo, Yo+0.053);
                        glVertex3f(tX, tY+0.08f, -1.0f);                  
                    glEnd();
                }
            }
        }
        
        case TEXT_TYPE_LITTLE: {
            float tX = -0.77+x;
            float tY =  0.48-y;

            for (unsigned int i = 0; i < text.size(); i++, tX += 0.05) {
                if (text[i] != ' ') {
                    float Xo = 0.03123 * (text[i] % 32);
                    float Yo = 0.9695 - (0.0320 * ((text[i]-32)/32));
                    glBegin(GL_QUADS);                     
                        glColor4f(r,g,b,a);
                        /* Texture Coord */
                        glTexCoord2f(Xo,Yo);
                        glVertex3f(tX, tY, -1.0f);            
                        /* Texture Coord */
                        glTexCoord2f(Xo+0.030,Yo);
                        glVertex3f(tX+0.05f, tY, -1.0f);              
                        /* Texture Coord */
                        glTexCoord2f(Xo+0.030,Yo+0.030);
                        glVertex3f(tX+0.05f,tY+0.05f, -1.0f);              
                        /* Texture Coord */
                        glTexCoord2f(Xo, Yo+0.030);
                        glVertex3f(tX, tY+0.05f, -1.0f);                  
                    glEnd();
                }
            }
        }
        break;

        default:

        break;        
    }
    
    glEnable(GL_LIGHTING);
}

/* All stuff related with debug menu goes here */
void gameCore::eventSystem_debugMenu() {
    char jump_to[20];

    renderText(0.42, 0.30, 0.0f, TEXT_TYPE_LITTLE, "--Debug Menu--");


    sprintf(jump_to, "Jump %03x %s",debugR[debug_jRoomNum].roomNum, debugR[debug_jRoomNum].roomName.c_str());

    renderText(0.35, 0.35, 0.0f, TEXT_TYPE_LITTLE, jump_to);
    glDisable(GL_LIGHTING);
    glDisable(GL_TEXTURE_2D);
    glLoadIdentity();


    glBegin(GL_QUADS);
        glColor4f(0.0f, 1.0f, 0.0f, 0.5f);

        glVertex3f(-0.64f, 0.25f, -1.0f);          
        glVertex3f( 0.64f, 0.25f, -1.0f);             
        glVertex3f( 0.64f,-0.25f, -1.0f);              
        glVertex3f(-0.64f,-0.25f, -1.0f);              

    glEnd();
    glEnable(GL_TEXTURE_2D);

    glEnable(GL_LIGHTING);
}

void eventSystem_debugJumpToRun() {



    /* Fade black effect 
    if (inFadeBlack) {
        if (fadeBlackNormal) {
            if (timer1_start < glutGet(GLUT_ELAPSED_TIME)) {
                if (fadeBlackEffect <= 1.0) {
                    fadeBlackEffect += 0.1;
                } else {
                    fadeBlackNormal  = false;
                    fadeBlackReverse = true;
                    sprintf(rdt, "resource/stages/re1/ROOM%X%02X0.RDT", (STAGE(debugR[debug_jRoomNum].roomNum) > 0 ? STAGE(debugR[debug_jRoomNum].roomNum) : 1),
                        ROOM(debugR[debug_jRoomNum].roomNum));

                    sprintf(bss, "resource/stages/re1/ROOM%X%02X.BSS", (STAGE(debugR[debug_jRoomNum].roomNum) > 0 ? STAGE(debugR[debug_jRoomNum].roomNum) : 1),
                        ROOM(debugR[debug_jRoomNum].roomNum));
                    std::cout << bss << std::endl;
                    std::cout << rdt << std::endl;

                     Load BSS and RDT 
                    background_Loader(bss);
                     Set Player new X,Y,Z 
                    mainPlayer.setPlayerX(debugR[debug_jRoomNum].x);
                    mainPlayer.setPlayerY(debugR[debug_jRoomNum].y);
                    mainPlayer.setPlayerZ(debugR[debug_jRoomNum].z);
                    playerRDT.rdtRE1LoadFile(rdt);
                    mainPlayer.setPlayerCam(playerRDT.rdtRE1CameraSwitch[0].from);

                }
                timer1_start = (glutGet(GLUT_ELAPSED_TIME) + 50);
            }    
        } else {
            if (timer1_start < glutGet(GLUT_ELAPSED_TIME)) {
                if (fadeBlackEffect >  0.0) {
                    fadeBlackEffect -= 0.1;
                } else {
                    fadeBlackEffect = 0.0;
                    fadeBlackReverse = false;
                    fadeBlackNormal  = false;
                    inFadeBlack = false;
                    jumpToRun = false;
                    gameState = STATE_IN_GAME;
                }

                timer1_start = (glutGet(GLUT_ELAPSED_TIME) + 50);
            }
        }
    }
    */
}

/* Here all stuff related with new room/stage goes here */
void eventSystem_newRoom() {


    /* Fade black effect 
    if (inFadeBlack) {
        if (fadeBlackNormal) {
            if (timer1_start < glutGet(GLUT_ELAPSED_TIME)) {
                if (fadeBlackEffect <= 1.0) {
                    fadeBlackEffect += 0.1;
                } else {
                    fadeBlackNormal  = false;
                    fadeBlackReverse = true;
                    printf("valor que eu quero: %d\n", playerRDT.door_set_re1[roomNum].next_stage_and_room);
                    sprintf(rdt, "resource/stages/re1/ROOM%X%02X0.RDT", (STAGE(playerRDT.door_set_re1[roomNum].next_stage_and_room) > 0 ? STAGE(playerRDT.door_set_re1[roomNum].next_stage_and_room) : 1),
                        ROOM(playerRDT.door_set_re1[roomNum].next_stage_and_room));

                    sprintf(bss, "resource/stages/re1/ROOM%X%02X.BSS", (STAGE(playerRDT.door_set_re1[roomNum].next_stage_and_room) > 0 ? STAGE(playerRDT.door_set_re1[roomNum].next_stage_and_room) : 1),
                        ROOM(playerRDT.door_set_re1[roomNum].next_stage_and_room));


                    std::cout << bss << std::endl;
                    std::cout << rdt << std::endl;

                     Load BSS and RDT 
                    background_Loader(bss);
                     Set Player new X,Y,Z 
                    mainPlayer.setPlayerX(playerRDT.door_set_re1[roomNum].next_x);
                    mainPlayer.setPlayerY(playerRDT.door_set_re1[roomNum].next_y);
                    mainPlayer.setPlayerZ(playerRDT.door_set_re1[roomNum].next_z);
                    playerRDT.rdtRE1LoadFile(rdt);
                    mainPlayer.setPlayerCam(playerRDT.rdtRE1CameraSwitch[0].from);

                }
                timer1_start = (glutGet(GLUT_ELAPSED_TIME) + 50);
            }    
        } else {
            if (timer1_start < glutGet(GLUT_ELAPSED_TIME)) {
                if (fadeBlackEffect >  0.0) {
                    fadeBlackEffect -= 0.1;
                } else {
                    fadeBlackEffect = 0.0;
                    fadeBlackReverse = false;
                    fadeBlackNormal  = false;
                    inFadeBlack = false;
                    gameState = STATE_IN_GAME;
                }

                timer1_start = (glutGet(GLUT_ELAPSED_TIME) + 50);
            }
        }
    }
    */
}


void gameCore::eventSystem_downKey(int key, int x, int y) {

    switch (gameState) {
        case STATE_MAIN_MENU:
            if (mainMenu == MAIN_MENU_START) {
                switch (key) {
                    case GLUT_KEY_UP:
                        soundEngine.enginePlaySoundEffect(mixList[0]);
                        if (menuArrow == 1)
                            menuArrow = 0;
                    break;

                    case GLUT_KEY_DOWN:
                        soundEngine.enginePlaySoundEffect(mixList[0]);
                        if (menuArrow == 0)
                            menuArrow = 1;
                    break;

                    default:

                    break;
                }
            }
        break;


        /*
         * Character Selection Menu
         */

        case STATE_SEL_CHAR: {
            case GLUT_KEY_LEFT:
                if (!charNextStep) {
                    if (!nextCharTransition) { 
                        soundEngine.enginePlaySoundEffect(mixList[2]);
                        menuArrow ^= 1;
                        nextCharTransition = true;
                    }
                }
            break;

            case GLUT_KEY_RIGHT:
                if (!charNextStep) {
                    if (!nextCharTransition) {
                        soundEngine.enginePlaySoundEffect(mixList[2]);
                        menuArrow ^= 1;
                        nextCharTransition = true;
                    }
                }
            break;            
        }
        break;


        case STATE_IN_DEBUG:
            switch (key) {
                case GLUT_KEY_LEFT:
                    if (debug_jRoomNum > 0) 
                        debug_jRoomNum--;
                break;

                case GLUT_KEY_RIGHT:
                    if (debug_jRoomNum < 2)
                        debug_jRoomNum++;
                break;

                default:

                break;
            }
        break;

        case STATE_IN_GAME:
            switch (key) {
                case GLUT_KEY_UP:
                    eventSystem_gameAction(EVENT_SYSTEM_KEY_UP, true);
                break;

                case GLUT_KEY_DOWN:
                    eventSystem_gameAction(EVENT_SYSTEM_KEY_DOWN, true);                    
                break;

                case GLUT_KEY_LEFT:
                    eventSystem_gameAction(EVENT_SYSTEM_KEY_LEFT, true);
                break;

                case GLUT_KEY_RIGHT:
                    eventSystem_gameAction(EVENT_SYSTEM_KEY_RIGHT, true);
                break;
            }
        break;


        default:

        break;
    }
}

void gameCore::eventSystem_upKey(int key, int x, int y) {
    switch (gameState) {
        case STATE_MAIN_MENU:

        break;

        case STATE_IN_GAME:
           switch (key) {
                case GLUT_KEY_UP:
                    eventSystem_gameAction(EVENT_SYSTEM_KEY_UP, false);
                break;

                case GLUT_KEY_DOWN:
                    eventSystem_gameAction(EVENT_SYSTEM_KEY_DOWN, false);                    
                break;

                case GLUT_KEY_LEFT:
                    eventSystem_gameAction(EVENT_SYSTEM_KEY_LEFT, false);
                break;

                case GLUT_KEY_RIGHT:
                    eventSystem_gameAction(EVENT_SYSTEM_KEY_RIGHT, false);
                break;

            }
        break;

        default:

        break;
    }


}

void gameCore::eventSystem_keyboardDown(unsigned char key, int x, int y) {

    switch (gameState) {
        /*
         * This is MainMenu State
         */
        case STATE_MAIN_MENU: {
            if (mainMenu == MAIN_MENU_START) {
                if (key == 'x') {
                    if (menuArrow == 0) {
                        soundEngine.enginePlaySoundEffect(mixList[1]);
                        mainMenu = MAIN_MENU_GAME;
                        miscStuff.setupFadeEffect(TYPE_FADE_SPECIAL, 0, 0, 0, 70);
                        glutIgnoreKeyRepeat(1);    
                    } else if (menuArrow == 1) {
                        exit(0);
                    }
                }
            }
        }
        break;

        case STATE_SEL_CHAR:
            if (key == 0x9) {
                charMusicNum > 2 ? charMusicNum = 0 : charMusicNum++;
            }

            if (key == 'x') {
                if (!charNextStep) {
                    miscStuff.setupFadeEffect(TYPE_FADE_SPECIAL, 0, 0, 0, 70);
                    mainPlayer.setPlayerAnimSection(EMD_SECTION_2);
                    mainPlayer.setPlayerAnim(SPECIAL_SEC2_ANIM_CHOICE, false);
                    selectState = SEL_PLAYER_START;
                    charNextStep = true;
                }
              }
        break;

        case STATE_IN_DEBUG:
            switch (key) {
                case 'z':
                    gameState = STATE_IN_GAME;
                break;

                case 'x':
                    jumpToRun = true;
                break;

                default:

                break;
            }
        break;

        case STATE_IN_GAME:
            switch (key) {
                case 'x':
                    eventSystem_gameAction(EVENT_SYSTEM_KEY_X, true);
                break;

                case 'z':
                    eventSystem_gameAction(EVENT_SYSTEM_KEY_Z, true);
                break;

                case 'c':
                    eventSystem_gameAction(EVENT_SYSTEM_KEY_C, true);
                break;

                case 'a':

                break;

                default:

                break;
            }
        break;

        default:

        break;
    }
}



void gameCore::eventSystem_keyboardUp(unsigned char key, int x, int y) {
    switch (key) {
        case 'z':

        break;

        case 'x':

        break;

        default:

        break;
    }
}

/* 
 * engineAnimation
 * Here all game animation is handled
 */
void gameCore::engineAnimation() {
    if (coreTmr_anim <= SDL_GetTicks()) {
        coreTmr_anim = SDL_GetTicks() + 24;

        /*
         * Verify if is the same animation, if not, should interpolate the animation
         */

        if ((inInterpolation == 0) && ((oldAnim != mainPlayer.getPlayerAnim()) 
                          || (oldSection != mainPlayer.getPlayerAnimSection()))) {

            if ((oldSection == EMD_SECTION_4) && (oldAnim == STAND_SEC4_ANIM_IDLE)) {
                if ((mainPlayer.getPlayerAnim() == STAND_SEC2_ANIM_BACKWARD) && (mainPlayer.getPlayerAnimSection() == EMD_SECTION_2)) {
                    inInterpolation = 1;
                }
            }

        }

        switch (inInterpolation) {
            /*
             * play normal animations 
             */
            case 0: {
                switch (mainPlayer.getPlayerAnimSection()) {
                    case EMD_SECTION_2: {
                        if (mainPlayer.getPlayerAnimCount() < (modelList[mainPlayer.getPlayerEMD()].emdSec2AnimInfo[mainPlayer.getPlayerAnim()].animCount-1))
                            mainPlayer.setPlayerAnimCount(mainPlayer.getPlayerAnimCount() + 1);
                        else {
                            if (mainPlayer.getPlayerRepeatAnim()) {
                                mainPlayer.setPlayerAnimCount(0);
                            }
                        }

                        mainPlayer.setPlayerEMDAnim(modelList[mainPlayer.getPlayerEMD()].emdSec2Data[mainPlayer.getPlayerAnimCount()+modelList[mainPlayer.getPlayerEMD()].emdSec2AnimInfo[mainPlayer.getPlayerAnim()].animStart]);
                    }
                    break;

                    case EMD_SECTION_4: {
                        if (mainPlayer.getPlayerAnimCount() < (modelList[mainPlayer.getPlayerEMD()].emdSec4AnimInfo[mainPlayer.getPlayerAnim()].animCount-1))
                            mainPlayer.setPlayerAnimCount(mainPlayer.getPlayerAnimCount() + 1);
                        else {
                            if (mainPlayer.getPlayerRepeatAnim()) {
                                mainPlayer.setPlayerAnimCount(0);
                            }
                        }

                        mainPlayer.setPlayerEMDAnim(modelList[mainPlayer.getPlayerEMD()].emdSec4Data[mainPlayer.getPlayerAnimCount()+modelList[mainPlayer.getPlayerEMD()].emdSec4AnimInfo[mainPlayer.getPlayerAnim()].animStart]);
                    }
                    break;
                 }
            }
            break;

            /* 
             * does the interpolation
             */
            case 1: {
                interAnimation.clear();
                switch (oldSection) {
                    case EMD_SECTION_2: {

                        EMD_SEC2_DATA_T  node;   // new animation (interpolated)
                        EMD_SEC2_DATA_T *node_2; // old animation
                        EMD_SEC2_DATA_T *node_3; // new animation
                        unsigned int     i_2      = 0;

                        float p_factor = (1.0f / ((modelList[mainPlayer.getPlayerEMD()].emdSec2AnimInfo[oldAnim].animCount-1) - mainPlayer.getPlayerAnimCount()));
                        float p = 0.0f;

                        for (unsigned int i = mainPlayer.getPlayerAnimCount(); i < (modelList[mainPlayer.getPlayerEMD()].emdSec2AnimInfo[oldAnim].animCount-1); i++, p += p_factor) {
                            node_2 = &modelList[mainPlayer.getPlayerEMD()].emdSec2Data[i+modelList[mainPlayer.getPlayerEMD()].emdSec2AnimInfo[oldAnim].animStart];
                            
                            switch (mainPlayer.getPlayerAnimSection()) {
                                case EMD_SECTION_2:
                                    node_3 = &modelList[mainPlayer.getPlayerEMD()].emdSec2Data[i_2+modelList[mainPlayer.getPlayerEMD()].emdSec2AnimInfo[mainPlayer.getPlayerAnim()].animStart];

                                    for (unsigned int t = 0; t <  modelList[mainPlayer.getPlayerEMD()].emdTotalObj; t++) {

                                       node.vector[t].x = mathEngine.interpolation(node_2->vector[t].x, node_3->vector[t].x, p);
                                       node.vector[t].y = mathEngine.interpolation(node_2->vector[t].y, node_3->vector[t].y, p);
                                       node.vector[t].z = mathEngine.interpolation(node_2->vector[t].z, node_3->vector[t].z, p);
                                    }

                                    if (i_2 < (modelList[mainPlayer.getPlayerEMD()].emdSec2AnimInfo[mainPlayer.getPlayerAnim()].animCount-1))
                                        i_2++;
                                    else
                                        i_2 = 0;

                                    interAnimation.push_back(node);

                                break;

                                case EMD_SECTION_4:
                                    node_3 = &modelList[mainPlayer.getPlayerEMD()].emdSec4Data[i_2+modelList[mainPlayer.getPlayerEMD()].emdSec4AnimInfo[mainPlayer.getPlayerAnim()].animStart];
            
                                    for (unsigned int t = 0; t <  modelList[mainPlayer.getPlayerEMD()].emdTotalObj; t++) {
                                       node.vector[t].x = mathEngine.interpolation(node_2->vector[t].x, node_3->vector[t].x, p);
                                       node.vector[t].y = mathEngine.interpolation(node_2->vector[t].y, node_3->vector[t].y, p);
                                       node.vector[t].z = mathEngine.interpolation(node_2->vector[t].z, node_3->vector[t].z, p);
                                    }

                                    if (i_2 < (modelList[mainPlayer.getPlayerEMD()].emdSec4AnimInfo[mainPlayer.getPlayerAnim()].animCount-1))
                                        i_2++;
                                    else
                                        i_2 = 0;
                                    
                                    interAnimation.push_back(node);
                                break;
                            }                    
                        }
                        mainPlayer.setPlayerAnimCount(0);
                        inInterpolation = 2;
                    }
                    break;

                    case EMD_SECTION_4: {
                        EMD_SEC2_DATA_T  node;   // new animation (interpolated)
                        EMD_SEC2_DATA_T *node_2; // old animation
                        EMD_SEC2_DATA_T *node_3; // new animation
                        unsigned int     i_2    = 0;
                        float p_factor = (1.0f / 3);
                        float p = 0.0f;

                        printf("p_factor: %.2f\n", p_factor);
                        for (unsigned int i = 0; i <= 3; i++, p += p_factor) {
                            node_2 = &modelList[mainPlayer.getPlayerEMD()].emdSec4Data[i+modelList[mainPlayer.getPlayerEMD()].emdSec4AnimInfo[oldAnim].animStart];
                            
                            switch (mainPlayer.getPlayerAnimSection()) {
                                case EMD_SECTION_2:
                                    node_3 = &modelList[mainPlayer.getPlayerEMD()].emdSec2Data[i_2+modelList[mainPlayer.getPlayerEMD()].emdSec2AnimInfo[mainPlayer.getPlayerAnim()].animStart];

                                    for (unsigned int t = 0; t <  modelList[mainPlayer.getPlayerEMD()].emdTotalObj; t++) {
                                       node.vector[t].x = mathEngine.interpolation(node_2->vector[t].x, node_3->vector[t].x, 0.01f);
                                       printf("node.x: %.2f node_2.x %.2f node_3.x %.2f p: %.2f\n", node.vector[t].x, node_2->vector[t].x, node_3->vector[t].x, p);
                                       node.vector[t].y = mathEngine.interpolation(node_2->vector[t].y, node_3->vector[t].y, 0.01f);
                                       printf("node.y: %.2f node_2.y %.2f node_3.y %.2f p: %.2f\n", node.vector[t].y, node_2->vector[t].y, node_3->vector[t].y, p);
                                       node.vector[t].z = mathEngine.interpolation(node_2->vector[t].z, node_3->vector[t].z, 0.01f);
                                       printf("node.z: %.2f node_2.z %.2f node_3.z %.2f p: %.2f\n", node.vector[t].z, node_2->vector[t].z, node_3->vector[t].z, p);    
                                    }

                                    if (i_2 < (modelList[mainPlayer.getPlayerEMD()].emdSec2AnimInfo[mainPlayer.getPlayerAnim()].animCount-1))
                                        i_2++;
                                    else
                                        i_2 = 0;

                                    interAnimation.push_back(node);

                                break;

                                case EMD_SECTION_4:
                                    node_3 = &modelList[mainPlayer.getPlayerEMD()].emdSec4Data[i_2+modelList[mainPlayer.getPlayerEMD()].emdSec4AnimInfo[mainPlayer.getPlayerAnim()].animStart];
            
                                    for (unsigned int t = 0; t <  modelList[mainPlayer.getPlayerEMD()].emdTotalObj; t++) {
                                       node.vector[t].x = mathEngine.interpolation(node_3->vector[t].x, node_2->vector[t].x, p);
                                       node.vector[t].y = mathEngine.interpolation(node_3->vector[t].y, node_2->vector[t].y, p);
                                       node.vector[t].z = mathEngine.interpolation(node_3->vector[t].z, node_2->vector[t].z, p);
                                        }

                                    if (i_2 < (modelList[mainPlayer.getPlayerEMD()].emdSec4AnimInfo[mainPlayer.getPlayerAnim()].animCount-1))
                                        i_2++;
                                    else
                                        i_2 = 0;
                                    
                                    interAnimation.push_back(node);
                                break;
                            }                    
                        }

                        mainPlayer.setPlayerAnimCount(0);
                        inInterpolation = 2;
                    }
                    break;
                }
            }
            break;

            /*
             * play the interpolated animation
             */
            case 2:
            if (mainPlayer.getPlayerAnimCount() < interAnimation.size())
                    mainPlayer.setPlayerAnimCount(mainPlayer.getPlayerAnimCount() + 1);
                else {
      
                    oldAnim         = mainPlayer.getPlayerAnim();
                    oldSection      = mainPlayer.getPlayerAnimSection();
                    inInterpolation = 0;
                    switch (mainPlayer.getPlayerAnimSection()) {
                        case EMD_SECTION_2:
                            if (mainPlayer.getPlayerAnimCount() > (modelList[mainPlayer.getPlayerEMD()].emdSec2AnimInfo[mainPlayer.getPlayerAnim()].animCount-1)) {
                                mainPlayer.setPlayerAnimCount(0);
                            }
                        break;

                        case EMD_SECTION_4:
                            if (mainPlayer.getPlayerAnimCount() > (modelList[mainPlayer.getPlayerEMD()].emdSec4AnimInfo[mainPlayer.getPlayerAnim()].animCount-1)) {
                                mainPlayer.setPlayerAnimCount(0);
                            }
                        break;
                    }
               
                }     

                mainPlayer.setPlayerEMDAnim(interAnimation[mainPlayer.getPlayerAnim()]);
            break;


        }
        
    }
}

void MainLoop() {
    bool canMove = true;

    if (tmr60FPS < SDL_GetTicks()) {
        tmr60FPS = (SDL_GetTicks() + (1000/60));

        switch (gameState) {
            case STATE_SEL_CHAR:
                core->engineAnimation();
            break;

            case STATE_IN_GAME: {
                core->engineAnimation();
                /* Enemy AI Stuff */
                enemyAI_followPlayer();


                switch (mainPlayer.getPlayerInRotation()) {
                    case PLAYER_ACTION_R_LEFT:
                        mainPlayer.setPlayerAngle(fmod((mainPlayer.getPlayerAngle() - 2), 360.0));
                    break;

                    case PLAYER_ACTION_R_RIGHT:
                        mainPlayer.setPlayerAngle(fmod((mainPlayer.getPlayerAngle() + 2), 360.0));
                    break;

                    case PLAYER_ACTION_R_NONE:

                    break;
                }

                if ((core->keyList[0] == true)) {
                    if (mainPlayer.getPlayerAnim() == STAND_SEC4_ANIM_WALK) {
                        mainPlayer.setPlayerX(mainPlayer.getPlayerX() + cos((mainPlayer.getPlayerAngle() * PI/180)) * 80.0);
                        mainPlayer.setPlayerZ(mainPlayer.getPlayerZ() - sin((mainPlayer.getPlayerAngle() * PI/180)) * 80.0);
                    }
                } else if ((core->keyList[1] == true)) {
                    if (mainPlayer.getPlayerAnim() == STAND_SEC2_ANIM_BACKWARD) {
                        mainPlayer.setPlayerX(mainPlayer.getPlayerX() - cos((mainPlayer.getPlayerAngle() * PI/180)) * 80.0);
                        mainPlayer.setPlayerZ(mainPlayer.getPlayerZ() + sin((mainPlayer.getPlayerAngle() * PI/180)) * 80.0);
                    }
                }

                    /* Collision Detection RE 1 
                    for (unsigned int i = 0; i < playerRDT.rdtRE1ColissionArray.size(); i++) {
                            if (mathEngine.collisionDetect(playerRDT.rdtRE1ColissionArray[i].type, playerRDT.rdtRE1ColissionArray[i].x1, playerRDT.rdtRE1ColissionArray[i].z1, 
                                                     playerRDT.rdtRE1ColissionArray[i].x2, playerRDT.rdtRE1ColissionArray[i].z2, mainPlayer.getPlayerX() + x, mainPlayer.getPlayerZ() - z) == true) {
                                canMove = false;

                                printf("Type: %d\n", playerRDT.rdtRE1ColissionArray[i].type);
                                printf("X1: %d\n",playerRDT.rdtRE1ColissionArray[i].x1);
                                printf("Z1: %d\n",playerRDT.rdtRE1ColissionArray[i].z1);
                                printf("X2: %d\n",playerRDT.rdtRE1ColissionArray[i].x2);
                                printf("Z2: %d\n",playerRDT.rdtRE1ColissionArray[i].z2);
                        }
                    } 
                    */

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

                    /* Camera Switch Zone/Camera Position RE 1.0 
                    for (unsigned int i = 0; i < playerRDT.rdtRE1CameraSwitch.size(); i++) {
                        if (mathEngine.mapSwitch(mainPlayer.getPlayerX(), mainPlayer.getPlayerZ(), playerRDT.rdtRE1CameraSwitch[i].x1, playerRDT.rdtRE1CameraSwitch[i].z1, playerRDT.rdtRE1CameraSwitch[i].x2, playerRDT.rdtRE1CameraSwitch[i].z2, 
                                                 playerRDT.rdtRE1CameraSwitch[i].x3, playerRDT.rdtRE1CameraSwitch[i].z3, playerRDT.rdtRE1CameraSwitch[i].x4, playerRDT.rdtRE1CameraSwitch[i].z4)) {
                            
                                if (playerRDT.rdtRE1CameraSwitch[i].to != 9) {
                                     mainPlayer.setPlayerCam(playerRDT.rdtRE1CameraSwitch[i].to);
                                }

                        } 
                    }                    
                    */


            }
            break;

            default:

            break;
        }

        if (gameState  == STATE_SEL_CHAR) {

            /*
             * Player Animation
             */
            core->engineAnimation();
        }

        if ((gameState == STATE_IN_GAME) || (gameState == STATE_IN_ROOM)) {

            if (gameState == STATE_IN_ROOM) 
                eventSystem_newRoom();

        }
        glutPostRedisplay();   
    }
}


/*
 * renderCamera
 * This function handle all cameras used in game
 */
void gameCore::renderCamera() {

    switch (gameState) {
        case STATE_SEL_CHAR: {

            float camX = 0  - sin((mainPlayer.getPlayerAngle() * PI/180)) * 800.0f;
            float camZ = 0  - cos((mainPlayer.getPlayerAngle() * PI/180)) * 800.0f;
            float distX =    -cos((mainPlayer.getPlayerAngle() * PI/180))  * 3500.0;
            float distZ =     sin((mainPlayer.getPlayerAngle() * PI/180))  * 3500.0;
            float distY = -2200.0f;
            gluLookAt(     camX + distX,  distY, camZ + distZ,
                           camX, -2600.0f, camZ,   
                           0.0f, -0.1f, 0.0f);

        }            
        break;

        case STATE_IN_GAME: {

            /*
             * Resident Evil 1.5 and 2 Cameras
             */
            /*
            gluLookAt(     playerRDT.rdtCameraPos[mainPlayer.getPlayerCam()].positionX, playerRDT.rdtCameraPos[mainPlayer.getPlayerCam()].positionY, playerRDT.rdtCameraPos[mainPlayer.getPlayerCam()].positionZ,
                           playerRDT.rdtCameraPos[mainPlayer.getPlayerCam()].targetX, playerRDT.rdtCameraPos[mainPlayer.getPlayerCam()].targetY, playerRDT.rdtCameraPos[mainPlayer.getPlayerCam()].targetZ,   
                           0.0f,   -0.1f,   0.0f);
            */

            /*
             * Resident Evil 1 Camera
             */
            /*  gluLookAt(     playerRDT.rdtRE1CameraPos[mainPlayer.getPlayerCam()].positionX, playerRDT.rdtRE1CameraPos[mainPlayer.getPlayerCam()].positionY, playerRDT.rdtRE1CameraPos[mainPlayer.getPlayerCam()].positionZ,
                           playerRDT.rdtRE1CameraPos[mainPlayer.getPlayerCam()].targetX, playerRDT.rdtRE1CameraPos[mainPlayer.getPlayerCam()].targetY, playerRDT.rdtRE1CameraPos[mainPlayer.getPlayerCam()].targetZ,   
                           0.0f,   -0.1f,   0.0f);
             */

            /*
             * Resident Evil Extreme Battle Camera System
             * Cam 1
             * Written by St4rk
             */

            float camX = mainPlayer.getPlayerX()  - sin((mainPlayer.getPlayerAngle() * PI/180)) * 800.0f;
            float camZ = mainPlayer.getPlayerZ()  - cos((mainPlayer.getPlayerAngle() * PI/180)) * 800.0f;
            float distX = -cos((mainPlayer.getPlayerAngle() * PI/180))  * 3500.0;
            float distZ =  sin((mainPlayer.getPlayerAngle() * PI/180))  * 3500.0;
            float distY = -3000.0f;
            gluLookAt(     camX + distX,  distY, camZ + distZ,
                           camX, -2500.0f, camZ,   
                           0.0f, -0.1f, 0.0f);
                }
        break;
    }
}

/*
 * eventSystem -> gameAction
 * this function is used to handle the keys pressed that change the player animation as well as 
 * the game actions
 */

void gameCore::eventSystem_gameAction(unsigned int key, bool pressed) {
    switch (key) {

        case EVENT_SYSTEM_KEY_UP: {
            if (pressed) {
                keyList[0] = true;
                mainPlayer.setPlayerAnimSection(EMD_SECTION_4);
                mainPlayer.setPlayerAnim(STAND_SEC4_ANIM_WALK);
            } else {
                keyList[0] = false;
                mainPlayer.setPlayerAnimSection(EMD_SECTION_4);
                mainPlayer.setPlayerAnim(STAND_SEC4_ANIM_IDLE);
            }
        }
        break;

        case EVENT_SYSTEM_KEY_DOWN:
            if (pressed) {
                keyList[1] = true;
                mainPlayer.setPlayerAnimSection(EMD_SECTION_2);
                mainPlayer.setPlayerAnim(STAND_SEC2_ANIM_BACKWARD);
            } else {
                keyList[1] = false;
                mainPlayer.setPlayerAnimSection(EMD_SECTION_4);
                mainPlayer.setPlayerAnim(STAND_SEC4_ANIM_IDLE);
            }
        break;

        case EVENT_SYSTEM_KEY_LEFT:
            if (pressed) {
                if ((keyList[0] == false) && (keyList[1] == false)) {
                    mainPlayer.setPlayerAnimSection(EMD_SECTION_4);
                    mainPlayer.setPlayerAnim(STAND_SEC4_ANIM_WALK);
                    mainPlayer.setPlayerInRotation(PLAYER_ACTION_R_LEFT);
                } else {
                    mainPlayer.setPlayerInRotation(PLAYER_ACTION_R_LEFT);
                }
            } else {
                if ((keyList[0] == false) && (keyList[1] == false)) 
                    mainPlayer.setPlayerAnim(STAND_SEC4_ANIM_IDLE);
                
                mainPlayer.setPlayerInRotation(PLAYER_ACTION_R_NONE);
            }
        break;

        case EVENT_SYSTEM_KEY_RIGHT:
            if (pressed) {
                if ((keyList[0] == false) && (keyList[1] == false)) {
                    mainPlayer.setPlayerAnimSection(EMD_SECTION_4);
                    mainPlayer.setPlayerAnim(STAND_SEC4_ANIM_WALK);
                    mainPlayer.setPlayerInRotation(PLAYER_ACTION_R_RIGHT);
                } else {
                    mainPlayer.setPlayerInRotation(PLAYER_ACTION_R_RIGHT);
                }
            } else {
                if ((keyList[0] == false) && (keyList[1] == false)) 
                    mainPlayer.setPlayerAnim(STAND_SEC4_ANIM_IDLE);
                
                mainPlayer.setPlayerInRotation(PLAYER_ACTION_R_NONE);
            }
        break;

        case EVENT_SYSTEM_KEY_X:

        break;

        case EVENT_SYSTEM_KEY_Z:

        break;

        case EVENT_SYSTEM_KEY_C:

        break;

    }
}

/* I had some problems with collision, so I decide to draw the collisions wire-frame
it will help me to figure if the collions boundaries are right or wrong */
void renderCollision() {

    glDisable(GL_LIGHTING);
    glDisable(GL_TEXTURE_2D);
    glLoadIdentity();
    gluLookAt(     playerRDT.rdtRE1CameraPos[mainPlayer.getPlayerCam()].positionX, playerRDT.rdtRE1CameraPos[mainPlayer.getPlayerCam()].positionY, playerRDT.rdtRE1CameraPos[mainPlayer.getPlayerCam()].positionZ,
                   playerRDT.rdtRE1CameraPos[mainPlayer.getPlayerCam()].targetX, playerRDT.rdtRE1CameraPos[mainPlayer.getPlayerCam()].targetY, playerRDT.rdtRE1CameraPos[mainPlayer.getPlayerCam()].targetZ,   
                   0.0f,   -0.1f,   0.0f);

    // Enable wireframe mode
    glPolygonMode(GL_FRONT, GL_LINE);

 
    for (unsigned int i = 0; i < playerRDT.rdtRE1ColissionArray.size(); i++) {
        if (playerRDT.rdtRE1ColissionArray[i].type == 1) {
            /* Wireframe collision */
            glBegin(GL_QUADS);      
                glColor3f(1.0f, 0.0f, 0.0f);                  
                glVertex3f(playerRDT.rdtRE1ColissionArray[i].x1, 0x0,  playerRDT.rdtRE1ColissionArray[i].z1);          
                glVertex3f((playerRDT.rdtRE1ColissionArray[i].x1 + playerRDT.rdtRE1ColissionArray[i].x2), 0x0,  playerRDT.rdtRE1ColissionArray[i].z1);              
                glVertex3f((playerRDT.rdtRE1ColissionArray[i].x1 + playerRDT.rdtRE1ColissionArray[i].x2), 0x0,  (playerRDT.rdtRE1ColissionArray[i].z1 + playerRDT.rdtRE1ColissionArray[i].z2));   
                glVertex3f(playerRDT.rdtRE1ColissionArray[i].x1, 0x0,  (playerRDT.rdtRE1ColissionArray[i].z1 + playerRDT.rdtRE1ColissionArray[i].z2));       
            glEnd();
        }
    }

    /* Wireframe collision */
    glBegin(GL_QUADS);      
        glColor3f(1.0f, 0.0f, 0.0f);                  
        glVertex3f(mainPlayer.getPlayerX(), 0x0,  mainPlayer.getPlayerZ());             
        glVertex3f((mainPlayer.getPlayerX() + 1280), 0x0,  mainPlayer.getPlayerZ());              
        glVertex3f((mainPlayer.getPlayerX() + 1280), 0x0,  (mainPlayer.getPlayerZ() + 1280));   
        glVertex3f(mainPlayer.getPlayerX(), 0x0,  (mainPlayer.getPlayerZ() + 1280));       
    glEnd();

    // disable wireframe
    glPolygonMode(GL_FRONT, GL_FILL);

    glEnable(GL_TEXTURE_2D);
    glEnable(GL_LIGHTING);


}

/* This function is responsible for all emd animation */
void renderEMD_modelAnimation(unsigned int objNum, unsigned int var, int var2, EMD_SEC2_DATA_T animFrame, unsigned int emdNum) {
    if (var == objNum) {
        glTranslatef((float)modelList[emdNum].emdSec2RelPos[var2].x, (float)modelList[emdNum].emdSec2RelPos[var2].y, (float)modelList[emdNum].emdSec2RelPos[var2].z);
        glRotatef(animFrame.vector[var2].x, 1.0f, 0.0f, 0.0f);
        glRotatef(animFrame.vector[var2].y, 0.0f, 1.0f, 0.0f);
        glRotatef(animFrame.vector[var2].z, 0.0f, 0.0f, 1.0f);
    }



    /*
     * need to fix this bug
     */

    for (unsigned int c = 0; c < modelList[emdNum].emdSec2Armature[var].meshCount; c++) {
        renderEMD_modelAnimation(objNum, modelList[emdNum].emdSec2Mesh[var][c], var2, animFrame, emdNum);
    }
}

/*
 * renderEMD
 * Resident Evil 1.5 and Resident Evil 2 model render
 */
void renderEMD(float m_x, float m_y, float m_z, float angle, unsigned int emdNum, EMD_SEC2_DATA_T animFrame) {
    float width_t  = (float) (modelList[emdNum].emdTimTexture.timTexture.imageWidth*2);
    float height_t = (float)  modelList[emdNum].emdTimTexture.timTexture.imageHeight;

    unsigned short uPage = 0;

    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
    glTexImage2D(GL_TEXTURE_2D, 0,GL_RGB, (modelList[emdNum].emdTimTexture.timTexture.imageWidth*2), modelList[emdNum].emdTimTexture.timTexture.imageHeight, 0,GL_RGB, GL_UNSIGNED_BYTE, modelList[emdNum].emdTimTexture.rawTexture);


    // Toda a renderização do personagem vai ficar aqui !
    for (unsigned int x = 0; x < modelList[emdNum].emdTotalObj; x++) {
        
        glLoadIdentity();        
            
        core->renderCamera();

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

/*
 * renderMapBackground
 * This function render a square with .bss texture of the current camera
 */
void drawMapBackground() {
    glDepthMask(GL_FALSE);
    glDisable(GL_LIGHTING);
    
    glLoadIdentity();
    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
    glTexImage2D(GL_TEXTURE_2D, 0,GL_RGB, bg[mainPlayer.getPlayerCam()]->w, bg[mainPlayer.getPlayerCam()]->h, 0,GL_RGB, GL_UNSIGNED_BYTE, bg[mainPlayer.getPlayerCam()]->pixels);

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
    glDepthMask(GL_TRUE);

    
}

/*
 * renderMainMenu
 * This function is used to render all image and text stuff used on intro
 */
void gameCore::renderMainMenu() {

       switch (mainMenu) {
            case MAIN_MENU_ENGINE_LOGO:
                if (miscStuff.isInFade()) {
                    miscStuff.renderSquareWithTexture(&engineLogo, false);
                } else {
                    miscStuff.setupFadeEffect(TYPE_FADE_OUT_AND_IN, 0, 0, 0, 60);
                    mainMenu = MAIN_MENU_LOGO;
                }
            break;

            case MAIN_MENU_LOGO: 
                if (miscStuff.isInFade()) {
                    miscStuff.renderSquareWithTexture(&engineThisGame, false);
                } else {
                    miscStuff.setupFadeEffect(TYPE_FADE_OUT, 0, 0, 0, 60);
                    mainMenu = MAIN_MENU_START;
                }
            break;


            case MAIN_MENU_START:
                miscStuff.renderSquareWithTexture(&engineMainMenu, false);
                if (!miscStuff.isInFade()) {
                    if (menuArrow == 0) {
                        renderText(0.60, 0.70, 0.0, TEXT_TYPE_LITTLE, "NEW GAME", 0.0f, 1.0f, 0.0f, 1.0f);
                        renderText(0.70, 0.80, 0.0, TEXT_TYPE_LITTLE, "EXIT");
                    } else if (menuArrow == 1) {
                        renderText(0.70, 0.80, 0.0, TEXT_TYPE_LITTLE, "EXIT", 0.0f, 1.0f, 0.0f, 1.0f);
                        renderText(0.60, 0.70, 0.0, TEXT_TYPE_LITTLE, "NEW GAME");
                    }
                }
            break;


           case MAIN_MENU_GAME:
                if (miscStuff.isInFade()) {
                    miscStuff.renderSquareWithTexture(&engineMainMenu, false);
                } else {
                    miscStuff.setupFadeEffect(TYPE_FADE_OUT, 0, 0, 0, 60);
                    gameState   = STATE_SEL_CHAR;
                    selectState = SEL_PLAYER_BEGIN; 
                    menuArrow   = 0x0;
                    oldAnim     = SPECIAL_SEC2_ANIM_POSE;
                    oldSection  = EMD_SECTION_2;
                    mainPlayer.setPlayerAnimSection(EMD_SECTION_2);
                    mainPlayer.setPlayerAnim(SPECIAL_SEC2_ANIM_POSE);
                    soundEngine.enginePlaySound(1);
                }
           break;

           default:
              std::cout << "Main Menu System bug !" << std::endl;
           break;
       }


}

/*
 * renderSelectChar
 * This function is used to draw the graphical user interface 
 * and the characters to be selected.
 */

void gameCore::renderSelectChar() {
    switch (selectState) {
        case SEL_PLAYER_BEGIN: {
            float x = 0;
            float z = 0;
            miscStuff.renderSquareWithTexture(&engineSelectChar, false);
            miscStuff.renderSquareWithColor(0, 0, 0, 0.65);

            /* 
             * char transition
             */
            if (nextCharTransition) {
                switch (menuArrow) {
                    case 0:
                        if (nextCharAngle >= 360.0f) {
                            nextCharAngle      = 0.0f;
                            nextCharTransition = false;
                        }
                        else {
                            nextCharAngle += 4.0f;
                        }
                        mainPlayer.setPlayerEMD(0);
                    break;

                    case 1:
                        if (nextCharAngle >= 180.0f) {
                            nextCharTransition = false;
                        } else {
                            nextCharAngle += 4.0f;
                        }
                        mainPlayer.setPlayerEMD(3);
                    break;

                    default:

                    break;
                }
            }

            switch (menuArrow) {

                /*
                 * Leon S. Kennedy
                 */

                case 0: {

                        x = cos(((70 + nextCharAngle) * PI/180)) * 1000.0;
                        z = sin(((70 + nextCharAngle) * PI/180)) * 1500.0;

                        renderEMD(x-500, -150.0f,z, 
                            230.0f + nextCharAngle, 0, mainPlayer.getPlayerEMDAnim());

                        x = cos(((240 + nextCharAngle) * PI/180)) * 1000.0;
                        z = sin(((240 + nextCharAngle) * PI/180)) * 1500.0;
                
                        /* 
                         * Chris standard
                         */
                        renderEMD(x-500, -150.0f,z,
                            100.0f+ nextCharAngle, 3, modelList[3].emdSec2Data[modelList[3].emdSec2AnimInfo[3].animStart]);



                        renderText(0.70, 0.10, 0.0, TEXT_TYPE_LITTLE, "Leon S. Kennedy", 1.0f, 1.0f, 1.0f, 1.0f);
                        renderText(0.80, 0.20, 0.0, TEXT_TYPE_LITTLE, "From R.P.D ", 1.0f, 1.0f, 1.0f, 1.0f);
                }
                break;
                
                /*
                 * Chris Redfield
                 */
                case 1: {

                        x = cos(((70 + nextCharAngle) * PI/180)) * 1000.0;
                        z = sin(((70 + nextCharAngle) * PI/180)) * 1500.0;

                        renderEMD(x-500, -150.0f,z, 
                            270.0f + nextCharAngle, 0, modelList[0].emdSec2Data[modelList[0].emdSec2AnimInfo[0].animStart]);

                        x = cos(((240 + nextCharAngle) * PI/180)) * 1000.0;
                        z = sin(((240 + nextCharAngle) * PI/180)) * 1500.0;
                
                        /* 
                         * Chris standard
                         */
                        renderEMD(x-500, -150.0f,z, 
                            45.0f+ nextCharAngle, 3,  mainPlayer.getPlayerEMDAnim());

                        renderText(0.70, 0.10, 0.0, TEXT_TYPE_LITTLE, "Chris Redfield", 1.0f, 1.0f, 1.0f, 1.0f);
                        renderText(0.80, 0.20, 0.0, TEXT_TYPE_LITTLE, "S.T.A.R.S. ", 1.0f, 1.0f, 1.0f, 1.0f);
                        renderText(0.80, 0.25, 0.0, TEXT_TYPE_LITTLE, "Alpha Team", 1.0f, 1.0f, 1.0f, 1.0f);
                }
                break;

                default:

                break;
            }

            /*
             * Music
             */

            switch (charMusicNum) {
                case 0:
                    renderText(0.1, 0.935, 0.0, TEXT_TYPE_LITTLE, "Resident Evil 2", 1.0f, 1.0f, 1.0f, 1.0f);
                    renderText(0.1, 1.005, 0.0, TEXT_TYPE_LITTLE, "Escape Alarm Theme", 1.0f, 1.0f, 1.0f, 1.0f);
                break;

                case 1:
                    renderText(0.1, 0.935, 0.0, TEXT_TYPE_LITTLE, "Resident Evil:TDSC", 1.0f, 1.0f, 1.0f, 1.0f);
                    renderText(0.1, 1.005, 0.0, TEXT_TYPE_LITTLE, "Sorrow - Steve Battle", 1.0f, 1.0f, 1.0f, 1.0f);
                break;

                case 2:
                    renderText(0.1, 0.935, 0.0, TEXT_TYPE_LITTLE, "Resident Evil:TDSC", 1.0f, 1.0f, 1.0f, 1.0f);
                    renderText(0.1, 1.005, 0.0, TEXT_TYPE_LITTLE, "Blood Judgement - Final Boss", 1.0f, 1.0f, 1.0f, 1.0f);
                break;
                
                case 3:
                    renderText(0.1, 0.935, 0.0, TEXT_TYPE_LITTLE, "Resident Evil CODE: Veronica", 1.0f, 1.0f, 1.0f, 1.0f);
                    renderText(0.1, 1.005, 0.0, TEXT_TYPE_LITTLE, "Alexia - Final Boss Theme", 1.0f, 1.0f, 1.0f, 1.0f);

                break;
            }
            
            miscStuff.renderSquareWithTexture(&engineCharMenu, true);             
        }
        break;

        case SEL_PLAYER_START: {
            float x = 0;
            float z = 0;

            if (miscStuff.isInFade()) {
                miscStuff.renderSquareWithTexture(&engineSelectChar, false);
                miscStuff.renderSquareWithColor(0, 0, 0, 0.65);
                

                switch (menuArrow) {

                    /*
                     * Leon S. Kennedy
                     */

                    case 0: {

                            x = cos(((70 + nextCharAngle) * PI/180)) * 1000.0;
                            z = sin(((70 + nextCharAngle) * PI/180)) * 1500.0;

                            renderEMD(x-500, -150.0f,z, 
                                230.0f + nextCharAngle, 0, mainPlayer.getPlayerEMDAnim());

                            x = cos(((240 + nextCharAngle) * PI/180)) * 1000.0;
                            z = sin(((240 + nextCharAngle) * PI/180)) * 1500.0;
                    
                            /* 
                             * Chris standard
                             */
                            renderEMD(x-500, -150.0f,z,
                                100.0f+ nextCharAngle, 3, modelList[3].emdSec2Data[modelList[3].emdSec2AnimInfo[3].animStart]);



                    }
                    break;
                    
                    /*
                     * Chris Redfield
                     */
                    case 1: {

                            x = cos(((70 + nextCharAngle) * PI/180)) * 1000.0;
                            z = sin(((70 + nextCharAngle) * PI/180)) * 1500.0;

                            renderEMD(x-500, -150.0f,z, 
                                270.0f + nextCharAngle, 0, modelList[0].emdSec2Data[modelList[0].emdSec2AnimInfo[0].animStart]);

                            x = cos(((240 + nextCharAngle) * PI/180)) * 1000.0;
                            z = sin(((240 + nextCharAngle) * PI/180)) * 1500.0;
                    
                            /* 
                             * Chris standard
                             */
                            renderEMD(x-500, -150.0f,z, 
                                45.0f+ nextCharAngle, 3,  mainPlayer.getPlayerEMDAnim());
                    }
                    break;

                    default:

                    break;
                }

                miscStuff.renderSquareWithTexture(&engineCharMenu, true);        
            } else {
                gameState = STATE_IN_GAME;
                inGame    = IN_GAME_BEGIN;
                mainPlayer.setPlayerEMD(menuArrow + 1);
                mainPlayer.setPlayerAnimSection(EMD_SECTION_4);
                mainPlayer.setPlayerAnim(STAND_SEC4_ANIM_IDLE);
                oldSection = EMD_SECTION_4;
                oldAnim    = STAND_SEC4_ANIM_IDLE;
                soundEngine.engineStopSound();
                miscStuff.setupFadeEffect(TYPE_FADE_OUT, 0, 0, 0, 60);
            }
        }
        break;

        default:

        break;
    }
}


// All in game stuff related with rendering is done here
void gameCore::renderGame() {

    switch (inGame) {
        case IN_GAME_BEGIN:
            if(!miscStuff.isInFade()) {
                switch (charMusicNum) {
                    case 0:
                        soundEngine.engineLoadSound("resource/ost/0.mp3");
                    break;

                    case 1:
                        soundEngine.engineLoadSound("resource/ost/1.mp3");
                    break;

                    case 2:

                    break;

                    case 3:

                    break;

                    default:

                    break;
                }
                soundEngine.enginePlaySound(1);
                inGame = IN_GAME_NORMAL;
            }
        break;

        case IN_GAME_NORMAL:
            /* .BSS Background stuff */
            //drawMapBackground();

            /* All model rendering is done by renderEMD Function */
            /* Player Rendering */
            renderEMD(mainPlayer.getPlayerX(), mainPlayer.getPlayerY(), mainPlayer.getPlayerZ(), 
                      mainPlayer.getPlayerAngle(), mainPlayer.getPlayerEMD(), mainPlayer.getPlayerEMDAnim());
            /* Enemy Rendering */
            renderEMD(gameEnemy.getX(), gameEnemy.getY(),gameEnemy.getZ(), 
                      gameEnemy.getAngle(), gameEnemy.getEMD(), gameEnemy.getAnim());


            if (wireFrameMode)
                renderCollision();

            char coord[0xFF];

            sprintf(coord, "X:%d-Y:%d-Z:%d-Angle:%d", (int)mainPlayer.getPlayerX(),(int)mainPlayer.getPlayerY(),(int)mainPlayer.getPlayerZ(), (int)mainPlayer.getPlayerAngle());

            renderText(0.0f, 0.02f, 100.0f, TEXT_TYPE_LITTLE, coord);

            if (gameState == STATE_IN_DEBUG) {
                if (jumpToRun) 
                    eventSystem_debugJumpToRun();
                else 
                    eventSystem_debugMenu();
            }
        break;
    }

}

/*
 * renderCallBack
 * This function is the display callback
 */
void gameCore::renderCallback() {
    core->renderStateMachine();
}

/*
 * eventSystemUpCallBack
 * Handle special up keys of keyboard
 */
void gameCore::eventSystemUpCallBack(int key, int x, int y) {
    core->eventSystem_upKey(key,x,y);
}

/*
 * eventSystemDownCallBack
 * Handle special down keys of keyboard
 */
void gameCore::eventSystemDownCallBack(int key, int x, int y)  {
    core->eventSystem_downKey(key,x,y);
}

/*
 * eventSystemKeyboardUpCallBack
 * Handle keyboard up keys 
 */
void gameCore::eventSystemKeyboardUpCallBack(unsigned char key, int x, int y) {
    core->eventSystem_keyboardUp(key,x,y);
}

/*
 * eventSystemKeyboardDownCallBack
 * Handle keyboard downKeys 
 */
void gameCore::eventSystemKeyboardDownCallBack(unsigned char key, int x, int y) {
    core->eventSystem_keyboardDown(key,x,y);
}



/*
 * This function set the gameCore obj 
 * used by static functions
 */
void gameCore::renderSetObj(gameCore *obj) { core = obj; }

/*
 * renderStateMachine
 * This function is used to handle the game state machine
 * each state has a different scene to draw
 */
void gameCore::renderStateMachine() {
    
    // Limpa a cor do buffer (background)
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glAlphaFunc(GL_GREATER,0.1);


    // Reseta a matriz ModelView
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    /* 
     * Game state machine 
     */
    switch (gameState) {
        case STATE_MAIN_MENU:
            renderMainMenu();
        break;

        case STATE_SEL_CHAR:
            renderSelectChar();
        break;

        case STATE_IN_DEBUG:
        case STATE_IN_ROOM:
        case STATE_IN_GAME:
            renderGame();
        break;

        case STATE_CREDITS:

        break;
    }
  
    // Filtros de textura, é necessário definilos para a textura ser apresentada
    // Eu utilizei até agora 2 filtros, o GL_NEAREST que deixa a qualidade da textura mais *original*
    // e o GL_LINEAR que é um filtro que deixa a textura mais bonita
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST ); 
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
    // Isso vai ser subistituido futuramente por um *sceneNum* ficará mais fácil
    // decidir o que vai ser renderizado no momento certo, como menu, menu de start
    // ou qualquer outro, mas por enquanto fica apenas esse bool =)

    /* 
     * render fadeEffect only if it's enable
     */
    miscStuff.renderFadeEffect();

    // Buffers de renderização
    glutSwapBuffers();
}



/*
 * renderInit
 * Initialize all OpenGL stuff 
 */
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
    glEnable(GL_ALPHA_TEST);
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

    /*
     * Callback list
     */
    glutDisplayFunc   ( renderCallback);
    glutSpecialFunc   ( eventSystemDownCallBack );
    glutSpecialUpFunc ( eventSystemUpCallBack );
    glutKeyboardFunc  ( eventSystemKeyboardDownCallBack );
    glutKeyboardUpFunc( eventSystemKeyboardUpCallBack );
    glutIdleFunc      ( MainLoop);

    // Evento de Loop
    glutMainLoop();

}