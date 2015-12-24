#include <Esplora.h>

void setup() {
  // initialize the serial communication:
  Serial.begin(9600);
}

int MODE_READY = 1;
int MODE_SETTING = 2;
int MODE_RUNNING = 3;
int MODE_FINISHED = 4;

boolean isDepressed = false;
int depressedSince;
int hitCount;

int mode = MODE_READY;
int minDuration = 1;
int maxDuration = 300;
int duration = 60;
int elapsed = 0;
int timeStarted;

void doReady() {
  isDepressed = false;
  hitCount = 0;
  Esplora.writeRGB(0,255,0);
  if(Esplora.readButton(SWITCH_UP) == LOW) {
    // start the countdown
    mode = MODE_RUNNING;
    Serial.print("0\n");
  } else if (Esplora.readButton(SWITCH_RIGHT) == LOW) {
    mode = MODE_SETTING;
    Serial.print(String(duration) + "\n");
  } else {
    delay(50);
  }

}

void doSetting() {
  if (millis() / 500 % 2 == 0) {
    Esplora.writeRGB(0,255,0);
  } else {
    Esplora.writeRGB(0,0,0);
  }

  int adjustment = 0;

  if (!isDepressed) {
    if (Esplora.readButton(SWITCH_DOWN) == LOW) {
      isDepressed = true;
      depressedSince = millis();
      adjustment = -1;
    } else if (Esplora.readButton(SWITCH_UP) == LOW) {
      isDepressed = true;
      depressedSince = millis();
      adjustment = 1;
    } else {
      if (Esplora.readButton(SWITCH_LEFT) == LOW) {
        isDepressed = false;
        mode = MODE_READY;
      }
    }
  } else {
    if (Esplora.readButton(SWITCH_DOWN) == HIGH && Esplora.readButton(SWITCH_UP) == HIGH) {
      isDepressed = false;
    } else {
      // one of the buttons is still held down
      if (Esplora.readButton(SWITCH_DOWN) == LOW && duration) {
        adjustment = -1;
      } else {
        adjustment = 1;
      }
    }
  }
  if ((adjustment > 0 && duration < maxDuration) || (adjustment < 0 && duration > minDuration)) {
    int millisElapsed = millis() - depressedSince;
    int sub = 1000 / sq(millisElapsed / 1000 + 1);
    if ((millisElapsed % 1000) % sub == 0) {
      duration += adjustment;
      Serial.print(String(duration) + "\n");
      delay(1);    
    } 
  }
}

void doRunning() {
  
  int millisElapsed;
  
  if (hitCount == 0) {
    millisElapsed = 0;
  } else {
    millisElapsed = millis() - timeStarted;
  }
  
  if (millisElapsed >= duration * 1000) {
    // done
    mode = MODE_FINISHED;
  } else {
    
    if (millisElapsed / 500 % 2 == 0) {
      Esplora.writeRGB(0,0,255);
    } else {
      Esplora.writeRGB(0,0,0);
    }
    
    if (!isDepressed && Esplora.readButton(SWITCH_DOWN) == LOW) {
      isDepressed = true;
      if (hitCount == 0) {
        timeStarted = millis();
      }
    } else if (isDepressed && Esplora.readButton(SWITCH_DOWN) == HIGH) {
      Serial.print(String(++hitCount) + "\n");
      isDepressed = false;
    }
    
  }
}

void doFinished() {
  mode = MODE_READY;
  Esplora.writeRGB(255,0,0);
  delay(1000);
  Esplora.writeRGB(0,0,0);
  delay(1000);
}

void loop() {
  switch (mode) {
    case 4:
      doFinished();
      break;
    case 3:
      doRunning();
      break;
    case 2:
      doSetting();
      break;
    case 1:
    default:
      doReady();
  }
}

