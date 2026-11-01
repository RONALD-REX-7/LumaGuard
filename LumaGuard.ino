#include <Wire.h>
#include <EEPROM.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// ---------------- OLED ----------------
const uint8_t SCREEN_WIDTH = 128, SCREEN_HEIGHT = 64;
const int8_t OLED_RESET = -1;
const uint8_t OLED_ADDRESS = 0x3C;
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
bool oledReady = false;

// ---------------- Existing pin map ----------------
const uint8_t TRIG_PIN = 2, ECHO_PIN = 3;
const uint8_t GREEN_LED_PIN = 4, RED_LED_PIN = 5;
const uint8_t BUZZER_PIN = 6, BUTTON_PIN = 7;
const uint8_t FLAME_DO_PIN = 9, FLAME_AO_PIN = A0;

// ---------------- Calibration parameters ----------------
const float DISTANCE_ALERT_CM = 30.0;
const float DISTANCE_CLEAR_CM = 33.0;
const unsigned long ECHO_TIMEOUT_US = 25000UL;
const unsigned long DISTANCE_CLEAR_HOLD_MS = 350;
const unsigned long NO_ECHO_CLEAR_MS = 1200;
const unsigned long DISPLAY_ECHO_STALE_MS = 800;
const int FLAME_AO_TRIGGER = 800;
const int FLAME_AO_CLEAR = 850;
const unsigned long FLAME_CONFIRM_MS = 250;
const unsigned long FLAME_CLEAR_HOLD_MS = 1200;
const float FLAME_AO_CLEAR_REFERENCE = 1000.0;
const float FLAME_AO_STRONG_REFERENCE = 400.0;
const unsigned long BUTTON_DEBOUNCE_MS = 35;
const unsigned long LONG_PRESS_MS = 2000;
const unsigned long BUZZER_ON_MS = 80;
const unsigned int BUZZER_FREQUENCY_HZ = 2400;
const unsigned long DISTANCE_SLOW_PERIOD_MS = 900;
const unsigned long FLAME_SLOW_PERIOD_MS = 650;
const unsigned long FASTEST_PERIOD_MS = 180;
const unsigned long SENSOR_INTERVAL_MS = 100;
const unsigned long DISPLAY_INTERVAL_MS = 250;
const unsigned long SERIAL_INTERVAL_MS = 500;

// ---------------- Hazard bit masks ----------------
const uint8_t HAZARD_NONE = 0x00;
const uint8_t HAZARD_ZONE = 0x01;
const uint8_t HAZARD_FLAME = 0x02;

// ---------------- Sensor state ----------------
float distanceCm = -1.0, filteredDistanceCm = -1.0;
bool distanceFilterReady = false, distanceDanger = false;
bool haveValidDistance = false, distanceClearTimerActive = false;
unsigned long lastValidDistanceMs = 0, distanceClearStartedMs = 0;
uint8_t consecutiveNoEcho = 0;
int flameDigital = HIGH, flameAnalog = 1023;
bool flameRawCandidate = false, flameConfirmed = false;
bool flameCandidateTimerActive = false, flameClearTimerActive = false;
unsigned long flameCandidateStartedMs = 0, flameClearStartedMs = 0;

// ---------------- Alarm and button state ----------------
bool armed = true, alertSilenced = false;
uint8_t activeHazardMask = HAZARD_NONE, silencedHazardMask = HAZARD_NONE;
int lastRawButtonState = HIGH, stableButtonState = HIGH;
unsigned long lastButtonChangeMs = 0, buttonPressStartedMs = 0;
bool longPressHandled = false;

// ---------------- Buzzer state ----------------
bool buzzerToneOn = false, buzzerCycleActive = false;
unsigned long buzzerToneStartedMs = 0, lastBeepStartedMs = 0;

// ---------------- EEPROM event counter ----------------
const int EEPROM_ADDRESS = 0;
const uint16_t EEPROM_SIGNATURE = 0xF17E;
struct PersistentEventLog { uint16_t signature; uint32_t hazardStarts; };
PersistentEventLog eventLog;

// ---------------- Scheduler ----------------
unsigned long lastSensorReadMs = 0, lastDisplayUpdateMs = 0;
unsigned long lastSerialUpdateMs = 0;

void loadEventCounter() {
EEPROM.get(EEPROM_ADDRESS, eventLog);
if (eventLog.signature != EEPROM_SIGNATURE) {
eventLog.signature = EEPROM_SIGNATURE;
eventLog.hazardStarts = 0;
EEPROM.put(EEPROM_ADDRESS, eventLog);
}
}

void saveEventCounter() { EEPROM.put(EEPROM_ADDRESS, eventLog); }

void stopBuzzer() {
noTone(BUZZER_PIN);
buzzerToneOn = false;
buzzerCycleActive = false;
}

