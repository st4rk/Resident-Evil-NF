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
EMD1 testModel;


// engineBackground é utilizado para carregar os backgrounds de cada .RDT
// que o player se encontra.
// engineFont é a fonte utilizad para desenhar textos, como so dedos
// de debug
// engineThisGame é a primeira imagem que aparece quando você abre o jogo
// dizendo que o jogo contem cenas bla bla e o mainMenu é o menu de start
BITMAP engineBackground;
BITMAP engineThisGame;
BITMAP engineMainMenu;
BITMAP engineLogo;
BITMAP engineSelectChar;
BITMAP engineCharMenu;
BITMAP engineResult;
NFP shadow;

player      mainPlayer;
gameMath    mathEngine;
gameSound   soundEngine;
std::vector<enemy> enemyList;

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

}

/*
 * renderLoadResource
 * This function is the hardcoded game initialization
 */
void gameCore::renderLoadResource() {
	enemy       gameEnemy;
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
    modelList[4].emdLoadFile("resource/models/EM01E.EMD");
    modelList[5].emdLoadFile("resource/models/EM030.EMD");

    testModel.loadFile("resource/models/EM1110.EMD");

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
    engineThisGame.loaderFile  ("resource/menus/intro_2.bmp",0);
    engineLogo.loaderFile      ("resource/menus/intro_1.bmp",0);
    engineMainMenu.loaderFile  ("resource/menus/mainMenu.bmp",0);
    engineSelectChar.loaderFile("resource/menus/selectChar.bmp", 0);
    engineCharMenu.loaderFile  ("resource/menus/charMenu.bmp", 1);
    engineResult.loaderFile    ("resource/menus/RESULT.BMP", 0);

    shadow.loadImage           ("resource/texture/1.png");

    // Carrega o background com número 5
    background_Loader("resource/stages/re1/ROOM106.BSS");

    // HardCode, modelo inicial, X,Y,Z e Número da câmera
    mainPlayer.setModel(0);
    /*
    mainPlayer.setX(18391);
    mainPlayer.setZ(12901);
    mainPlayer.setY(0);
    */
    mainPlayer.setX(0.0f);
    mainPlayer.setY(0.0f);
    mainPlayer.setZ(0.0f);
    mainPlayer.setHitPoints(5);

    mainPlayer.setCam(CAMERA_STYLE_SEL_CHAR);
    mainPlayer.setAngle(90.0);

    for (int i = 0; i < VR_ENEMY_NUM; i++) {
        gameEnemy.setX(-2000.0f + (-1000.0f * i));
        gameEnemy.setZ(-5000.0f + (-1000.0f * i));
        gameEnemy.setY(0);
        gameEnemy.setModel(4);
        gameEnemy.setAngle(0);
        gameEnemy.setState(AI_STATE_BEGIN);
        gameEnemy.setType(AI_TYPE_ZOMBIE_RE2);
        gameEnemy.setAnimSection(EMD_SECTION_4);
        gameEnemy.setAnimType(ZOMBIE_SEC4_ANIM_IDLE);
        gameEnemy.setHitPoints(3);
        enemyList.push_back(gameEnemy);
    }

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


/* All stuff related with debug menu goes here */
void gameCore::eventSystem_debugMenu() {
     /*
    char jump_to[20];

    miscStuff.renderText(0.42, 0.30, 0.0f, TEXT_TYPE_LITTLE, "--Debug Menu--");


    sprintf(jump_to, "Jump %03x %s",debugR[debug_jRoomNum].roomNum, debugR[debug_jRoomNum].roomName.c_str());

    miscStuff.renderText(0.35, 0.35, 0.0f, TEXT_TYPE_LITTLE, jump_to);
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

    */
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
                    mainPlayer.setX(debugR[debug_jRoomNum].x);
                    mainPlayer.setY(debugR[debug_jRoomNum].y);
                    mainPlayer.setZ(debugR[debug_jRoomNum].z);
                    playerRDT.rdtRE1LoadFile(rdt);
                    mainPlayer.setCam(playerRDT.rdtRE1CameraSwitch[0].from);

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
                    mainPlayer.setX(playerRDT.door_set_re1[roomNum].next_x);
                    mainPlayer.setY(playerRDT.door_set_re1[roomNum].next_y);
                    mainPlayer.setZ(playerRDT.door_set_re1[roomNum].next_z);
                    playerRDT.rdtRE1LoadFile(rdt);
                    mainPlayer.setCam(playerRDT.rdtRE1CameraSwitch[0].from);

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
                    mainPlayer.setAnimSection(EMD_SECTION_2);
                    mainPlayer.setAnimType(SPECIAL_SEC2_ANIM_CHOICE, false);
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
    switch (gameState) {
        case STATE_IN_GAME:
            switch (key) {
                case 'x':
                    eventSystem_gameAction(EVENT_SYSTEM_KEY_X, false);
                break;

                case 'z':
                    eventSystem_gameAction(EVENT_SYSTEM_KEY_Z, false);
                break;

                case 'c':
                    eventSystem_gameAction(EVENT_SYSTEM_KEY_C, false);
                break;

                case 'a':

                break;

                default:

                break;
            }
        break;
    }
}

/* 
 * engineAnimation
 * Here all game animation is handled
 */

void gameCore::engineAnimation(entity *e) {
    /*
     * Verify if is the same animation, if not, should interpolate the animation
     */

    if ((inInterpolation == 0) && ((oldAnim != e->getAnimType()) 
                      || (oldSection != e->getAnimSection()))) {

        if ((oldSection == EMD_SECTION_4) && (oldAnim == STAND_SEC4_ANIM_IDLE)) {
            if ((e->getAnimType() == STAND_SEC2_ANIM_BACKWARD) && (e->getAnimSection() == EMD_SECTION_2)) {
            //  inInterpolation = 1;
            }
        }

    }

    switch (inInterpolation) {
        /*
         * play normal animations 
         */
        case 0: {
            switch (e->getAnimSection()) {
                case EMD_SECTION_2: {
                    if (e->getAnimCount() < (modelList[e->getModel()].emdSec2AnimInfo[e->getAnimType()].animCount-1))
                        e->setAnimCount(e->getAnimCount() + 1);
                    else {
                        if (e->getAnimRepeat()) {
                            e->setAnimCount(0);
                        }
                    }

                    e->setAnimFrame(modelList[e->getModel()].emdSec2Data[e->getAnimCount()+modelList[e->getModel()].emdSec2AnimInfo[e->getAnimType()].animStart]);
                }
                break;

                case EMD_SECTION_4: {
                    if (e->getAnimCount() < (modelList[e->getModel()].emdSec4AnimInfo[e->getAnimType()].animCount-1))
                        e->setAnimCount(e->getAnimCount() + 1);
                    else {
                        if (e->getAnimRepeat()) {
                            switch (e->getAnimType()) {
                                case STAND_SEC4_ANIM_S_SHOOT:
                                    e->setAnimType(STAND_SEC4_ANIM_AIM);
                                break;
                            }
                            e->setAnimCount(0);
                        }
                    }

                    e->setAnimFrame(modelList[e->getModel()].emdSec4Data[e->getAnimCount()+modelList[e->getModel()].emdSec4AnimInfo[e->getAnimType()].animStart]);
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

                    float p_factor = (1.0f / ((modelList[e->getModel()].emdSec2AnimInfo[oldAnim].animCount-1) - e->getAnimCount()));
                    float p = 0.0f;

                    for (unsigned int i = e->getAnimCount(); i < (modelList[e->getModel()].emdSec2AnimInfo[oldAnim].animCount-1); i++, p += p_factor) {
                        node_2 = &modelList[e->getModel()].emdSec2Data[i+modelList[e->getModel()].emdSec2AnimInfo[oldAnim].animStart];
                        
                        switch (e->getAnimSection()) {
                            case EMD_SECTION_2:
                                node_3 = &modelList[e->getModel()].emdSec2Data[i_2+modelList[e->getModel()].emdSec2AnimInfo[e->getAnimType()].animStart];

                                for (unsigned int t = 0; t <  modelList[e->getModel()].emdTotalObj; t++) {

                                   node.vector[t].x = mathEngine.interpolation(node_2->vector[t].x, node_3->vector[t].x, p);
                                   node.vector[t].y = mathEngine.interpolation(node_2->vector[t].y, node_3->vector[t].y, p);
                                   node.vector[t].z = mathEngine.interpolation(node_2->vector[t].z, node_3->vector[t].z, p);
                                }

                                if (i_2 < (modelList[e->getModel()].emdSec2AnimInfo[e->getAnimType()].animCount-1))
                                    i_2++;
                                else
                                    i_2 = 0;

                                interAnimation.push_back(node);

                            break;

                            case EMD_SECTION_4:
                                node_3 = &modelList[e->getModel()].emdSec4Data[i_2+modelList[e->getModel()].emdSec4AnimInfo[e->getAnimType()].animStart];
        
                                for (unsigned int t = 0; t <  modelList[e->getModel()].emdTotalObj; t++) {
                                   node.vector[t].x = mathEngine.interpolation(node_2->vector[t].x, node_3->vector[t].x, p);
                                   node.vector[t].y = mathEngine.interpolation(node_2->vector[t].y, node_3->vector[t].y, p);
                                   node.vector[t].z = mathEngine.interpolation(node_2->vector[t].z, node_3->vector[t].z, p);
                                }

                                if (i_2 < (modelList[e->getModel()].emdSec4AnimInfo[e->getAnimType()].animCount-1))
                                    i_2++;
                                else
                                    i_2 = 0;
                                
                                interAnimation.push_back(node);
                            break;
                        }                    
                    }
                    e->setAnimCount(0);
                    inInterpolation = 2;
                }
                break;

                case EMD_SECTION_4: {
                    EMD_SEC2_DATA_T  node;   // new animation (interpolated)
                    EMD_SEC2_DATA_T *node_2; // old animation
                    EMD_SEC2_DATA_T *node_3; // new animation
                    unsigned int     i_2    = 0;
                    float p_factor = (1.0f / 3.0f);
                    float p = 0.0f;

                    for (unsigned int i = 0; i < 3; i++, p += p_factor) {
                        node_2 = &modelList[e->getModel()].emdSec4Data[i+modelList[e->getModel()].emdSec4AnimInfo[oldAnim].animStart];
                        
                        switch (e->getAnimSection()) {
                            case EMD_SECTION_2:
                                node_3 = &modelList[e->getModel()].emdSec2Data[i_2+modelList[e->getModel()].emdSec2AnimInfo[e->getAnimType()].animStart];
                                // here
                                for (unsigned int t = 0; t <  modelList[e->getModel()].emdTotalObj; t++) {
                                   node.vector[t].x = mathEngine.interpolation(node_2->vector[t].x, node_3->vector[t].x, p);
                                   printf("node.x: %.2f node_2.x %.2f node_3.x %.2f p: %.2f\n", node.vector[t].x, node_2->vector[t].x, node_3->vector[t].x, p);
                                   node.vector[t].y = mathEngine.interpolation(node_2->vector[t].y, node_3->vector[t].y, p);
                                   printf("node.y: %.2f node_2.y %.2f node_3.y %.2f p: %.2f\n", node.vector[t].y, node_2->vector[t].y, node_3->vector[t].y, p);
                                   node.vector[t].z = mathEngine.interpolation(node_2->vector[t].z, node_3->vector[t].z, p);
                                   printf("node.z: %.2f node_2.z %.2f node_3.z %.2f p: %.2f\n", node.vector[t].z, node_2->vector[t].z, node_3->vector[t].z, p);    
                                }

                                if (i_2 < (modelList[e->getModel()].emdSec2AnimInfo[e->getAnimType()].animCount-1))
                                    i_2++;
                                else
                                    i_2 = 0;

                                interAnimation.push_back(node);

                            break;

                            case EMD_SECTION_4:
                                node_3 = &modelList[e->getModel()].emdSec4Data[i_2+modelList[e->getModel()].emdSec4AnimInfo[e->getAnimType()].animStart];
        
                                for (unsigned int t = 0; t <  modelList[e->getModel()].emdTotalObj; t++) {
                                   node.vector[t].x = mathEngine.interpolation(node_3->vector[t].x, node_2->vector[t].x, p);
                                   node.vector[t].y = mathEngine.interpolation(node_3->vector[t].y, node_2->vector[t].y, p);
                                   node.vector[t].z = mathEngine.interpolation(node_3->vector[t].z, node_2->vector[t].z, p);
                                    }

                                if (i_2 < (modelList[e->getModel()].emdSec4AnimInfo[e->getAnimType()].animCount-1))
                                    i_2++;
                                else
                                    i_2 = 0;
                                
                                interAnimation.push_back(node);
                            break;
                        }                    
                    }

                    e->setAnimCount(0);
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
        if (e->getAnimCount() < 3)
                e->setAnimCount(e->getAnimCount() + 1);
            else {

                e->setAnimCount(0);

/*                    oldAnim         = e->getAnimType();
                oldSection      = e->getAnimTypeSection();
                inInterpolation = 0;
                switch (e->getAnimTypeSection()) {
                    case EMD_SECTION_2:
                        if (e->getAnimCount() > (modelList[e->getModel()].emdSec2AnimInfo[e->getAnimType()].animCount-1)) {
                            e->setAnimCount(0);
                        }
                    break;

                    case EMD_SECTION_4:
                        if (e->getAnimCount() > (modelList[e->getModel()].emdSec4AnimInfo[e->getAnimType()].animCount-1)) {
                            e->setAnimCount(0);
                        }
                    break;
                }
*/
            }     

            e->setAnimFrame(interAnimation[1]);
        break;


    }
}

void MainLoop() {
    if (tmr60FPS < SDL_GetTicks()) {
        tmr60FPS = (SDL_GetTicks() + (1000/60));


	    if (core->coreTmr_anim <= SDL_GetTicks()) {
	        core->coreTmr_anim = SDL_GetTicks() + 24; 
	        core->engineAnimation(&mainPlayer);
	        for (unsigned int i = 0; i < enemyList.size(); i++) {
	        	core->engineAnimation(&enemyList[i]);
	    	}
	    }

        switch (gameState) {
            case STATE_SEL_CHAR:

            break;

            case STATE_IN_GAME: {

                    core->handlePlayerAction();

                    unsigned int gameScore = 0;

                    for (unsigned int i = 0; i < enemyList.size(); i++) {
                        core->engineAI.zombie_re_2(&mainPlayer, &enemyList[i]);
                        if (enemyList[i].getState() == AI_STATE_DEATH) 
                            gameScore++;
                    }

                    core->vrMode.setGameScore(gameScore);

                    /* Collision Detection RE 1 
                    for (unsigned int i = 0; i < playerRDT.rdtRE1ColissionArray.size(); i++) {
                            if (mathEngine.collisionDetect(playerRDT.rdtRE1ColissionArray[i].type, playerRDT.rdtRE1ColissionArray[i].x1, playerRDT.rdtRE1ColissionArray[i].z1, 
                                                     playerRDT.rdtRE1ColissionArray[i].x2, playerRDT.rdtRE1ColissionArray[i].z2, mainPlayer.getX() + x, mainPlayer.getZ() - z) == true) {
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
                                                 playerRDT.rdtColissionArray[i].W, playerRDT.rdtColissionArray[i].D, mainPlayer.getX() + x, mainPlayer.getZ()-z) == true) {
                            canMove = false;
                        } 
                    }
                    */
                    
                    /* Camera Switch RDT 1.5 and 2.0
                    for (unsigned int i = 0; i < playerRDT.rdtCameraSwitch.size(); i++) {
                        if (mathEngine.mapSwitch(mainPlayer.getX(), mainPlayer.getZ(), playerRDT.rdtCameraSwitch[i].x1, playerRDT.rdtCameraSwitch[i].z1, playerRDT.rdtCameraSwitch[i].x2, playerRDT.rdtCameraSwitch[i].z2, 
                                                 playerRDT.rdtCameraSwitch[i].x3, playerRDT.rdtCameraSwitch[i].z3, playerRDT.rdtCameraSwitch[i].x4, playerRDT.rdtCameraSwitch[i].z4)) {
                            
                            if (playerRDT.rdtCameraSwitch[i].cam1 != 0) {//|| ((playerRDT.rdtCameraSwitch[i].cam1 == 0) && (playerRDT.rdtCameraSwitch[i].cam0 == 1)))
                                   backgroundNow = playerRDT.rdtCameraSwitch[i].cam1;// background_Loader(playerRDT.rdtCameraSwitch[i].cam1);
                                   mainPlayer.setCam(backgroundNow);
                            }
                        
                        } 
                    }
                    */

                    /* Camera Switch Zone/Camera Position RE 1.0 
                    for (unsigned int i = 0; i < playerRDT.rdtRE1CameraSwitch.size(); i++) {
                        if (mathEngine.mapSwitch(mainPlayer.getX(), mainPlayer.getZ(), playerRDT.rdtRE1CameraSwitch[i].x1, playerRDT.rdtRE1CameraSwitch[i].z1, playerRDT.rdtRE1CameraSwitch[i].x2, playerRDT.rdtRE1CameraSwitch[i].z2, 
                                                 playerRDT.rdtRE1CameraSwitch[i].x3, playerRDT.rdtRE1CameraSwitch[i].z3, playerRDT.rdtRE1CameraSwitch[i].x4, playerRDT.rdtRE1CameraSwitch[i].z4)) {
                            
                                if (playerRDT.rdtRE1CameraSwitch[i].to != 9) {
                                     mainPlayer.setCam(playerRDT.rdtRE1CameraSwitch[i].to);
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


        }

        if ((gameState == STATE_IN_GAME) || (gameState == STATE_IN_ROOM)) {

            if (gameState == STATE_IN_ROOM) 
                eventSystem_newRoom();

        }
        glutPostRedisplay();   
    }
}


/*
 * eventSystem -> gameAction
 * this function is used to handle the keys pressed that change the player animation as well as 
 * the game actions
 */

void gameCore::eventSystem_gameAction(unsigned int key, bool pressed) {
    switch (mainPlayer.getState()) {
        case PLAYER_STATE_NORMAL: {
            switch (key) {

                case EVENT_SYSTEM_KEY_UP: {
                    if (pressed) {
                        keyList[EVENT_SYSTEM_KEY_UP] = true;
                        mainPlayer.setAnimSection(EMD_SECTION_4);
                        if (keyList[EVENT_SYSTEM_KEY_X] == true) {
                            mainPlayer.setAnimType(STAND_SEC4_ANIM_UAIM);
                        } else {
                            mainPlayer.setAnimType(STAND_SEC4_ANIM_WALK);
                        }
                    } else {
                        keyList[EVENT_SYSTEM_KEY_UP] = false;
                        mainPlayer.setAnimSection(EMD_SECTION_4);
                        if (keyList[EVENT_SYSTEM_KEY_X] == true) {
                            mainPlayer.setAnimType(STAND_SEC4_ANIM_AIM);
                        } else {
                            mainPlayer.setAnimType(STAND_SEC4_ANIM_IDLE);
                        }
                    }
                }
                break;

                case EVENT_SYSTEM_KEY_DOWN:
                    if (pressed) {
                        keyList[EVENT_SYSTEM_KEY_DOWN] = true;

                        if (keyList[EVENT_SYSTEM_KEY_X] == true) {
                            mainPlayer.setAnimSection(EMD_SECTION_4);
                            mainPlayer.setAnimType(STAND_SEC4_ANIM_DAIM);
                        } else {
                            mainPlayer.setAnimSection(EMD_SECTION_2);
                            mainPlayer.setAnimType(STAND_SEC2_ANIM_BACKWARD);
                        }
                    } else {
                        keyList[EVENT_SYSTEM_KEY_DOWN] = false;
                        mainPlayer.setAnimSection(EMD_SECTION_4);
                        if (keyList[EVENT_SYSTEM_KEY_X] == true) {
                            mainPlayer.setAnimType(STAND_SEC4_ANIM_AIM);
                        } else {
                            mainPlayer.setAnimType(STAND_SEC4_ANIM_IDLE);
                        }
                    }
                break;

                case EVENT_SYSTEM_KEY_LEFT:
                    if (pressed) {
                        if ((keyList[EVENT_SYSTEM_KEY_UP] == false) && (keyList[EVENT_SYSTEM_KEY_DOWN] == false) &&
                            (keyList[EVENT_SYSTEM_KEY_X]  == false)) { 
                            mainPlayer.setAnimSection(EMD_SECTION_4);
                            mainPlayer.setAnimType(STAND_SEC4_ANIM_WALK);
                            mainPlayer.setAnimRotationDir(PLAYER_ACTION_R_LEFT);
                        } else {
                            mainPlayer.setAnimRotationDir(PLAYER_ACTION_R_LEFT);
                        }
                    } else {
                        if ((keyList[EVENT_SYSTEM_KEY_UP] == false) && (keyList[EVENT_SYSTEM_KEY_DOWN] == false) && 
                            (keyList[EVENT_SYSTEM_KEY_X]  == false))      
                            mainPlayer.setAnimType(STAND_SEC4_ANIM_IDLE);
                        
                        mainPlayer.setAnimRotationDir(PLAYER_ACTION_R_NONE);
                    }
                break;

                case EVENT_SYSTEM_KEY_RIGHT:
                    if (pressed) {
                        if ((keyList[EVENT_SYSTEM_KEY_UP] == false) && (keyList[EVENT_SYSTEM_KEY_DOWN] == false) &&
                            (keyList[EVENT_SYSTEM_KEY_X]  == false)) {
                            mainPlayer.setAnimSection(EMD_SECTION_4);
                            mainPlayer.setAnimType(STAND_SEC4_ANIM_WALK);

                            mainPlayer.setAnimRotationDir(PLAYER_ACTION_R_RIGHT);
                        } else {
                            mainPlayer.setAnimRotationDir(PLAYER_ACTION_R_RIGHT);
                        }
                    } else {
                        if ((keyList[EVENT_SYSTEM_KEY_UP] == false) && (keyList[EVENT_SYSTEM_KEY_DOWN] == false) && 
                            (keyList[EVENT_SYSTEM_KEY_X]  == false)) 
                            mainPlayer.setAnimType(STAND_SEC4_ANIM_IDLE);
                        
                        mainPlayer.setAnimRotationDir(PLAYER_ACTION_R_NONE);
                    }
                break;

                case EVENT_SYSTEM_KEY_X:
                    if (pressed) {
                        keyList[EVENT_SYSTEM_KEY_X] = true;
                            mainPlayer.setCam(CAMERA_STYLE_SPECIAL_2);
                            mainPlayer.setAnimSection(EMD_SECTION_4);
                        if (keyList[EVENT_SYSTEM_KEY_UP]) {
                            mainPlayer.setAnimType(STAND_SEC4_ANIM_UAIM);
                        } else if (keyList[EVENT_SYSTEM_KEY_DOWN]) {
                            mainPlayer.setAnimType(STAND_SEC4_ANIM_DAIM);
                        } else {
                            mainPlayer.setAnimType(STAND_SEC4_ANIM_S_SHOOT);
                        }
                    } else {
                        keyList[EVENT_SYSTEM_KEY_X] = false;
                        mainPlayer.setAnimSection(EMD_SECTION_4);
                        mainPlayer.setAnimType(STAND_SEC4_ANIM_IDLE);
                        mainPlayer.setCam(CAMERA_STYLE_SPECIAL);
                    }

                break;

                case EVENT_SYSTEM_KEY_Z:
                    if (pressed) {
                        if (keyList[EVENT_SYSTEM_KEY_X] == true) {              
                            keyList[EVENT_SYSTEM_KEY_Z] = true;

                            mainPlayer.setState(PLAYER_STATE_BEGIN_GUN);
                        }
                    } else {
                        keyList[EVENT_SYSTEM_KEY_Z] = false;          
                    }
                break;

                case EVENT_SYSTEM_KEY_C:

                break;

            }
        }

        break;

        case PLAYER_STATE_BEGIN_GUN:
        case PLAYER_STATE_GUN: {
            switch (key) {
                case EVENT_SYSTEM_KEY_X:
                    keyList[EVENT_SYSTEM_KEY_X] = pressed;
                break;

                case EVENT_SYSTEM_KEY_Z:
                    keyList[EVENT_SYSTEM_KEY_Z] = pressed;
                break;

            }
        }
        break;

        case PLAYER_STATE_HIT:
            for (int i = 0; i < 7; i++)
                keyList[i] = false;

        break;

    }

}



void renderShadow(float x, float y, float z) {

    glDisable(GL_LIGHTING);
   
    glLoadIdentity();
 
    core->camMode.camList(mainPlayer.getCam(), mainPlayer.getX(), mainPlayer.getY(),
                      mainPlayer.getZ(),  mainPlayer.getAngle());
 
 
    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
    glTexImage2D(GL_TEXTURE_2D, 0,GL_RGBA, shadow.getWidth(), shadow.getHeight(), 0,GL_RGBA, GL_UNSIGNED_BYTE, shadow.getPixelData());
 
 
    glBegin(GL_QUADS);  
        glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
        glTexCoord2f(0.0f,0.0f);                
        glVertex3f(x-1024.0f,             0.0f,      z-1024.0f);  
        glTexCoord2f(1.0f,0.0f);          
        glVertex3f((x + 1024.0f), 0.0f,      z-1024.0f);  
        glTexCoord2f(1.0f,1.0f);          
        glVertex3f((x + 1024.0f), 0.0f,     (z + 1024.0f));
        glTexCoord2f(0.0f,1.0f);  
        glVertex3f(x-1024.0f,             0.0f,     (z + 1024.0f));
    glEnd();
 
    glEnable(GL_LIGHTING);


}


void renderBoundingBox(float x, float y, float z) {

    glDisable(GL_LIGHTING);
    glDisable(GL_TEXTURE_2D);
    glLoadIdentity();

    core->camMode.camList(mainPlayer.getCam(), mainPlayer.getX(), mainPlayer.getY(),
                      mainPlayer.getZ(),  mainPlayer.getAngle());

    // Enable wireframe mode
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    x = (x - 512.0f);
    z = (z - 512.0f);

    /* Wireframe collision */
    glBegin(GL_QUADS);      
        glColor3f(1.0f, 0.0f, 0.0f);                  
        glVertex3f(x,             0.0f,      z);             
        glVertex3f((x + 1024.0f), 0.0f,      z);              
        glVertex3f((x + 1024.0f), 0.0f,     (z + 1024.0f));   
        glVertex3f(x,             0.0f,     (z + 1024.0f));
    glEnd();

    // disable wireframe
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

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
            
        core->camMode.camList(mainPlayer.getCam(), mainPlayer.getX(), mainPlayer.getY(),
                              mainPlayer.getZ(),  mainPlayer.getAngle());

        glTranslatef(m_x, m_y, m_z);  

        glRotatef(angle, 0.0f, 1.0f, 0.0f);



        /*
         * to solve the bug: render the model starting with value N
         */
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
    GLfloat ambientColor[] = {(float)((playerRDT.rdtLight[mainPlayer.getCam()].colorAmbient.R/255.0f)*1.0f + 1.0),
        (float)((playerRDT.rdtLight[mainPlayer.getCam()].colorAmbient.G/255.0f)*1.0f + 1.0),
        (float)((playerRDT.rdtLight[mainPlayer.getCam()].colorAmbient.B/255.0f)*1.0f + 1.0),  1.0 };
    GLfloat lightPos1[] = {(float)(playerRDT.rdtLight[mainPlayer.getCam()].lightPos[0].X), 
        (float)(playerRDT.rdtLight[mainPlayer.getCam()].lightPos[0].Y), 
        (float)(playerRDT.rdtLight[mainPlayer.getCam()].lightPos[0].Z), 1.0};
    GLfloat lightPos2[] = {(float)(playerRDT.rdtLight[mainPlayer.getCam()].lightPos[1].X), 
        (float)(playerRDT.rdtLight[mainPlayer.getCam()].lightPos[1].Y), 
        (float)(playerRDT.rdtLight[mainPlayer.getCam()].lightPos[1].Z), 1.0};
    GLfloat lightColor1[] = {(float)((playerRDT.rdtLight[mainPlayer.getCam()].colorLight[0].R/255.0f)*1.0f),
        (float)((playerRDT.rdtLight[mainPlayer.getCam()].colorLight[0].G/255.0f)*1.0f),
        (float)((playerRDT.rdtLight[mainPlayer.getCam()].colorLight[0].B/255.0f)*1.0f), 1.0};
    GLfloat lightColor2[] = {(float)((playerRDT.rdtLight[mainPlayer.getCam()].colorLight[1].R/255.0f)*1.0f),
        (float)((playerRDT.rdtLight[mainPlayer.getCam()].colorLight[1].G/255.0f)*1.0f),
        (float)((playerRDT.rdtLight[mainPlayer.getCam()].colorLight[1].B/255.0f)*1.0f), 1.0};

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
    glTexImage2D(GL_TEXTURE_2D, 0,GL_RGB, bg[mainPlayer.getCam()]->w, bg[mainPlayer.getCam()]->h, 0,GL_RGB, GL_UNSIGNED_BYTE, bg[mainPlayer.getCam()]->pixels);

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
                        miscStuff.renderText(0.60, 0.70, 0.0, TEXT_TYPE_LITTLE, "NEW GAME", 0.0f, 1.0f, 0.0f, 1.0f);
                        miscStuff.renderText(0.70, 0.80, 0.0, TEXT_TYPE_LITTLE, "EXIT");
                    } else if (menuArrow == 1) {
                        miscStuff.renderText(0.70, 0.80, 0.0, TEXT_TYPE_LITTLE, "EXIT", 0.0f, 1.0f, 0.0f, 1.0f);
                        miscStuff.renderText(0.60, 0.70, 0.0, TEXT_TYPE_LITTLE, "NEW GAME");
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
                    mainPlayer.setAnimSection(EMD_SECTION_2);
                    mainPlayer.setAnimType(SPECIAL_SEC2_ANIM_POSE);
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
                        mainPlayer.setModel(0);
                    break;

                    case 1:
                        if (nextCharAngle >= 180.0f) {
                            nextCharTransition = false;
                        } else {
                            nextCharAngle += 4.0f;
                        }
                        mainPlayer.setModel(3);
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
                            230.0f + nextCharAngle, 0, mainPlayer.getAnimFrame());

                        x = cos(((240 + nextCharAngle) * PI/180)) * 1000.0;
                        z = sin(((240 + nextCharAngle) * PI/180)) * 1500.0;
                
                        /* 
                         * Chris standard
                         */
                        renderEMD(x-500, -150.0f,z,
                            100.0f+ nextCharAngle, 3, modelList[3].emdSec2Data[modelList[3].emdSec2AnimInfo[3].animStart]);



                        miscStuff.renderText(0.70, 0.10, 0.0, TEXT_TYPE_LITTLE, "Leon S. Kennedy", 1.0f, 1.0f, 1.0f, 1.0f);
                        miscStuff.renderText(0.80, 0.20, 0.0, TEXT_TYPE_LITTLE, "From R.P.D ", 1.0f, 1.0f, 1.0f, 1.0f);
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
                            45.0f+ nextCharAngle, 3,  mainPlayer.getAnimFrame());

                        miscStuff.renderText(0.70, 0.10, 0.0, TEXT_TYPE_LITTLE, "Chris Redfield", 1.0f, 1.0f, 1.0f, 1.0f);
                        miscStuff.renderText(0.80, 0.20, 0.0, TEXT_TYPE_LITTLE, "S.T.A.R.S. ", 1.0f, 1.0f, 1.0f, 1.0f);
                        miscStuff.renderText(0.80, 0.25, 0.0, TEXT_TYPE_LITTLE, "Alpha Team", 1.0f, 1.0f, 1.0f, 1.0f);
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
                    miscStuff.renderText(0.1, 0.935, 0.0, TEXT_TYPE_LITTLE, "Resident Evil 2", 1.0f, 1.0f, 1.0f, 1.0f);
                    miscStuff.renderText(0.1, 1.005, 0.0, TEXT_TYPE_LITTLE, "Escape Alarm Theme", 1.0f, 1.0f, 1.0f, 1.0f);
                break;

                case 1:
                    miscStuff.renderText(0.1, 0.935, 0.0, TEXT_TYPE_LITTLE, "Resident Evil:TDSC", 1.0f, 1.0f, 1.0f, 1.0f);
                    miscStuff.renderText(0.1, 1.005, 0.0, TEXT_TYPE_LITTLE, "Sorrow - Steve Battle", 1.0f, 1.0f, 1.0f, 1.0f);
                break;

                case 2:
                    miscStuff.renderText(0.1, 0.935, 0.0, TEXT_TYPE_LITTLE, "Resident Evil:TDSC", 1.0f, 1.0f, 1.0f, 1.0f);
                    miscStuff.renderText(0.1, 1.005, 0.0, TEXT_TYPE_LITTLE, "Blood Judgement - Final Boss", 1.0f, 1.0f, 1.0f, 1.0f);
                break;
                
                case 3:
                    miscStuff.renderText(0.1, 0.935, 0.0, TEXT_TYPE_LITTLE, "Resident Evil CODE: Veronica", 1.0f, 1.0f, 1.0f, 1.0f);
                    miscStuff.renderText(0.1, 1.005, 0.0, TEXT_TYPE_LITTLE, "Alexia - Final Boss Theme", 1.0f, 1.0f, 1.0f, 1.0f);

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
                                230.0f + nextCharAngle, 0, mainPlayer.getAnimFrame());

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
                                45.0f+ nextCharAngle, 3,  mainPlayer.getAnimFrame());
                    }
                    break;

                    default:

                    break;
                }

                miscStuff.renderSquareWithTexture(&engineCharMenu, true);        
            } else {
                gameState = STATE_IN_GAME;
                inGame    = IN_GAME_BEGIN;
                mainPlayer.setModel(menuArrow + 1);
                mainPlayer.setAnimSection(EMD_SECTION_4);
                mainPlayer.setAnimType(STAND_SEC4_ANIM_IDLE);
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
                        soundEngine.engineLoadSound("resource/ost/8.mp3");
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
                mainPlayer.setCam(CAMERA_STYLE_SPECIAL);
                soundEngine.enginePlaySound(1);
                inGame = IN_GAME_NORMAL;
            }
        break;

        case IN_GAME_NORMAL:
            switch (vrMode.getState()) {
                case VR_STATE_IN_GAME:
                    /* .BSS Background stuff */
                    //drawMapBackground();

                    /* All model rendering is done by renderEMD Function */
                    /* Player Rendering */
                    renderEMD(mainPlayer.getX(), mainPlayer.getY(), mainPlayer.getZ(), 
                              mainPlayer.getAngle(), mainPlayer.getModel(), mainPlayer.getAnimFrame());



                    /* Enemy Rendering */

                    for (unsigned int i = 0; i < enemyList.size(); i++) {
                        renderEMD(enemyList[i].getX(), enemyList[i].getY(),enemyList[i].getZ(), 
                                  enemyList[i].getAngle(), enemyList[i].getModel(), enemyList[i].getAnimFrame());

                        renderBoundingBox(enemyList[i].getX(), enemyList[i].getY(), enemyList[i].getZ());
                        renderShadow(enemyList[i].getX(), enemyList[i].getY(), enemyList[i].getZ());
                    }

                    renderBoundingBox(mainPlayer.getX(), mainPlayer.getY(), mainPlayer.getZ());
                    renderShadow(mainPlayer.getX(), mainPlayer.getY(), mainPlayer.getZ());



                    char coord[0xFF];

                    sprintf(coord, "XYZA %d/%d/%d/%d", (int)mainPlayer.getX(),(int)mainPlayer.getY(),(int)mainPlayer.getZ(), (int)mainPlayer.getAngle());
                    miscStuff.renderText(0.0f, 1.00f, 100.0f, TEXT_TYPE_LITTLE, coord);

                    vrMode.gameLogic();

                break;

                case VR_STATE_IN_BEND:
                    miscStuff.setupFadeEffect(TYPE_FADE_OUT, 0, 0, 0, 260);
                    vrMode.setState(VR_STATE_IN_END);
                break;

                case VR_STATE_IN_END: {
                    miscStuff.renderSquareWithTexture(&engineResult, false);
                    
                    if (!miscStuff.isInFade()) {
                        /*
                         * result information
                         */


                         /*
                          * player name
                          */
                         if (mainPlayer.getModel() == 1) {
                            miscStuff.renderText(0.15, 0.25, 0.0, TEXT_TYPE_NORMAL, "LEON", 0.0f, 0.0f, 1.0f, 1.0f);
                         } else {
                            miscStuff.renderText(0.15, 0.25, 0.0, TEXT_TYPE_NORMAL, "CHRIS", 0.0f, 0.0f, 1.0f, 1.0f);
                         }


                        /* 
                         * game time
                         */
                        miscStuff.renderText(0.15, 0.35, 0.0, TEXT_TYPE_NORMAL, "TIME: ", 1.0f, 1.0f, 1.0f, 1.0f);
                        miscStuff.renderText(0.50, 0.35, 0.0, TEXT_TYPE_NORMAL, vrMode.timer.buffer, 0.0f, 1.0f, 0.0f, 1.0f);
                       
                        /* 
                         * ammo
                         */
                        miscStuff.renderText(0.15, 0.45, 0.0, TEXT_TYPE_NORMAL, "AMMO: ", 1.0f, 1.0f, 1.0f, 1.0f);
                        char dBuffer[4];
                        sprintf(dBuffer, "%d", vrMode.ammo);
                        miscStuff.renderText(0.50, 0.45, 0.0, TEXT_TYPE_NORMAL, dBuffer, 1.0f, 0.0f, 0.0f, 1.0f);
                        
                        /*
                         * kills
                         */
                        miscStuff.renderText(0.15, 0.55, 0.0, TEXT_TYPE_NORMAL, "KILLS: ", 1.0f, 1.0f, 1.0f, 1.0f);
                        sprintf(dBuffer, "%d", vrMode.gameScore);
                        miscStuff.renderText(0.60, 0.55, 0.0, TEXT_TYPE_NORMAL, dBuffer, 1.0f, 0.0f, 0.0f, 1.0f);


                        int rankNum = 0;

                        /* RANK B OR C */
                        if ((vrMode.timer.min > 2)) {
                            /* RANK B OR C */
                            if (vrMode.ammo > ((vrMode.gameScore * 3) + 6)) {
                                rankNum = 3;
                            } else {
                                rankNum = 2;
                            }
                        } else {
                            /* RANK B */
                            if (vrMode.ammo > ((vrMode.gameScore * 3) + 6)) {
                                rankNum = 2;
                            } else {
                                rankNum = 1;
                            }
                        }
                         
                        switch (rankNum) {
                            case 1: {
                                /*
                                 * RANK
                                 */
                                miscStuff.renderText(0.15, 0.65, 0.0, TEXT_TYPE_NORMAL, "YOUR RANK: ", 1.0f, 1.0f, 1.0f, 1.0f);
                                miscStuff.renderText(0.85, 0.65, 0.0, TEXT_TYPE_NORMAL, "A", 1.0f, 1.0f, 0.0f, 1.0f);

                                /*
                                 * RANK INFO
                                 */

                                miscStuff.renderText(0.10, 0.85, 0.0, TEXT_TYPE_LITTLE, "Nice Job ! You're the", 1.0f, 1.0f, 1.0f, 1.0f);
                                miscStuff.renderText(0.10, 0.90, 0.0, TEXT_TYPE_LITTLE, "true Big Boss", 1.0f, 1.0f, 1.0f, 1.0f);
                            }
                            break;

                            case 2: {
                            
                                /*
                                 * RANK
                                 */
                                miscStuff.renderText(0.15, 0.65, 0.0, TEXT_TYPE_NORMAL, "YOUR RANK: ", 1.0f, 1.0f, 1.0f, 1.0f);
                                miscStuff.renderText(0.85, 0.65, 0.0, TEXT_TYPE_NORMAL, "B", 0.1f, 0.1f, 1.0f, 1.0f);

                                /*
                                 * RANK INFO
                                 */

                                miscStuff.renderText(0.10, 0.85, 0.0, TEXT_TYPE_LITTLE, "Well Done ! Wait to see you", 1.0f, 1.0f, 1.0f, 1.0f);
                                miscStuff.renderText(0.10, 0.90, 0.0, TEXT_TYPE_LITTLE, "for the next mission", 1.0f, 1.0f, 1.0f, 1.0f);

                            }
                            break;

                            case 3: {
                                /*
                                 * RANK
                                 */
                                miscStuff.renderText(0.15, 0.65, 0.0, TEXT_TYPE_NORMAL, "YOUR RANK: ", 1.0f, 1.0f, 1.0f, 1.0f);
                                miscStuff.renderText(0.85, 0.65, 0.0, TEXT_TYPE_NORMAL, "C", 1.0f, 0.1f, 0.1f, 1.0f);

                                /*
                                 * RANK INFO
                                 */

                                miscStuff.renderText(0.10, 0.85, 0.0, TEXT_TYPE_LITTLE, "It's a Job !", 1.0f, 1.0f, 1.0f, 1.0f);
                                miscStuff.renderText(0.10, 0.90, 0.0, TEXT_TYPE_LITTLE, "At least you tried", 1.0f, 1.0f, 1.0f, 1.0f);

                            }

                            break;
                        }

                    }
                }
                break;
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
    glClearColor(0.4, 0.4, 0.4, 1.0);

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

/*
 * Handle everythings related with gun and ammo
 */
void gameCore::gunHandle() {
    std::vector<int>    colList;
    std::vector<float>  Dlist;

    for (unsigned int i = 0; i < enemyList.size(); i++) {
        if (enemyList[i].getState() != AI_STATE_DEATH) {
            if (mathEngine.collisionShoot(mainPlayer.getX(), mainPlayer.getY(), mainPlayer.getZ(),
                                                                  enemyList[i].getX(), enemyList[i].getY(), enemyList[i].getZ(),
                                                                  mainPlayer.getAngle())) {
                colList.push_back(i);
                Dlist.push_back(mathEngine.d2Point(mainPlayer.getX(), mainPlayer.getY(), mainPlayer.getZ(),
                                   enemyList[i].getX(), enemyList[i].getY(), enemyList[i].getZ()));
            }
        }
    }


    /*
     * there is not target
     */
    if (colList.size() == 0) 
        return;

    /*
     * there are targets at the same angle 
     */
    if (colList.size() > 1) {

        float sD = Dlist[0];
        int   eI = colList[0];
        for (unsigned int i = 1; i < colList.size(); i++) {
            if (sD > Dlist[i]) {
                sD = Dlist[i];
                eI = colList[i];
            }
        }

        enemyList[eI].setState(AI_STATE_HIT);   
    } else {
        enemyList[colList[0]].setState(AI_STATE_HIT);
    }



}


/*
 * This function get the player state and handle it
 */
void gameCore::handlePlayerAction() {
    float x = 0;
    float z = 0;
    bool canMove = true;

    switch (mainPlayer.getState()) {
        case PLAYER_STATE_NORMAL: {

            switch (mainPlayer.getAnimRotationDir()) {
                case PLAYER_ACTION_R_LEFT:
                    mainPlayer.setAngle(fmod((mainPlayer.getAngle() - 2), 360.0));
                break;

                case PLAYER_ACTION_R_RIGHT:
                    mainPlayer.setAngle(fmod((mainPlayer.getAngle() + 2), 360.0));
                break;

                case PLAYER_ACTION_R_NONE:

                break;
            }

            if ((keyList[EVENT_SYSTEM_KEY_UP] == true)) {
                if (mainPlayer.getAnimSection() == EMD_SECTION_4) {
                    if (mainPlayer.getAnimType() == STAND_SEC4_ANIM_WALK) {
                        x = mainPlayer.getX() + cos((mainPlayer.getAngle() * PI/180)) * 80.0;
                        z = mainPlayer.getZ() - sin((mainPlayer.getAngle() * PI/180)) * 80.0;
                        for (unsigned int i = 0; i < enemyList.size(); i++) {                        
                            if (mathEngine.collisionRectangle(x, mainPlayer.getY(), z,
                                                              enemyList[i].getX(),enemyList[i].getY(), enemyList[i].getZ())) {
                                canMove = false;
                                break;
                            } 
                        }

                        if (canMove) {
                            mainPlayer.setX(x);
                            mainPlayer.setZ(z);
                        }

                    }
                }

            } else if ((keyList[EVENT_SYSTEM_KEY_DOWN] == true)) {
                if (mainPlayer.getAnimSection() == EMD_SECTION_2) {
                    if (mainPlayer.getAnimType() == STAND_SEC2_ANIM_BACKWARD) {
                        x = mainPlayer.getX() - cos((mainPlayer.getAngle() * PI/180)) * 80.0;
                        z = mainPlayer.getZ() + sin((mainPlayer.getAngle() * PI/180)) * 80.0;

                        for (unsigned int i = 0; i < enemyList.size(); i++) {
                            if (mathEngine.collisionRectangle(x, mainPlayer.getY(), z,
                                                              enemyList[i].getX(),enemyList[i].getY(), enemyList[i].getZ())) {
                                canMove = false;
                                break;
                            } 
                        }   
                    
                        if (canMove) {
                            mainPlayer.setX(x);
                            mainPlayer.setZ(z);
                        }

                    }
                }

            }     
        }

        break;

        case PLAYER_STATE_BEGIN_HIT: {
            mainPlayer.setAnimSection(EMD_SECTION_2);
            mainPlayer.setAnimType(STAND_SEC2_ANIM_HIT_2);
            mainPlayer.setCam(CAMERA_STYLE_SPECIAL);
            for (int i = 0; i < 7; i++) {
                keyList[i] = false;
            }

            if (mainPlayer.getHitPoints() == 0) {
                mainPlayer.setState(PLAYER_STATE_BEGIN_DEATH);
            } else {
                mainPlayer.setState(PLAYER_STATE_HIT);
            }
        }
        break;

        case PLAYER_STATE_HIT: {

        }
        break;

        case PLAYER_STATE_BEGIN_GUN: {
            mainPlayer.setAnimType(STAND_SEC4_ANIM_SHOOTING);
            mainPlayer.setState(PLAYER_STATE_GUN);
        }
        break;

        case PLAYER_STATE_GUN: {
            /*
             * Shoot action
             */
            if ((mainPlayer.getAnimSection() == EMD_SECTION_4) && (mainPlayer.getAnimType() == STAND_SEC4_ANIM_SHOOTING)) {

                if (mainPlayer.getAnimCount() == 22) {
                    gunHandle();
                    vrMode.ammo++;
                    if (keyList[EVENT_SYSTEM_KEY_Z]) {
                        mainPlayer.setAnimCount(0);
                    } else {

                        if (keyList[EVENT_SYSTEM_KEY_X]) {
                            mainPlayer.setAnimType(STAND_SEC4_ANIM_AIM);
                        } else {
                            mainPlayer.setCam(CAMERA_STYLE_SPECIAL);
                            mainPlayer.setAnimType(STAND_SEC4_ANIM_IDLE);
                        }

                        mainPlayer.setAnimSection(EMD_SECTION_4);
                        mainPlayer.setState(PLAYER_STATE_NORMAL);

                    }
                }
            }
        }
        break;

        case PLAYER_STATE_BEGIN_DEATH:
            mainPlayer.setAnimSection(EMD_SECTION_2);
            mainPlayer.setAnimType(STAND_SEC2_ANIM_DEATH, false);
            mainPlayer.setState(PLAYER_STATE_DEATH);
            miscStuff.setupFadeEffect(TYPE_FADE_IN, 0, 0, 0, 260);
        break;

        case PLAYER_STATE_DEATH: {
            EMD_SEC2_DATA_T node = mainPlayer.getAnimFrame();    
            mainPlayer.setY(2000 + node.yOffset);

            if (!miscStuff.isInFade()) {
                
            }
        }
        break;

    }

}

