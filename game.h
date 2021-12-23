#include "utils.h"
#include "song.h"

struct point
{
  int x;
  int y;
};

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

char moving[12][8];
char stationary[8][8];

unsigned long lastUpdate;
int updateInterval;

unsigned long lastInputTimer;
const unsigned int inputDelay = 120;

int score;
int level;
int countUpdates;

void transformMoving(int x, int y)
{
  char updatedArr[12][8];
  int xValid = 1;

  for (int i = 0; i < 12; i++)
  {
    for (int j = 0; j < 8; j++)
    {
      updatedArr[i][j] = 0;
    }
  }

  for (int i = 0; i < 12 && xValid; i++)
  {
    for (int j = 0; j < 8 && xValid; j++)
    {
      if (moving[i][j])
      {
        if (j + x >= 8 || j + x < 0)
        {
          xValid = 0;
        }
      }
    }
  }

  for (int i = 0; i < 12; i++)
  {
    for (int j = 0; j < 8; j++)
    {
      if (moving[i][j])
      {
        if (i + y < 12 && i + y >= 0)
        {
          if (xValid)
          {
            if (i >= 4 && stationary[i - 4 + y][j + x])
            {
              return;
            }
            updatedArr[i + y][j + x] = moving[i][j];
          }
          else
          {
            updatedArr[i + y][j] = moving[i][j];
          }
        }
      }
    }
  }

  for (int i = 0; i < 12; i++)
  {
    for (int j = 0; j < 8; j++)
    {
      moving[i][j] = updatedArr[i][j];
    }
  }
}


int isGameOver()
{
  for (int j = 0; j < 8; j++)
  {
    if (stationary[0][j])
    {
      return 1;
    }
  }
  return 0;
}

void rotate90()
{
  struct point pivot = {-1, -1};

  char updatedArr[12][8];

  for (int i = 0; i < 12; i++)
  {
    for (int j = 0; j < 8; j++)
    {
      updatedArr[i][j] = 0;
      if (moving[i][j] == 2)
      {
        pivot.y = i;
        pivot.x = j;
      }
    }
  }

  if (pivot.y == -1 || pivot.x == -1)
  {
    return;
  }

  for (int i = 0; i < 12; i++)
  {
    for (int j = 0; j < 8; j++)
    {
      if (moving[i][j] == 1)
      {
        struct point toCoordinate = {j - pivot.x, i - pivot.y};
        struct point transformedCoordinate = {-toCoordinate.y, toCoordinate.x};
        struct point transformedGrid = {transformedCoordinate.x + pivot.x, transformedCoordinate.y + pivot.y};

        if (transformedGrid.x >= 8 || transformedGrid.x < 0 || transformedGrid.y >= 12 || transformedGrid.y < 0 || (transformedGrid.y >= 4 && stationary[transformedGrid.y - 4][transformedGrid.x]))
        {
          return;
        }

        updatedArr[transformedGrid.y][transformedGrid.x] = 1;
      }
      else if (moving[i][j] == 2)
      {
        updatedArr[i][j] = 2;
      }
    }
  }

  for (int i = 0; i < 12; i++)
  {
    for (int j = 0; j < 8; j++)
    {
      moving[i][j] = updatedArr[i][j];
    }
  }
}

void handleInput()
{

  int input = readInput();

  switch (input)
  {
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
  case enter:
    rotate90();
    break;

  case pause:
    if (!isGameOver())
    {
      gamePaused = true;
      screenStatus = STATUS_MAINMENU;
      pointerMode = POINTER_SCROLL;
      currentMainMenuIndex = 0;
      lc.shutdown(0,true);
      lcd.clear();
      printLine(0, "===GAME PAUSED===");
      lcd.setCursor(0, 1);
      lcd.write("Please resume");
      for (int i = 0; i < 3; i++)
      {
        lcd.write("!"); 
        delay(400);
      }
    }
    break;
  }
}

int isMovingAtBottom()
{
  for (int i = 0; i < 12; i++)
  {
    for (int j = 0; j < 8; j++)
    {
      if (moving[i][j] && i == 11 || (moving[i][j] && i >= 4 && i + 1 < 12 && stationary[i - 4 + 1][j]))
      {
        return 1;
      }
    }
  }

  return 0;
}

void handleAtBottom()
{
  for (int i = 0; i < 8; i++)
  {
    for (int j = 0; j < 8; j++)
    {
      stationary[i][j] = stationary[i][j] || moving[i + 4][j];
      moving[i + 4][j] = 0;
    }
  }
}

void render()
{
  for (int i = 0; i < 8; i++)
  {
    for (int j = 0; j < 8; j++)
    {
      lc.setLed(0, i, j, moving[i + 4][j] | stationary[i][j]);
    }
  }
}

