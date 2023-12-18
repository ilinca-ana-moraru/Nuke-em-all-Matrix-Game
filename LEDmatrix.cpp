#include "LEDmatrix.h"
#include "LCD.h"

extern unsigned long long lastMoved;
extern LedControl lc;

extern short xBias;
extern short yBias;
extern byte xDefaultDistanceBetweenPosAndBias;
extern byte yDefaultDistanceBetweenPosAndBias;
extern byte xPos;
extern byte yPos;
extern byte xLastPos;
extern byte yLastPos;

extern bool matrixChanged;
extern byte matrix[MAP_SIZE][MAP_SIZE];

extern bool playerBlinkingState;
extern unsigned long playerLastBlink;

extern bomb* bombs;

extern byte nrOfBombs;
extern bool pressState;
extern bool pressRead;
extern unsigned long lastPressCheck;

extern bool bombsBlinkingState;
extern unsigned long bombsLastBlink;
extern unsigned long randomNumber;

extern direction directions[NR_OF_DIRECTIONS];
extern direction extendedBombCoordonates[NR_OF_EXTENDED_BOMB_COORDONATES];

extern bool skullAnimation[MATRIX_SIZE][MATRIX_SIZE];
extern unsigned long showDeathStart;
extern unsigned long showFinishLevelStart;
extern unsigned int timeBetweenDeathFrames;

extern bool finishGameAnimation[MATRIX_SIZE][MATRIX_SIZE];
extern unsigned long AnimationStart;
extern unsigned long timeBetweenStartFrames;
extern bool currentStartAnimationFrame;
extern bool startAnimation[NR_OF_START_ANIMATION_FRAMES][MATRIX_SIZE][MATRIX_SIZE];
extern byte lifes;
extern byte gameState;
extern byte LCDState;
extern bool LCDChanged;
extern byte level;
enemy enemys[4];
byte nrOfEnemys;
bool enemyBlinkingState = true;
int enemyMoveInterval;
int damageInterval;
extern int nrOfWalls;
extern long bombWallsTimer;
extern int score;
extern int totalScore;
extern byte lifes;

void updateMatrix() {

  // check for things to update in the matrix
  if(sizeof(bombs) > 0){
    for(int i = 0; i < nrOfBombs; i++){
      if(bombs[i].state == DROPPED_BOMB){
        matrix[bombs[i].x][bombs[i].y] = BOMB;
      }
      if(bombs[i].state == EXPAND_BOMB){
        expandBomb(bombs[i]);
      }
      if(bombs[i].state == DELETE_BOMB){
        if(checkIfLostLife(bombs[i])){
          loseLife();
        }
        deleteBomb(bombs[i]);
        updateScore();
        if(checkIfWon()){
          gameState = FINISH_LEVEL_ANIMATION;
        }

        
      }
    }
  }
  

  if(gameState == DEATH_ANIMATION){
    showFrameAnimation(skullAnimation);
    showDeathStart = millis();
  }

  else if(gameState == FINISH_LEVEL_ANIMATION){
    showFrameAnimation(finishGameAnimation);
    showFinishLevelStart = millis();
  }

  else if(gameState == START_ANIMATION){
    showFrameAnimation(startAnimation[currentStartAnimationFrame]);
  }
  
  // display visible space of the map is game is played
  if(gameState == PLAYING){

    for (int row = xBias; row < xBias + MATRIX_SIZE; row++) {
      for (int col = yBias; col < yBias + MATRIX_SIZE; col++) {
        if(row == xPos && col ==yPos){
          lc.setLed(0, xPos - xBias, yPos - yBias, playerBlinkingState);
        }
        else{
          if(matrix[row][col] == ENEMY){
            lc.setLed(0, row - xBias, col - yBias, enemyBlinkingState);
          }
          else if(matrix[row][col] == WALL){
            lc.setLed(0, row - xBias, col - yBias, 1);
          }
          else if(matrix[row][col] == EMPTY_SPACE){
            lc.setLed(0, row - xBias, col - yBias, 0);
          }
          else if(matrix[row][col] == BOMB){
            lc.setLed(0, row - xBias, col - yBias, bombsBlinkingState);
          }
        }
      }
    }
  }

}

