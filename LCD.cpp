#include "LCD.h"
#include "LEDmatrix.h"
#include <LiquidCrystal.h>

extern const byte rs;
extern const byte en;
extern const byte d4;
extern const byte d5;
extern const byte d6;
extern const byte d7;
extern LiquidCrystal lcd;

extern LedControl lc;

extern char* welcomeText;
extern char* endGameText;
extern byte LCDState; // WELCOME, END_GAME, MENU, PLAYING
extern int currentTextPos;
extern unsigned long lastChangeText;

extern byte wasLCDResetForMessageDisplay;
extern byte currentMenuPosBias;
extern bool LCDChanged;
extern byte menuSelectingDot[8];
extern byte currentMenuPos;
extern bool joyMoved;

unsigned long lastMenuMoved = 0;
extern byte gameState;
extern byte menuLevel;
extern byte mainMenuPick;
extern bool wasAboutTextPrinted;
unsigned long lastScrollChange = 0;
extern byte secondMenuPick;

extern byte matrixBrightness;
unsigned long lastBrightnessChange = 0;

extern byte matrixBrightness;
extern byte lcdBrightness;
extern byte lcdScaledBrighness;
extern byte lifes;
extern byte level;
char currentName[3];
byte currentNameSettingPosition = 0;

bool goBackRead = 0;
unsigned long lastGoBackCheck = 0;
bool goBackState = 0;

void showTextLCD(char* text){
    char space = ' ';
    if(millis() - lastChangeText > TIME_BETWEEN_LCD_LETTERS){
        lastChangeText = millis();
        currentTextPos++;
        if(currentTextPos < strlen(text)){
          lcd.print(text[currentTextPos]);
        }
        else if(currentTextPos >= strlen(text) && currentTextPos <= (strlen(text) + LCD_ROWS)){
          lcd.print(space);
        }
        else{
          stopTextLCD();
          LCDState = MAIN_MENU;
        }
    }
}

void stopTextLCD(){
  wasLCDResetForMessageDisplay = false;
  lcd.clear();
}


void setLCDForMessageDisplay(){
  lcd.clear();
  lcd.setCursor(LCD_ROWS, FIRST_COLUMN);
  lcd.autoscroll();
  wasLCDResetForMessageDisplay = true;
  currentTextPos = -1;
}

void displayMenu(){
  LCDChanged = false;
  lcd.clear();
  lcd.noAutoscroll();        

  if(menuLevel == FIRST_MENU){
    switch (currentMenuPosBias){
      case FIRST_COLUMN:
      lcd.setCursor(FIRST_ROW,FIRST_COLUMN);
        if(currentMenuPos == FIRST_COLUMN){
          lcd.write(MENU_SELECTING_DOT);
        }
        else{
          lcd.print(" ");
        }
        lcd.print("Start Game");

        lcd.setCursor(FIRST_ROW,SECOND_COLUMN);
        if(currentMenuPos == SECOND_COLUMN){
          lcd.write(MENU_SELECTING_DOT);
        }
        else{
          lcd.print(" ");
        }
        lcd.print("Settings");
        break;

      case SECOND_COLUMN:
        lcd.setCursor(FIRST_ROW,FIRST_COLUMN);
        if(currentMenuPos == SECOND_COLUMN){
          lcd.write(MENU_SELECTING_DOT);
        }
        else{
          lcd.print(" ");
        }
        lcd.print("Settings");

        lcd.setCursor(FIRST_ROW,SECOND_COLUMN);
        if(currentMenuPos == THIRD_COLUMN){
          lcd.write(MENU_SELECTING_DOT);
        }
        else{
          lcd.print(" ");
        }
        lcd.print("About");
        break;

      default:
        break;
      }
    }
  else if(menuLevel == SECOND_MENU){
    if(mainMenuPick == SETTINGS){
      switch (currentMenuPosBias){
        case FIRST_COLUMN:
          lcd.setCursor(FIRST_ROW,FIRST_COLUMN);
          if(currentMenuPos == FIRST_COLUMN){
            lcd.write(MENU_SELECTING_DOT);
          }
          else{
            lcd.print(" ");
          }
          lcd.write("Name");
          lcd.setCursor(FIRST_ROW,SECOND_COLUMN);
          if(currentMenuPos == SECOND_COLUMN){
            lcd.write(MENU_SELECTING_DOT);
          }
          else{
            lcd.print(" ");
          }
          lcd.print("LCD brightness");
          break;

        case SECOND_COLUMN:
          lcd.setCursor(FIRST_ROW,FIRST_COLUMN);
          if(currentMenuPos == SECOND_COLUMN){
            lcd.write(MENU_SELECTING_DOT);
          }
          else{
            lcd.print(" ");
          }
          lcd.print("LCD brightness");

          lcd.setCursor(FIRST_ROW,SECOND_COLUMN);
          if(currentMenuPos == THIRD_COLUMN){
            lcd.write(MENU_SELECTING_DOT);
          }
          else{
            lcd.print(" ");
          }
          lcd.print("Game brightness");
          break;
      }
    }
  }
  else if(menuLevel == THIRD_MENU){
    if(secondMenuPick == CHANGE_NAME){
      lcd.setCursor(FIRST_ROW,FIRST_COLUMN);
      for(int i = 0; i < LENGTH_OF_NAME; i++){
        lcd.print(" ");
        lcd.print(currentName[i]);
      }
      lcd.setCursor(FIRST_ROW,SECOND_COLUMN);
      for(int i = 0; i < LENGTH_OF_NAME; i++){
        lcd.print(" ");
        if(currentNameSettingPosition == i){
          lcd.write(MENU_SELECTING_DOT);
          break;
        }
        else{
          lcd.print(" ");
        }
      }
    }


    else if(secondMenuPick == LCD_BRIGHTNESS){
      lcd.setCursor(FIRST_ROW,FIRST_COLUMN);
      lcd.print("LCD brightness");
      lcd.setCursor(FIRST_ROW,SECOND_COLUMN);
      for(int i = 0; i < lcdScaledBrighness; i++){
        lcd.write(SELECT_BRIGHTNESS);
      }
    }

    else if(secondMenuPick == MATRIX_BRIGHTNESS){
      lcd.setCursor(FIRST_ROW,FIRST_COLUMN);
      lcd.print("Game brightness");
      lcd.setCursor(FIRST_ROW,SECOND_COLUMN);
      for(int i = 0; i < matrixBrightness; i++){
        lcd.write(SELECT_BRIGHTNESS);
      }
    }
  }
}


