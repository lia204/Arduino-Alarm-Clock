#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <ThreeWire.h>
#include <RtcDS1302.h>

LiquidCrystal_I2C lcd(0x27, 16, 2);

ThreeWire myWire(4, 5, 3);
RtcDS1302<ThreeWire> Rtc(myWire);

const int SET = 8;
const int UP = 9;
const int DOWN = 10;
const int BUZZER = 7;

int alarmHour = 7;
int alarmMinute = 0;

bool alarmEnabled = true;
bool alarmRinging = false;
bool showClock = true;

int mode = 0; // 0=רגיל, 1=שעה, 2=דקות

unsigned long screenTimer = 0;

void setup() {
  pinMode(SET, INPUT_PULLUP);
  pinMode(UP, INPUT_PULLUP);
  pinMode(DOWN, INPUT_PULLUP);
  pinMode(BUZZER, OUTPUT);

  lcd.init();
  lcd.backlight();

  Rtc.Begin();

  RtcDateTime compiled = RtcDateTime(__DATE__, __TIME__);
  Rtc.SetDateTime(compiled);

  if (!Rtc.GetIsRunning()) {
    Rtc.SetIsRunning(true);
  }
}

void loop() {
  RtcDateTime now = Rtc.GetDateTime();

  buttons();

  if (alarmEnabled &&
      now.Hour() == alarmHour &&
      now.Minute() == alarmMinute) {
    alarmRinging = true;
  }

  if (alarmRinging) {
    tone(BUZZER, 1000);
  } else {
    noTone(BUZZER);
  }

  if (millis() - screenTimer > 5000 && mode == 0) {
    showClock = !showClock;
    screenTimer = millis();
    lcd.clear();
  }

  lcd.setCursor(0, 0);

  if (mode == 1) {
    lcd.print("SET HOUR        ");
    lcd.setCursor(0, 1);

    if (alarmHour < 10) lcd.print("0");
    lcd.print(alarmHour);
    lcd.print("              ");
  }

  else if (mode == 2) {
    lcd.print("SET MINUTE      ");
    lcd.setCursor(0, 1);

    if (alarmMinute < 10) lcd.print("0");
    lcd.print(alarmMinute);
    lcd.print("              ");
  }

  else if (showClock) {
    if (now.Hour() < 10) lcd.print("0");
    lcd.print(now.Hour());
    lcd.print(":");

    if (now.Minute() < 10) lcd.print("0");
    lcd.print(now.Minute());
    lcd.print(":");

    if (now.Second() < 10) lcd.print("0");
    lcd.print(now.Second());

    lcd.print("        ");

    lcd.setCursor(0, 1);

    if (now.Day() < 10) lcd.print("0");
    lcd.print(now.Day());
    lcd.print("/");

    if (now.Month() < 10) lcd.print("0");
    lcd.print(now.Month());
    lcd.print("/");

    lcd.print(now.Year());
    lcd.print("      ");
  }

  else {
    if (alarmEnabled) {
      lcd.print("ACTIVE          ");
    } else {
      lcd.print("DISABLED        ");
    }

    lcd.setCursor(0, 1);

    if (alarmHour < 10) lcd.print("0");
    lcd.print(alarmHour);
    lcd.print(":");

    if (alarmMinute < 10) lcd.print("0");
    lcd.print(alarmMinute);

    lcd.print("           ");
  }

  delay(150);
}

void buttons() {
  static bool lastSet = HIGH;
  static bool lastUp = HIGH;
  static bool lastDown = HIGH;

  bool setState = digitalRead(SET);
  bool upState = digitalRead(UP);
  bool downState = digitalRead(DOWN);

  if (upState == LOW && downState == LOW) {
    alarmEnabled = false;
    alarmRinging = false;
  }

  if (setState == LOW && lastSet == HIGH) {
    if (alarmRinging) {
      alarmRinging = false;
      alarmEnabled = false;
    } else {
      mode++;

      if (mode > 2) {
        mode = 0;
        alarmEnabled = true;
      }

      lcd.clear();
    }
  }

  if (mode == 1) {
    if (upState == LOW && lastUp == HIGH) {
      alarmHour = (alarmHour + 1) % 24;
    }

    if (downState == LOW && lastDown == HIGH) {
      alarmHour--;

      if (alarmHour < 0) {
        alarmHour = 23;
      }
    }
  }

  if (mode == 2) {
    if (upState == LOW && lastUp == HIGH) {
      alarmMinute = (alarmMinute + 1) % 60;
    }

    if (downState == LOW && lastDown == HIGH) {
      alarmMinute--;

      if (alarmMinute < 0) {
        alarmMinute = 59;
      }
    }
  }

  lastSet = setState;
  lastUp = upState;
  lastDown = downState;
}
