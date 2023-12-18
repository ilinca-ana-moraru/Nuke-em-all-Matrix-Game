#include "LCD.h"
#include "LEDmatrix.h"

#include "LedControl.h"
#include <LiquidCrystal.h>
#include <EEPROM.h>

const byte rs = 9;
const byte en = 8;
const byte d4 = 7;
const byte d5 = 6;
const byte d6 = 5;
const byte d7 = 4;

LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

char* welcomeText = {"Welcome!!!"};
char* endGameText = {"END GAME"};
byte LCDState; //WELCOME, END_GAME, PLAYING
int currentTextPos = -1;
unsigned long lastChangeText = 0;

LedControl lc = LedControl(DIN_PIN, CLOCK_PIN, LOAD_PIN, 1); // DIN, CLK, LOAD, No. DRIVER

unsigned long long lastMoved = 0; 

short xBias = MATRIX_SIZE/2;
short yBias = MATRIX_SIZE/2;
byte xDefaultDistanceBetweenPosAndBias = MATRIX_SIZE/2;
byte yDefaultDistanceBetweenPosAndBias = MATRIX_SIZE/2;
byte xPos = xDefaultDistanceBetweenPosAndBias + xBias;
byte yPos = yDefaultDistanceBetweenPosAndBias + yBias;
byte xLastPos = 0;
byte yLastPos = 0;