void changeMenu() {
  int xValue = analogRead(Y_PIN);
  int yValue = analogRead(X_PIN);
  
   //go back to previous menu
  goBackRead = !digitalRead(GO_BACK_PIN);
  if(millis() - lastGoBackCheck > PRESS_DEBOUNCE_TIME){
    lastGoBackCheck = millis();

      if(goBackRead != goBackState){
        goBackState = goBackRead;

        if(goBackState == HIGH){
          if(menuLevel == THIRD_MENU && secondMenuPick == CHANGE_NAME){
            saveName();
          }
          if(menuLevel > FIRST_MENU){
            menuLevel--;
            LCDChanged = true;
          }
        }
      }
  }


  //navigate the menu
  if (xValue > MAX_THRESHOLD || xValue < MIN_THRESHOLD) {
    if(millis() - lastMenuMoved < MENU_MOVE_INTERVAL){
      return;
    }
    else{
      lastMenuMoved = millis();
    }
  }
  byte menuCols;
  if(menuLevel == FIRST_MENU){
    menuCols =  MAIN_MENU_COLS;
  }
  else if(menuLevel == SECOND_MENU){
    menuCols = SETTINGS_MENU_COLS;
  }
  if (xValue > MAX_THRESHOLD) {
    if (currentMenuPos < currentMenuPosBias + LCD_COLS - 1) {
      currentMenuPos++;
      LCDChanged = true;

    } 
    else if (currentMenuPosBias + LCD_COLS < menuCols) {
      currentMenuPosBias++;
      currentMenuPos++;
      LCDChanged = true;

    }
  }

  if (xValue < MIN_THRESHOLD) {
    if (currentMenuPos > currentMenuPosBias) {
      currentMenuPos--;
      LCDChanged = true;

    } 
    else if (currentMenuPosBias > FIRST_COLUMN) {
      currentMenuPosBias--;
      currentMenuPos = currentMenuPosBias;
      LCDChanged = true;
    }
  }
  
}

void resetLCDForGame(){
  lcd.clear();
  lcd.setCursor(FIRST_ROW,FIRST_COLUMN);

}

void displayAboutText(){
  if(!wasAboutTextPrinted){

    lcd.setCursor(FIRST_ROW,FIRST_COLUMN);
    lcd.print("Nuke 'em All by Ilinca. Available at :");

    lcd.setCursor(FIRST_ROW,SECOND_COLUMN);
    lcd.print("github.com/ilinca-ana-moraru");

    wasAboutTextPrinted = true;
  }
  else{
    int yValue = analogRead(X_PIN);

    if(yValue > MAX_THRESHOLD){
      if(millis() - lastScrollChange > TIME_BETWEEN_SCROLL){
        lcd.scrollDisplayLeft();
        lastScrollChange = millis();
      }
    }

    if(yValue < MIN_THRESHOLD){
      if(millis() - lastScrollChange > TIME_BETWEEN_SCROLL){
        lcd.scrollDisplayRight();
        lastScrollChange = millis();
      }
    }

  }
}

