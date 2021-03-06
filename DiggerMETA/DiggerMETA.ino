//#include <SPI.h>
#include <Gamebuino-Meta.h>
#include "Defines.h"
#include "Sprites.h"
#include "Level.h"
#include "Menu.h"

byte world[WORLD_W][WORLD_H];

//generate Multilang-Array for pause menu
const MultiLang* Pausemenu[MENULENGTH]  = {
  lang_new,
  lang_resume,
  lang_load,
  lang_save,
  lang_levelup,
  lang_leveldown,
  lang_toggle,
};


// cursor
int cursor_x, cursor_y;
int camera_x, camera_y;

byte diams_need, diamonds;
byte curLevel = 0;
byte maxlevel = 0;
byte maxhard = 0;
byte curhard = 0;
byte maxeasy = 0;
byte cureasy = 0;
byte lives = LIVES;
bool dead = false;
bool gamemode = EASY;
byte gamestate = RUNNING;
int  codepos = 0;
int  worldpos = 0;
byte dir = 7;
byte timer = 0;
byte color=0;
unsigned long starttime=0;
void setup(){
  gb.begin();
  initGame();
}

void loop() {
    while (!gb.update()); 
    switch (gamestate) {
      case PAUSED:
        switch (gb.gui.menu(lang_title,Pausemenu, MENULENGTH)) {
          case -1: //nothing selected
            break;
          case 3: //save
            if (lives > 0) {
              lives--;
              color=200;
              saveGame();
              gamestate = RUNNING;
              dead = false;
            }
            break;
          case 2: //load game
            loadGame();
            gamestate = RUNNING;
            dead = false;
            initWorld(curLevel);
            break;
          case 1: //resume game
            gamestate = RUNNING;
            break;
          case 0: //new game
            newGame();

            initWorld(curLevel);
            break;
          case 4: //lvlup
            gamestate = RUNNING;
            dead = false;
            if (curLevel < ((gamemode == EASY) ? maxeasy : maxhard)) nextLevel();
            break;
          case 5: //lvldown
            if (curLevel > 0) {
              (gamemode == EASY) ? cureasy-- : curhard--;
              curLevel--;
            }
            dead = false;
            gamestate = RUNNING;
            initWorld(curLevel);
            break;
          case 6: //gamemode
            gamemode = !gamemode;
            curLevel = (gamemode == EASY) ? cureasy : curhard;
            dead = false;
            initWorld(curLevel);
            gamestate = RUNNING;
            break;
          default:
            break;
        }; break;

      case RUNNING:
        //pause the game if menu is pressed
        if (gb.buttons.pressed(BUTTON_MENU)) {
          gamestate = PAUSED;
          color=0;
        }
        updateCursor();
        updatePhysics();
        drawWorld();
        if (lives <1 || lives>LIVES) {
          gamestate=GAMEOVER;
        }
        break;
      case GAMEOVER:
        newGame();
        dead = false;
        gamestate = RUNNING;
        break;
      default:
        break;
    }
    
    if (color > 0){ 
        color -=10;
        gb.lights.fill(gb.createColor(color, 0, 0));
    }
  
}

void initGame() {
  loadGame();
  initWorld(curLevel);
}

void newGame() {
  curLevel = 0;
  lives = LIVES;
  maxeasy = 0;
  cureasy = 0;
  maxhard = 0;
  curhard = 0;
  gamestate = RUNNING;
  dead = false;
}

//decoder of compressed level data
void initWorld(byte _level) {
  diamonds = 0;
  codepos = 0;
  worldpos = 0;
  diams_need = getBits(_level, 8);
  while (worldpos < WORLD_H * WORLD_W) {
    if (getBits(_level, 1)) {
      byte r = getBits(_level, BITREF);
      byte c = getBits(_level, BITCOUNT) + MINMATCH;

      for (byte i = 0; i < c; i++) {
        if (worldpos < WORLD_W * WORLD_H) {
          int refpos = worldpos - r;
          setTile(world[refpos % WORLD_W][refpos / WORLD_W]);
          worldpos++;
        }
      }
    } else {
      setTile(getBits(_level, CODELENGTH));
      worldpos++;
    }

  }
}

//helper for decompressor, returns number of bits of an compressed level starting at (bit-) position "codepos" 
byte getBits(byte _level, byte bits) {
  //uses global var codepos for bitoffset
  const byte *pos = levels[_level] + codepos / 8;
  int w = pgm_read_byte(pos) * 256 + pgm_read_byte(pos + 1);
  w >>= (16 - (codepos % 8) - bits); //shift bits to right
  w = ( w & ((1 << bits) - 1)); //mask desired bits
  codepos += bits;
  return (byte)w;
}

// set Tile to worldmap. Note: codes in level data have slightly different meanings than in world map.
void setTile( byte sprite) {
  byte x = worldpos % WORLD_W;
  byte y = worldpos / WORLD_W;
  switch (sprite) {
    case MONSTER_RT: world[x][y] = MONSTER_R | TYPE_RT; break;
    case MONSTER_LT: world[x][y] = MONSTER_L | TYPE_LT; break;
    case MONSTER_D:  world[x][y] = MONSTER_D | TYPE_UD; break;
    case MONSTER_U:  world[x][y] = MONSTER_U | TYPE_UD; break;
    case MONSTER_L:  world[x][y] = MONSTER_L | TYPE_LR; break;
    case MONSTER_R:  world[x][y] = MONSTER_R | TYPE_LR; break;
    default: world[x][y] = sprite; break;
  }
  if (sprite == PLAYER) {
    cursor_x =  x;
    cursor_y = y;
  }
}
void nextLevel() {
  if (curLevel < NUM_LEVELS-3) {
    (gamemode == EASY) ? cureasy++ : curhard++;
    maxeasy = max(maxeasy, cureasy);
    maxhard = max(maxhard, curhard);
    curLevel++;
  }
  initWorld(curLevel);
}

void loadGame(){
  gamemode = gb.save.get(0);
  cureasy = gb.save.get(1);
  maxeasy = gb.save.get(2);
  lives   = gb.save.get(3);
  curhard = gb.save.get(5);
  maxhard = gb.save.get(6);
  maxeasy = max(maxeasy, cureasy);
  maxhard = max(maxhard, curhard);
  curLevel = (gamemode == EASY) ? cureasy : curhard;
  maxlevel = (gamemode == EASY) ? maxeasy : maxhard;
}

void saveGame() {
 gb.save.set(0, gamemode);
 gb.save.set(1, cureasy);
 gb.save.set(2, maxeasy);
 gb.save.set(3, lives);
 gb.save.set(5, curhard);
 gb.save.set(6, maxhard);
}

