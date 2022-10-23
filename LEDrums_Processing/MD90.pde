// Instrument for MD-90 drums.
class MD90 extends Instrument
{
  MD90()
  {
    super("e-drum", "edrum");
  }

  void noteOn(int channel, int pitch, int velocity)
  {
  }
  void noteOff(int channel, int pitch, int velocity)
  {
  }
  void controllerChange(int channel, int number, int value)
  {
  }

  void drawOn(PGraphics g)
  {
  }
}