bool matrixChanged = true; 
// 2D array representing what is on the map currently (wall/bomb)
byte matrix[MAP_SIZE][MAP_SIZE] = {
  {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
  {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
  {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
  {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
  {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
  {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
  {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
  {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
  {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
  {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
  {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
  {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
  {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
  {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
  {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
  {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}}
;

bool playerBlinkingState = true;
unsigned long playerLastBlink = 0;
unsigned long enemyLastBlink = 0;
extern bool enemyBlinkingState;

bomb* bombs;

byte nrOfBombs = 0;
bool pressState = false;
bool pressRead = false;
unsigned long lastPressCheck = 0;

bool bombsBlinkingState = true;
unsigned long bombsLastBlink = 0;
unsigned long randomNumber;

                            // up     down    left    right
direction directions[NR_OF_DIRECTIONS] = {{0, -1}, {0, 1}, {-1, 0}, {1, 0}};
direction extendedBombCoordonates[NR_OF_EXTENDED_BOMB_COORDONATES] = {{-2, 0}, {-1, -1},{-1, 0}, {-1, 1},
{0, -2}, {0, -1}, {0, 1}, {0, 2}, {1, -1}, {1, 0}, {1, 1}, {2, 0}, {0, 0}};

bool skullAnimation[MATRIX_SIZE][MATRIX_SIZE] = {
  {0, 1, 1, 1, 1, 1, 1, 0},
  {1, 1, 1, 1, 1, 1, 1, 1},
  {1, 0, 0, 1, 1, 0, 0, 1},
  {1, 0, 0, 1, 1, 0, 0, 1},
  {0, 1, 1, 0, 0, 1, 1, 0},
  {0, 1, 1, 1, 1, 1, 1, 0},
  {0, 1, 0, 0, 0, 0, 1, 0},
  {0, 0, 1, 1, 1, 1, 0, 0}  
};
unsigned long showDeathStart = 0;
unsigned long showFinishLevelStart = 0;
unsigned int timeBetweenDeathFrames = 0;

bool finishGameAnimation[MATRIX_SIZE][MATRIX_SIZE] = {
  {0, 0, 0, 0, 0, 0, 0, 0},
  {0, 1, 1, 0, 0, 1, 1, 0},
  {0, 1, 1, 0, 0, 1, 1, 0},
  {0, 0, 0, 0, 0, 0, 0, 0},
  {1, 0, 0, 0, 0, 0, 0, 1},
  {0, 1, 0, 0, 0, 0, 1, 0},
  {0, 0, 1, 1, 1, 1, 0, 0},
  {0, 0, 0, 0, 0, 0, 0, 0}  
};

unsigned long AnimationStart = 0;
unsigned long timeBetweenStartFrames = 0;
bool currentStartAnimationFrame;

bool startAnimation[NR_OF_START_ANIMATION_FRAMES][MATRIX_SIZE][MATRIX_SIZE]{
  {
    {0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 1, 1, 1, 1, 0, 0},
    {0, 1, 0, 0, 0, 0, 1, 0},
    {0, 1, 0, 1, 1, 0, 1, 0},
    {0, 1, 0, 1, 1, 0, 1, 0},
    {0, 1, 0, 0, 0, 0, 1, 0},
    {0, 0, 1, 1, 1, 1, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0}

  },
  {
    {0, 1, 1, 1, 1, 1, 1, 0},
    {1, 0, 0, 0, 0, 0, 0, 1},
    {1, 0, 0, 1, 1, 0, 0, 1},
    {1, 0, 1, 0, 0, 1, 0, 1},
    {1, 0, 1, 0, 0, 1, 0, 1},
    {1, 0, 0, 1, 1, 0, 0, 1},
    {1, 0, 0, 0, 0, 0, 0, 1},
    {0, 1, 1, 1, 1, 1, 1, 0}

  }

};

bool winAnimation[MATRIX_SIZE][MATRIX_SIZE] = {
  {0, 0, 0, 0, 0, 0, 0, 0},
  {0, 1, 1, 1, 1, 1, 1, 0},
  {1, 0, 1, 1, 1, 1, 0, 1},
  {0, 1, 1, 1, 1, 1, 1, 0},
  {0, 0, 1, 1, 1, 1, 0, 0},
  {0, 0, 0, 1, 1, 0, 0, 0},
  {0, 0, 0, 1, 1, 0, 0, 0},
  {0, 0, 1, 1, 1, 1, 0, 0}
};
unsigned long winAnimationStart = 0;
byte gameState;

byte wasLCDResetForMessageDisplay = false;

byte currentMenuPosBias = FIRST_COLUMN;
byte currentMenuPos = FIRST_COLUMN;
bool LCDChanged = true;

byte menuSelectingDot[HEIGHT_OF_CUSTOM_CHARS] = {
  0b00000,
  0b00100,
  0b01110,
  0b11111,
  0b01110,
  0b00100,
  0b00000,
  0b00000
};
byte selectBrightness[HEIGHT_OF_CUSTOM_CHARS] = {
  0b11111,
  0b11111,
  0b11111,
  0b11111,
  0b11111,
  0b11111,
  0b11111,
  0b11111
};

byte heart[HEIGHT_OF_CUSTOM_CHARS] = {
  0b00000,
  0b01010,
  0b11111,
  0b11111,
  0b11111,
  0b01110,
  0b00100,
  0b00000
};



bool joyMoved = false;
byte menuLevel = FIRST_MENU;
byte mainMenuPick = FIRST_COLUMN;
bool wasAboutTextPrinted = false;
byte secondMenuPick;
byte matrixBrightness = EEPROM[MATRIX_BRIGHTNESS_STORRING_SPACE];
byte lcdBrightness = EEPROM[LCD_BRIGHTNESS_STORRING_SPACE];
byte lcdScaledBrighness = map(lcdBrightness,0,255,0,16) + 1;

byte lifes = MAX_LIFES;
byte level = 1;
extern byte currentNameSettingPosition;
extern char currentName[3];
unsigned long enemyLastMove = 0;
unsigned long playerDamage = 0;
bool playerIsDamagedByEnemy = 0;
extern byte nrOfEnemys;
extern enemy enemys[4];
extern int enemyMoveInterval;
extern int damageInterval;

long bombWallsTimer = 0;
int score = 0;
extern int totalScore;
int nrOfWalls;
int wasCheckedForHighScore = false;

void setup() {
  lcd.begin(LCD_ROWS, LCD_COLS);
  lcd.setCursor(LCD_ROWS, 0);
  lcd.autoscroll();
  LCDState = WELCOME;
  pinMode(GO_BACK_PIN, INPUT_PULLUP);
  pinMode(PRESS_PIN, INPUT_PULLUP);
  pinMode(X_PIN, INPUT);
  pinMode(Y_PIN, INPUT);
  pinMode(DIN_PIN, OUTPUT);
  pinMode(CLOCK_PIN, OUTPUT);
  pinMode(LOAD_PIN, OUTPUT);
  pinMode(LCD_BRIGHTNESS_PIN, OUTPUT);
  analogWrite(LCD_BRIGHTNESS_PIN, lcdBrightness);
  lc.shutdown(0, false); 
  lc.setIntensity(0, matrixBrightness); 
  lc.clearDisplay(0);
  lcd.createChar(MENU_SELECTING_DOT, menuSelectingDot);
  lcd.createChar(SELECT_BRIGHTNESS, selectBrightness);
  lcd.createChar(HEART, heart);
  randomSeed(analogRead(PRESS_PIN));
  startGameAnimation();
  for(int i = 0; i < LENGTH_OF_NAME; i++){
    currentName[i] = EEPROM[NAME_STORRING_SPACE+i];
  }
}

void loop() {
  if(LCDState == WELCOME){
    if(wasLCDResetForMessageDisplay == false){
      setLCDForMessageDisplay();
    }
    showTextLCD(welcomeText);
  }

  if(LCDState == END_GAME){
    if(wasLCDResetForMessageDisplay == false){
      setLCDForMessageDisplay();
    }
    showTextLCD(endGameText);
  }

  if(LCDState == WINNING_INFO1 || LCDState == WINNING_INFO2){
    if(wasCheckedForHighScore == false){
      checkForHighScores();
      wasCheckedForHighScore = true;

    }
    if(LCDChanged == true){
      displayWinGameInfo();
    }
  }

  if(LCDState == MENU){
    if(menuLevel == SECOND_MENU && mainMenuPick == ABOUT){
      displayAboutText();
    }
    else{
      if(menuLevel == THIRD_MENU && (secondMenuPick == MATRIX_BRIGHTNESS || secondMenuPick == LCD_BRIGHTNESS)){
        checkForBrightnessChange();
      }
      if(menuLevel == THIRD_MENU && secondMenuPick == CHANGE_NAME){
        changeName();
      }
      if(LCDChanged == true){
        displayMenu();
      }
    }
    changeMenu();

  }

  if(LCDState == PLAYING){
    if(LCDChanged == true){
      displayInGameLCD();
    }
  }

  // shows smiley face for setting matrix brightness
  if(gameState == MENU){
    if(menuLevel == THIRD_MENU && secondMenuPick == MATRIX_BRIGHTNESS){
      showFrameAnimation(finishGameAnimation);
    }
    else{
      showMatrixMenu();
    }
  }

  //check if the joystick moved
  if(gameState == PLAYING){
    if (millis() - lastMoved > MOVE_INTERVAL) { 
      updatePositions(); 
      lastMoved = millis(); 
    }
  }

  if(gameState == WINNING_ANIMATION){
    if(matrixChanged == false){
      showFrameAnimation(winAnimation);
      matrixChanged = true;
    }
  }

  // change matrix if necessary
  if (matrixChanged == true) {
    updateMatrix(); 
    matrixChanged = false; 
  }

  if(gameState == PLAYING){
    // player blink
    if(millis() - playerLastBlink > PLAYER_BLINK_INTERVAL){
      playerBlink();
      playerLastBlink = millis();
    }

    if(millis() - enemyLastBlink > ENEMY_BLINK_INTERVAL){
      enemyBlink();
      enemyLastBlink = millis();
    }

    if(millis() - enemyLastMove > enemyMoveInterval){
      for(int i = 0; i < nrOfEnemys; i++){
        enemyMove(i);
      }
      enemyLastMove = millis();
    }

    //damage from enemy
    if(matrix[xPos][yPos] == ENEMY){
      if(playerIsDamagedByEnemy == false){
        playerIsDamagedByEnemy = true;
        playerDamage = millis();
      }
      else{
        if(millis() - playerDamage >= damageInterval){
          loseLife();
          playerDamage = millis();
        }
      }
    }
    if(playerIsDamagedByEnemy == true){
      if(matrix[xPos][yPos] != ENEMY){
        playerIsDamagedByEnemy = false;
      }
    }

    // bombs blink
    if(sizeof(bombs)){
      if(millis() - bombsLastBlink > BOMBS_BLINK_INTERVAL){
        bombsBlink();
        bombsLastBlink = millis();
      }
    }
  }

  buttonPressLogic();

////////////logic for when time is up


  for(int i = 0; i < nrOfBombs; i++){
    //if visibility time of bomb is up, it's state becomes delete_bomb so that it is deleted in updateMatrix() 
    if(millis() - bombs[i].bombStart > BOMBS_DISAPPEARING_INTERVAL){
      bombs[i].state = DELETE_BOMB;
      matrixChanged = true;
    }

    //if it is time to expand bomb, it's state becomes expand_bomb so that it is expanded in updateMatrix()
    else if(millis() - bombs[i].bombStart > BOMBS_EXPANDING_INTERVAL){
      bombs[i].state = EXPAND_BOMB;
      matrixChanged = true;
    }
  }


 //if the time for the death animation is up, generate next map
  if(gameState == DEATH_ANIMATION){
    if(millis() - showDeathStart > DEFAULT_ANIMATION_DISPLAY && LCDState != END_GAME){
      // enterMenu();
      LCDState = WINNING_INFO1;
      LCDChanged = true;
    }
  }

  //if the time for the finish level animation is up, start next level
  if(gameState == FINISH_LEVEL_ANIMATION){
    if(millis() - showFinishLevelStart > DEFAULT_ANIMATION_DISPLAY){
      level++;
      if(level <= NR_OF_LEVELS){
        startLevel();
      }
      //last level is finished
      else{
        gameState = WINNING_ANIMATION;
        LCDState = WINNING_INFO1;
        LCDChanged = true;
        matrixChanged = true;
      }
    }
  }

    //if the time for the start animation is up, generate next map
  if(gameState == START_ANIMATION){
    if(millis() - AnimationStart > DEFAULT_START_ANIMATION && LCDState != WELCOME){
      enterMenu();
    }
    else{
      if(millis() - timeBetweenStartFrames > INTERVAL_BETWEEN_FRAMES){
        currentStartAnimationFrame = !currentStartAnimationFrame;
        timeBetweenStartFrames = millis();
        matrixChanged = true;
      }
    }
  }

}


//new level, if lost, function is called
void startLevel(){
  wasCheckedForHighScore = false;
  lifes = MAX_LIFES;
  totalScore += score;
  score = 0;
  bombWallsTimer = millis();
  xBias = MATRIX_SIZE/2;
  yBias = MATRIX_SIZE/2;
  xPos = xDefaultDistanceBetweenPosAndBias + xBias;
  yPos = yDefaultDistanceBetweenPosAndBias + yBias;

  generateMap();
  nrOfWalls = countWalls();

  stopTextLCD();
  gameState = PLAYING;
  LCDState = PLAYING;
  resetLCDForGame();
  matrixChanged = true;
  LCDChanged = true;
}

void startGameAnimation(){
  AnimationStart = millis();
  gameState = START_ANIMATION;
  currentStartAnimationFrame = 0;
}

void enterMenu(){
  stopTextLCD();
  menuLevel = FIRST_MENU;
  gameState = MENU;
  LCDState = MENU;
  LCDChanged = true;
}

void buttonPressLogic(){
  
  pressRead = !digitalRead(PRESS_PIN);
  //button press debounce
  if(millis() - lastPressCheck > PRESS_DEBOUNCE_TIME){
    lastPressCheck = millis();

      if(pressRead != pressState){
        pressState = pressRead;

        if(pressState == HIGH){
          // if user wants death animation to be skipped
          if(gameState == DEATH_ANIMATION){
            LCDState = WINNING_INFO1;
            LCDChanged = true;
            // enterMenu();
          }
          // if user wants finish level animation to be skipped
          else if(gameState == FINISH_LEVEL_ANIMATION){
            level++;
            if(level <= NR_OF_LEVELS){
              startLevel();
            }
            //last level was finished
            else{
              gameState = WINNING_ANIMATION;
              LCDState = WINNING_INFO1;
              LCDChanged = true;
              matrixChanged = true;
            }
          }

          else if(LCDState == WINNING_INFO1){
            LCDState = WINNING_INFO2;
            LCDChanged = true;
          }

          else if(LCDState == WINNING_INFO2){
            LCDState = MENU;
            gameState = MENU;
            LCDChanged = true;
            matrixChanged = true; 
          }
          else if(LCDState == WELCOME){
            enterMenu();
          }

          //if user wants to drop bomb
          else if(gameState == PLAYING){
            putBomb();
          }

          else if(LCDState == MENU){
            selectMenu();
          }

          
        }
      }
    
  }

}


void selectMenu(){
  LCDChanged = true;
  if(menuLevel == FIRST_MENU){
  switch (currentMenuPos){
    case FIRST_COLUMN:
      level = FIRST_LEVEL;
      startLevel();
      break;
      
    case SECOND_COLUMN:
      menuLevel = SECOND_MENU;
      mainMenuPick = HIGHSCORES;
      break;
    
    case THIRD_COLUMN:
      menuLevel = SECOND_MENU;
      currentMenuPos = FIRST_COLUMN;
      currentMenuPosBias = FIRST_COLUMN;
      mainMenuPick = SETTINGS;
      break;

    case FORTH_COLUMN:
      menuLevel = SECOND_MENU;
      mainMenuPick = ABOUT;
      wasAboutTextPrinted = false;
      break;

    default:
      break;
    }
  }
  else if(menuLevel == SECOND_MENU){
    menuLevel = THIRD_MENU;
    if(mainMenuPick == SETTINGS){ 

      switch (currentMenuPos){
        case FIRST_COLUMN:
          secondMenuPick = CHANGE_NAME;
          currentNameSettingPosition = 0;
          break;

        case SECOND_COLUMN:
          secondMenuPick = LCD_BRIGHTNESS;
          break;

        case THIRD_COLUMN:
          secondMenuPick = MATRIX_BRIGHTNESS;
          break;
        
        default:
          break;
      }
    }

    else if(mainMenuPick == HIGHSCORES){
      switch(currentMenuPos){
        case FIRST_COLUMN:
          deleteHighscores();
          secondMenuPick == RESET_HIGHSCORES;
          break;

        case SECOND_COLUMN:
          secondMenuPick = DISPLAY_HIGHSCORES;
          break;
        
        default:
          break;
      }
    }
  }

}