#include <Esplora.h>
#include <TFT.h>            // Arduino LCD library
#include <SPI.h>

void setup() {
  // initialize the serial communication:
  Serial.begin(9600);
  
  EsploraTFT.begin();
  EsploraTFT.background(255, 255, 255);
  EsploraTFT.stroke(0, 0, 0);
  EsploraTFT.fill(255, 255, 255);
  
  EsploraTFT.text("Time", 1, 1);
  EsploraTFT.line(0, 63, 159, 63);
  EsploraTFT.text("Score", 1, 65);

  EsploraTFT.setTextSize(5);
  
  enterRunning();

}

int MODE_READY = 1;
int MODE_SETTING = 2;
int MODE_RUNNING = 3;
int MODE_FINISHED = 4;

boolean hit = false;
boolean isDepressed = false;
unsigned long depressedSince;
int hitCount;

int mode = MODE_RUNNING;
int minDuration = 1;
int maxDuration = 300;
unsigned long duration = 60;
int elapsed = 0;
unsigned long timeStarted;

char scoreChars[5]; 
char timeChars[5]; 

void displayTime() {
  EsploraTFT.stroke(255, 255, 255);
  EsploraTFT.text(timeChars, 1, 18);
}

void displayTime(unsigned long value) {
  displayTime();
  String(value).toCharArray(timeChars, 5);
  EsploraTFT.stroke(0, 0, 0);
  EsploraTFT.text(timeChars, 1, 18);
}

void displayScore() {
  EsploraTFT.stroke(255, 255, 255);
  EsploraTFT.text(scoreChars, 1, 82);
}

void displayScore(int value) {
  displayScore();
  String(value).toCharArray(scoreChars, 5);
  EsploraTFT.stroke(0, 0, 0);
  EsploraTFT.text(scoreChars, 1, 82);
}


void enterSetting() {
  isDepressed = false;
  displayTime(duration);
  displayScore(0);
  mode = MODE_SETTING;
}


unsigned int oldTimeBlink;

void doSetting() {

  // handle done / exit setting mode
  if (Esplora.readButton(SWITCH_DOWN) == LOW) {
    enterRunning();
  }

  unsigned long mills = millis();

  // blink LED green
  if (mills / 500 % 2 == 0) {
    Esplora.writeRGB(0,255,0);
  } else {
    Esplora.writeRGB(0,0,0);
  }

  boolean isLeftPressed = (Esplora.readButton(SWITCH_LEFT) == LOW);
  boolean isRightPressed = (Esplora.readButton(SWITCH_RIGHT) == LOW);

  if (isDepressed && !isLeftPressed && !isRightPressed) {
    isDepressed = false;
  } else if (!isDepressed && (isLeftPressed || isRightPressed)) {
    isDepressed = true;
    depressedSince = mills;
  }

  if ((isLeftPressed && (duration > minDuration)) || (isRightPressed && (duration < maxDuration))) {
    // receiving input
    int adjustment = isLeftPressed ? -1 : 1;
    unsigned long millisElapsed = mills - depressedSince;
    int sub = 1000 / sq(millisElapsed / 1000 + 1);
    if ((millisElapsed > 4000) || (millisElapsed % 1000) % sub == 0) {
      duration += adjustment;
      displayTime(duration);
    } 
  } else {
    // waiting for input
    // blink current setting while standing by
    int timeBlink = mills / 500 % 2;  // result is either 0 or 1
    if (timeBlink && !oldTimeBlink) {
      displayTime();
    } else if (!timeBlink && oldTimeBlink) {
      displayTime(duration);
    }
    oldTimeBlink = timeBlink;
  }

  
}

void enterRunning() {
  isDepressed = false;
  hitCount = 0;
  displayScore(0);
  displayTime(duration);
  delay(10);
  mode = MODE_RUNNING;
}

unsigned int oldScoreBlink;
unsigned int oldSecondsRemaining;

void doRunning() {

  // handle escape / go to time setting mode
  if (Esplora.readButton(SWITCH_UP) == LOW) {
    enterSetting();
  } else if (Esplora.readButton(SWITCH_LEFT) == LOW) {
    // reset / restart
    enterRunning();
  }


  unsigned long mills = millis();
  unsigned long millisElapsed;
  unsigned int secondsRemaining;

  if (hitCount == 0) {
    // standby (no hits counted yet)
    millisElapsed = 0;
    secondsRemaining = duration;

    // blink zero score while standing by
    int scoreBlink = mills / 500 % 2;  // result is either 0 or 1
    if (scoreBlink && !oldScoreBlink) {
      displayScore(0);
      Esplora.writeRGB(0,0,255);
    } else if (!scoreBlink && oldScoreBlink) {
      displayScore();
      Esplora.writeRGB(0,0,0);
    }
    oldScoreBlink = scoreBlink;
  
  } else {
    
    // timer is running
    millisElapsed = mills - timeStarted;
    secondsRemaining = duration - millisElapsed / 1000;

    // update time display if needed
    if (secondsRemaining != oldSecondsRemaining) {
      displayTime(secondsRemaining);
    }
    oldSecondsRemaining = secondsRemaining;

  }

  if (hitCount && !secondsRemaining) {
    // done: timer just ran out
    mode = MODE_FINISHED;
  } else {

    // check for a hit and update counter / display if so.

    boolean newHit = false;
    unsigned int thresh = Esplora.readSlider();
    unsigned int level = Esplora.readMicrophone();
  
    if (level > thresh) {
      if (!hit) {
        if (!hitCount) {
          Esplora.writeRGB(0,0,255);
          timeStarted = millis();
        }
        newHit = true;
        hit = true;
        hitCount++;
      }
    } else if (hit && (level < (thresh / 2))) {
      hit = false;
      displayScore(hitCount);
    }

  }
}

void doFinished() {

  // hit left button to run again
  if (Esplora.readButton(SWITCH_LEFT) == LOW) {
    enterRunning();
  }

  // blink LED red
  if (millis() / 1000 % 2 == 0) {
    Esplora.writeRGB(255,0,0);
  } else {
    Esplora.writeRGB(0,0,0);
  }

}

void loop() {
  switch (mode) {
    case 4:
      doFinished();
      break;
    case 2:
      doSetting();
      break;
    case 3:
    default:
      doRunning();
  }
}