float readDistanceCm() {
digitalWrite(TRIG_PIN, LOW);
delayMicroseconds(2);
digitalWrite(TRIG_PIN, HIGH);
delayMicroseconds(10);
digitalWrite(TRIG_PIN, LOW);
unsigned long duration = pulseIn(ECHO_PIN, HIGH, ECHO_TIMEOUT_US);
if (duration == 0) return -1.0;
float cm = duration * 0.0343 / 2.0;
if (cm < 2.0 || cm > 400.0) return -1.0;
return cm;
}

void updateDistanceState(float rawDistance, unsigned long now) {
distanceCm = rawDistance;
if (rawDistance >= 2.0 && rawDistance <= 400.0) {
haveValidDistance = true;
lastValidDistanceMs = now;
consecutiveNoEcho = 0;
if (!distanceFilterReady) {
filteredDistanceCm = rawDistance;
distanceFilterReady = true;
} else {
filteredDistanceCm = 0.60 * filteredDistanceCm + 0.40 * rawDistance;
}
if (!distanceDanger && filteredDistanceCm <= DISTANCE_ALERT_CM) {
distanceDanger = true;
distanceClearTimerActive = false;
Serial.println(F("EVENT: ZONE ALERT STARTED"));
}
if (distanceDanger) {
if (filteredDistanceCm > DISTANCE_CLEAR_CM) {
if (!distanceClearTimerActive) {
distanceClearTimerActive = true;
distanceClearStartedMs = now;
} else if (now - distanceClearStartedMs >= DISTANCE_CLEAR_HOLD_MS) {
distanceDanger = false;
distanceClearTimerActive = false;
Serial.println(F("EVENT: ZONE ALERT CLEARED"));
}
} else distanceClearTimerActive = false;
}
} else {
distanceClearTimerActive = false;
if (consecutiveNoEcho < 255) consecutiveNoEcho++;
if (distanceDanger && haveValidDistance && now - lastValidDistanceMs >= NO_ECHO_CLEAR_MS) {
distanceDanger = false;
Serial.println(F("EVENT: DISTANCE ECHO LOST"));
}
}
}

void updateFlameState(unsigned long now) {
flameDigital = digitalRead(FLAME_DO_PIN);
flameAnalog = analogRead(FLAME_AO_PIN);
flameRawCandidate = (flameDigital == LOW) || (flameAnalog <= FLAME_AO_TRIGGER);
bool signalClear = (flameDigital == HIGH) && (flameAnalog >= FLAME_AO_CLEAR);
if (flameRawCandidate) {
flameClearTimerActive = false;
if (!flameCandidateTimerActive) {
flameCandidateTimerActive = true;
flameCandidateStartedMs = now;
}
if (!flameConfirmed && now - flameCandidateStartedMs >= FLAME_CONFIRM_MS) {
flameConfirmed = true;
Serial.println(F("EVENT: FLAME CONFIRMED"));
}
} else {
flameCandidateTimerActive = false;
if (flameConfirmed && signalClear) {
if (!flameClearTimerActive) {
flameClearTimerActive = true;
flameClearStartedMs = now;
} else if (now - flameClearStartedMs >= FLAME_CLEAR_HOLD_MS) {
flameConfirmed = false;
flameClearTimerActive = false;
Serial.println(F("EVENT: FLAME CONDITION CLEARED"));
}
} else flameClearTimerActive = false;
}
}

void updateHazardMask() {
uint8_t newMask = HAZARD_NONE;
if (distanceDanger) newMask |= HAZARD_ZONE;
if (flameConfirmed) newMask |= HAZARD_FLAME;
uint8_t newlyActive = newMask & (uint8_t)(~activeHazardMask);
if (newlyActive != HAZARD_NONE) {
eventLog.hazardStarts++;
saveEventCounter();
Serial.print(F("HAZARD START COUNT: "));
Serial.println(eventLog.hazardStarts);
}
activeHazardMask = newMask;
silencedHazardMask &= activeHazardMask;
}

uint8_t getUnacknowledgedMask() {
return activeHazardMask & (uint8_t)(~silencedHazardMask);
}

bool distanceReadingFresh() {
return distanceFilterReady && (millis() - lastValidDistanceMs < DISPLAY_ECHO_STALE_MS);
}

const char* getFlameStatusText() {
if (flameConfirmed) return "DETECTED";
if (flameRawCandidate) return "CHECK";
return "CLEAR";
}

const char* getAlertStatusText() {
if (!armed) return "DISARMED";
uint8_t pending = getUnacknowledgedMask();
if (pending == (HAZARD_FLAME | HAZARD_ZONE)) return "FLAME + ZONE";
if (pending & HAZARD_FLAME) return "FLAME ALERT";
if (pending & HAZARD_ZONE) return "ZONE ALERT";
if (activeHazardMask != HAZARD_NONE) return "SILENCED";
if (flameRawCandidate) return "FLAME CHECK";
if (!distanceReadingFresh()) return "ZONE UNKNOWN";
return "SAFE";
}

