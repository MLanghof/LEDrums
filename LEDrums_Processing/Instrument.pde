// Base class for MidiBus based instruments.
abstract class Instrument
{
  Instrument(String midiInputDeviceName, String busName)
  {
    myBus = new MidiBus(this, busName);
    if (!myBus.addInput(midiInputDeviceName))
      myBus = null;
    else
      println(midiInputDeviceName, "found!");
  }

  abstract void noteOn(int channel, int pitch, int velocity);
  abstract void noteOff(int channel, int pitch, int velocity);
  abstract void controllerChange(int channel, int number, int value);

  abstract void drawOn(PGraphics g);

  protected MidiBus myBus;
}
