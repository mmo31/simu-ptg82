/*
  Projet : Joystick USB avec Arduino Leonardo - Axes absolus
  Carte  : Arduino Leonardo (HID joystick)

  --- Branchement ---

  Potentiomètre 10k (axe X) Profondeur, Embout jaune :
    - Extrémités -> 5V et GND  (vert et bleu)
    - Curseur    -> A0  Blanc
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
    - Vert/jaune   -> Voie A -> D0
    - Blanc  -> Voie B -> D1
    - Blindage/Orange
     (si présent) -> GND

  ⚠ Remarques importantes :
    - Beaucoup d’E38S6G5 sortent des signaux 5V TTL classiques ; certains modèles sont "open-collector" (NPN).
      Si c’est ton cas, l’INPUT_PULLUP activé ci-dessous fournit une polarisation interne.
    - D0/D1 servent aussi à Serial1 sur Leonardo : n’utilise pas Serial1 dans ce sketch. Sinon, déplace l’encodeur Z
      vers D7 et D4 (par exemple) et adapte les pins dans le code.

  --- Comportement des axes ---
    - X (potentiomètre) : 0..1023 direct (absolu)
    - Y & Z (encodeurs) : position absolue sur plusieurs tours (configurable)
      → Par défaut : 4 tours = course complète 0..1023
      → Modifie TURNS_FOR_RANGE_Y / TURNS_FOR_RANGE_Z pour régler la "sensibilité" (plus de tours = plus fin)
*/

#include <Joystick.h>
#include <Encoder.h>

// ===================== Brochage =====================
const int POT_PIN = A0;        // Potentiomètre 10k -> axe X

// Encodeur Y sur D2/D3 (interrupts)
const uint8_t ENC_Y_A = 2;
const uint8_t ENC_Y_B = 3;
Encoder encY(ENC_Y_A, ENC_Y_B);

// Encodeur Z sur D0/D1 (attention: Serial1)
const uint8_t ENC_Z_A = 0;
const uint8_t ENC_Z_B = 1;
Encoder encZ(ENC_Z_A, ENC_Z_B);

// ===================== Joystick HID =================
Joystick_ Joystick(
  JOYSTICK_DEFAULT_REPORT_ID,
  JOYSTICK_TYPE_JOYSTICK,
  0,    // buttonCount
  0,    // hatSwitchCount
  true, // X
  true, // Y
  true, // Z
  false, false, false, // Rx, Ry, Rz
  false, false, false, false, false
);

// ===================== Paramètres ===================
// Plage HID
const int AXIS_MIN = 0;
const int AXIS_MAX = 1023;

// Potar
const int POT_DEADBAND = 2;

// Encodeurs E38S6G5 : souvent 600 PPR (impulsions par tour).
// En quadrature, on compte 4 fois plus d'événements si la lib capte les 4 fronts.
const long PPR = 200;              // Ajuste si ton modèle diffère
const int QUAD_MULT = 4;           // 4 si comptage des 4 fronts, 1 si comptage 1 impulsion
const long COUNTS_PER_TURN = PPR * QUAD_MULT;

// Choisis combien de tours couvrent la course 0..1023 (sensibilité)
const int TURNS_FOR_RANGE_Y = 4;   // 4 tours -> toute la plage Y
const int TURNS_FOR_RANGE_Z = 4;   // 4 tours -> toute la plage Z

// ===================== État =========================
int xVal = 512;
int yVal = 512;
int zVal = 512;

// Mémoire pour limiter les envois USB
int lastMappedY = -9999;
int lastMappedZ = -9999;
int lastRawPot  = 0;

// ===================== Utilitaires ==================
long wrap_mod(long value, long mod) {
  // modulo positif même si value est négatif
  long r = value % mod;
  return (r < 0) ? (r + mod) : r;
}

int mapCountToAxis(long count, long countsPerRange) {
  // Convertit un comptage "continu" en position 0..1023 sur une fenêtre circulaire
  long idx = wrap_mod(count, countsPerRange);
  // map() standard attend des int, on évite les débordements en castant prudemment
  return map((int)idx, 0, (int)(countsPerRange - 1), AXIS_MIN, AXIS_MAX);
}

// ===================== Setup/Loop ===================
void setup() {
  // Entrées pullup pour A/B (utile si sorties open-collector)
  pinMode(ENC_Y_A, INPUT_PULLUP);
  pinMode(ENC_Y_B, INPUT_PULLUP);
  pinMode(ENC_Z_A, INPUT_PULLUP);
  pinMode(ENC_Z_B, INPUT_PULLUP);

  Joystick.setXAxisRange(AXIS_MIN, AXIS_MAX);
  Joystick.setYAxisRange(AXIS_MIN, AXIS_MAX);
  Joystick.setZAxisRange(AXIS_MIN, AXIS_MAX);
  Joystick.begin();

  // Init potar
  lastRawPot = analogRead(POT_PIN);
  xVal = lastRawPot;
  Joystick.setXAxis(xVal);

  // Position initiale Y/Z = 0 (index arbitraire au démarrage)
  Joystick.setYAxis(yVal);
  Joystick.setZAxis(zVal);
}

void loop() {
  // -------- Potentiomètre -> Axe X (absolu direct) --------
  int raw = analogRead(POT_PIN); // 0..1023
  raw = (raw + lastRawPot) / 2;  // lissage léger
  if (abs(raw - xVal) > POT_DEADBAND) {
    xVal = raw;
    Joystick.setXAxis(xVal);
  }
  lastRawPot = raw;

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

  delay(1); // souffle USB
}