// generates new map
void generateMap(){
  matrix[6][6] = WALL;
  xPos = xDefaultDistanceBetweenPosAndBias + xBias;
  yPos = yDefaultDistanceBetweenPosAndBias + yBias;
  // for(int i = 0; i < MAP_SIZE; i++){
  //   for(int j = 0; j < MAP_SIZE; j++){
  //     randomNumber = random(2);
  //       if(randomNumber > 0){
  //         matrix[i][j] = WALL;
  //       }
  //       else{
  //         matrix[i][j] = EMPTY_SPACE;
  //       }
  //   }
  // }
  // make sure that it is possible for the player to move from initial position and drop bomb
  matrix[xPos][yPos] = EMPTY_SPACE;
  // checks if there is already a way to move from  initial position
  for(int i = 0; i < NR_OF_DIRECTIONS; i++){
    if(matrix[xPos + 3 * directions[i].x][yPos + 3* directions[i].y] == EMPTY_SPACE &&
    matrix[xPos + 2 * directions[i].x][yPos + 2 * directions[i].y] == EMPTY_SPACE &&
    matrix[xPos + directions[i].x][yPos + directions[i].y] == EMPTY_SPACE)
      return;
  }

  // if there is no direction to move, generate one randomly
  direction randomDirection = directions[random(NR_OF_DIRECTIONS)];
  randomDirection = directions[random(NR_OF_DIRECTIONS)];
  

  matrix[xPos + 3 * randomDirection.x][yPos + 3 * randomDirection.y] = EMPTY_SPACE;
  matrix[xPos + 2 * randomDirection.x][yPos + 2 * randomDirection.y] = EMPTY_SPACE;
  matrix[xPos + randomDirection.x][yPos + randomDirection.y] = EMPTY_SPACE;      

  // if(level == LEVEL1){
  //   nrOfEnemys = NR_OF_ENEMYS_LEV1;
  // }

  // //enemys
  // if(level == LEVEL2){
  //   nrOfEnemys = NR_OF_ENEMYS_LEV2;
  //   enemyMoveInterval = ENEMY_MOVE_INTERVAL_LEV2;
  //   damageInterval = enemyMoveInterval * PLAYER_IS_DAMAGED_DURATION_PERCENTAGE;

  //   enemys[FIRST_ENEMY].x = random(MAP_SIZE);
  //   if(enemys[FIRST_ENEMY].x > 4 && enemys[FIRST_ENEMY].x < 12){
  //     enemys[FIRST_ENEMY].y = random(4);
  //   }
  //   else{
  //     enemys[FIRST_ENEMY].y = random(MAP_SIZE/2);
  //   }
  //   matrix[enemys[FIRST_ENEMY].x][enemys[FIRST_ENEMY].y] = ENEMY;
  //   enemys[FIRST_ENEMY].moveDirection = FIRST_DIRECTION;

  //   enemys[SECOND_ENEMY].x = MAP_SIZE - 1 - enemys[FIRST_ENEMY].x;
  //   enemys[SECOND_ENEMY].y = MAP_SIZE - 1 - enemys[FIRST_ENEMY].y;
  //   matrix[enemys[SECOND_ENEMY].x][enemys[SECOND_ENEMY].y] = ENEMY;
  //   enemys[SECOND_ENEMY].moveDirection = FIRST_DIRECTION;

  // }
  //   if(level == LEVEL3){
  //   nrOfEnemys = NR_OF_ENEMYS_LEV3;
  //   enemyMoveInterval = ENEMY_MOVE_INTERVAL_LEV3;
  //   damageInterval = enemyMoveInterval * PLAYER_IS_DAMAGED_DURATION_PERCENTAGE;

  //   enemys[FIRST_ENEMY].x = random(MAP_SIZE/2);
  //   if(enemys[FIRST_ENEMY].x > 4){
  //     enemys[FIRST_ENEMY].y = random(4);
  //   }
  //   else{
  //     enemys[FIRST_ENEMY].y = random(MAP_SIZE/2);
  //   }
  //   matrix[enemys[FIRST_ENEMY].x][enemys[FIRST_ENEMY].y] = ENEMY;
  //   enemys[FIRST_ENEMY].moveDirection = FIRST_DIRECTION;

  //   enemys[SECOND_ENEMY].x = MAP_SIZE - 1 - enemys[FIRST_ENEMY].x;
  //   enemys[SECOND_ENEMY].y = MAP_SIZE - 1 - enemys[FIRST_ENEMY].y;
  //   matrix[enemys[SECOND_ENEMY].x][enemys[SECOND_ENEMY].y] = ENEMY;
  //   enemys[SECOND_ENEMY].moveDirection = FIRST_DIRECTION;


  //   enemys[THIRD_ENEMY].x = random(MAP_SIZE/2)+ MAP_SIZE/2;
  //   if(enemys[THIRD_ENEMY].x < 12){
  //     enemys[THIRD_ENEMY].y = random(4);
  //   }
  //   else{
  //     enemys[THIRD_ENEMY].y = random(MAP_SIZE/2);
  //   }

  //   matrix[enemys[THIRD_ENEMY].x][enemys[THIRD_ENEMY].y] = ENEMY;
  //   enemys[THIRD_ENEMY].moveDirection = FIRST_DIRECTION;

  //   enemys[FORTH_ENEMY].x = MAP_SIZE - 1 - enemys[THIRD_ENEMY].x;
  //   enemys[FORTH_ENEMY].y = MAP_SIZE - 1 - enemys[THIRD_ENEMY].y;
  //   matrix[enemys[FORTH_ENEMY].x][enemys[THIRD_ENEMY].y] = ENEMY;
  //   enemys[FORTH_ENEMY].moveDirection = FIRST_DIRECTION;

  // }

}


