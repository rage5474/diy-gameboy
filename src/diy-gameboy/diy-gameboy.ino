#include "LedControl.h"
#include <binary.h>

int startSign[8][8] = {
  {0,0,0,0,0,0,0,0},
  {0,1,1,0,0,1,1,0},
  {0,1,1,0,0,0,0,1},
  {0,0,0,0,0,0,0,1},
  {0,0,0,0,0,0,0,1},
  {0,1,1,0,0,0,0,1},
  {0,1,1,0,0,1,1,0},
  {0,0,0,0,0,0,0,0}
};

int gameOverSign[8][8] = {
  {0,0,0,0,0,0,0,0},
  {0,1,1,0,0,0,1,1},
  {0,1,1,0,0,1,0,0},
  {0,0,0,0,0,1,0,0},
  {0,0,0,0,0,1,0,0},
  {0,1,1,0,0,1,0,0},
  {0,1,1,0,0,0,1,1},
  {0,0,0,0,0,0,0,0}
};

int field[8][8];

LedControl lc=LedControl(8,10,9,1);
unsigned int delaytime = 100;
unsigned int buttonId = 12;
bool trace = true;

int oldPlayerPosX;
const int oldPlayerPosY = 7;
int currentPlayerPosX;
const int currentPlayerPosY = 7;
int oldBallPosX;
int oldBallPosY;
int currentBallPosX;
int currentBallPosY;

long countRuns;
bool notStarted;
bool gameOver;
unsigned int level;

void setup() 
{
  setupLogging();
  setupDisplay();
  setupButton();
  setupGameParameter();
}

void setupLogging()
{
  Serial.begin(9600);
}

void setupDisplay()
{
  lc.shutdown(0,false);
  lc.setIntensity(0,0);
  lc.clearDisplay(0);
}

void setupGameParameter()
{
  setupGameStateVars();
  setupField();
  setupPlayerPosition();
  setupBallPosition();
}

void setupBallPosition()
{
  const int startPosition = random(0,8);
  oldBallPosX = startPosition;
  currentBallPosX = startPosition;
  oldBallPosY = -1;
  currentBallPosY = -1;
}

void setupPlayerPosition()
{
  oldPlayerPosX = 0;
  currentPlayerPosX = 0;
}

void setupGameStateVars()
{
  countRuns = 0;
  notStarted = true;
  gameOver = false;
  level = 0;
}

void setupField()
{
  for(int i = 0; i < 8; i++)
  {
    for(int j = 0; j < 8; j++)
    {
      field[i][j] = 0;
    }
  }
}

void setupButton()
{
  pinMode(buttonId,INPUT);
  digitalWrite(buttonId,HIGH);
}

void loop() {

  if(notStarted)
  {
    printField(startSign);
    int buttonState = digitalRead(buttonId);
    if (buttonState == LOW) 
    { 
      notStarted = false;
      logMsg("Game starting.");
    } 
  }
  else if(!gameOver)
  {
    countRuns++;
    
    int x = analogRead(1);
    logVal("Player X: ",x);
    if(x < 50)
    {
      movePlayerRight();
    }
    else if(x > 1000)
    {
      movePlayerLeft();
    }
  
    updatePosition();

    // Ball hit
    if(currentPlayerPosX == currentBallPosX && currentBallPosY == 6)
    {
      resetBallPosition();
      updateLevel();
    }
    else
    {
      // Change ball depending on level
      if(countRuns % (10 - level) == 0)
      {
        changeBallPosition();
      }  
    }
  
    printField(field);
    delay(100);
  }
  else
  {
    printField(gameOverSign); 
    delay(10000);
    setupGameParameter();
  }
 }

void logMsg(String message)
{
  if(trace)
  {
    Serial.println(message);
  }
}

void logVal(String message, int value)
{
  if(trace)
  {
    Serial.print(message);
    Serial.println(value);
  }
}

void movePlayerRight()
{
  oldPlayerPosX = currentPlayerPosX;
  currentPlayerPosX += 1;
  if(currentPlayerPosX > 7)
  {
    currentPlayerPosX = 7;
  }
  logVal("Moved right. New position: ", currentPlayerPosX);
}

void movePlayerLeft()
{
  oldPlayerPosX = currentPlayerPosX;
  currentPlayerPosX -= 1;
  if(currentPlayerPosX < 0)
  {
    currentPlayerPosX = 0;
  }
  logVal("Moved left. New position: ", currentPlayerPosX);
}

void updatePosition()
{
  field[oldPlayerPosX][oldPlayerPosY] = 0;
  field[currentPlayerPosX][currentPlayerPosY] = 1;
  // logVal("Update position. Old X: ", oldPlayerPosX);
  // logVal("Update position. Old Y: ", oldPlayerPosY);
  // logVal("Update position. New X: ", currentPlayerPosX);
  // logVal("Update position. New Y: ", currentPlayerPosY);
}

void resetBallPosition()
{
  field[currentBallPosX][currentBallPosY] = 0;
  setupBallPosition();
}

void changeBallPosition()
{
  oldBallPosY = currentBallPosY;
  currentBallPosY += 1;
  if(currentBallPosY > 7)
  {
    currentBallPosY = -1;
    gameOver = true;
    logMsg("Game over.");
  }
  field[oldBallPosX][oldBallPosY] = 0;
  field[currentBallPosX][currentBallPosY] = 1;
}

void updateLevel()
{
  level++;
  if(level > 9)
  {
    level = 9;
  }
  logVal("New level: ", level);
}

void printField(int field[8][8])
{
   for(int x = 0; x < 8; x++)
   {
    for(int y= 0; y < 8; y++)
    {
      int val = field[x][y];
      if(val == 0)
      {
        lc.setLed(0,y,x,false);
      }
      else
      {
        lc.setLed(0,y,x,true);
      }
    }
  }
}

// Currently not needed
void drop(int col)
{
  int secondPower = 1 << col;
  dropDo(secondPower);
}

void dropDo(int val)
{
  for(int i = 0; i < 7; i++)
  {
    if(i > 0)
      lc.setRow(0,i-1,0);
    lc.setRow(0,i,val);
    delay(500);
  }
  lc.setRow(0,6,0);
}
