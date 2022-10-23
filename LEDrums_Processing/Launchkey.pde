// Instrument for Launchkey keyboard.  
class Launchkey extends Instrument
{
  Launchkey()
  {
    super("Launchkey MIDI", "launchkey");
  }

  void noteOn(int channel, int pitch, int velocity) {
    //println("NoteOn:", channel, pitch, velocity);
  }

  void noteOff(int channel, int pitch, int velocity) {
    //println("NoteOff:", channel, pitch, velocity);
  }

  void controllerChange(int channel, int number, int value) {
    //println("CC:", channel, number, value);
  }

  void drawOn(PGraphics g)
  {
  }
}
