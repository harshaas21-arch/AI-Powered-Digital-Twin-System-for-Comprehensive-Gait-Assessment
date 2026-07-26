// ============================================================
//  AI Gait Assessment — ESP32 Firmware
//  Reads 16 FSR sensors via CD74HC4067 MUX + IMU data,
//  packs into GaitPayload struct and notifies via BLE.
//
//  FLASH ONE ESP32 AS "LEFT" AND ONE AS "RIGHT"
//  Change the line below accordingly.
// ============================================================

#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <BLE2902.h>
#include <Wire.h>   // ← ADDED for I2C

// ── CONFIGURE THIS BEFORE FLASHING ──────────────────────────
#define SHOE_SIDE "LEFT"   // Change to "RIGHT" for the other foot
// ────────────────────────────────────────────────────────────

// BLE UUIDs — must match the app exactly
#define SERVICE_UUID      "4fafc201-1fb5-459e-8fcc-c5c9c331914b"
#define CHAR_NOTIFY_UUID  "beb5483e-36e1-4688-b7f5-ea07361b26a8"

// CD74HC4067 MUX control pins 
#define ADC_PIN  34
#define S0       25
#define S1       26
#define S2       27
#define S3       14

// ── LSM6DSO I2C config ───────────────────────────────────────
#define LSM6DSO_ADDR      0x6A   // SDO/SA0 → GND
#define LSM6DSO_WHO_AM_I  0x0F
#define LSM6DSO_CTRL1_XL  0x10   // Accel config register
#define LSM6DSO_CTRL2_G   0x11   // Gyro config register
#define LSM6DSO_OUTX_L_A  0x28   // Accel X low byte (start of 6-byte block)
#define LSM6DSO_OUTX_L_G  0x22   // Gyro  X low byte (start of 6-byte block)

bool imuReady = false;

// ── Write one byte to LSM6DSO register ──────────────────────
void imuWriteReg(uint8_t reg, uint8_t val) {
  Wire.beginTransmission(LSM6DSO_ADDR);
  Wire.write(reg);
  Wire.write(val);
  Wire.endTransmission();
}

// ── Read N bytes starting from reg into buf ──────────────────
void imuReadRegs(uint8_t reg, uint8_t* buf, uint8_t len) {
  Wire.beginTransmission(LSM6DSO_ADDR);
  Wire.write(reg);
  Wire.endTransmission(false);          // repeated start
  Wire.requestFrom(LSM6DSO_ADDR, len);
  for (uint8_t i = 0; i < len && Wire.available(); i++) {
    buf[i] = Wire.read();
  }
}

// ── Init LSM6DSO ─────────────────────────────────────────────
void initIMU() {
  Wire.begin(21, 22);   // SDA=GPIO21, SCL=GPIO22
  Wire.setClock(400000); // 400 kHz Fast Mode

  // Check WHO_AM_I — LSM6DSO returns 0x6C
  uint8_t whoami = 0;
  imuReadRegs(LSM6DSO_WHO_AM_I, &whoami, 1);
  if (whoami != 0x6C) {
    Serial.printf("LSM6DSO not found! WHO_AM_I=0x%02X (expected 0x6C)\n", whoami);
    imuReady = false;
    return;
  }
  Serial.println("LSM6DSO found ✓");

  // CTRL1_XL: ODR=104Hz, ±4g, LPF enabled
  // Bits [7:4]=0100 (104Hz), [3:2]=10 (±4g), [1:0]=00
  imuWriteReg(LSM6DSO_CTRL1_XL, 0b01001000);

  // CTRL2_G:  ODR=104Hz, ±500dps
  // Bits [7:4]=0100 (104Hz), [3:2]=10 (500dps), [1:0]=00
  imuWriteReg(LSM6DSO_CTRL2_G, 0b01001000);

  imuReady = true;
}

