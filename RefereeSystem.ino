/*
 *  This sketch sends a message to a TCP server
 *
 */
#include <WiFi.h>
#include <WiFiMulti.h>
#include "ArduinoJson.h"

WiFiMulti WiFiMulti;

typedef enum
{
  NOCOLOR = 1,
  Red = 3,
  Blue = 5,
  Yellow = 7,
  Green = 9,
} Color_e;

typedef struct
{
  String transmitName;
  bool resetFlag;
  int remainTime;
  Color_e transmitColor;
} TransmitData_t;

typedef struct
{
  String receiveName;
  bool resetFlag;
  Color_e receiveColor;
  bool receive_isAvailable;
  bool receive_isStable;
} ReceiveData_t;

TransmitData_t transmitData;
ReceiveData_t receiveData;

int32_t start_time;
bool start_trigger;

void Transmit_data_init()
{
  transmitData.transmitName = "Blue";
  transmitData.resetFlag = false;
  transmitData.remainTime = -1;
  transmitData.transmitColor = NOCOLOR;
}

void Receive_Data_init()
{
  receiveData.receiveName = "Blue";
  receiveData.resetFlag = false;
  receiveData.receiveColor = NOCOLOR;
  receiveData.receive_isAvailable = false;
  receiveData.receive_isStable = false;
}

void setup()
{
  Serial.begin(115200);
  delay(10);

  Transmit_data_init();
  Receive_Data_init();
  start_trigger = false;
  // We start by connecting to a WiFi network
  WiFiMulti.addAP("enterprize2019", "enterprize2020");

  //    Serial.println();
  //    Serial.println();
  //    Serial.print("Waiting for WiFi... ");

  while (WiFiMulti.run() != WL_CONNECTED)
  {
    //        Serial.print("./");
    delay(500);
  }
  //
  //    Serial.println("");
  //    Serial.println("WiFi connected");
  //    Serial.println("IP address: ");
  //    Serial.println(WiFi.localIP());

  delay(500);
}

void loop()
{

  const uint16_t port = 8124;
  const char *host = "192.168.1.4";

  //    Serial.print("Connecting to ");
  //    Serial.println(host);

  // Use WiFiClient class to create TCP connections
  WiFiClient client;

  if (client.connect(host, port))
  {
    //      Serial.println("con/nected");
    while (client.connected() || client.available())
    {
      if (client.available())
      {
        if (start_trigger == true)
        {
          transmitData.remainTime = 300 - ((millis() - start_time) / 1000);
        }

        client.print("{");
        client.print("\"Name\":");
        client.print(transmitData.transmitName);
        client.print(",");
        client.print("\"Color\":");
        client.print(transmitData.transmitColor);
        client.print("}");
        client.print("\n");

        String line = client.readStringUntil('\n');
        if (receiveData.receive_isAvailable && receiveData.receive_isStable)
        {
          Serial.println(receiveData.receiveColor);
        }
        else
        {
          Serial.println(0);
        }

        if (line.substring(9, 12) == "red")
        {
          receiveData.receiveName = "Red";
          if (line.substring(23, 27) == "Blue")
          {
            receiveData.receiveColor = Blue;
            if (line.substring(40, 45) == "false")
            {
              receiveData.receive_isStable = false;
              if (line.substring(60, 65) == "false")
              {
                receiveData.receive_isAvailable = false;
              }
              else
              {
                receiveData.receive_isAvailable = true;
              }
            }
            else
            {
              receiveData.receive_isStable = true;
              if (line.substring(59, 64) == "false")
              {
                receiveData.receive_isAvailable = false;
              }
              else
              {
                receiveData.receive_isAvailable = true;
              }
            }
          }
          else if (line.substring(23, 27) == "Yell")
          {
            receiveData.receiveColor = Yellow;
            if (line.substring(42, 47) == "false")
            {
              receiveData.receive_isStable = false;
              if (line.substring(62, 67) == "false")
              {
                receiveData.receive_isAvailable = false;
              }
              else
              {
                receiveData.receive_isAvailable = true;
              }
            }
            else
            {
              receiveData.receive_isStable = true;
              if (line.substring(61, 66) == "false")
              {
                receiveData.receive_isAvailable = false;
              }
              else
              {
                receiveData.receive_isAvailable = true;
              }
            }
          }
          else if (line.substring(23, 27) == "Gree")
          {
            receiveData.receiveColor = Green;
            if (line.substring(41, 46) == "false")
            {
              receiveData.receive_isStable = false;
              if (line.substring(61, 66) == "false")
              {
                receiveData.receive_isAvailable = false;
              }
              else
              {
                receiveData.receive_isAvailable = true;
              }
            }
            else
            {
              receiveData.receive_isStable = true;
              if (line.substring(60, 65) == "false")
              {
                receiveData.receive_isAvailable = false;
              }
              else
              {
                receiveData.receive_isAvailable = true;
              }
            }
          }
          else
          {
            receiveData.receiveColor = Red;
            if (line.substring(39, 44) == "false")
            {
              receiveData.receive_isStable = false;
              if (line.substring(59, 64) == "false")
              {
                receiveData.receive_isAvailable = false;
              }
              else
              {
                receiveData.receive_isAvailable = true;
              }
            }
            else
            {
              receiveData.receive_isStable = true;
              if (line.substring(58, 63) == "false")
              {
                receiveData.receive_isAvailable = false;
              }
              else
              {
                receiveData.receive_isAvailable = true;
              }
            }
          }
        }
        else
        {
          receiveData.receiveName = "Blue";
          if (line.substring(24, 28) == "Blue")
          {
            receiveData.receiveColor = Blue;
          }
          else if (line.substring(24, 28) == "Yell")
          {
            receiveData.receiveColor = Yellow;
          }
          else if (line.substring(24, 28) == "Gree")
          {
            receiveData.receiveColor = Green;
          }
          else
          {
            receiveData.receiveColor = Red;
          }
        }

        //          Serial.println(line/);
      }

      if (receiveData.resetFlag == true)
      {
        start_time = millis();
        transmitData.resetFlag = true;
        start_trigger = true;
      }
      else
      {
        transmitData.resetFlag = false;
      }

      transmitData.transmitName = receiveData.receiveName;
      transmitData.transmitColor = receiveData.receiveColor;
    }
    client.stop();
    Serial.println("\n[Disconnected]");
  }

  //
  //   if (receiveData.resetFlag == true)
  //   {
  //      start_time = millis();
  //      transmitData.resetFlag = true;
  //      start_trigger = true;
  //   }
  //   else
  //   {
  //      transmitData.resetFlag = false;
  //   }
  //
  //    transmitData.transmitName = receiveData.receiveName;
  //    Serial.println(receiveData.receiveName);
  //    Serial.println(transmitData.transmitName);
  delay(50);
}
