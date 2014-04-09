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

void setup() 
{
    Serial.begin(9600);
    delay(1000);
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
}
