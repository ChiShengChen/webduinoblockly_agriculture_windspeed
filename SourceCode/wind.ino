//wind
const int analogInPin = A1;
int windsensorValue = 0;
float windsensorVoltage = 0.0;
float windSpeed = 0.0;

void setup() {
  Serial.begin(9600);
  // put your setup code here, to run once:

}

void loop() {
  windsensorValue = analogRead(analogInPin);
   if (windsensorValue == 0 || windsensorValue == 1023) return;
   windsensorVoltage = windsensorValue * .004882814;
   //int nMapSensorVal = map(sensorValue, 0, 1023, 0.4, 2);

  Serial.print("WindSensor Value: ");
  Serial.print(windsensorValue);
  Serial.print("\t");
  Serial.print("WindSensor Voltage: ");
  Serial.print(int(windsensorVoltage*100));

  if (windsensorVoltage <= 0.4)
    windSpeed = 0;
  else
  {
   windSpeed = (windsensorVoltage-0.4) * 10 * 2;
  }

  Serial.println("\t");
  //Serial.print("Wind Speed: ");
  //Serial.println(windSpeed);
  //delay(100); 
}
