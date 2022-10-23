// Instrument for MD-90 drums.
class MD90 extends Instrument
{
  MD90()
  {
    super("e-drum", "edrum");
  }

  void noteOn(int channel, int pitch, int velocity)
  {
    mailman.enqueue(channel, pitch, velocity, '9');
  }
  void noteOff(int channel, int pitch, int velocity)
  {
    // No point in sending note off events - all drum hits are instantaneous.
  }

  void controllerChange(int channel, int number, int value)
  {
    // Don't care about controller changes.
  }

  void drawOn(PGraphics g)
  {
  }
}
