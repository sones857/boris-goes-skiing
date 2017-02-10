/*
  To do list:

    Get game score to reset and return to title screen after boris dies.
    Add high score save
    Figure out what is making boris vanish on left button (check borisleft) + button presses
    Widen gap hit on flags to overlap so it's a bit easier.

*/

//boris_goes_skiing.ino

#include <Arduboy2.h>
#include "bitmaps.h"
#include <ArduboyTones.h>
//#include <EEPROM.h>

Arduboy2 arduboy;
ArduboyTones sound(arduboy.audio.enabled);

int borisX = 56;
int borisY = 5;
byte lives = 3;
byte borisDirection = 2;
// 0=left    1=diag left    2=down    3=diag right    4=right
int speed = 0;
long score = 0;
unsigned long thingFrameRate;
unsigned long keyPress;
int randomThing;

struct scrollingData
{
  int x;
  int y;
  byte state;
};

/*
    0 = Nothing
    1 = tree
    2 = flag
    3 = bump
*/

scrollingData things[10];
byte thingIndex = 0;

// =================================================================
void setup()
{
  //arduboy.begin();
  arduboy.boot();
  arduboy.flashlight();
  arduboy.blank();
  arduboy.audio.on();
  Serial.begin(9600);
  arduboy.drawSlowXYBitmap(0, 0, titlescreen, 128, 64);
  arduboy.display();
  sound.tone(NOTE_B4,300, NOTE_G4,300, NOTE_F4,500);
  delay(5000);
  initialiseScrolling();
  thingFrameRate = millis();
  keyPress = millis();
  arduboy.initRandomSeed();
  randomThing = 2000;
}

// =================================================================
void initialiseScrolling()
{
  for (int index = 0; index < 10; index++)
  {
    things[index].state = false;
  }
}

// =================================================================
void checkButtons()
{
  if (arduboy.pressed(LEFT_BUTTON)) 
  {
    borisDirection--;
    if (borisDirection < 0) borisDirection = 0;
  }

  if (arduboy.pressed(RIGHT_BUTTON)) 
  {
    borisDirection++;
    if (borisDirection > 4) borisDirection = 4;
  }
  keyPress = millis();
}

// =================================================================
void drawThings()
{
  for (int index = 0; index < 9; index++)
  {
    if (things[index].state > 0)
    {
      if (things[index].state == 1)
      {
        arduboy.drawBitmap(things[index].x, things[index].y, treeinverted, 16, 16, BLACK);
        checkForTreeHit(index);
      }
      if (things[index].state == 2)
      {
        arduboy.drawBitmap(things[index].x, things[index].y, flag, 4, 8, BLACK);
        arduboy.drawBitmap(things[index].x + 32, things[index].y, flag, 4, 8, BLACK);
        checkForFlagPass(index);
      }

      things[index].y = things[index].y - 1;
    }
  }
}

// =================================================================
void checkForFlagPass(byte index)
{
  if ((things[index].y + 7 == borisY + 15) && ((borisX > things[index].x + 1) && (borisX + 15 < things[index].x + 31))) score = score + 1, sound.tone(NOTE_A4,200);
}

// =================================================================
void checkForTreeHit(byte index)
{
  if (borisY + 15 == things[index].y + 15 && borisX < things[index].x + 15 && borisX + 15 >= things[index].x)
  {
    lives = lives - 1;
    sound.tone(NOTE_F4,400);
    delay(1000);
    if (lives < 1) borisDies();
  }
}

// =================================================================
void borisDies()
{
  arduboy.clear();
  arduboy.drawSlowXYBitmap(0, 0, borisdiesscreen, 128, 64);
  arduboy.display();
  sound.tone(NOTE_A4,500, NOTE_A4,500, NOTE_F4,1000);
  delay(6000);
  arduboy.clear();
  arduboy.begin();
}

// =================================================================
void drawBoris()
{
  switch (borisDirection)
  {
    case 0:
      arduboy.drawBitmap(borisX, borisY, borisleft, 16, 16, BLACK);
      break;
    case 1:
      arduboy.drawBitmap(borisX, borisY, borisleftdiag, 16, 16, BLACK);
      break;
    case 2:
      arduboy.drawBitmap(borisX, borisY, borisdown, 16, 16, BLACK);
      break;
    case 3:
      arduboy.drawBitmap(borisX, borisY, borisrightdiag, 16, 16, BLACK);
      break;
    case 4:
      arduboy.drawBitmap(borisX, borisY, borisright, 16, 16, BLACK);
      break;
  }
}

// =================================================================
void createThings()
{
  if ((millis() - thingFrameRate) > randomThing)
  {
    things[thingIndex].state = random(2) + 1;

    if (things[thingIndex].state == 1) things[thingIndex].x = random(111);
    if (things[thingIndex].state == 2) things[thingIndex].x = random(87);
    things[thingIndex].y = 64;
    thingIndex++;
    if (thingIndex > 9) thingIndex = 0;
    thingFrameRate = millis();
    randomThing = 500 + random(1000);
  }
}

// =================================================================
void moveBoris()
{
  switch (borisDirection)
  {
    case 0:
      borisX = borisX - 1;
      break;
    case 1:
      borisX = borisX - 1;
      break;
    case 3:
      borisX = borisX + 1;
      break;
    case 4:
      borisX = borisX + 1;
      break;
  }

  if (borisX < 0)
  {
    borisX = 0;
    borisDirection = 2;
  }
  if (borisX > 111)
  {
    borisX = 111;
    borisDirection = 2;
  }

}

// =================================================================
void loop() {

  if (!arduboy.nextFrame()) 
  {
    return;
  }
  arduboy.clear();
  arduboy.fillScreen(1);
  arduboy.setCursor(0, 0);
  arduboy.setTextColor(BLACK);
  arduboy.setTextBackground(WHITE);
  arduboy.print("Score: " + String(score) + "   Lives : " + String(lives));
  if ((millis() - keyPress) > 100) checkButtons();
  moveBoris();
  drawThings();
  drawBoris();
  createThings();

  Serial.write(arduboy.getBuffer(), 128 * 64 / 8);

  arduboy.display();
}


