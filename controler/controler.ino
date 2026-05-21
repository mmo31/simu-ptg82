/*
  Projet : Joystick USB avec Arduino Leonardo - Axes absolus (+ bouton)
  Carte  : Arduino Leonardo (HID joystick)

  --- Branchement ---

  Potentiomètre 10k (axe X) Profondeur, Embout jaune :
    - Extrémités -> 5V et GND  (vert et bleu)
    - Curseur    -> A0  Blanc
    - (Option) 0.1 µF entre curseur et GND pour filtrer

  Potentiomètre 10k (axe RX) Gauchissement, Embout orange :
    - Extrémités -> 5V et GND  (Marron et Mar ron/blanc)
    - Curseur    -> A1  Vert
    - (Option) 0.1 µF entre curseur et GND pour filtrer

  Encodeur incrémental #1 E38S6G5 (axe Y) :
    - Rouge  -> +5V
    - Noir   -> GND
    - Vert   -> Voie A -> D2
    - Blanc  -> Voie B -> D3
    - Blindage (si présent) -> GND (au plus près de l’Arduino)

  Encodeur incrémental #2 E38S6G5 (axe Z) :
    - Rouge  -> +5V
    - Noir   -> GND
    - Vert/Jaune -> Voie A -> D0
    - Blanc  -> Voie B -> D1
    - Blindage/Orange (si présent) -> GND

  Bouton (Joystick bouton #0) :
    - Un côté -> GND
    - Autre côté -> D4
    - Schéma : on utilise la PULL-UP interne (pas de résistance externe nécessaire)
        5V
         |
        [ Rpullup interne ]  (activée par INPUT_PULLUP)
         |
        D4 -----[ Bouton ]----- GND
      -> Lecture à 1 (HIGH) au repos, 0 (LOW) quand appuyé.

    - (Option anti-rebond hardware) : 100 nF entre D4 et GND + résistance série 100 Ω
      sinon on gère un léger anti-rebond logiciel.

  ⚠ Remarques importantes :
    - Certains E38S6G5 peuvent être "open-collector" (NPN). INPUT_PULLUP fournit la polarisation.
    - D0/D1 servent aussi à Serial1 : ne pas utiliser Serial1 dans ce sketch. Sinon, déplacer l’encodeur Z vers D7/D4 (par ex.)
      et adapter les pins.

  --- Comportement des axes ---
    - X (potentiomètre) : 0..1023 direct (absolu)
    - RX (potentiomètre) : 0..1023 direct (absolu)
    - Y & Z (encodeurs) : position absolue multi-tours (configurable via TURNS_FOR_RANGE_*)
*/

#include <Joystick.h>
#include <Encoder.h>

// ===================== Brochage =====================
const int POTX_PIN  = A0;  // Potentiomètre 10k -> axe X
const int POTRX_PIN = A1;  // Potentiomètre 10k -> axe RX

// Encodeur Y sur D2/D3 (interrupts)
const uint8_t ENC_Y_A = 2;
const uint8_t ENC_Y_B = 3;
Encoder encY(ENC_Y_A, ENC_Y_B);

// Encodeur Z sur D0/D1 (attention: Serial1)
const uint8_t ENC_Z_A = 0;
const uint8_t ENC_Z_B = 1;
Encoder encZ(ENC_Z_A, ENC_Z_B);

// Bouton (avec pull-up interne)
const uint8_t BTN_PIN = 4;       // D4
const uint8_t BTN_INDEX = 0;     // index du bouton HID (0 = premier)

// ===================== Joystick HID =================
Joystick_ Joystick(
  JOYSTICK_DEFAULT_REPORT_ID,
  JOYSTICK_TYPE_JOYSTICK,
  4,    // buttonCount (→ on a 1 bouton)
  0,    // hatSwitchCount
  true, // X
  true, // Y
  true, // Z
  true, // Rx
  false, false, // Ry, Rz
  false, false, false, false, false
);

// ===================== Paramètres ===================
// Plage HID
const int AXIS_MIN = 0;
const int AXIS_MAX = 1023;

// Potars
const int POT_DEADBAND = 2;

// Encodeurs E38S6G5 : souvent 200..600 PPR. Tu as mis 200 → je conserve.
const long PPR = 200;
const int  QUAD_MULT = 4;                 // 4 si comptage des 4 fronts
const long COUNTS_PER_TURN = PPR * QUAD_MULT;

// Sensibilité (nb de tours pour couvrir 0..1023)
const int TURNS_FOR_RANGE_Y = 1;
const int TURNS_FOR_RANGE_Z = 1;

// Anti-rebond logiciel bouton (ms)
const unsigned long BTN_DEBOUNCE_MS = 10;

