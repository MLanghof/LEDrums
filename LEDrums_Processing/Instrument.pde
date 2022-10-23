// Base class for MidiBus based instruments.
abstract class Instrument
{
  Instrument(String midiInputDeviceName, String busName)
  {
    myBus = new MidiBus(LEDrums_Processing.this, busName);
    if (!myBus.addInput(midiInputDeviceName))
      myBus = null;
    else
      println(midiInputDeviceName, "found!");
  }

  abstract void noteOn(int channel, int pitch, int velocity);
  abstract void noteOff(int channel, int pitch, int velocity);
  abstract void controllerChange(int channel, int number, int value);

  abstract void drawOn(PGraphics g);

  boolean hasBusName(String busName)
  {
    return myBus != null && myBus.getBusName() == busName;
  }

  protected MidiBus myBus;
}