// ── GaitPayload struct — DO NOT change, must match app ──────
struct __attribute__((packed)) GaitPayload {
  uint16_t sensors[16];  // 32 bytes — FSR pressure readings
  int16_t  accel[3];     //  6 bytes — x, y, z accelerometer
  float    pitch;        //  4 bytes
  float    roll;         //  4 bytes
  uint16_t steps;        //  2 bytes
  uint16_t cadence;      //  2 bytes
  uint16_t fsr_raw;      //  2 bytes — raw reading from sensor 0
  uint8_t  activity_id;  //  1 byte  — 0:stationary 1:walking 2:running
  uint8_t  battery;      //  1 byte
  uint8_t  padding[10];  // 10 bytes — pad to 64 bytes total
};                       // Total: 64 bytes ✓

GaitPayload payload;

// ── Step / cadence tracking ──────────────────────────────────
uint16_t stepCount        = 0;
uint32_t lastCadenceTime  = 0;
uint16_t lastStepForCad   = 0;
uint16_t currentCadence   = 0;
float    prevAccelMag     = 0;
bool     stepPending      = false;

// ── BLE state ────────────────────────────────────────────────
BLEServer*         pServer               = NULL;
BLECharacteristic* pNotifyCharacteristic = NULL;
bool deviceConnected    = false;
bool oldDeviceConnected = false;

class MyServerCallbacks : public BLEServerCallbacks {
  void onConnect(BLEServer* pServer) {
    deviceConnected = true;
    BLEDevice::setMTU(512);
    Serial.println("Client connected");
  }
  void onDisconnect(BLEServer* pServer) {
    deviceConnected = false;
    Serial.println("Client disconnected");
  }
};

// ── Select MUX channel ────────────────────────────────────────
void selectChannel(int ch) {
  digitalWrite(S0,  ch       & 1);
  digitalWrite(S1, (ch >> 1) & 1);
  digitalWrite(S2, (ch >> 2) & 1);
  digitalWrite(S3, (ch >> 3) & 1);
}

// ── Read all 16 FSR sensors through MUX ─────────────────────
void readFSRSensors() {
  for (int ch = 0; ch < 16; ch++) {
    selectChannel(ch);
    delayMicroseconds(200);
    analogRead(ADC_PIN);
    int raw = analogRead(ADC_PIN);
    payload.sensors[ch] = (uint16_t)(raw >> 2);
  }
  payload.fsr_raw = payload.sensors[0];
}

// ── Simple peak-detection step counter ───────────────────────
void updateStepsAndCadence() {
  uint32_t totalLoad = 0;
  for (int i = 0; i < 16; i++) totalLoad += payload.sensors[i];
  float loadNorm = totalLoad / 16.0f;

  bool footDown = (loadNorm > 150);
  if (footDown && !stepPending) {
    stepCount++;
    stepPending = true;
  }
  if (!footDown) stepPending = false;
  payload.steps = stepCount;

  uint32_t now = millis();
  if (now - lastCadenceTime >= 6000) {
    uint16_t stepsInWindow = stepCount - lastStepForCad;
    currentCadence = (uint16_t)((stepsInWindow * 10000UL) / 6000);
    lastStepForCad = stepCount;
    lastCadenceTime = now;
  }
  payload.cadence = currentCadence;
}

// ── Read real IMU data from LSM6DSO over I2C ─────────────────
void readIMU() {
  if (!imuReady) {
    // Fallback: zeros — app will see flat/no motion
    payload.accel[0] = 0;
    payload.accel[1] = 0;
    payload.accel[2] = 0;
    payload.pitch    = 0.0f;
    payload.roll     = 0.0f;
    return;
  }

  // ── Read 6 bytes of accelerometer (X_L, X_H, Y_L, Y_H, Z_L, Z_H)
  uint8_t buf[6];
  imuReadRegs(LSM6DSO_OUTX_L_A, buf, 6);

  int16_t rawAx = (int16_t)((buf[1] << 8) | buf[0]);
  int16_t rawAy = (int16_t)((buf[3] << 8) | buf[2]);
  int16_t rawAz = (int16_t)((buf[5] << 8) | buf[4]);

  // ±4g → sensitivity = 0.122 mg/LSB
  // Store as milli-g (payload.accel is int16_t, matches original usage)
  payload.accel[0] = (int16_t)(rawAx * 0.122f);
  payload.accel[1] = (int16_t)(rawAy * 0.122f);
  payload.accel[2] = (int16_t)(rawAz * 0.122f);

  // ── Compute pitch and roll from accelerometer
  //    Standard tilt equations, output in degrees
  float ax = payload.accel[0];
  float ay = payload.accel[1];
  float az = payload.accel[2];

  payload.pitch = atan2f(-ax, sqrtf(ay * ay + az * az)) * (180.0f / M_PI);
  payload.roll  = atan2f( ay, az)                        * (180.0f / M_PI);
}

