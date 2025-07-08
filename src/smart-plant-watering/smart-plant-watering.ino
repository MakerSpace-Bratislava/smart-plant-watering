// --- Čísla pinov ---
const int BUTTON_PIN1 = 2; // Tlačidlo na polievanie rastliny
const int BUTTON_PIN2 = 3; // Tlačidlo na polievanie rastliny

const int PUMP_PIN = 13;   // Čerpadlo na polievanie rastliny
const int SENSOR_PIN = A0; // Senzor vlhkosti pôdy
const int KNOB_PIN = A1;   // Otočný gombík (potenciometer) na nastavenie suchosti

// --- Nastavenia časov ---
const unsigned long CHECK_INTERVAL = 60 * 1000UL; // Kontrola každých 60 sekúnd (1 minúta)
const unsigned long PUMP_TIME = 4 * 1000UL;       // Čerpadlo beží 4 sekundy

// --- Premenenné ---
unsigned long lastCheckTime = 0; // Pamätáme si, kedy sme naposledy kontrolovali pôdu
bool pumpIsOn = false;           // Je čerpadlo práve zapnuté?

void setup() {
  // --- Inicializácia pinov ---
  pinMode(BUTTON_PIN1, INPUT_PULLUP); // Tlačidlo je pripojené s pull-up rezistorom
  pinMode(BUTTON_PIN2, OUTPUT);
  pinMode(PUMP_PIN, OUTPUT);         // Čerpadlo je výstup

  digitalWrite(BUTTON_PIN2, LOW);
  turnPumpOff(); // Istota, že čerpadlo je vypnuté

  Serial.begin(9600); // Spustíme komunikáciu s počítačom
}

void loop() {
  unsigned long now = millis(); // Koľko času prešlo od štartu Arduina?

  // --- Kontrola vlhkosti pôdy každú minútu --- 
  if (now - lastCheckTime >= CHECK_INTERVAL) {
    lastCheckTime = now; // Zapamätáme si, kedy sme kontrolovali naposledy
    checkMoisture();
  }

  // Polievanie tlacidlom
  if (digitalRead(BUTTON_PIN1) == LOW && !pumpIsOn) {
    turnPumpOn();
  } else if (digitalRead(BUTTON_PIN1) == HIGH && pumpIsOn) {
    turnPumpOff();
  }
}

// Táto funkcia skontroluje, či rastlina potrebuje vodu a ak áno, poleje ju
void checkMoisture() {
  int sensorValue = analogRead(SENSOR_PIN);                 // Prečítame hodnotu zo senzora pôdy
  int measuredPercent = map(sensorValue, 590, 230, 0, 100); // Namapujeme hodnotu zo senzora na 0-100%
  int knobValue = analogRead(KNOB_PIN);                     // Prečítame hodnotu z gombíka (potenciometra)
  int tresholdPercent = map(knobValue, 0, 1023, 0, 100);    // Namapujeme hodnotu z potenciometru 0-1024 na 0-100%

  // Odošleme namerané údaje na počítač
  Serial.print("Aktuálna vlhkosť pôdy: ");
  Serial.print(measuredPercent);
  Serial.print("%, Polievaj keď je menej ako: ");
  Serial.print(tresholdPercent);
  Serial.println("%");

  // Kontrola
  if (measuredPercent < tresholdPercent) {
    turnPumpOn();      // Zapneme čerpadlo
    delay(PUMP_TIME);  // Necháme čerpadlo zapnuté nastavený čas
    turnPumpOff();     // Vypneme čerpadlo
  }
}

void turnPumpOn() {
  pumpIsOn = true;
  digitalWrite(PUMP_PIN, HIGH);
}

void turnPumpOff() {
  pumpIsOn = false;
  digitalWrite(PUMP_PIN, LOW);
}
