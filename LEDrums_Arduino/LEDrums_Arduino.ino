
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

RgbwColor baseColor(120, 90, 60, 0);

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

  if ((char)d == '9')
    dispatchMD90((uint8_t)a, (uint8_t)b, (uint8_t)c);
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

  composeStrip();
  advanceTime();

  if (strip.CanShow())
    strip.Show();
}

void dispatchMD90(uint8_t channel, uint8_t pitch, uint8_t velocity)
{
  if (channel != 9)
    return;

  switch (pitch)
  {
    case 36: return kick();      // (foot pedal not velocity sensitive)
    case 44: return hiHatFoot(); // (foot pedal not velocity sensitive)
    case 42: return hiHat(false, velocity);
    case 46: return hiHat(true, velocity);
    case 38: return snare(velocity);
    case 48: return tom(0, velocity);
    case 45: return tom(1, velocity);
    case 43: return tom(2, velocity);
    case 49: return crash(velocity);
    case 51: return ride(velocity);
  }
}

uint32_t framesSinceKick = 1000;
static void kick()
{
  framesSinceKick = 0;
}

int hiHatOpenness = 0;
unsigned hiHatIntensity = 0;
int hiHatCounter = 0;
static void hiHatFoot()
{
  hiHatOpenness = 2;
  hiHatIntensity = 100;
  hiHatCounter++;
}

static void hiHat(bool closed, uint8_t velocity)
{
  hiHatOpenness = closed ? 1 : 3;
  hiHatIntensity = velocity;
  hiHatCounter++;
}

int snareIntensity = 0;
static void snare(uint8_t velocity)
{
  snareIntensity = velocity;
}

int tomIntensity[3] = {};
static void tom(int num, uint8_t velocity) // 0 = hi, 1 = mid, 2 = low
{
  tomIntensity[num] = velocity;
}

unsigned crashIntensity = 0;
static void crash(uint8_t velocity)
{
  crashIntensity = velocity;
}

int rideCount = 0;
int rideIntensity = 0;
static void ride(uint8_t velocity)
{
  rideIntensity = velocity;
  rideCount++;
}

static void advanceTime()
{
  // (This could instead be done by storing "frame number when event happened". Who cares.)
  framesSinceKick++;

  hiHatIntensity -= (hiHatIntensity >> hiHatOpenness);
  if (hiHatIntensity > 0)
    hiHatIntensity--;

  snareIntensity /= 2;

  for (int& ti : tomIntensity)
    ti = max(0, ti * 2 / 3 - 1);

  crashIntensity -= (crashIntensity >> 4);
  if (crashIntensity > 0)
    crashIntensity--;

  rideIntensity = max(0, rideIntensity * 15 / 16 - 1);
}

static int randFromCounter(int counter)
{
  unsigned i = (unsigned)counter;
  return (int)(i * i * (i + 100));
}

static void composeStrip()
{
  // All intensity values are in fixed point with unit 256.

  int kickIntensity = 3 * 128 / (framesSinceKick + 3);
  int kickDistFromCenter = min(1000, 10 * framesSinceKick) + 2;
  int kickWidth = 80;

  int hiHatLeftRight = hiHatCounter & 1;
  int hiHatLocation = (randFromCounter(hiHatCounter) % (N / 2)) + ((N / 2) * hiHatLeftRight);
  int hiHatWidth = 10 * hiHatOpenness + 1;

  int tomWidth[3] = { 20, 30, 40 };

  int maxCrashParticlesAsfractionOfN = 64;

  int rideLocation = ((rideCount * 24) % N) + (randFromCounter(rideCount) % 8);
  int rideWidth = rideIntensity / 20 + 10;

  for (int i = 0; i < N; ++i)
  {
    int distFromKick = abs(abs(i - (int)N / 2) - kickDistFromCenter);
    int fromKick = max(0, kickWidth - distFromKick) * kickIntensity / kickWidth;

    int distFromHiHat = abs(i - hiHatLocation);
    int fromHiHat = max(0, hiHatWidth - distFromHiHat) * hiHatIntensity / hiHatWidth;

    int fromSnare = snareIntensity / 2; //?

    int fromTom[3];
    for (int tom = 0; tom < 3; ++tom) {
      int tomLocation = (N / 5) * (tom + 1);
      int distFromTom = abs(i - tomLocation);
      fromTom[tom] = max(0, tomWidth[tom] - distFromTom) * tomIntensity[tom] / tomWidth[tom];
    }

    // This doesn't work how I think it should (the > should be <... ?!?), but it looks cool.
    int isCrash = (randFromCounter(i * N * crashIntensity) % (128 * maxCrashParticlesAsfractionOfN)) > (int)crashIntensity;
    int fromCrash = isCrash ? (int)crashIntensity : 0;

    int distFromRide = abs(i - rideLocation);
    int fromRide = max(0, rideWidth - distFromRide) * rideIntensity / rideWidth;

    int total = fromKick + fromHiHat + fromSnare + fromTom[0] + fromTom[1] + fromTom[2] + fromCrash + fromRide;
    total *= 2; // MIDI velocities tend to only go to 128.
    if (total > 255)
      total = 255;

    RgbwColor result(baseColor.R, baseColor.G, baseColor.B, total);
    strip.SetPixelColor(i, colorGamma.Correct(result));
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