void renderLcd()
{
  lcd.clear();
  lcd.setCursor(1, 0);
  lcd.print("Level: ");
  lcd.print(level);

  lcd.setCursor(1, 1);
  lcd.print("Score: ");
  lcd.print(score);
}

void updateRows()
{
  int rowsUpdated = 0;
  for (int i = 0; i < 8; i++)
  {
    int validRow = 1;
    for (int j = 0; j < 8; j++)
    {
      if (!stationary[i][j])
      {
        validRow = 0;
      }
    }
    if (validRow)
    {
      tone(BUZZER, 1000);
      delay(50);
      noTone(BUZZER);
      rowsUpdated++;
      for (int j = 0; j < 8; j++)
      {
        stationary[i][j] = 0;
      }
      char updated[8][8];
      for (int i = 0; i < 8; i++)
        for (int j = 0; j < 8; j++)
        {
          updated[i][j] = 0;
        }
      for (int k = i; k >= 0; k--)
      {
        for (int l = 0; l < 8; l++)
        {
          if (k - 1 >= 0)
          {
            updated[k][l] = stationary[k - 1][l];
          }
        }
      }
      for (int k = i + 1; k < 8; k++)
      {
        for (int l = 0; l < 8; l++)
        {
          updated[k][l] = stationary[k][l];
        }
      }
      for (int k = 0; k < 8; k++)
      {
        for (int l = 0; l < 8; l++)
        {
          stationary[k][l] = updated[k][l];
        }
      }
    }
  }
  switch (rowsUpdated)
  {
  case 1:
    score += 40 * (level + 1 + difficultyValue - EASY);
    break;
  case 2:
    score += 100 * (level + 1 + difficultyValue - EASY);
    break;
  case 3:
    score += 300 * (level + 1 +difficultyValue - EASY);
    break;
  case 4:
    score += 1200 * (level + 1 + difficultyValue - EASY);
    break;
  }
}


void queueNewBlock()
{
  int randomBlock = random(BLOCK_COUNT - 1);
  for (int i = 0; i < 4; i++)
  {
    for (int j = 0; j < 8; j++)
    {
      moving[i][j] = blocks[randomBlock][i][j];
    }
  }
}

void updateState()
{
  if (isMovingAtBottom())
  {
    handleAtBottom();
    queueNewBlock();
  }
  transformMoving(0, 1);
  updateRows();
}

void restart()
{
  memset(moving, 0, sizeof(moving) * sizeof(char));
  memset(stationary, 0, sizeof(stationary) * sizeof(char));
  lastUpdate = 0;
  lastInputTimer = 0;
  score = 0;
  level = 0; 
  countUpdates = 0;
}

void bootGame()
{
  // put your setup code here, to run once:
  restart();
  lc.shutdown(0, false);
  pinMode(UNUSED_ANALOG, INPUT);
  randomSeed(analogRead(UNUSED_ANALOG));
  queueNewBlock();
  lcd.begin(16, 2);

  if(difficultyValue == EASY) {
  for (int i = 0; i < 7; i++)
    stationary[7][i] = 1;
  updateInterval = 1000 - level * 4;
  }
  else if(difficultyValue == MEDIUM) {
    updateInterval = 700 - level * 8;
  }
  else if(difficultyValue == HARD) {
    updateInterval = 300 - level * 15;
  }
}

void congrulations(const int &score) {
  int place = saveHighScore(currentUsername, score);
  Serial.println(place);
  if(place == -1) {
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print(currentUsername);
    lcd.print(', your score');
    lcd.setCursor(0,1);
    lcd.print("is not in top :(");
  }
  else {
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print(currentUsername);
    lcd.setCursor(0,1);
    lcd.print("you are ");
    if(place == 1) {
      lcd.print("1st     ");
    }
    else if(place == 2) {
      lcd.print("2nd     ");
    }
    else if(place == 3) {
      lcd.print("3rd    ");
    }
  }

}

int runGame()
{
  if (isGameOver())
  {
    lcd.clear();
    lcd.setCursor(2, 0);
    lcd.print("Game Over!");
    lcd.setCursor(0, 1);
    lcd.print("Score: ");
    lcd.print(score);
    for (int t = 0; t < 3; t++)
    {
      for (int i = 0; i < 8; i++)
      {
        for (int j = 0; j < 8; j++)
        {
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
    screenStatus = STATUS_MAINMENU;
    pointerMode = POINTER_SCROLL;
    currentMainMenuIndex = 0;
    return 1;
  }
  else
  {
    if (millis() - lastInputTimer > inputDelay)
    {
      handleInput();
      lastInputTimer = millis();
    }
    if (millis() - lastUpdate > updateInterval)
    {
      updateState();
      renderLcd();
      lastUpdate = millis();
      countUpdates++;
      if(countUpdates == 5) {
        countUpdates = 0;
        level++;
      }
    }
    render();
    // playSong();
    return 0;
  }
}