void checkForBrightnessChange(){
  int yValue = analogRead(X_PIN);
  if(secondMenuPick == MATRIX_BRIGHTNESS){
    if(yValue < MIN_THRESHOLD){
      if(millis() - lastBrightnessChange > TIME_BETWEEN_BRIGHTNESS_CHANGE){
        if(matrixBrightness > MIN_SCALED_BRIGHTNESS){
          lastBrightnessChange = millis();
          matrixBrightness--;
          EEPROM[MATRIX_BRIGHTNESS_STORRING_SPACE] = matrixBrightness; 
          LCDChanged = true;
          lc.setIntensity(0, matrixBrightness); 
        }
      }
    }

    if(yValue > MAX_THRESHOLD){
      if(millis() - lastBrightnessChange > TIME_BETWEEN_BRIGHTNESS_CHANGE){
        if(matrixBrightness < MAX_SCALED_BRIGHTNESS){
          lastBrightnessChange = millis();
          matrixBrightness++;
          EEPROM[MATRIX_BRIGHTNESS_STORRING_SPACE] = matrixBrightness; 
          LCDChanged = true;
          lc.setIntensity(0, matrixBrightness); 
        }
      }
    }

  }

  else if(secondMenuPick == LCD_BRIGHTNESS){
    if(yValue < MIN_THRESHOLD){
      if(millis() - lastBrightnessChange > TIME_BETWEEN_BRIGHTNESS_CHANGE){
        if(lcdScaledBrighness > MIN_SCALED_BRIGHTNESS){
          lastBrightnessChange = millis();
          lcdScaledBrighness--;
          LCDChanged = true;
          lcdBrightness = map(lcdScaledBrighness, MIN_SCALED_BRIGHTNESS, MAX_SCALED_BRIGHTNESS, MIN_BRIGHTNESS, MAX_BRIGHTNESS);
          EEPROM[LCD_BRIGHTNESS_STORRING_SPACE] = lcdBrightness;
          analogWrite(LCD_BRIGHTNESS_PIN, lcdBrightness);
        }
      }
    }

    if(yValue > MAX_THRESHOLD){
      if(millis() - lastBrightnessChange > TIME_BETWEEN_BRIGHTNESS_CHANGE){
        if(lcdScaledBrighness < MAX_SCALED_BRIGHTNESS){
          lastBrightnessChange = millis();
          lcdScaledBrighness++;
          LCDChanged = true;
          lcdBrightness = map(lcdScaledBrighness, MIN_SCALED_BRIGHTNESS, MAX_SCALED_BRIGHTNESS, MIN_BRIGHTNESS, MAX_BRIGHTNESS);
          EEPROM[LCD_BRIGHTNESS_STORRING_SPACE] = lcdBrightness;
          analogWrite(LCD_BRIGHTNESS_PIN, lcdBrightness);
        }
      }
    }

  }

}

void displayInGameLCD(){
  resetLCDForGame();

  lcd.print("Level ");
  lcd.print(level);
  lcd.print(" ");

  lcd.setCursor(LIFES_ROW, FIRST_COLUMN);

  for(int i = MIN_LIFES; i <= lifes; i++){
      lcd.write(HEART);
  }
  LCDChanged = true;
}

void changeName(){
  int yValue = analogRead(X_PIN);

  if(yValue > MAX_THRESHOLD){
    if(millis() - lastScrollChange > TIME_BETWEEN_SELECTING_LETTERS){
      if(currentNameSettingPosition + 1 < LENGTH_OF_NAME){
        currentNameSettingPosition++;
        lastScrollChange = millis();
        LCDChanged = true;      
        }
    }
  }

  if(yValue < MIN_THRESHOLD){
    if(millis() - lastScrollChange > TIME_BETWEEN_SELECTING_LETTERS){
      if(currentNameSettingPosition > 0){
        currentNameSettingPosition--;
        lastScrollChange = millis();
        LCDChanged = true;
      }
    }
  }

  int xValue = analogRead(Y_PIN);
    if(xValue > MAX_THRESHOLD){
      if(millis() - lastScrollChange > TIME_BETWEEN_SELECTING_LETTERS){
        if(currentName[currentNameSettingPosition] < 'Z'){
          currentName[currentNameSettingPosition]++;
          }
        else{
          currentName[currentNameSettingPosition] = 'A';
        }
        lastScrollChange = millis();
        LCDChanged = true;
      }
    }
  

  if(xValue < MIN_THRESHOLD){
    if(millis() - lastScrollChange > TIME_BETWEEN_SELECTING_LETTERS){
      if(currentName[currentNameSettingPosition] > 'A'){
        currentName[currentNameSettingPosition]--;
      }
      else{
        currentName[currentNameSettingPosition] = 'Z';
      }
      lastScrollChange = millis();
      LCDChanged = true;
    }
  }
}

void saveName(){
  for(int i = 0; i < LENGTH_OF_NAME; i++){
    EEPROM[NAME_STORRING_SPACE+i] = currentName[i];
  }
}

void displayWinGameInfo(){
  resetLCDForGame();
  if(LCDState == WINNING_INFO1){
    lcd.print("Congratulations,");
    lcd.setCursor(FIRST_ROW, SECOND_COLUMN);
    char ch;
    for(int i = 0; i < LENGTH_OF_NAME; i++){
      ch = EEPROM[NAME_STORRING_SPACE+i];
      lcd.print(ch);
    }
    lcd.print(" !");
  }
  if(LCDState == WINNING_INFO2){
    lcd.print("statistics");
  }
  LCDChanged = false;
}