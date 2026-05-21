/*
  Version avec calibration persistante.

  Fonctionnement calibration :
  1) Maintenir le bouton (D4) appuyé pendant 5 secondes -> entrée en mode calibration.
  2) Bouger tous les axes sur toute leur course (min/max).
  3) Relâcher le bouton (armement), puis faire un appui court (appui + relâchement)
     -> sauvegarde des min/max en EEPROM et retour au mode normal.
*/

#include <Joystick.h>
#include <Encoder.h>
#include <EEPROM.h>

// ===================== Brochage =====================
const int POTX_PIN  = A0;
const int POTRX_PIN = A1;

const uint8_t ENC_Y_A = 2;
const uint8_t ENC_Y_B = 3;
Encoder encY(ENC_Y_A, ENC_Y_B);

const uint8_t ENC_Z_A = 0;
const uint8_t ENC_Z_B = 1;
Encoder encZ(ENC_Z_A, ENC_Z_B);

const uint8_t BTN_PIN = 4;
const uint8_t BTN_INDEX = 0;

// ===================== Joystick HID =================
Joystick_ Joystick(
  JOYSTICK_DEFAULT_REPORT_ID,
  JOYSTICK_TYPE_JOYSTICK,
  4,
  0,
  true, true, true, true,
  false, false,
  false, false, false, false, false
);

// ===================== Paramètres ===================
const int AXIS_MIN = 0;
const int AXIS_MAX = 1023;
const int AXIS_MID = (AXIS_MIN + AXIS_MAX) / 2;

const int POT_DEADBAND = 2;

const long PPR = 200;
const int  QUAD_MULT = 4;
const long COUNTS_PER_TURN = PPR * QUAD_MULT;
const int TURNS_FOR_RANGE_Y = 1;
const int TURNS_FOR_RANGE_Z = 1;

const unsigned long BTN_DEBOUNCE_MS = 10;
const unsigned long CALIB_HOLD_MS = 5000;

// ===================== EEPROM calibration ===========
const uint32_t CAL_MAGIC = 0x43414C31UL; // "CAL1"
const uint16_t CAL_VERSION = 1;

struct CalibrationData {
  uint32_t magic;
  uint16_t version;
  int potXMin;
  int potXMax;
  int potRXMin;
  int potRXMax;
  long encYMin;
  long encYMax;
  long encZMin;
  long encZMax;
};

CalibrationData cal;
bool calibrationLoaded = false;

// ===================== État =========================
int xVal  = AXIS_MID;
int yVal  = AXIS_MID;
int zVal  = AXIS_MID;
int rxVal = AXIS_MID;

int lastMappedY = -9999;
int lastMappedZ = -9999;
int lastRawPotX  = 0;
int lastRawPotRX = 0;

bool btnLastStable = false;
bool btnLastRead = false;
unsigned long btnLastChangeMs = 0;

unsigned long btnPressStartMs = 0;
bool longPressHandled = false;

enum RunMode {
  MODE_NORMAL,
  MODE_CALIBRATION
};

RunMode mode = MODE_NORMAL;

bool calibrationArmed = false;
bool saveClickInProgress = false;

// ===================== Utilitaires ==================
long wrap_mod(long value, long mod) {
  long r = value % mod;
  return (r < 0) ? (r + mod) : r;
}

int mapCountToAxis(long count, long countsPerRange) {
  long idx = wrap_mod(count, countsPerRange);
  return map((int)idx, 0, (int)(countsPerRange - 1), AXIS_MIN, AXIS_MAX);
}

bool isCalibrationValid(const CalibrationData &d) {
  if (d.magic != CAL_MAGIC || d.version != CAL_VERSION) return false;

  if ((d.potXMax - d.potXMin) < 20) return false;
  if ((d.potRXMax - d.potRXMin) < 20) return false;
  if ((d.encYMax - d.encYMin) < 10) return false;
  if ((d.encZMax - d.encZMin) < 10) return false;

  return true;
}

int mapWithCalibration(long value, long inMin, long inMax) {
  if (inMax <= inMin) return AXIS_MID;
  long clamped = constrain(value, inMin, inMax);
  return (int)map(clamped, inMin, inMax, AXIS_MIN, AXIS_MAX);
}

void loadCalibration() {
  EEPROM.get(0, cal);
  calibrationLoaded = isCalibrationValid(cal);
}

void saveCalibration() {
  cal.magic = CAL_MAGIC;
  cal.version = CAL_VERSION;
  EEPROM.put(0, cal);
  calibrationLoaded = true;
}

void enterCalibrationMode() {
  mode = MODE_CALIBRATION;
  calibrationArmed = false;
  saveClickInProgress = false;

  int pX = analogRead(POTX_PIN);
  int pRX = analogRead(POTRX_PIN);
  long eY = encY.read();
  long eZ = encZ.read();

  cal.potXMin = pX;
  cal.potXMax = pX;
  cal.potRXMin = pRX;
  cal.potRXMax = pRX;
  cal.encYMin = eY;
  cal.encYMax = eY;
  cal.encZMin = eZ;
  cal.encZMax = eZ;

  // On neutralise l'état bouton HID pendant calibration
  Joystick.setButton(BTN_INDEX, 0);
}

void updateCalibrationCapture() {
  int pX = analogRead(POTX_PIN);
  int pRX = analogRead(POTRX_PIN);
  long eY = encY.read();
  long eZ = encZ.read();

  if (pX < cal.potXMin) cal.potXMin = pX;
  if (pX > cal.potXMax) cal.potXMax = pX;

  if (pRX < cal.potRXMin) cal.potRXMin = pRX;
  if (pRX > cal.potRXMax) cal.potRXMax = pRX;

  if (eY < cal.encYMin) cal.encYMin = eY;
  if (eY > cal.encYMax) cal.encYMax = eY;

  if (eZ < cal.encZMin) cal.encZMin = eZ;
  if (eZ > cal.encZMax) cal.encZMax = eZ;
}