void toggleArmMode() {
armed = !armed;
silencedHazardMask = HAZARD_NONE;
stopBuzzer();
Serial.println(armed ? F("SYSTEM: ARMED") : F("SYSTEM: DISARMED"));
}

void handleShortPress() {
if (!armed) {
Serial.println(F("Short press ignored: DISARMED"));
return;
}
if (activeHazardMask != HAZARD_NONE) {
silencedHazardMask |= activeHazardMask;
stopBuzzer();
Serial.println(F("ALERT ACKNOWLEDGED - monitoring continues"));
} else Serial.println(F("No confirmed hazard to acknowledge"));
}

void handleButton(unsigned long now) {
int raw = digitalRead(BUTTON_PIN);
if (raw != lastRawButtonState) {
lastRawButtonState = raw;
lastButtonChangeMs = now;
}
if (now - lastButtonChangeMs >= BUTTON_DEBOUNCE_MS && raw != stableButtonState) {
stableButtonState = raw;
if (stableButtonState == LOW) {
buttonPressStartedMs = now;
longPressHandled = false;
} else if (!longPressHandled) {
unsigned long held = now - buttonPressStartedMs;
if (held >= LONG_PRESS_MS) toggleArmMode();
else handleShortPress();
}
}
if (stableButtonState == LOW && lastRawButtonState == LOW && !longPressHandled &&
now - buttonPressStartedMs >= LONG_PRESS_MS) {
toggleArmMode();
longPressHandled = true;
}
}

unsigned long calculateDistancePeriod() {
float d = filteredDistanceCm;
if (d < 2.0) d = 2.0;
if (d > DISTANCE_ALERT_CM) d = DISTANCE_ALERT_CM;
float urgency = (DISTANCE_ALERT_CM - d) / (DISTANCE_ALERT_CM - 2.0);
float period = DISTANCE_SLOW_PERIOD_MS - urgency * (DISTANCE_SLOW_PERIOD_MS - FASTEST_PERIOD_MS);
if (period < FASTEST_PERIOD_MS) period = FASTEST_PERIOD_MS;
return (unsigned long)(period + 0.5);
}

unsigned long calculateFlamePeriod() {
float response = (FLAME_AO_CLEAR_REFERENCE - (float)flameAnalog) /
(FLAME_AO_CLEAR_REFERENCE - FLAME_AO_STRONG_REFERENCE);
if (response < 0.0) response = 0.0;
if (response > 1.0) response = 1.0;
float period = FLAME_SLOW_PERIOD_MS - response * (FLAME_SLOW_PERIOD_MS - FASTEST_PERIOD_MS);
if (period < FASTEST_PERIOD_MS) period = FASTEST_PERIOD_MS;
return (unsigned long)(period + 0.5);
}

unsigned long calculateBeepPeriod(uint8_t pending) {
unsigned long period = 10000UL;
if (pending & HAZARD_ZONE) period = calculateDistancePeriod();
if (pending & HAZARD_FLAME) {
unsigned long flamePeriod = calculateFlamePeriod();
if (flamePeriod < period) period = flamePeriod;
}
return period;
}

void updateBuzzer(unsigned long now, uint8_t pending) {
if (!armed || pending == HAZARD_NONE) {
stopBuzzer();
return;
}
unsigned long period = calculateBeepPeriod(pending);
if (!buzzerCycleActive) {
tone(BUZZER_PIN, BUZZER_FREQUENCY_HZ);
buzzerToneOn = true;
buzzerCycleActive = true;
buzzerToneStartedMs = now;
lastBeepStartedMs = now;
return;
}
if (buzzerToneOn && now - buzzerToneStartedMs >= BUZZER_ON_MS) {
noTone(BUZZER_PIN);
buzzerToneOn = false;
}
if (!buzzerToneOn && now - lastBeepStartedMs >= period) {
tone(BUZZER_PIN, BUZZER_FREQUENCY_HZ);
buzzerToneOn = true;
buzzerToneStartedMs = now;
lastBeepStartedMs = now;
}
}

void updateOutputs(unsigned long now) {
uint8_t pending = getUnacknowledgedMask();
if (!armed) {
digitalWrite(GREEN_LED_PIN, LOW);
digitalWrite(RED_LED_PIN, LOW);
stopBuzzer();
return;
}
if (pending != HAZARD_NONE) {
digitalWrite(GREEN_LED_PIN, LOW);
digitalWrite(RED_LED_PIN, HIGH);
updateBuzzer(now, pending);
return;
}
if (activeHazardMask != HAZARD_NONE || flameRawCandidate) {
digitalWrite(GREEN_LED_PIN, LOW);
digitalWrite(RED_LED_PIN, LOW);
stopBuzzer();
return;
}
if (!distanceReadingFresh()) {
digitalWrite(GREEN_LED_PIN, LOW);
digitalWrite(RED_LED_PIN, LOW);
stopBuzzer();
return;
}
digitalWrite(GREEN_LED_PIN, HIGH);
digitalWrite(RED_LED_PIN, LOW);
stopBuzzer();
}

