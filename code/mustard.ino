#include <WiFiS3.h>
#include <MQUnifiedsensor.h>

#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>


#define BLYNK_TEMPLATE_ID "FROM THE SCREEN"
#define BLYNK_TEMPLATE_NAME "TO THE RING"
#define BLYNK_AUTH_TOKEN "TO THE PEN TO THE KING"

#include <BlynkSimpleWifi.h>

char auth[] = BLYNK_AUTH_TOKEN;
char ssid[] = ""; // Wi-Fi name
char pass[] = ""; // Wi-Fi password

#define placa "Arduino UNO"
#define Voltage_Resolution 5
#define pin A0
#define type "MQ-135"
#define ADC_Bit_Resolution 10
#define RatioMQ135CleanAir 3.6

#define pin_but1 4
#define OLED_RESET 4
int page = 0;
int limit = 6;
int ptmp;

MQUnifiedsensor MQ135(placa, Voltage_Resolution, ADC_Bit_Resolution, pin, type);


Adafruit_SSD1306 display(OLED_RESET);

int red_but = 10;
int yel_but = 9;
int gre_but = 8;
int buzz = 7;

void setup() {
  
  Serial.begin(9600); // Init serial port

  display.begin(SSD1306_SWITCHCAPVCC, 0x3c);
  display.clearDisplay();

  pinMode(red_but,OUTPUT);
  pinMode(yel_but,OUTPUT);
  pinMode(gre_but,OUTPUT);

  pinMode(buzz,OUTPUT);
  
  int but1 = 0;
  pinMode(pin_but1,INPUT);
  


  Serial.print("Im Starting");
  Blynk.begin(auth, ssid, pass);
  Serial.print("connected!");
  MQ135.setRegressionMethod(1);
  MQ135.init();





  Serial.print("Calibrating please wait.");
  float calcR0 = 0;
  for (int i = 1; i <= 10; i++) {
    MQ135.update();
    calcR0 += MQ135.calibrate(RatioMQ135CleanAir);
    Serial.print(".");
  }
  MQ135.setR0(calcR0 / 10);
  Serial.println("  done!");

  if (isinf(calcR0)) {
    Serial.println("Warning: Connection issue, R0 is infinite (Open circuit detected) please check your wiring and supply");
    while (1);
  }
  if (calcR0 == 0) {
    Serial.println("Warning: Connection issue found, R0 is zero (Analog pin shorts to ground) please check your wiring and supply");
    while (1);
  }

  Serial.println("** Values from MQ-135 **");
  Serial.println("|    CO   |  Alcohol |   CO2  |  Toluen  |  NH4  |  Aceton  |  Air Quality  |");
  digitalWrite(gre_but,HIGH);
  digitalWrite(red_but,HIGH);
  digitalWrite(yel_but,HIGH);
  delay(5000);
}

