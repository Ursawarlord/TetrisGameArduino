/* 
 * game.h has runGame() function used by main_menu.h 
 * and it contains the game logic 
 * in a few variables and functions
 */

#include "utils.h"
#include "song.h"

struct point {
    int x;
    int y;
};

// Blocks used to validate rows and complete the game
char blocks[][4][8] = {
    {{0, 0, 0, 1, 0, 0, 0, 0},
     {0, 0, 0, 2, 1, 0, 0, 0},
     {0, 0, 0, 1, 0, 0, 0, 0}},
    {{0, 0, 0, 0, 1, 0, 0, 0},
     {0, 0, 0, 0, 2, 0, 0, 0},
     {0, 0, 0, 0, 1, 0, 0, 0},
     {0, 0, 0, 0, 1, 0, 0, 0}},
    {
        {0, 0, 0, 0, 1, 0, 0, 0},
        {0, 0, 1, 2, 1, 0, 0, 0},
    },
    {
        {0, 0, 0, 1, 1, 0, 0, 0},
        {0, 0, 0, 1, 1, 0, 0, 0},
    },
    {
        {0, 0, 0, 0, 1, 1, 0, 0},
        {0, 0, 0, 1, 2, 0, 0, 0},
    },
    {
        {0, 0, 0, 1, 0, 0, 0, 0},
        {0, 0, 1, 2, 1, 0, 0, 0},
    },
    {
        {0, 0, 0, 1, 1, 0, 0, 0},
        {0, 0, 0, 0, 2, 1, 0, 0},
    },
};
 
/*
 *  Keeps track of the moving block
 *  First 4 rows are not visible on LED matrix
 */
char moving[12][8];

// Keeps track of stationary block elements
char stationary[8][8];

// Game variables
int score;
int level;
int countUpdates;
unsigned long lastUpdate;
int updateInterval;
const unsigned int inputDelay = 120;  // minimum delay for input (possibly more w/ debounce)
unsigned long lastInputTimer;

/*
 * Function to change (x,y) for the moving block
 */
void transformMoving(int x, int y) {
    char updatedArr[12][8];
    int xValid = 1;
    for (int i = 0; i < 12; i++) {
        for (int j = 0; j < 8; j++) {
            updatedArr[i][j] = 0;
        }
    }

    for (int i = 0; i < 12 && xValid; i++) {
        for (int j = 0; j < 8 && xValid; j++) {
            if (moving[i][j]) {
                if (j + x >= 8 || j + x < 0) {
                    xValid = 0;
                }
            }
        }
    }

    for (int i = 0; i < 12; i++) {
        for (int j = 0; j < 8; j++) {
            if (moving[i][j]) {
                if (i + y < 12 && i + y >= 0) {
                    if (xValid) {
                        if (i >= 4 && stationary[i - 4 + y][j + x]) {
                            return;
                        }
                        updatedArr[i + y][j + x] = moving[i][j];
                    } else {
                        updatedArr[i + y][j] = moving[i][j];
                    }
                }
            }
        }
    }
    for (int i = 0; i < 12; i++) {
        for (int j = 0; j < 8; j++) {
            moving[i][j] = updatedArr[i][j];
        }
    }
}

// Game is over if first row has stationary block elements
int isGameOver() {
    for (int j = 0; j < 8; j++) {
        if (stationary[0][j]) {
            return 1;
        }
    }
    return 0;
}

// Rotation by 90 for moving block when pressing button
void rotate90() {
    struct point pivot = {
        -1,
        -1
    };
    char updatedArr[12][8];
    for (int i = 0; i < 12; i++) {
        for (int j = 0; j < 8; j++) {
            updatedArr[i][j] = 0;
            if (moving[i][j] == 2) {
                pivot.y = i;
                pivot.x = j;
            }
        }
    }
    if (pivot.y == -1 || pivot.x == -1) {
        return;
    }
    for (int i = 0; i < 12; i++) {
        for (int j = 0; j < 8; j++) {
            if (moving[i][j] == 1) {
                struct point toCoordinate = {
                    j - pivot.x,
                    i - pivot.y
                };
                struct point transformedCoordinate = {
                    -toCoordinate.y,
                    toCoordinate.x
                };
                struct point transformedGrid = {
                    transformedCoordinate.x + pivot.x,
                    transformedCoordinate.y + pivot.y
                };

                if (transformedGrid.x >= 8 || transformedGrid.x < 0 || transformedGrid.y >= 12 || transformedGrid.y < 0 || (transformedGrid.y >= 4 && stationary[transformedGrid.y - 4][transformedGrid.x])) {
                    return;
                }

                updatedArr[transformedGrid.y][transformedGrid.x] = 1;
            } else if (moving[i][j] == 2) {
                updatedArr[i][j] = 2;
            }
        }
    }
    for (int i = 0; i < 12; i++) {
        for (int j = 0; j < 8; j++) {
            moving[i][j] = updatedArr[i][j];
        }
    }
}

