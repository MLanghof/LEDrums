
// The Processing side of LEDrums, for use/tested with Processing 3.x
//
// Requires the MidiBus library to be installed:
//   Sketch -> Import Library... -> Add Library... -> The MidiBus.

import themidibus.*;

import processing.serial.*;

// The serial port:
Serial myPort;

ArrayList<Instrument> instruments = new ArrayList();

void setup()
{
  // With the default renderer, opening a Serial connection causes a 200 ms delay every few frames.
  // Using e.g. P2D or P3D avoids this.
  size(300, 300, P2D);

  myPort = new Serial(this, "COM4", 115200);
  myPort.buffer(1);

  // For simplicity, we will perform one send over serial per rendered frame for now.
  // Use a high frame rate to achieve decent throughput.
  frameRate(100);

  // Uncomment to see the names of connected MIDI devices.
  //MidiBus.list();

  instruments.add(new MD90());
  instruments.add(new Launchkey());
}

// Print any bytes currently waiting in the serial port.
void cout()
{
  while (myPort.available() > 0) {
    int inByte = myPort.read();
    println("Read one byte:", inByte, "; ASCII = ", (char)inByte);
  }
}

// Very basic handshake function. Returns true only if the arduino sent the string PASS.
String pass = "";
boolean ensureHandshake()
{
  if (pass.indexOf("PASS") >= 0)
    return true;

  if (myPort.available() > 0) {
    int inByte = myPort.read();
    pass += (char)inByte;
    println("Read one handshake byte:", inByte, "; ASCII = ", (char)inByte);
  }
  return false;
}

void draw()
{
  // Without this delay, the arduino will sometimes get stuck during startup.
  // Not a great workaround, but avoids the hassle of dealing with it further.
  if (frameCount == 1)
    delay(4000);

  // Wait for Arduino to actually become ready.
  if (!ensureHandshake())
    return;

  background(0);
  
  cout();
}
