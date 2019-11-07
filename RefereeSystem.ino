/*
    This sketch sends a message to a TCP server

*/
#include <WiFi.h>
#include <WiFiMulti.h>
#include <Ticker.h>
#include <HardwareSerial.h>

WiFiMulti WiFiMulti;
Ticker sender;
HardwareSerial MySerial(2);

typedef enum
{
  NOCOLOR = 1,
  Red = 3,
  Blue = 5,
  Yellow = 7,
  Green = 9,
} Feedback;

typedef struct
{
  String transmitName;
  bool resetFlag;
  int remainTime;
  Feedback transmitColor;
} TransmitData_t;

typedef struct
{
  bool resetFlag;
  Feedback receiveColor;
  bool receive_isAvailable;
  bool receive_isStable;
} ReceiveData_t;

TransmitData_t transmitData;
volatile ReceiveData_t receiveData;

int32_t start_time;
bool start_trigger;

void sendMsg() {
  if (receiveData.receive_isAvailable && receiveData.receive_isStable)
  {
    MySerial.write((uint8_t)receiveData.receiveColor);
  }
  else
  {
    MySerial.write((uint8_t)NOCOLOR);
  }
}

void Transmit_data_init()
{
  transmitData.transmitName = "Blue";
  transmitData.resetFlag = false;
  transmitData.remainTime = -1;
  transmitData.transmitColor = NOCOLOR;
}

void Receive_Data_init()
{
  receiveData.resetFlag = false;
  receiveData.receiveColor = NOCOLOR;
  receiveData.receive_isAvailable = false;
  receiveData.receive_isStable = false;
}

void setup()
{
  Serial.begin(115200);
  MySerial.begin(115200, SERIAL_8E1);
  delay(10);

  Transmit_data_init();
  Receive_Data_init();
  sender.attach(0.05, sendMsg);
  start_trigger = false;
  // We start by connecting to a WiFi network
  WiFiMulti.addAP("enterprize2019", "enterprize2020");

  //Serial.println();
  //Serial.println();
  //Serial.print("Waiting for WiFi... ");

  while (WiFiMulti.run() != WL_CONNECTED)
  {
    //Serial.print("./");
    delay(500);
  }
  //Serial.println("");
  //Serial.println("WiFi connected");
  //Serial.println("IP address: ");
  //Serial.println(WiFi.localIP());

  delay(500);
}

static uint8_t line[4];

void loop()
{

  const uint16_t port = 8124;
  const char *host = "192.168.1.148";

  //Serial.print("Connecting to ");
  //Serial.println(host);

  // Use WiFiClient class to create TCP connections
  WiFiClient client;
  client.setTimeout(50);

  if (client.connect(host, port))
  {
    while (client.connected())
    {
      if (client.available())
      {
        if (start_trigger == true)
        {
          transmitData.remainTime = 300 - ((millis() - start_time) / 1000);
        }
        int s;
        while ((s = client.read(line, 1)) >= 0 && line[0] != '\n')
          ;
        if (s < 0) {
          //Serial.println("Failed reading buffer");
          continue;
        }
        int failed = client.read(line, 4);
        //Serial.println("Finished reading buffer");
        if (failed < 0)
          continue;
        switch (line[1])
        {
          case 'r':
            receiveData.receiveColor = Red;
            break;
          case 'b':
            receiveData.receiveColor = Blue;
            break;
          case 'y':
            receiveData.receiveColor = Yellow;
            break;
          case 'g':
            receiveData.receiveColor = Green;
            break;
        }
        receiveData.receive_isAvailable = line[2] == 'T';
        receiveData.receive_isStable = line[3] == 'T';
        
        transmitData.transmitName = line[0] == 'r' ? "Red" : "Blue";
        transmitData.transmitColor = receiveData.receiveColor;
        client.print("{");
        client.print("\"Name\":");
        client.print(transmitData.transmitName);
        client.print(",");
        client.print("\"Color\":");
        client.print(transmitData.transmitColor);
        client.print("}");
        client.print("\n");
      }
    }
    receiveData.receive_isAvailable = false;
    receiveData.receive_isStable = false;
    client.stop();
    //Serial.println("\n[Disconnected]");
  }

  delay(50);
}