// ===================== État =========================
int xVal  = 512;
int yVal  = 512;
int zVal  = 512;
int rxVal = 512;

int lastMappedY = -9999;
int lastMappedZ = -9999;
int lastRawPotX  = 0;
int lastRawPotRX = 0;

bool btnState = false;                // état logique envoyé au HID
bool btnLastStable = false;
bool btnLastRead = true;              // true = HIGH (pull-up) = relâché
unsigned long btnLastChangeMs = 0;

// ===================== Utilitaires ==================
long wrap_mod(long value, long mod) {
  long r = value % mod;
  return (r < 0) ? (r + mod) : r;
}

int mapCountToAxis(long count, long countsPerRange) {
  long idx = wrap_mod(count, countsPerRange);
  return map((int)idx, 0, (int)(countsPerRange - 1), AXIS_MIN, AXIS_MAX);
}

// ===================== Setup/Loop ===================
void setup() {
  // Entrées pullup pour A/B (utile si sorties open-collector)
  pinMode(ENC_Y_A, INPUT_PULLUP);
  pinMode(ENC_Y_B, INPUT_PULLUP);
  pinMode(ENC_Z_A, INPUT_PULLUP);
  pinMode(ENC_Z_B, INPUT_PULLUP);

  // Bouton en pull-up interne
  pinMode(BTN_PIN, INPUT_PULLUP);

  Joystick.setXAxisRange(AXIS_MIN, AXIS_MAX);
  Joystick.setYAxisRange(AXIS_MIN, AXIS_MAX);
  Joystick.setZAxisRange(AXIS_MIN, AXIS_MAX);
  Joystick.setRxAxisRange(AXIS_MIN, AXIS_MAX);
  Joystick.begin();

  // Init potars
  lastRawPotX  = analogRead(POTX_PIN);
  xVal         = lastRawPotX;
  Joystick.setXAxis(xVal);

  lastRawPotRX = analogRead(POTRX_PIN);
  rxVal        = lastRawPotRX;
  Joystick.setRxAxis(rxVal);   // ← correction ici

  // Position initiale Y/Z
  Joystick.setYAxis(yVal);
  Joystick.setZAxis(zVal);

  // Bouton initial (relâché)
  Joystick.setButton(BTN_INDEX, 0);
}

void loop() {
  // -------- Potentiomètre -> Axe X (absolu direct) --------
  int raw = analogRead(POTX_PIN);
  raw = (raw + lastRawPotX) / 2;  // lissage léger
  if (abs(raw - xVal) > POT_DEADBAND) {
    xVal = raw;
    Joystick.setXAxis(xVal);
  }
  lastRawPotX = raw;

  // -------- Potentiomètre -> Axe RX (absolu direct) -------
  int rawrx = analogRead(POTRX_PIN);
  rawrx = (rawrx + lastRawPotRX) / 2;
  if (abs(rawrx - rxVal) > POT_DEADBAND) {
    rxVal = rawrx;
    Joystick.setRxAxis(rxVal);
  }
  lastRawPotRX = rawrx;

  // -------- Encodeur Y -> Axe Y (absolu multi-tours) -------
  long countsPerRangeY = COUNTS_PER_TURN * (long)TURNS_FOR_RANGE_Y;
  long yCount = encY.read();
  int mappedY = mapCountToAxis(yCount, countsPerRangeY);
  if (mappedY != lastMappedY) {
    lastMappedY = mappedY;
    Joystick.setYAxis(mappedY);
  }

  // -------- Encodeur Z -> Axe Z (absolu multi-tours) -------
  long countsPerRangeZ = COUNTS_PER_TURN * (long)TURNS_FOR_RANGE_Z;
  long zCount = encZ.read();
  int mappedZ = mapCountToAxis(zCount, countsPerRangeZ);
  if (mappedZ != lastMappedZ) {
    lastMappedZ = mappedZ;
    Joystick.setZAxis(mappedZ);
  }

  // -------- Bouton (anti-rebond logiciel simple) -----------
  // Lecture brute (active LOW)
  bool btnRead = (digitalRead(BTN_PIN) == LOW);

  unsigned long now = millis();
  if (btnRead != btnLastRead) {
    btnLastRead = btnRead;
    btnLastChangeMs = now;  // on repart pour une fenêtre de stabilité
  }

  // Si l'état est stable depuis BTN_DEBOUNCE_MS, on valide
  if ((now - btnLastChangeMs) >= BTN_DEBOUNCE_MS && btnRead != btnLastStable) {
    btnLastStable = btnRead;
    btnState = btnRead; // true = appuyé

    // Mise à jour HID
    Joystick.setButton(BTN_INDEX, btnState ? 1 : 0);
  }

  delay(1); // souffle USB
}