void enemyMove(int enemyIndex){
  while(inMatrix(enemys[enemyIndex].x + directions[enemys[enemyIndex].moveDirection].x, enemys[enemyIndex].y + directions[enemys[enemyIndex].moveDirection].y)== false ||
     matrix[enemys[enemyIndex].x + directions[enemys[enemyIndex].moveDirection].x][enemys[enemyIndex].y + directions[enemys[enemyIndex].moveDirection].y] == WALL){
    enemys[enemyIndex].moveDirection = random(NR_OF_DIRECTIONS);
  }
  matrix[enemys[enemyIndex].x][enemys[enemyIndex].y] = EMPTY_SPACE;
  enemys[enemyIndex].x += directions[enemys[enemyIndex].moveDirection].x;
  enemys[enemyIndex].y += directions[enemys[enemyIndex].moveDirection].y;
  matrix[enemys[enemyIndex].x][enemys[enemyIndex].y] = ENEMY;
  matrixChanged = true;
}

//expands the bomb on the map
void expandBomb(bomb currentBomb){  
  for(int currentCoordonates = 0; currentCoordonates < NR_OF_EXTENDED_BOMB_COORDONATES; currentCoordonates++){
    int i = extendedBombCoordonates[currentCoordonates].x + currentBomb.x;
    int j = extendedBombCoordonates[currentCoordonates].y + currentBomb.y;
      if(inMatrix(i, j)){
        matrix[i][j] = BOMB;
      }
    }
  }

//deletes bomb on the matrix
void deleteBomb(bomb currentBomb) {

  for (int i = 0; i < nrOfBombs - 1; i++) {
    bombs[i] = bombs[i + 1];
  }
  nrOfBombs--;
  bombs = (bomb*)realloc(bombs, nrOfBombs * sizeof(bomb));
  for(int currentCoordonates = 0; currentCoordonates < NR_OF_EXTENDED_BOMB_COORDONATES; currentCoordonates++){
    int i = extendedBombCoordonates[currentCoordonates].x + currentBomb.x;
    int j = extendedBombCoordonates[currentCoordonates].y + currentBomb.y;
      if(inMatrix(i, j)){
        matrix[i][j] = EMPTY_SPACE;
      }
    }

}

//checks if user was killed by the exploding bomb
bool checkIfLostLife(bomb currentBomb){
  
  for(int currentCoordonates = 0; currentCoordonates < NR_OF_EXTENDED_BOMB_COORDONATES; currentCoordonates++){
    int i = extendedBombCoordonates[currentCoordonates].x + currentBomb.x;
    int j = extendedBombCoordonates[currentCoordonates].y + currentBomb.y;
      if(xPos == i && yPos == j){
        return true;
      }
    }
  return false;
}

void loseLife(){
  lifes--;
  LCDChanged = true;

  if(lifes == NR_OF_LIFES_FOR_LOOSING){
    totalScore += score;
    gameState = WINNING_ANIMATION;
    LCDState = WINNING_INFO1;
    matrixChanged = true;
  }        
}

