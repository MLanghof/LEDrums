
// The Arduino side of LEDrums.
//
// Requires the NeoPixelBus library to be installed.

#include <NeoPixelBrightnessBus.h>

const uint16_t PixelCount = 240;
const uint8_t PixelPin = 42;

NeoPixelBrightnessBus<NeoGrbwFeature, Neo800KbpsMethod> strip(PixelCount, PixelPin);

// (Shorter name for terser code.)
const uint16_t N = PixelCount;

RgbColor black(0);

NeoGamma<NeoGammaTableMethod> colorGamma;

void setup() {
  strip.Begin();

  // Safeguard against drawing too much power.
  // (The 240 LED RGBW strip can draw up to 20 A @ 5 V with all LEDs at full power,
  // whereas the power supply caps out at 10 A @ 5 V in my case).
  strip.SetBrightness(120);
  
  // Initialize serial connection with high-ish baud rate.
  Serial.begin(115200);
  while (!Serial) {
    ;  // wait for serial port to connect. Needed for native USB port only.
  }

  delay(500);
}

// Reimplementation of Stream::timedRead because it's private but needed. Yawn.
long _startMillis;
int timedRead()
{
  int c;
  _startMillis = millis();
  do {
    c = Serial.read();
    if (c >= 0) return c;
  } while(millis() - _startMillis < 1000);
  return -1;     // -1 indicates timeout
}

// Serial.readStringUntil(255) does not work. It's fixed in 1.x but still broken in 2.0.
// https://github.com/arduino/Arduino/pull/7053
String readStringUntil255()
{
  String ret;
  int c = timedRead();
  while (c >= 0 && c != 255)
  {
    ret += (char)c;
    c = timedRead();
  }
  return ret;
}

int lastAckedTxId = -1;
void handleSerialInput() {
  String s = readStringUntil255();
  if (s.length() != 5) {
    //Serial.print((char)s.length());
    return;
  }

  int txId = s[0];
  if (txId == lastAckedTxId)
    return;

  Serial.print((char)txId);
  lastAckedTxId = txId;

  int a = s[1];
  int b = s[2];
  int c = s[3];
  int d = s[4];

  // TODO: Use these values :)
}

bool loaded = false;

void loop() {
  if (!loaded)
  {
    // Small loading animation, also stalls for a bit.
    load();
    strip.ClearTo(black);
    strip.Show();
    Serial.print("PASS");
    Serial.flush();
    delay(200);
    loaded = true;
    return;
  }
  
  while (Serial.available() > 0) {
    handleSerialInput();
  }
}

// Gradually fills the strip with a desaturated rainbow.
static void load() {
  strip.ClearTo(black);
  for(uint16_t i = 0; i < N; i++) {
    uint16_t v = (i * 8) % N;
    float hue = float(v) / N;
    auto color = RgbColor(HsbColor(hue, 0.6f, 0.4f));
    RgbwColor colorG = colorGamma.Correct(RgbwColor(color));
    strip.SetPixelColor(i, colorG);
    if (i % 3 == 0)
      strip.Show();
  }
}