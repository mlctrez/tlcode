import gnu.io.CommPort;
import gnu.io.CommPortIdentifier;
import gnu.io.NoSuchPortException;
import gnu.io.PortInUseException;

import java.io.ByteArrayOutputStream;
import java.io.IOException;
import java.io.OutputStream;
import java.util.HashMap;
import java.util.Map;

public class TransLuxTest {

  private static Map<String, byte[]> charMap = new HashMap<String, byte[]>();

  static {
    charMap.put(" ", new byte[] { 0, 0, 0, 0, 0 });
    charMap.put("_", new byte[] { 64, 64, 64, 64, 64 });
    charMap.put("A", new byte[] { 124, 18, 17, 18, 124 });
    charMap.put("B", new byte[] { 65, 127, 73, 73, 54 });
    charMap.put("C", new byte[] { 62, 65, 65, 65, 34 });
    charMap.put("D", new byte[] { 65, 127, 65, 65, 62 });
    charMap.put("E", new byte[] { 127, 73, 73, 65, 65 });
    charMap.put("F", new byte[] { 127, 9, 9, 1, 1 });
    charMap.put("G", new byte[] { 62, 65, 73, 73, 58 });
    charMap.put("H", new byte[] { 127, 8, 8, 8, 127 });
    charMap.put("I", new byte[] { 0, 65, 127, 65, 0 });
    charMap.put("J", new byte[] { 32, 64, 65, 63, 1 });
    charMap.put("K", new byte[] { 127, 8, 20, 34, 65 });
    charMap.put("L", new byte[] { 127, 64, 64, 64, 64 });
    charMap.put("M", new byte[] { 127, 2, 12, 2, 127 });
    charMap.put("N", new byte[] { 127, 2, 4, 8, 127 });
    charMap.put("O", new byte[] { 62, 65, 65, 65, 62 });
    charMap.put("P", new byte[] { 127, 9, 9, 9, 6 });
    charMap.put("Q", new byte[] { 62, 65, 81, 33, 94 });
    charMap.put("R", new byte[] { 127, 9, 25, 41, 70 });
    charMap.put("S", new byte[] { 38, 73, 73, 73, 50 });
    charMap.put("T", new byte[] { 1, 1, 127, 1, 1 });
    charMap.put("U", new byte[] { 63, 64, 64, 64, 63 });
    charMap.put("V", new byte[] { 7, 24, 96, 24, 7 });
    charMap.put("W", new byte[] { 127, 32, 24, 32, 127 });
    charMap.put("X", new byte[] { 99, 20, 8, 20, 99 });
    charMap.put("Y", new byte[] { 3, 4, 120, 4, 3 });
    charMap.put("Z", new byte[] { 97, 81, 73, 69, 67 });
    charMap.put("[", new byte[] { 0, 127, 65, 65, 0 });
    charMap.put("]", new byte[] { 0, 65, 65, 127, 0 });
    charMap.put("*", new byte[] { 127, 127, 127, 127, 127 });
  }

  public static void main(String[] args) throws PortInUseException, NoSuchPortException, IOException,
      InterruptedException {

    CommPortIdentifier portIdentifier = CommPortIdentifier.getPortIdentifier("/dev/ttyUSB0");

    CommPort port = portIdentifier.open("Arduino", 2000);

    Thread.sleep(1500);

    System.out.println("after first sleep");

    OutputStream outputStream = port.getOutputStream();

    // disable display
    outputStream.write(new byte[] { (byte) 0x01, (byte) (322 / 256), (byte) (322 & 255), 1, 0 });
    outputStream.flush();
    Thread.sleep(40);

    String[] split = null;
    split = "ALL YOUR BASE ".split("");
    for (int j = 1; j < split.length; j++) {
      String string2 = split[j];
      writeCharacter(outputStream, string2, ((j + 15) * 10), true, false);
    }

    split = "ARE BELONG TO US".split("");
    for (int j = 1; j < split.length; j++) {
      String string2 = split[j];
      writeCharacter(outputStream, string2, ((j - 1) * 10), true, false);
    }

    // brightness
    outputStream.write(new byte[] { (byte) 0x01, (byte) (321 / 256), (byte) (321 & 255), 1, (byte) 50 });
    outputStream.flush();
    Thread.sleep(40);

    // enable display
    outputStream.write(new byte[] { (byte) 0x01, (byte) (322 / 256), (byte) (322 & 255), 1, 1 });
    outputStream.flush();
    Thread.sleep(40);

    port.close();
  }

  private static void writeCharacter(OutputStream outputStream, String string, int i, boolean red, boolean green)
      throws IOException, InterruptedException {
    byte[] data = charMap.get(string);
    if (data == null) {
      return;
    }

    ByteArrayOutputStream baos = new ByteArrayOutputStream();
    baos.write(new byte[] { (byte) 0x01, (byte) (i / 256), (byte) (i & 255), 10 });
    for (int j = 0; j < data.length; j++) {
      baos.write(red ? data[j] : 0);
      baos.write(green ? data[j] : 0);
    }
    outputStream.write(baos.toByteArray());
    outputStream.flush();
    Thread.sleep(40);

  }
}