#ifndef LED_MATRIX_H
#define LED_MATRIX_H

#include "LedControl.h"

struct bomb{
  byte x;
  byte y;
  byte state;
  unsigned long bombStart;
};

struct direction{
  int x;
  int y;
};

struct enemy{
  int x;
  int y;
  int moveDirection;
};

#define DIN_PIN  12
#define CLOCK_PIN  11
#define LOAD_PIN  10

#define GO_BACK_PIN 2
#define PRESS_PIN 13
#define X_PIN A0
#define Y_PIN  A1

#define EMPTY_SPACE 0
#define PLAYER 1
#define BOMB 2
#define WALL 3
#define ENEMY 4

#define DROPPED_BOMB 4
#define EXPAND_BOMB 5
#define DELETE_BOMB 6

#define MIN_THRESHOLD  312
#define MAX_THRESHOLD 712
#define DEFAULT_JOYSTICK 512
#define MOVE_INTERVAL 100 

#define MATRIX_SIZE 8 
#define MAP_SIZE 16
#define HEIGHT_OF_CUSTOM_CHARS 8

#define SELECT_BRIGHTNESS 2
#define PLAYER_BLINK_INTERVAL  200
#define ENEMY_BLINK_INTERVAL 100

#define PRESS_DEBOUNCE_TIME 50

#define BOMBS_BLINK_INTERVAL  30
#define BOMBS_EXPANDING_INTERVAL 1000
#define BOMBS_DISAPPEARING_INTERVAL 1500

#define NR_OF_DIRECTIONS 4
#define NR_OF_EXTENDED_BOMB_COORDONATES 13
#define DEFAULT_ANIMATION_DISPLAY 2000
#define SKULL_ANIMATION_BLINK 100

#define NR_OF_START_ANIMATION_FRAMES 2
#define INTERVAL_BETWEEN_FRAMES 300
#define DEFAULT_START_ANIMATION 2000

#define PLAYING 0
#define DEATH_ANIMATION 1
#define FINISH_LEVEL_ANIMATION 2
#define START_ANIMATION 3
#define MENU 4
#define WINNING_ANIMATION 5

#define LEVEL1 1
#define LEVEL2 2
#define LEVEL3 3
#define NR_OF_ENEMYS_LEV1 0
#define NR_OF_ENEMYS_LEV2 2
#define NR_OF_ENEMYS_LEV3 4

#define FIRST_ENEMY 0
#define SECOND_ENEMY 1
#define THIRD_ENEMY 2
#define FORTH_ENEMY 3

#define ENEMY_MOVE_INTERVAL_LEV2 300
#define ENEMY_MOVE_INTERVAL_LEV3 200
#define PLAYER_IS_DAMAGED_DURATION_PERCENTAGE 3/4
#define FIRST_DIRECTION 0
void updateMatrix();


void generateMap();

void expandBomb(bomb currentBomb);

void deleteBomb(bomb currentBomb);

bool checkIfLostLife(bomb currentBomb);

void updatePositions();

bool inMatrix(int i,int j);

void putBomb();

void bombsBlink();

void playerBlink();

void enemyBlink();

void enemyMove(int enemyIndex);

void showFrameAnimation(bool frame[MATRIX_SIZE][MATRIX_SIZE]);

bool checkIfWon();

void loseLife();

void showMatrixMenu();

int module(int number);

int countWalls();

int updateScore();

#endif