// ===================== Setup/Loop ===================
void setup() {
  pinMode(ENC_Y_A, INPUT_PULLUP);
  pinMode(ENC_Y_B, INPUT_PULLUP);
  pinMode(ENC_Z_A, INPUT_PULLUP);
  pinMode(ENC_Z_B, INPUT_PULLUP);

  pinMode(BTN_PIN, INPUT_PULLUP);

  Joystick.setXAxisRange(AXIS_MIN, AXIS_MAX);
  Joystick.setYAxisRange(AXIS_MIN, AXIS_MAX);
  Joystick.setZAxisRange(AXIS_MIN, AXIS_MAX);
  Joystick.setRxAxisRange(AXIS_MIN, AXIS_MAX);
  Joystick.begin();

  loadCalibration();

  lastRawPotX  = analogRead(POTX_PIN);
  lastRawPotRX = analogRead(POTRX_PIN);

  xVal = calibrationLoaded ? mapWithCalibration(lastRawPotX, cal.potXMin, cal.potXMax) : lastRawPotX;
  rxVal = calibrationLoaded ? mapWithCalibration(lastRawPotRX, cal.potRXMin, cal.potRXMax) : lastRawPotRX;

  Joystick.setXAxis(xVal);
  Joystick.setRxAxis(rxVal);
  Joystick.setYAxis(yVal);
  Joystick.setZAxis(zVal);
  Joystick.setButton(BTN_INDEX, 0);

  btnLastRead = (digitalRead(BTN_PIN) == LOW);
  btnLastStable = btnLastRead;
  btnLastChangeMs = millis();
}

void loop() {
  // -------- Gestion bouton + anti-rebond --------
  bool btnRead = (digitalRead(BTN_PIN) == LOW);
  unsigned long now = millis();

  if (btnRead != btnLastRead) {
    btnLastRead = btnRead;
    btnLastChangeMs = now;
  }

  bool btnPressedEvent = false;
  bool btnReleasedEvent = false;

  if ((now - btnLastChangeMs) >= BTN_DEBOUNCE_MS && btnRead != btnLastStable) {
    btnLastStable = btnRead;
    if (btnLastStable) btnPressedEvent = true;
    else btnReleasedEvent = true;
  }

  if (mode == MODE_NORMAL) {
    // Bouton exposé au HID uniquement en mode normal
    Joystick.setButton(BTN_INDEX, btnLastStable ? 1 : 0);

    // Appui long 5s pour entrer en calibration
    if (btnLastStable) {
      if (btnPressStartMs == 0) btnPressStartMs = now;
      if (!longPressHandled && (now - btnPressStartMs) >= CALIB_HOLD_MS) {
        longPressHandled = true;
        enterCalibrationMode();
      }
    } else {
      btnPressStartMs = 0;
      longPressHandled = false;
    }

    // -------- Potentiomètre -> Axe X --------
    int raw = analogRead(POTX_PIN);
    raw = (raw + lastRawPotX) / 2;

    int xOut = calibrationLoaded ? mapWithCalibration(raw, cal.potXMin, cal.potXMax) : raw;
    if (abs(xOut - xVal) > POT_DEADBAND) {
      xVal = xOut;
      Joystick.setXAxis(xVal);
    }
    lastRawPotX = raw;

    // -------- Potentiomètre -> Axe RX --------
    int rawrx = analogRead(POTRX_PIN);
    rawrx = (rawrx + lastRawPotRX) / 2;

    int rxOut = calibrationLoaded ? mapWithCalibration(rawrx, cal.potRXMin, cal.potRXMax) : rawrx;
    if (abs(rxOut - rxVal) > POT_DEADBAND) {
      rxVal = rxOut;
      Joystick.setRxAxis(rxVal);
    }
    lastRawPotRX = rawrx;

    // -------- Encodeur Y -> Axe Y --------
    int mappedY;
    if (calibrationLoaded) {
      mappedY = mapWithCalibration(encY.read(), cal.encYMin, cal.encYMax);
    } else {
      long countsPerRangeY = COUNTS_PER_TURN * (long)TURNS_FOR_RANGE_Y;
      mappedY = mapCountToAxis(encY.read(), countsPerRangeY);
    }

    if (mappedY != lastMappedY) {
      lastMappedY = mappedY;
      Joystick.setYAxis(mappedY);
    }

    // -------- Encodeur Z -> Axe Z --------
    int mappedZ;
    if (calibrationLoaded) {
      mappedZ = mapWithCalibration(encZ.read(), cal.encZMin, cal.encZMax);
    } else {
      long countsPerRangeZ = COUNTS_PER_TURN * (long)TURNS_FOR_RANGE_Z;
      mappedZ = mapCountToAxis(encZ.read(), countsPerRangeZ);
    }

    if (mappedZ != lastMappedZ) {
      lastMappedZ = mappedZ;
      Joystick.setZAxis(mappedZ);
    }
  } else {
    // MODE_CALIBRATION
    updateCalibrationCapture();

    // Il faut d'abord relâcher après l'appui long
    if (!calibrationArmed) {
      if (btnReleasedEvent) {
        calibrationArmed = true;
      }
    } else {
      // Puis un appui court (appui + relâchement) pour sauvegarder
      if (btnPressedEvent) {
        saveClickInProgress = true;
      }

      if (saveClickInProgress && btnReleasedEvent) {
        saveCalibration();
        mode = MODE_NORMAL;
        saveClickInProgress = false;
        btnPressStartMs = 0;
        longPressHandled = false;
      }
    }
  }

  delay(1);
}