void loop() {
  
  Blynk.run(); // Run Blynk
  MQ135.update(); // Update sensor readings

  MQ135.setA(605.18); MQ135.setB(-3.937); // CO concentration
  float CO = MQ135.readSensor();

  MQ135.setA(77.255); MQ135.setB(-3.18); // Alcohol concentration
  float Alcohol = MQ135.readSensor();

  MQ135.setA(110.47); MQ135.setB(-2.862); // CO2 concentration
  float CO2 = MQ135.readSensor();

  MQ135.setA(44.947); MQ135.setB(-3.445); // Toluen concentration
  float Toluen = MQ135.readSensor();

  MQ135.setA(102.2); MQ135.setB(-2.473); // NH4 concentration
  float NH4 = MQ135.readSensor();

  MQ135.setA(34.668); MQ135.setB(-3.369); // Aceton concentration
  float Aceton = MQ135.readSensor();

  float averageConcentration = (CO + Alcohol + (CO2 + 400) + Toluen + NH4 + Aceton) / 6.0;
  
  float airQualityPercent = 100 - (averageConcentration / 1000.0) * 100;
  if (airQualityPercent < 0) airQualityPercent = 0;
  if (airQualityPercent > 100) airQualityPercent = 100;

  String airQualityLevel = (airQualityPercent > 90) ? "Good" : (airQualityPercent > 75) ? "Moderate" : (airQualityPercent > 50) ? "Bad" : "Danger";

  Blynk.virtualWrite(V1, CO);
  Blynk.virtualWrite(V2, Alcohol);
  Blynk.virtualWrite(V3, CO2 + 400); // Add baseline for CO2
  Blynk.virtualWrite(V4, Toluen);
  Blynk.virtualWrite(V5, NH4);
  Blynk.virtualWrite(V7, airQualityPercent);
  Blynk.virtualWrite(V8, airQualityLevel);
  ptmp = page;

  int but1 = digitalRead(pin_but1);
  if (but1 == 0){
    page = page + 1;
    if (page > limit){
      page = 0;
    }
  }

  if(airQualityPercent > 90.00){
    digitalWrite(red_but,LOW);
    digitalWrite(yel_but,LOW);
    digitalWrite(gre_but,HIGH);
    digitalWrite(buzz,HIGH);
  }
  else if(airQualityPercent > 75.00){
    digitalWrite(red_but,LOW);
    digitalWrite(yel_but,HIGH);
    digitalWrite(gre_but,LOW);
    digitalWrite(buzz,HIGH);
  }
  else if(airQualityPercent > 50.00){
    digitalWrite(red_but,HIGH);
    digitalWrite(yel_but,LOW);
    digitalWrite(gre_but,LOW);
    digitalWrite(buzz,HIGH);
  }
  else {
    static unsigned long previousMillis = 0;
    const long interval = 500;

    unsigned long currentMillis = millis();
    if (currentMillis - previousMillis >= interval) {
      previousMillis = currentMillis;
      digitalWrite(red_but, !digitalRead(red_but));
      digitalWrite(yel_but, !digitalRead(yel_but));
      digitalWrite(gre_but, !digitalRead(gre_but));
      digitalWrite(buzz, !digitalRead(buzz));
    }
  }

    
  Serial.print(but1);
  Serial.print("  ");
  Serial.println(page);

  display.begin(SSD1306_SWITCHCAPVCC, 0x3c);

  if (page == 0){
    display.clearDisplay();
    display.setCursor(0,0);
    display.setTextSize(1);
    display.setTextColor(WHITE, BLACK);
    display.println("CO level");
    

    display.setTextSize(1);
    display.setTextColor(WHITE, BLACK);
    display.println("---------------------");
    
    display.setTextSize(2);
    display.setTextColor(WHITE, BLACK);
    display.println(CO);
    
    
  }
  

  if (page == 1){
  display.clearDisplay();
  display.setCursor(0,0);
  display.setTextSize(1);
  display.setTextColor(WHITE, BLACK);
  display.println("Alcohol");

  display.setTextSize(1);
  display.setTextColor(WHITE, BLACK);
  display.println("---------------------");
  
  display.setTextSize(2);
  display.setTextColor(WHITE, BLACK);
  display.println(Alcohol);
  }


  if (page == 2){
  display.clearDisplay();
  display.setCursor(0,0);
  display.setTextSize(1);
  display.setTextColor(WHITE, BLACK);
  display.println("Baseline CO2");
  display.setTextSize(1);
  display.setTextColor(WHITE, BLACK);
  display.println("---------------------");
  display.setTextSize(2);
  display.setTextColor(WHITE, BLACK);
  display.println(CO2 + 400);
  }


  if (page == 3){
  display.clearDisplay();
  display.setCursor(0,0);
  display.setTextSize(1);
  display.setTextColor(WHITE, BLACK);
  display.println("Toluen");
  display.setTextSize(1);
  display.setTextColor(WHITE, BLACK);
  display.println("---------------------");
  display.setTextSize(2);
  display.setTextColor(WHITE, BLACK);
  display.println(Toluen);
  }


  if (page == 4){
  display.clearDisplay();
  display.setCursor(0,0);
  display.setTextSize(1);
  display.setTextColor(WHITE, BLACK);
  display.println("NH4");
  display.setTextSize(1);
  display.setTextColor(WHITE, BLACK);
  display.println("---------------------");
  display.setTextSize(2);
  display.setTextColor(WHITE, BLACK);
  display.println(NH4);
  }

    if (page == 5){
  display.clearDisplay();
  display.setCursor(0,0);
  display.setTextSize(1);
  display.setTextColor(WHITE, BLACK);
  display.println("Aceton");
  display.setTextSize(1);
  display.setTextColor(WHITE, BLACK);
  display.println("---------------------");
  display.setTextSize(2);
  display.setTextColor(WHITE, BLACK);
  display.println(Aceton);
  }

    if (page == 6){
  display.clearDisplay();
  display.setCursor(0,0);
  display.setTextSize(1);
  display.setTextColor(WHITE, BLACK);
  display.println("airQuality :  ");
  display.print(airQualityPercent);
  display.println("%");
  display.setTextSize(2);
  display.println(airQualityLevel);
  
  }

  Serial.print("|   "); Serial.print(CO);
  Serial.print("   |   "); Serial.print(Alcohol);
  Serial.print("   |   "); Serial.print(CO2 + 400);
  Serial.print("   |   "); Serial.print(Toluen);
  Serial.print("   |   "); Serial.print(NH4);
  Serial.print("   |   "); Serial.print(Aceton);
  Serial.print("   |   "); Serial.print(airQualityPercent); Serial.print("% (");
  Serial.print(airQualityLevel); Serial.print(")   |");
  Serial.println();

  display.display();
  delay(500);
}
