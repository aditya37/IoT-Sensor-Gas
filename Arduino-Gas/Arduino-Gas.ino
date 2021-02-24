#include <LiquidCrystal_I2C.h>
#include <ArduinoJson.h>
#include <Wire.h>
LiquidCrystal_I2C lcd(0x3F ,20,4);

#define pinSensor A0 // MQ-7
#define MQ136  A1 // pin A1 adalah untuk sensor MQ-136
#define MG_PIN  A3 // MG-811
#define DC_GAIN  (8.5) 
#define Photodioda A2

// Dump Sensor Value
float sensorvalue = 0;

// Menampung nilai tegangan
float VRL = 0; 

//Menampung nilai hambatan
float rs = 0;

float ppma = 0;
float VRLB = 0;
float rsB = 0;
float ppmb = 0;
long RL = 1000; // 1000 Ohm
long Ro = 6474; // 830 ohm ( SILAHKAN DISESUAIKAN)
long Rob = 531.66;
int percentage;
float volts = 0;

/***********************Software Related Macros************************************/
#define         READ_SAMPLE_INTERVAL         (50)    //define how many samples you are going to take in normal operation
#define         READ_SAMPLE_TIMES            (5)     //define the time interval(in milisecond) between each samples in 
                                                     //normal operation

/**********************Application Related Macros**********************************/
//These two values differ from sensor to sensor. user should derermine this value.
#define         ZERO_POINT_VOLTAGE           (0.220) //define the output of the sensor in volts when the concentration of CO2 is 400PPM
#define         REACTION_VOLTGAE             (0.020) //define the voltage drop of the sensor when move the sensor from air into 1000ppm CO2

/*****************************Globals***********************************************/
float           CO2Curve[3]  =  {2.602,ZERO_POINT_VOLTAGE,(REACTION_VOLTGAE/(2.602-3))};   

void setup() {
  
  Serial.begin(9600);
  
  Wire.begin();
  lcd.begin();
  lcd.backlight(); // LCD Ukuran 16X2
  
}

void loop() {
  
  //  Mendapatakna Nilai tegangan
  sensorvalue = analogRead(A0);
  VRL = sensorvalue*5/1024;
  rs = (RL*5)/VRL;
  
  //  Kadar ppma CO
  ppma = 100*pow(rs/Ro,-1.53); // ppm = 100 * ((rs/ro)^-1.53);
  lcd.setCursor(0, 0);
  lcd.print("CO : ");
  lcd.print(ppma);
  lcd.println(" ppm");

  if (ppma < 50){
    lcd.setCursor(0,1);
    lcd.print("Ket: ");
    lcd.println("Aman");
  }else if(ppma >= 50 && ppma < 250){
    lcd.setCursor(0,1);
    lcd.print("Ket: ");
    lcd.println("Peringatan");
  }else  if (ppma >= 250) {
    lcd.setCursor(0,1);
    lcd.print("Ket: ");
    lcd.println("Berbahaya");
  }
  delay(3000);
  lcd.clear();

  //  Sensor MQ-136
  
  //  Membaca nilai ADC sensor
  int sensorvalueB = analogRead(MQ136);
  VRLB = sensorvalueB * 5.00/1024;
  rsB = (5.00 * RL / VRLB) - RL;
  
  //  ppm SO2
  ppmb = 21.79 * pow(rsB / Rob,-0.897);
  lcd.setCursor(0, 0);
  lcd.print("SO2 : ");
  lcd.print(ppmb);
  lcd.println(" ppm");

  if (ppmb < 15){
    lcd.setCursor(0,1);
    lcd.print("Ket: ");
    lcd.println("Aman");
  }else if (ppmb >= 15 && ppmb < 50) {
    lcd.setCursor(0,1);
    lcd.print("Ket: ");
    lcd.println("Peringatan");
  }else if (ppmb >= 50) {
    lcd.setCursor(0,1);
    lcd.print("Ket: ");
    lcd.println("Berbahaya");
  }
  delay(3000);
  lcd.clear();

  //  Bagian Sensor MG-811
  volts = MGRead(MG_PIN);
  percentage = MGGetPercentage(volts,CO2Curve)/10;
  lcd.setCursor(0, 0);
  lcd.print("CO2:");
  lcd.print(abs(percentage));
  lcd.print( " ppm" );  
  lcd.print("\n");
  
  if (abs(percentage) < 1000){
    lcd.setCursor(0,1);
    lcd.print("Ket: ");
    lcd.println("Aman");
  }else if (abs(percentage) >= 1000 && abs(percentage) < 2000) {
    lcd.setCursor(0,1);
    lcd.print("Ket: ");
    lcd.println("Peringatan");
  }else if (abs(percentage) >= 2000) {
    lcd.setCursor(0,1);
    lcd.print("Ket: ");
    lcd.println("Berbahaya");
  }
  delay(3000);
  lcd.clear();

  //  Sensor Jarak pandang
  int sensorValueC = analogRead(Photodioda);
  float TeganganB = 5.0 * sensorValueC / 1024;
  float jarakPandang = 13 * 3.2 / (1.94 / TeganganB);
  float meter = jarakPandang / 10;

  lcd.setCursor(0, 0);
  lcd.print("Jarak Pandang");
  lcd.setCursor(0, 1);
  lcd.print(meter);
  lcd.println("Meter");
  delay(3000);
  lcd.clear();
  
  StaticJsonDocument<300> doc;
  JsonObject object = doc.to<JsonObject>();
  object["jarakPandang"] = meter;
  object["ppm_co"] = ppma;
  object["ppm_co2"] = abs(percentage);
  object["ppm_so2"] = ppmb;
  serializeJson(doc,Serial);

}

//Membaca data sensor MG-811
float MGRead(int mg_pin)
{
    int i;
    float v=0;

    for (i=0;i<READ_SAMPLE_TIMES;i++) {
        v += analogRead(mg_pin);
         delay(READ_SAMPLE_INTERVAL);
    }
    v = (v/READ_SAMPLE_TIMES) *5/1024 ;
    return v;  
}

// Merubah nilai MG-811 ke persen 
int  MGGetPercentage(float volts, float *pcurve)
{
   if ((volts/DC_GAIN )>=ZERO_POINT_VOLTAGE) {
      return percentage;
   } else { 
      return pow(10, ((volts/DC_GAIN)-pcurve[1])/pcurve[2]+pcurve[0]);
   }
}
