# Requirements

Develop a small game on the 8x8 matrix. The game must have at least 3 types of elements: player (blinks slowly), bombs/bullets (blinks fast), wall (doesn’t blink). The walls
are generated on the map and then the player moves around and destroys them. 

- Minimal components: an LCD, a joystick, a buzzer and the led matrix.<br>
– You must add basic sounds to the game.<br>
– Each level / instance should work on 16x16 matrix. You can apply the concept of visibility / fog of war ( you only see 8x8 of the total 16x16 matrix, and you discover more
 as you move around) or you can use the concept of ”rooms”.<br>
– It must be intuitive and fun to play.<br>
– It must make sense in the current setup.<br>
– The player should have a feeling of progression in difficulty. Depending on the dynamic of the game, this is done in the same level or with multiple levels. You can make them
progress dynamically or have a number of fixed levels with an endgame.<br>

Menu Task: Create a menu for the game. You should scroll on the LCD with the joystick. The menu should include the following functionality:
1. Intro Message - When powering up a game, a greeting message should be shown for a few moments.
2. Should contain roughly the following categories:
(a) Start game, starts the initial level of your game<br><br>
(b) Highscore:<br><br>
– Initially, we have 0. <br>
– Update it when the game is done. Highest possible score should be achieved by starting at a higher level.<br>
– Save the top 3+ values in EEPROM with name and score.<br>
(c) Settings:<br>
– Enter name. The name should be shown in highscore. Maybe somewhere else, as well? You decide.<br>
– LCD brightness control. Save it to EEPROM.<br>
– Matrix brightness control. Make sure to display something on the matrix when selecting it. Save it to EEPROM.
– Sounds on or off. Save it to EEPROM.<br>
(d) About: should include details about the creator(s) of the game. At least game name, author and github link or user
(e) How to play: short and informative description<br>
3. While playing the game: display all relevant info<br><br>
– Lives<br>
– Level<br>
– Score<br>
– Player name<br>
– etc<br>
4. Upon game ending:<br><br>
(a) Screen 1: a message such as ”Congratulations on reaching level/score X”. ”You did better than y people.” etc. Switches to screen 2 upon interaction (button press)
or after a few moments.<br>
(b) Screen 2: display relevant game info: score, time, lives left etc. Must inform player if he/she beat the highscore. This menu should only be closed by the player,
pressing a button.<br>


# Nuke 'em all

I chose to build a mining game because I like the visual of a bomb being exploded and thought it would look nice and  give dimention on a limiting 8X8 LED matrix. The game 
takes place in a mine, and the player's scope is to bomb every wall while running from the explosion, as he can lose lifes otherwise. The game has 3 levels, and at the start of each level the player starts with 3 lifes. If all 3 lifes are lost, the game is ended. If the player has at least 1 life after bombing all walls, the next level starts. <br>
The levels have progressing difficulty, as in level 2 and 3 the player encounters 2 and 4 enemys. When an enemy hits an wall, he takes a new direction. The walls, as well as the enemys starting position and movement are randomly generated using the Arduino random() function. 

In the LCD menu, you can select to start the game,highscores( see top 3 or reset) , settings (set name, matrix or LCD brightness) or about, were you can read minimal information about the game.

# Components used 
- LCD => displays menu and various messages <br>
- joystick => used for playing the game and navigating the menu <br>
- 8 x 8 led  matrix => the game and some animations are displayed here <br>
- pushbutton => for going back in the menu <br>

# Photo
![photo](https://github.com/ilinca-ana-moraru/Nuke-em-all-Matrix-Game/blob/main/led_matrix_game_photo.jpeg)