void updateOLED(unsigned long now) {
if (!oledReady) return;
display.clearDisplay();
display.setTextColor(SSD1306_WHITE);
display.setTextWrap(false);
display.setTextSize(1);
display.setCursor(0, 0);
display.print(F("LUMAGUARD"));
display.setCursor(78, 0);
display.print(armed ? F("ARMED") : F("DISARMED"));
display.drawFastHLine(0, 10, SCREEN_WIDTH, SSD1306_WHITE);
display.setCursor(0, 13);
display.print(F("FLAME: "));
display.print(getFlameStatusText());
display.setCursor(0, 23);
display.print(F("DISTANCE"));
display.setTextSize(2);
display.setCursor(0, 33);
bool validDisplay = distanceFilterReady && (now - lastValidDistanceMs < DISPLAY_ECHO_STALE_MS);
if (validDisplay) {
display.print(filteredDistanceCm, 1);
display.print(F(" cm"));
} else display.print(F("NO ECHO"));
display.setTextSize(1);
display.setCursor(0, 54);
display.print(F("ALERT: "));
display.print(getAlertStatusText());
display.display();
}

void printSerialStatus(unsigned long now) {
bool validDisplay = distanceFilterReady && (now - lastValidDistanceMs < DISPLAY_ECHO_STALE_MS);
Serial.print(F("Distance_cm="));
if (validDisplay) Serial.print(filteredDistanceCm, 1);
else Serial.print(F("NO_ECHO"));
Serial.print(F(" | Flame_DO=")); Serial.print(flameDigital);
Serial.print(F(" | Flame_AO=")); Serial.print(flameAnalog);
Serial.print(F(" | Flame=")); Serial.print(getFlameStatusText());
Serial.print(F(" | Mode=")); Serial.print(armed ? F("ARMED") : F("DISARMED"));
Serial.print(F(" | ActiveMask=")); Serial.print(activeHazardMask);
Serial.print(F(" | SilencedMask=")); Serial.print(silencedHazardMask);
Serial.print(F(" | Alert=")); Serial.print(getAlertStatusText());
Serial.print(F(" | HazardStarts=")); Serial.println(eventLog.hazardStarts);
}

void setup() {
Serial.begin(9600);
pinMode(TRIG_PIN, OUTPUT); pinMode(ECHO_PIN, INPUT);
pinMode(FLAME_DO_PIN, INPUT); pinMode(FLAME_AO_PIN, INPUT);
pinMode(GREEN_LED_PIN, OUTPUT); pinMode(RED_LED_PIN, OUTPUT);
pinMode(BUZZER_PIN, OUTPUT); pinMode(BUTTON_PIN, INPUT_PULLUP);
digitalWrite(TRIG_PIN, LOW);
digitalWrite(GREEN_LED_PIN, LOW); digitalWrite(RED_LED_PIN, LOW);
noTone(BUZZER_PIN);
loadEventCounter();
oledReady = display.begin(SSD1306_SWITCHCAPVCC, OLED_ADDRESS);
if (oledReady) {
display.clearDisplay(); display.setTextColor(SSD1306_WHITE);
display.setTextSize(2); display.setCursor(0, 12);
display.print(F("LUMAGUARD"));
display.setTextSize(1); display.setCursor(0, 40);
display.print(F("System starting...")); display.display();
} else Serial.println(F("OLED init failed - check A4/A5, power and 0x3C"));
unsigned long now = millis();
lastSensorReadMs = now - SENSOR_INTERVAL_MS;
lastDisplayUpdateMs = now; lastSerialUpdateMs = now;
lastButtonChangeMs = now;
Serial.println(F("LumaGuard started; DHT22 excluded"));
Serial.println(F("Short press: acknowledge; long press: ARM/DISARM"));
}

void loop() {
unsigned long now = millis();
handleButton(now);
if (now - lastSensorReadMs >= SENSOR_INTERVAL_MS) {
lastSensorReadMs = now;
updateDistanceState(readDistanceCm(), now);
updateFlameState(now);
updateHazardMask();
}
updateOutputs(millis());
now = millis();
if (now - lastDisplayUpdateMs >= DISPLAY_INTERVAL_MS) {
lastDisplayUpdateMs = now;
updateOLED(now);
}
if (now - lastSerialUpdateMs >= SERIAL_INTERVAL_MS) {
lastSerialUpdateMs = now;
printSerialStatus(now);
}
}
