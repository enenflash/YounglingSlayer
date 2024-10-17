SoftwareSerial bluetooth(0, 1); // RX, TX

/* :::::::: BLUETOOTH :::::::: */
class Bluetooth {
  public:
    void Send_Data(int x, int y, int direction) {
      String full_data = String(x) + "," + String(y) + "," + String(direction) + ";";
      bluetooth.println(full_data);
    }

    String Get_Data() {
      if (bluetooth.available()) {
        String data = bluetooth.readStringUntil(';');
        return data;
      }

      else {
        return "NONE";
      }
    }
};