// Using input to control the moving block and to pause the game
void handleInput() {

    int input = none;
    int X_Axis = analogRead(joystickPinX); // read the x axis value
    int Y_Axis = analogRead(joystickPinY); // read the y axis value
    int buttonValue = registerReadPin(buttonPin); // read the state of the button
    buttonValue = map(buttonValue, 0, 1, 1, 0); // invert the input from the button to be high when pressed
    if (reverseJoystick) {
        X_Axis = map(X_Axis, 0, 1024, 1024, 0);
    }
    int pauseButtonValue = registerReadPin(pauseButtonPin);
    pauseButtonValue = map(pauseButtonValue, 0, 1, 1, 0); // invert the input from the button to be high when pressed
    if (pauseButtonValue == 1) {
        lastPress = millis() / divideTime;
        input = pause;
    } else if (X_Axis >= maxThreshold) {
        input = right;
    } else if (X_Axis <= minThreshold) {
        input = left;
    } else if (Y_Axis >= maxThreshold) {
        input = up;
    } else if (Y_Axis <= minThreshold) {
        input = down;
    }
    // not inside switch because I want more interactions
    if (buttonValue == 1 && (millis() / divideTime - lastPress) > debounceDelay / divideTime) {
        lastPress = millis() / divideTime;
        rotate90();
    }
    switch (input) {
    case left:
        transformMoving(-1, 0);
        break;
    case right:
        transformMoving(1, 0);
        break;
    case up:
    case down:
        lastUpdate -= updateInterval;
        score++;
        break;
    case pause:
        if (!isGameOver()) {
            gamePaused = true;
            screenStatus = STATUS_MAINMENU;
            pointerMode = POINTER_SCROLL;
            currentMainMenuIndex = 0;
            lc.shutdown(0, true);
            lcd.clear();
            printLine(0, "===GAME PAUSED===");
            lcd.setCursor(0, 1);
            lcd.write("Please resume");
            for (int i = 0; i < 3; i++) {
                lcd.write("!");
                delay(400);
            }
            debounceDelay = 800;
        }
        break;
    }
}

// Checking if moving block has landed
int isMovingAtBottom() {
    for (int i = 0; i < 12; i++) {
        for (int j = 0; j < 8; j++) {
            if (moving[i][j] && i == 11 || (moving[i][j] && i >= 4 && i + 1 < 12 && stationary[i - 4 + 1][j])) {
                return 1;
            }
        }
    }
    return 0;
}

// Landing the moving block and become stationary
void handleAtBottom() {
    for (int i = 0; i < 8; i++) {
        for (int j = 0; j < 8; j++) {
            stationary[i][j] = stationary[i][j] || moving[i + 4][j];
            moving[i + 4][j] = 0;
        }
    }
}

// Render LED matrix
void render() {
    for (int i = 0; i < 8; i++) {
        for (int j = 0; j < 8; j++) {
            lc.setLed(0, i, j, moving[i + 4][j] | stationary[i][j]);
        }
    }
}

// Render LCD display
void renderLcd() {
    lcd.clear();
    lcd.setCursor(1, 0);
    lcd.print("Level: ");
    lcd.print(level);
    lcd.setCursor(1, 1);
    lcd.print("Score: ");
    lcd.print(score);
}