// ── Activity recognition from cadence ────────────────────────
void updateActivity() {
  if (stepCount <= 5) {
    payload.activity_id = 0;
  } else if (payload.cadence > 140) {
    payload.activity_id = 2;
  } else if (payload.cadence > 40) {
    payload.activity_id = 1;
  } else {
    payload.activity_id = 0;
  }
}

// ── Battery (fixed 85% — replace with ADC read if wired) ─────
void updateBattery() {
  payload.battery = 85;
}

// ── Setup ────────────────────────────────────────────────────
void setup() {
  Serial.begin(115200);

  pinMode(S0, OUTPUT);
  pinMode(S1, OUTPUT);
  pinMode(S2, OUTPUT);
  pinMode(S3, OUTPUT);

  analogReadResolution(12);
  analogSetAttenuation(ADC_11db);

  memset(&payload, 0, sizeof(payload));

  // ── Init IMU over I2C ─────────────────────────────────────
  initIMU();   // ← ADDED

  // BLE init
  String deviceName = String("Gait ") + SHOE_SIDE;
  BLEDevice::init(deviceName.c_str());

  pServer = BLEDevice::createServer();
  pServer->setCallbacks(new MyServerCallbacks());

  BLEService* pService = pServer->createService(SERVICE_UUID);
  pNotifyCharacteristic = pService->createCharacteristic(
    CHAR_NOTIFY_UUID,
    BLECharacteristic::PROPERTY_NOTIFY
  );
  pNotifyCharacteristic->addDescriptor(new BLE2902());

  pService->start();

  BLEAdvertising* pAdvertising = BLEDevice::getAdvertising();
  pAdvertising->addServiceUUID(SERVICE_UUID);
  pAdvertising->setScanResponse(true);
  BLEDevice::startAdvertising();

  Serial.println("BLE advertising as: " + deviceName);
  Serial.println("Payload size: " + String(sizeof(payload)) + " bytes");
}

// ── Main loop ────────────────────────────────────────────────
void loop() {
  if (deviceConnected) {
    readFSRSensors();
    readIMU();
    updateStepsAndCadence();
    updateActivity();
    updateBattery();

    pNotifyCharacteristic->setValue((uint8_t*)&payload, sizeof(payload));
    pNotifyCharacteristic->notify();

    static int dbgCount = 0;
    if (++dbgCount >= 20) {
      dbgCount = 0;
      Serial.printf(
        "[%s] Steps:%d Cad:%d Act:%d S0:%d S1:%d S8:%d Bat:%d%% "
        "Ax:%d Ay:%d Az:%d Pitch:%.1f Roll:%.1f\n",
        SHOE_SIDE, payload.steps, payload.cadence,
        payload.activity_id, payload.sensors[0],
        payload.sensors[1], payload.sensors[8], payload.battery,
        payload.accel[0], payload.accel[1], payload.accel[2],
        payload.pitch, payload.roll
      );
    }

    delay(50); // 20 Hz
  }

  if (!deviceConnected && oldDeviceConnected) {
    delay(500);
    pServer->startAdvertising();
    Serial.println("Restarting advertising...");
    oldDeviceConnected = deviceConnected;
  }
  if (deviceConnected && !oldDeviceConnected) {
    oldDeviceConnected = deviceConnected;
  }
}
