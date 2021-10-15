#include <Arduino.h>

#define K 0.003300
#define CELLS 2
#define MAX_CELLS 12
#define TEM_Sen A7

int intTemp = 0;

float vCells[CELLS];

String Data_In = "";
int batState[CELLS];
bool lowBat = false;

unsigned long sysClock;
int updateTime = 500;

double cell_const[MAX_CELLS] =
    {
        1.4598,
        1.8500,
        1.3704,
        1.65,
        4.7333,
        6.6000,
        6.6000,
        7.8293,
        8.4667,
        9.2353,
        11.0000,
        11.0000};

//################## FUNCTION DECLARATIONS ####################
void printData();
void internalTemperature();

void setup()
{
  Serial.begin(115200);
  Serial.setTimeout(20);
  Serial1.begin(115200);
  Serial1.setTimeout(20);
  Serial1.print("batMon#");
  Serial.println("Battery Monitor...");
  // initialize system clock
  sysClock = millis();
}

void loop()
{ // Battery status.
  if (sysClock + updateTime < millis())
  {
    internalTemperature();
    // Populate battery state message.
    for (int i = 0; i < CELLS; i++)
    {
      // Read raw voltage from analog pin.
      double cellVoltage = analogRead(i) * K;
      // Scale reading to full voltage.
      cellVoltage *= cell_const[i];
      // Set current cell voltage to message.
      vCells[i] = (float)cellVoltage;
      if (vCells[i] < 3.20)
      {
        batState[i] = 0;
        lowBat = true;
      }
      else if (vCells[i] < 3.70)
      {
        batState[i] = 1;
      }
      else if (vCells[i] < 4.21)
      {
        batState[i] = 2;
      }
    }
    printData();
    sysClock = millis(); //Reset SysClock
  }
}

void printData()
{
  Serial.print("Drive Bat Temp = " + String(intTemp) + " --");
  Serial.print("Cell_0 = " + String(vCells[0]) + " State = " + String(batState[0]));
  Serial.println("--Cell_ = " + String(vCells[1]) + " State = " + String(batState[1]));
}

void internalTemperature()
{
  int reading = analogRead(TEM_Sen);
  float voltage = reading * 3.3;
  voltage /= 1024.0;
  float temp = (voltage - 0.5) * 100;
  intTemp = int(temp);
}

void serialEvent1()
{ //Get Request for battery data
  Data_In = Serial1.readStringUntil('#');
  if (Data_In == "dbat")
  {
    Serial1.print("dbat@");
    Serial1.print(vCells[0]);
    Serial1.print("-");
    Serial1.print(vCells[1]);
    Serial1.println("#");
    Data_In = "";
  }
  else if (Data_In == "dtemp")
  {
    Serial1.print("dtemp@");
    Serial1.print(intTemp);
    Serial1.println("#");
    Data_In = "";
  }
}