//read joystick input and update the position of the player
void updatePositions() {
  int xValue = analogRead(Y_PIN);
  int yValue = analogRead(X_PIN);
  xLastPos = xPos;
  yLastPos = yPos;
  
  // update xPos based on joystick movement (X-axis)
  if (xValue > MAX_THRESHOLD) {
    if (xPos < MAP_SIZE - 1) {
      xPos++;
    } 
  }
  if (xValue < MIN_THRESHOLD) {
    if (xPos > 0) {
      xPos--;
    }
  }

  // update yPos based on joystick movement (Y-axis)
  if (yValue > MAX_THRESHOLD) {
    if (yPos < MAP_SIZE - 1) {
      yPos++;
    } 
  }
  if (yValue < MIN_THRESHOLD) {
    if (yPos > 0) {
      yPos--;
    }
  }
 // check if the position has changed and update the matrix if necessary
  if (xPos != xLastPos || yPos != yLastPos) {
    // if both the X-axis and Y-axis were changed and chenge only the one with the biggest movement on the axis
    if (xPos != xLastPos && yPos != yLastPos){
      if(module(DEFAULT_JOYSTICK - xValue) > module(DEFAULT_JOYSTICK - yValue)){
        yPos = yLastPos;
      }
      else{
        xPos = xLastPos;
      } 
    }
    //check that the next space is empty
    if(matrix[xPos][yPos] == EMPTY_SPACE){
      //move the "view" of the map with the player movement, unless the map is at its end
      xBias = xPos - xDefaultDistanceBetweenPosAndBias;
      yBias = yPos - yDefaultDistanceBetweenPosAndBias;

      if(xBias < 0){
        xBias = 0;
      }
      if(xBias + MATRIX_SIZE > MAP_SIZE){
        xBias = MAP_SIZE - MATRIX_SIZE;
      }

      if(yBias < 0){
        yBias = 0;
      }
      if(yBias + MATRIX_SIZE > MAP_SIZE){
        yBias = MAP_SIZE - MATRIX_SIZE;
      }

      matrixChanged = true;
      playerBlinkingState = true;

    }
    // if the next space is not empty, do not move
    else{
      xPos = xLastPos;
      yPos = yLastPos;
    }
  }
}

// checks if the coordonates are within the map
bool inMatrix(int i,int j){
  if(i < 0 || i > MAP_SIZE - 1 || j < 0 || j > MAP_SIZE - 1){
    return false;
  }
  return true;
}

// allocates memory for a bomb and makes the bomb visible
void putBomb(){
  nrOfBombs++;
  bombs = (bomb*) realloc(bombs, nrOfBombs * sizeof(bomb));

  bombs[nrOfBombs - 1].x = xPos;
  bombs[nrOfBombs - 1].y = yPos;
  bombs[nrOfBombs - 1].state = DROPPED_BOMB;
  bombs[nrOfBombs - 1].bombStart = millis();
  matrix[xPos][yPos] = BOMB;
  matrixChanged = true;

}

//changes the state of the bomb blink
void bombsBlink(){
  bombsBlinkingState = !bombsBlinkingState;
  matrixChanged = true;
}

//changes the state of the player blink
void playerBlink(){
  playerBlinkingState = !playerBlinkingState;
  matrixChanged = true;
}

void enemyBlink(){
  enemyBlinkingState = !enemyBlinkingState;
  matrixChanged = true;
}

//shows animation
void showFrameAnimation(bool frame[MATRIX_SIZE][MATRIX_SIZE]){
  for (int row = 0; row < MATRIX_SIZE; row++) {
    for (int col = 0; col < MATRIX_SIZE; col++) {
      lc.setLed(0, row, col, frame[row][col]);
    }
  }

}

//checks if there are walls left
bool checkIfWon(){
  for(int i = 0; i < MAP_SIZE; i++){
    for(int j = 0; j < MAP_SIZE; j++){
      if(matrix[i][j] == WALL){
        return false;
      }
    }
  }
  return true;
    
}

void showMatrixMenu(){
    for (int row = 0; row < MATRIX_SIZE; row++) {
    for (int col = 0; col < MATRIX_SIZE; col++) {
      lc.setLed(0, row, col, false);
    }
  }

}

//arithmethic module operation
int module(int number){
  if(number < 0){
    return number * (-1);
  }
  return number;
}

int countWalls(){
  int walls = 0;
  for (int row = 0; row < MAP_SIZE; row++) {
    for (int col = 0; col < MAP_SIZE; col++) {
      if(matrix[row][col] == WALL){
        walls++;
      }
    }
  }
  return walls;
}

int updateScore(){
  int newWalls = countWalls();
  int bombedWalls = nrOfWalls - newWalls;
  nrOfWalls = newWalls;
  long bombingSpeed = millis() - bombWallsTimer;
  bombWallsTimer = millis();
  int addToScore;
  if(5000 - bombingSpeed < 100){
    addToScore = bombedWalls * lifes * level;
  }
  else{
    addToScore = bombedWalls * lifes * level * (5000 - bombingSpeed)/100;
  }
  score += addToScore;
}