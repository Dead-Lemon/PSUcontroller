
//defining pinouts for power relays for each power rail.
const int v19_1pin = 8;
const int fan_pin = 7;
const int v5_1pin = 6;
const int v5_2pin = 5;
const int v5_3pin = 4;
const int v5_4pin = 3;
const int v12_1pin = A5;
const int v12_2pin = A4;


// define pinout for power monitoring
const int ACamps_in = A1;
const int  DCamps_in = A0;
const int  DCvolts_in = A2;

float ACamps, DCamps, DCvolts; 
const float Vfloat = 2.5; //sets the voltage expected at 0A
const float sensitivity = 0.2; //(100mA / 500mV) sets the voltage steps expected.


uint32_t currentMillis, previousMillis, battMillis = 0;
uint32_t updateRate = 1000; // 1second
uint32_t updateBattery = 60000; //1 minuite
uint8_t powerLevel = 0; //set power availability, 0 = all off 3 = all on
const uint8_t powerLevelMax = 4;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  
  pinMode(v19_1pin, OUTPUT);
  pinMode(v12_1pin, OUTPUT);
  pinMode(v12_2pin, OUTPUT);
  pinMode(v5_1pin, OUTPUT);
  pinMode(v5_2pin, OUTPUT);
  pinMode(v5_3pin, OUTPUT);
  pinMode(v5_4pin, OUTPUT);
  pinMode(fan_pin, OUTPUT);

  digitalWrite(v19_1pin, HIGH);
  digitalWrite(v12_1pin, HIGH);
  digitalWrite(v12_2pin, HIGH);
  digitalWrite(v5_1pin, HIGH);
  digitalWrite(v5_2pin, HIGH);
  digitalWrite(v5_3pin, HIGH);
  digitalWrite(v5_4pin, HIGH);
  digitalWrite(fan_pin, HIGH);
  
}

void loop() {

  ACamps = ac_read(analogRead(ACamps_in));
  DCamps = (((((float)analogRead(DCamps_in))-509.0f)/512)*30.0f); //
  DCvolts = ((((float)analogRead(DCvolts_in))/1024.0f)*16.2f); 
  checkBattery();
  currentMillis = millis();
  
  if ((currentMillis - previousMillis) > updateRate) { //serial print every second  
  Serial.print("AC Amps: ");
  Serial.println(ACamps);  
  Serial.print("DC Amps: ");
  Serial.println(DCamps);
  Serial.print("DC Volts: ");
  Serial.println(DCvolts);
  Serial.print("Power Level: ");
  Serial.println(powerLevel);
  Serial.print("AC Power: ");
  Serial.println(ACamps*230);
  Serial.print("DC Power: ");
  Serial.println(DCamps*DCvolts);
  Serial.println("----------------------");
  previousMillis = currentMillis;
  }

}

double ac_read(int rawAnalog) {
  int rVal = 0;
  int sampleDuration = 100;       // 100ms
  uint32_t sampleCount = 0;
  unsigned long rSquaredSum = 0;
  int rZero = 510;                // should be measured to calibrate sensor.

  uint32_t startTime = millis();  // take samples for 100ms
  while((millis()-startTime) < sampleDuration)
  {
    rVal = rawAnalog - rZero;
    rSquaredSum += rVal * rVal;
    sampleCount++;
  }

  double voltRMS = 5.0 * sqrt(rSquaredSum / sampleCount) / 1024.0;
  //Serial.print(rSquaredSum);
  //Serial.print(sampleCount);
  // x 1000 to convert volts to millivolts
  // divide by the number of millivolts per amp to determine amps measured
  // the 20A module 100 mv/A (so in this case ampsRMS = 10 * voltRMS
  double ampsRMS = voltRMS * 10.0;
  //Serial.print(ampsRMS);
  return(ampsRMS);
}

void checkBattery() { //when battery is lower than 12.1V, system will start to reduce load. 
  if ((currentMillis-battMillis) > updateBattery) { 
    if ((DCvolts > 12.5f) and (powerLevel < powerLevelMax)) {
      powerLevel++;
    } else if ((DCvolts < 11.9f) and (powerLevel > 0)) {
      powerLevel--;
    }
    battMillis = currentMillis;
    updatePowerOutlet();
  }
}
void updatePowerOutlet() {
  switch (powerLevel) {
    case 0:
      digitalWrite(v19_1pin, HIGH); //all off
      digitalWrite(v12_1pin, HIGH);
      digitalWrite(v12_2pin, HIGH);
      digitalWrite(v5_1pin, HIGH);
      digitalWrite(v5_2pin, HIGH);
      digitalWrite(v5_3pin, HIGH);
      digitalWrite(v5_4pin, HIGH);
      digitalWrite(fan_pin, LOW);
    break;
    case 1:
      digitalWrite(v19_1pin, HIGH);
      digitalWrite(v12_1pin, LOW); //enable switches
      digitalWrite(v12_2pin, LOW); //enable router
      digitalWrite(v5_1pin, LOW); //enable PiHole
      digitalWrite(v5_2pin, HIGH);
      digitalWrite(v5_3pin, HIGH);
      digitalWrite(v5_4pin, HIGH);
      digitalWrite(fan_pin, HIGH); //enable fan
    break;
    case 2:
      digitalWrite(v19_1pin, LOW); //enable NAS
      digitalWrite(v12_1pin, LOW); //enable switches
      digitalWrite(v12_2pin, LOW); //enable router
      digitalWrite(v5_1pin, LOW); //enable PiHole
      digitalWrite(v5_2pin, HIGH);
      digitalWrite(v5_3pin, HIGH);
      digitalWrite(v5_4pin, HIGH);
      digitalWrite(fan_pin, HIGH); //enable fan
    break;
    case 3:
      digitalWrite(v19_1pin, LOW); //enable NAS
      digitalWrite(v12_1pin, LOW); //enable switches
      digitalWrite(v12_2pin, LOW); //enable router
      digitalWrite(v5_1pin, LOW); //enable PiHole
      digitalWrite(v5_2pin, LOW); //enable downloadPi
      digitalWrite(v5_3pin, HIGH);
      digitalWrite(v5_4pin, HIGH);
      digitalWrite(fan_pin, HIGH); //enable Fan
    break;
    case 4:
      digitalWrite(v19_1pin, LOW); //enable all loads
      digitalWrite(v12_1pin, LOW);
      digitalWrite(v12_2pin, LOW);
      digitalWrite(v5_1pin, LOW);
      digitalWrite(v5_2pin, LOW);
      digitalWrite(v5_3pin, LOW);
      digitalWrite(v5_4pin, LOW);
      digitalWrite(fan_pin, HIGH);
    break;
 
  }
}
  
