#include &lt;Wire.h&gt;
#include &lt;LiquidCrystal_I2C.h&gt;

#include &lt;ThreeWire.h&gt;
#include &lt;RtcDS1302.h&gt;

LiquidCrystal_I2C lcd(0x27, 16, 2);

ThreeWire myWire(4, 5, 3);
RtcDS1302&lt;ThreeWire&gt; Rtc(myWire);

const int SET = 8;
const int UP = 9;
const int DOWN = 10;
const int BUZZER = 7;

int alarmHour = 7;
int alarmMinute = 0;

bool alarmEnabled = true;
bool alarmRinging = false;
bool showClock = true;

int mode = 0; // 0=רגיל 1=שעה 2=דקות

unsigned long screenTimer = 0;

void setup() {
  pinMode(SET, INPUT_PULLUP);
  pinMode(UP, INPUT_PULLUP);
  pinMode(DOWN, INPUT_PULLUP);
  pinMode(BUZZER, OUTPUT);

  lcd.init();
  lcd.backlight();

  Rtc.Begin();

  // הגדרת השעה לפי זמן המחשב בזמן העלאת התוכנית
  RtcDateTime compiled = RtcDateTime(__DATE__, __TIME__);
  Rtc.SetDateTime(compiled);

  if (!Rtc.GetIsRunning()) {
    Rtc.SetIsRunning(true);
  }
}

void loop() {
  RtcDateTime now = Rtc.GetDateTime();

  buttons();

  if (alarmEnabled &amp;&amp;
      now.Hour() == alarmHour &amp;&amp;
      now.Minute() == alarmMinute) {
    alarmRinging = true;
  }

  if (alarmRinging) {
    tone(BUZZER, 1000);
  } else {

    noTone(BUZZER);
  }

  if (millis() - screenTimer &gt; 5000 &amp;&amp; mode == 0) {
    showClock = !showClock;
    screenTimer = millis();
    lcd.clear();
  }

  lcd.setCursor(0, 0);

  if (mode == 1) {
    lcd.print(&quot;SET HOUR      &quot;);
    lcd.setCursor(0, 1);
    lcd.print(alarmHour);
    lcd.print(&quot;            &quot;);
  }

  else if (mode == 2) {
    lcd.print(&quot;SET MINUTE    &quot;);
    lcd.setCursor(0, 1);
    lcd.print(alarmMinute);
    lcd.print(&quot;          &quot;);
  }

  else if (showClock) {
    if (now.Hour() &lt; 10) lcd.print(&quot;0&quot;);
    lcd.print(now.Hour());
    lcd.print(&quot;:&quot;);

    if (now.Minute() &lt; 10) lcd.print(&quot;0&quot;);
    lcd.print(now.Minute());
    lcd.print(&quot;:&quot;);

    if (now.Second() &lt; 10) lcd.print(&quot;0&quot;);
    lcd.print(now.Second());

    lcd.setCursor(0, 1);
    lcd.print(now.Day());
    lcd.print(&quot;/&quot;);
    lcd.print(now.Month());
    lcd.print(&quot;/&quot;);
    lcd.print(now.Year());
    lcd.print(&quot;   &quot;);
  }

  else {
    if (alarmEnabled)
      lcd.print(&quot;ACTIVE        &quot;);
    else
      lcd.print(&quot;DISABLED      &quot;);

    lcd.setCursor(0, 1);

    if (alarmHour &lt; 10) lcd.print(&quot;0&quot;);
    lcd.print(alarmHour);
    lcd.print(&quot;:&quot;);

    if (alarmMinute &lt; 10) lcd.print(&quot;0&quot;);
    lcd.print(alarmMinute);
    lcd.print(&quot;       &quot;);
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

  if (alarmRinging &amp;&amp; setState == LOW &amp;&amp; lastSet == HIGH) {
    alarmRinging = false;
    alarmEnabled = false;
  }

  if (upState == LOW &amp;&amp; downState == LOW) {
    alarmEnabled = false;
    alarmRinging = false;
  }

  if (setState == LOW &amp;&amp; lastSet == HIGH) {
    mode++;

    if (mode &gt; 2) {
      mode = 0;
      alarmEnabled = true;
    }

    lcd.clear();
  }

  if (mode == 1) {
    if (upState == LOW &amp;&amp; lastUp == HIGH) alarmHour = (alarmHour + 1) %
24;

    if (downState == LOW &amp;&amp; lastDown == HIGH) {
      alarmHour--;
      if (alarmHour &lt; 0) alarmHour = 23;
    }
  }

  if (mode == 2) {
    if (upState == LOW &amp;&amp; lastUp == HIGH) alarmMinute = (alarmMinute + 1)
% 60;

    if (downState == LOW &amp;&amp; lastDown == HIGH) {
      alarmMinute--;
      if (alarmMinute &lt; 0) alarmMinute = 59;
    }
 }

  lastSet = setState;

  lastUp = upState;
  lastDown = downState;
}void setup() {
  // put your setup code here, to run once:

}

void loop() {
  // put your main code here, to run repeatedly:

}