// Upgrade score and clear row if we finish the bottom row
void updateRows() {
    int rowsUpdated = 0;
    for (int i = 0; i < 8; i++) {
        int validRow = 1;
        for (int j = 0; j < 8; j++) {
            if (!stationary[i][j]) {
                validRow = 0;
            }
        }
        if (validRow) {
            tone(BUZZER, 1000);
            delay(50);
            noTone(BUZZER);
            rowsUpdated++;
            for (int j = 0; j < 8; j++) {
                stationary[i][j] = 0;
            }
            char updated[8][8];
            for (int i = 0; i < 8; i++)
                for (int j = 0; j < 8; j++) {
                    updated[i][j] = 0;
                }
            for (int k = i; k >= 0; k--) {
                for (int l = 0; l < 8; l++) {
                    if (k - 1 >= 0) {
                        updated[k][l] = stationary[k - 1][l];
                    }
                }
            }
            for (int k = i + 1; k < 8; k++) {
                for (int l = 0; l < 8; l++) {
                    updated[k][l] = stationary[k][l];
                }
            }
            for (int k = 0; k < 8; k++) {
                for (int l = 0; l < 8; l++) {
                    stationary[k][l] = updated[k][l];
                }
            }
        }
    }
    switch (rowsUpdated) {
    case 1:
        score += 40 * (level + 1 + difficultyValue - EASY);
        break;
    case 2:
        score += 100 * (level + 1 + difficultyValue - EASY);
        break;
    case 3:
        score += 300 * (level + 1 + difficultyValue - EASY);
        break;
    case 4:
        score += 1200 * (level + 1 + difficultyValue - EASY);
        break;
    }
}

// Generate new moving block
void queueNewBlock() {
    int randomBlock = random(BLOCK_COUNT - 1);
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 8; j++) {
            moving[i][j] = blocks[randomBlock][i][j];
        }
    }
}


// Called per updateInterval seconds
void updateState() {
    if (isMovingAtBottom()) {
        handleAtBottom();
        queueNewBlock();
    }
    transformMoving(0, 1);
    updateRows();
}

// Called when game is over and when booting first time
void restart() {
    memset(moving, 0, sizeof(moving) * sizeof(char));
    memset(stationary, 0, sizeof(stationary) * sizeof(char));
    lastUpdate = 0;
    lastInputTimer = 0;
    score = 0;
    level = 0;
    countUpdates = 0;
}

// Setup for game
void bootGame() {
    restart();
    lc.shutdown(0, false);
    pinMode(UNUSED_ANALOG, INPUT);
    randomSeed(analogRead(UNUSED_ANALOG));
    queueNewBlock();
    lcd.begin(16, 2);
    if (difficultyValue == EASY) {
        for (int i = 0; i < 7; i++)
            stationary[7][i] = 1;
        updateInterval = 1000 - level * 4;
    } else if (difficultyValue == MEDIUM) {
        updateInterval = 700 - level * 8;
    } else if (difficultyValue == HARD) {
        updateInterval = 300 - level * 15;
    }
}

// Print name & score position in the top
void congrulations(const int & score) {
    int place = saveHighScore(currentUsername, score);
    if (place == -1) {
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print(currentUsername);
        lcd.print(', your score');
        lcd.setCursor(0, 1);
        lcd.print("is not in top :(");
    } else {
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print(currentUsername);
        lcd.setCursor(0, 1);
        lcd.print("you are ");
        if (place == 1) {
            lcd.print("1st     ");
        } else if (place == 2) {
            lcd.print("2nd     ");
        } else if (place == 3) {
            lcd.print("3rd    ");
        }
    }
}

// Used by Main Menu for controlling the game state
int runGame() {
    if (isGameOver()) {
        lcd.clear();
        lcd.setCursor(2, 0);
        lcd.print("Game Over!");
        lcd.setCursor(0, 1);
        lcd.print("Score: ");
        lcd.print(score);
        for (int t = 0; t < 3; t++) {
            for (int i = 0; i < 8; i++) {
                for (int j = 0; j < 8; j++) {
                    lc.setLed(0, i, j, true);
                }
            }
            delay(500);
            render();
            delay(500);
        }

        delay(3000);
        congrulations(score);
        lc.shutdown(0, true);
        delay(3000);
        return 1;
    } else {
        if (millis() - lastInputTimer > inputDelay) {
            handleInput();
            lastInputTimer = millis();
        }
        if (millis() - lastUpdate > updateInterval) {
            tone(BUZZER, 100);
            delay(2);
            noTone(BUZZER);
            updateState();
            renderLcd();
            lastUpdate = millis();
            countUpdates++;
            if (countUpdates == 15) {
                countUpdates = 0;
                level++;
            }
        }
        render();
        return 0;
    }
}