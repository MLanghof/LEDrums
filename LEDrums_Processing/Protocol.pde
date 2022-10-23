
// Reliable serial communication with the arduino is tricky, because the connection is
// asynchronous and the arduino "loses" incoming serial bytes while writing data to the
// LED strip (because it has interrupts disabled while doing so, for timing reasons).
// Therefore, data may have to be sent multiple times until we can be sure it has been received.
// The transmissions from arduino to host computer appears to be reliable (thankfully).
//
// Therefore, the protocol uses two additional bytes to be more fault tolerant:
// - A sync byte (value 255, reserved) is used to re-sync after bytes have been lost. The other
//   bytes in a packet must never take on this value.
// - Packets are re-sent multiple times from the host computer until ACKed by the arduino.
//   The ACK is a single byte containing the value of the current packet ID (as the Processing
//   side must be able to distinguish ACKs for the different in-flight packets).

// The value 255 is reserved for syncing the data stream.
byte escape(byte value)
{
  if (value == 255)
    return (byte)254;
  return value;
}

class DataPacket4
{
  DataPacket4(int a, int b, int c, int d)
  {
    this.a = escape((byte)a);
    this.b = escape((byte)b);
    this.c = escape((byte)c);
    this.d = escape((byte)d);
  }

  byte a, b, c, d;
}

// A class grouping data and functions related to the serial communication. 
class Mailman
{
  Mailman(Serial port)
  {
    this.myPort = port;
  }

  Serial myPort;

  int txId = 0;

  int totalSendAttempts = 0;
  int totalSendSuccesses = 0;

  private ArrayList<DataPacket4> dataToSend = new ArrayList();

  // Request the given four bytes to be transmitted to the arduino.
  void enqueue(int a, int b, int c, int d)
  {
    dataToSend.add(new DataPacket4(a, b, c, d));
  }

  // Performs one "roundtrip": Checks for an ACK and (if needed) sends another packet down the wire.
  void attemptOneDelivery()
  {
    // Check if we got an ACK from the arduino.
    int inByte = -1000;
    while (myPort.available() > 0) {
      inByte = myPort.read();
      println("Read byte:", inByte);
    }

    // If the arduino ACKed the last data packet, mark it as delivered.
    if (inByte == txId) {
      txId++;
      if (txId > 100)
        txId = 0;
      dataToSend.remove(0);
      totalSendSuccesses++;
    }

    // If there is more data to send, fire off the next un-ACKed packet.
    if (!dataToSend.isEmpty())
    {
      DataPacket4 dp = dataToSend.get(0);
      send(dp.a, dp.b, dp.c, dp.d);
    }
  }

  // Print some information regarding the reliability of the serial connection so far.
  void reportSerialStatistics()
  {
    float percent = 0;
    if (totalSendAttempts > 0)
      percent = float(totalSendSuccesses) / totalSendAttempts * 100;
    println(totalSendSuccesses, "of", totalSendAttempts, "send attempts succeeded ->", percent, "percent");
  }

  // Sends one packet of data down the wire.
  private void send(byte a, byte b, byte c, byte d)
  {
    // Four data bytes are transmitted with a preceding transaction id and a terminating sync byte.
    String s = "";
    s += (char)txId;
    s += (char)a;
    s += (char)b;
    s += (char)c;
    s += (char)d;
    s += (char)255;
    myPort.write(s);
    println("Sent data", a, b, c, d, "with txId", txId);

    totalSendAttempts++;
  }
}
