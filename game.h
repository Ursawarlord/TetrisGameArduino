#include "utils.h"
// #include "tetris_song.h"

struct point {
  int x;
  int y;
};

 char blocks[][4][8] = {
  {
    {0, 0, 0, 1, 0, 0, 0, 0},
    {0, 0, 0, 2, 1, 0, 0, 0},
    {0, 0, 0, 1, 0, 0, 0, 0}
  },
  {
    {0, 0, 0, 0, 1, 0, 0, 0},
    {0, 0, 0, 0, 2, 0, 0, 0},
    {0, 0, 0, 0, 1, 0, 0, 0},
    {0, 0, 0, 0, 1, 0, 0, 0}
  },
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


 char moving[12][8];
 char stationary[8][8];

 unsigned long lastUpdate;
 const unsigned long updateInterval = 1000;

 unsigned long lastInput;
 const unsigned long inputDelay = 120;

 int score;
 int level;



void transformMoving(int x, int y) {
  y = -y;
  int updatedArr[12][8];
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



void rotate90() {
  struct point pivot = { -1, -1};

  int updatedArr[12][8];

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
        struct point toCoordinate = {j - pivot.x, i - pivot.y};
        struct point transformedCoordinate = { -toCoordinate.y, toCoordinate.x};
        struct point transformedGrid = {transformedCoordinate.x + pivot.x, transformedCoordinate.y + pivot.y};

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

void handleInput() {

  int joystick = readJoystick();

  switch(joystick){
    case left:
    transformMoving(-1, 0);
    break;
    case right:
    transformMoving(1, 0);
    break;
    case up:
    lastUpdate -= updateInterval;
    score++;
    break;
    case down:
    rotate90();
    break;
  }
}



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


void handleAtBottom() {
  for (int i = 0; i < 8; i++) {
    for (int j = 0; j < 8; j++) {
      stationary[i][j] = stationary[i][j] || moving[i + 4][j];
      moving[i + 4][j] = 0;
    }
  }
}


void render() {
  for (int i = 0; i < 8; i++) {
    for (int j = 0; j < 8; j++) {
      lc.setLed(0, i, j, moving[i + 4][j] | stationary[i][j]);
    }
  }
}

void renderLcd() {
  lcd.clear();
  lcd.setCursor(1, 0);
  lcd.print("Level: ");
  lcd.print(level);

  lcd.setCursor(1, 1);
  lcd.print("Score: ");
  lcd.print(score);
}



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

      int updated[8][8];
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
      score += 40 * (level + 1);
      break;
    case 2:
      score += 100 * (level + 1);
      break;
    case 3:
      score += 300 * (level + 1);
      break;
    case 4:
      score += 1200 * (level + 1);
      break;
  }
}


int isGameOver() {
  for (int j = 0; j < 8; j++) {
    if (stationary[0][j]) {
      return 1;
    }
  }

  return 0;
}


void queueNewBlock() {
  int randomBlock = random(BLOCK_COUNT - 1);
  for (int i = 0; i < 4; i++) {
    for (int j = 0; j < 8; j++) {
      moving[i][j] = blocks[randomBlock][i][j];
    }
  }
}


void updateState() {
  if (isMovingAtBottom()) {
    handleAtBottom();
    queueNewBlock();
  }
  transformMoving(0, -1);

  updateRows();
}


void restart() {
  memset(moving, 0, sizeof(moving) * sizeof(char));
  memset(stationary, 0, sizeof(stationary) * sizeof(char));
  lastUpdate = 0;
  lastInput = 0;
  score = 0;
  level = 0;
}


void bootGame() {
  // put your setup code here, to run once:
  restart();
  lc.shutdown(0, false);
  lc.setIntensity(0, 1);

  pinMode(UNUSED_ANALOG, INPUT);


  randomSeed(analogRead(UNUSED_ANALOG));
  queueNewBlock();

  lcd.begin(16, 2);

  Serial.begin(9600);


}



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
    lc.shutdown(0, true);
    restart();
    return -1;

  }
  else {
    if (millis() - lastInput > inputDelay) {
      handleInput();
      lastInput = millis();
    }
    if (millis() - lastUpdate > updateInterval) {
      updateState();
      renderLcd();
      lastUpdate = millis();
    }
    render();
    // playSong();
    return 1;
  }
}