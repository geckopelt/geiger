#include <SPI.h>
#include <Ethernet.h>

class Geiger
{
public:
  void init()
  {
    byte mac[] = {  0xAE, 0xCD, 0x14, 0x14, 0xFE, 0xEB };
    byte ip[]      = { 192, 168, 1, 42 }; 
    byte gateway[] = { 192, 168, 1, 1 }; // Go through router
    byte subnet[]  = { 255, 255, 255, 0 };
   
    Ethernet.begin(mac, ip, gateway, subnet);
    Serial.begin(9600);

    delay(1000);
  }
  
  void sendData(float data)
  {
    byte server[] = { 173,203,98,29 }; // Xively
    Client client(server, 80);
    
    // if there's a successful connection:
    if (client.connect()) 
    {
      Serial.println("Sending request");
      
      client.print("PUT /api/YOUR_FEED_ID.csv HTTP/1.1\n");
      client.print("Host: www.pachube.com\n");
      client.print("X-PachubeApiKey: YOUR_KEY_HERE\n"); // API key
      client.print("Content-Length: ");
  
      // Convert data to string    
      char buf[8] = { 0, 0, 0, 0, 0, 0, 0, 0 };
      char * str = dtostrf(data, 5, 3, buf);

      // Send data
      Serial.print("Data: \"");
      Serial.print(str);
      Serial.println("\"");
      
      // Print Content-Length value
      client.println(strlen(str), DEC);

      // last pieces of the HTTP PUT request:
      client.print("Content-Type: text/csv\n");
      client.println("Connection: close\n");

      // Data itself
      client.println(str);
      
      // Now wait for response
      Serial.print("Waiting for server");
      while (!client.available())
      {
        Serial.print(".");
        delay(1000);
      }
      Serial.println();
      
      // Print response
      while (client.connected())
      {
        char c = client.read();
        Serial.print(c);
      }
      client.stop();
    }
    else
    {
      Serial.println("Can't connect");
    }
  }
};

//
// CPM Stuff
//

volatile int count = 0;

int getCpm()
{
    count = 0;
    attachInterrupt(0, pulse, FALLING); // digital pin 2
    for (int i = 0; i < 6; i++) // wait 60 secs
    {
      delay(10000); 
      Serial.print(".");
    }
    detachInterrupt(0);
    return count;
}

void pulse()
{
  count++;
}

float cpmToMkSv(int cpm)
{
  return (cpm < 5) ? 0 : ((cpm - 5) * 0.0017191); // TODO
}

//
// Main section
//
    
Geiger geiger;
    
void setup() 
{
  geiger.init();
}

void loop()
{
  const int Count = 15;
  int total = 0;
  Serial.print("Measure");
  for (int i = 0; i < Count; i++)
  {
    int cpm = getCpm();
    Serial.print(cpm);
    total += cpm;
  }
  total = total / Count;
  Serial.print(", avg: ");
  Serial.print(total);
  Serial.print(", ");
  float mksv = cpmToMkSv(total);
  Serial.print(mksv);
  Serial.println(" mkSv/h");
  geiger.sendData(mksv);
}

