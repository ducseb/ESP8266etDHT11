// MINI WEATHER STATION
// TEMPERATURE - HUMIDITY  - LCD DISPLAY
// REQUIRE LCD1602 and I2C module
// Developped with NodeMCU ESP8266


// Pin connection -----------------
// LCD SDA to pin 5 (NodeMCU pin D1)
// LCD SCL to pin 2 (NodeMCU pin D4)
// DHT11 data to pin 14 (NodeMCU pin D5)
// Button switch for backlight LCD to pin 0 (NodeMCU flash buttton or pin D3)



#include <LiquidCrystal_I2C.h>
#include <Wire.h>
#include <ESP8266WiFi.h>


#include "DHT.h"
#define DHTPIN 14     // what pin we're connected to
#define WIRESDAPIN 5 //Pin for I2C SDA (for LCD display)
#defin WIRESCLPIN 2 //Ping for I2C SCL (for LCD displa)


#define I2C_ADDR    0x27
#define BACKLIGHT_PIN  3  //7
#define En_pin  2  //4
#define Rw_pin  1
#define Rs_pin  0  //6
#define D4_pin  4  //0
#define D5_pin  5  //1
#define D6_pin  6  //2
#define D7_pin  7  //3

LiquidCrystal_I2C  lcd(I2C_ADDR,En_pin,Rw_pin,Rs_pin,D4_pin,D5_pin,D6_pin,D7_pin,BACKLIGHT_PIN,POSITIVE ); //NEGATIVE, POSITIVE

// Uncomment whatever type you're using!
#define DHTTYPE DHT11   // DHT 11 
//define DHTTYPE DHT22   // DHT 22  (AM2302)
//#define DHTTYPE DHT21   // DHT 21 (AM2301)


// Initialize DHT sensor for normal 16mhz Arduino
//DHT dht(DHTPIN, DHTTYPE);
// NOTE: For working with a faster chip, like an Arduino Due or Teensy, you
// might need to increase the threshold for cycle counts considered a 1 or 0.
// You can do this by passing a 3rd parameter for this threshold.  It's a bit
// of fiddling to find the right value, but in general the faster the CPU the
// higher the value.  The default for a 16mhz AVR is a value of 6.  For an
// Arduino Due that runs at 84mhz a value of 30 works.
// Example to initialize DHT sensor for Arduino Due:
DHT dht(DHTPIN, DHTTYPE, 30);


const char* ssid     = "SSIDWIFI";
const char* password = "PASSWIFI";


const char* host = "api.thingspeak.com";
const char* cleData   = "KEYDATATHINGSPEAK";


bool ModeBackLight=true;



void setup() {
  
  
   
   int pinInterrupt = 0;
    pinMode(pinInterrupt, INPUT);
   attachInterrupt(0, changeBackLight, CHANGE);
   
   
  Serial.begin(9600); 
  Serial.println("DHTxx test!");
 Serial.println();
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
  
    
  
  

  
  
  WiFi.begin(ssid, password);
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");  
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());  
  dht.begin();
  
  
   Wire.pins(5, 2);
  // initialize library
  lcd.begin(16,2); 
  lcd.clear();
  lcd.backlight();
  // set cursor to positon x=0, y=0
  
  lcd.clear();
  lcd.setCursor(0,0);
  // print Hello!
  lcd.print("THE DUCSEB");
  
  lcd.setCursor(0,1);
 
  lcd.print("---METEO---:");
  lcd.noCursor();

}

void loop() {
  // Wait a few seconds between measurements.
  

  // Reading temperature or humidity takes about 250 milliseconds!
  // Sensor readings may also be up to 2 seconds 'old' (its a very slow sensor)
  float h = dht.readHumidity();
  // Read temperature as Celsius
  float t = dht.readTemperature();
  // Read temperature as Fahrenheit
  float f = dht.readTemperature(true);
  
  // Check if any reads failed and exit early (to try again).
  if (isnan(h) || isnan(t) || isnan(f)) {
    Serial.println("Failed to read from DHT sensor!");
    return;
  }

  // Compute heat index
  // Must send in temp in Fahrenheit!
  float hi = dht.computeHeatIndex(f, h);

  Serial.print("Humidity: "); 
  Serial.print(h);
  Serial.print(" %\t");
  Serial.print("Temperature: "); 
  Serial.print(t);
  Serial.print(" *C ");
  Serial.print(f);
  Serial.print(" *F\t");
  Serial.print("Heat index: ");
  Serial.print(hi);
  Serial.println(" *F");
  
  
  
  WiFiClient client;
  const int httpPort = 80;
  if (!client.connect(host, httpPort)) {
    Serial.println("connection failed");
    return;
  }
  
  // We now create a URI for the request
  String url = "/update?key=";
  url += cleData;
  url += "&field1=";
  url += t;
  url += "&field2=";
  url += h;
  
   Serial.print("Requesting URL: ");
  Serial.println(url);
  
  client.print(String("GET ") + url + " HTTP/1.1\r\n" +
               "Host: " + host + "\r\n" + 
               "Connection: close\r\n\r\n");
   delay(10);
   
    while(client.available()){
    String line = client.readStringUntil('\r');
    Serial.print(line);
  }
  
  Serial.println();
  Serial.println("closing connection");
  
  /*
  lcd.setCursor(15,0);  // set the cursor to column 15, line 0
    for (int positionCounter1 = 0; positionCounter1 < 26; positionCounter1++)
    {
      lcd.scrollDisplayLeft();  //Scrolls the contents of the display one space to the left.
      lcd.print(array1[positionCounter1]);  // Print a message to the LCD.
      delay(tim);  //wait for 250 microseconds
    }
    lcd.clear();  //Clears the LCD screen and positions the cursor in the upper-left corner.
    lcd.setCursor(15,1);  // set the cursor to column 15, line 1
    for (int positionCounter = 0; positionCounter < 26; positionCounter++)
    {
      lcd.scrollDisplayLeft();  //Scrolls the contents of the display one space to the left.
      lcd.print(array2[positionCounter]);  // Print a message to the LCD.
      delay(tim);  //wait for 250 microseconds
    }
    lcd.clear();  //Clears the LCD screen and positions the cursor in the upper-left corner.
    */
    
  lcd.clear();
  lcd.setCursor(0,0);
  // print Hello!
  String textTemp= "Temp:";
  textTemp+=t;
  textTemp+=(char)223;
  textTemp+="C";
  lcd.print(textTemp);
  // wait a second.
  delay(1000);
  // set cursor on second line
  lcd.setCursor(0,1);
String textHum= "Humidite:";
textHum+=h;
textHum+="%";

  lcd.print(textHum);
  lcd.noCursor();

  
  delay(20000);
}


//Called on button press
void changeBackLight()
{
  Serial.println("Changement mode backlight");
  if(!ModeBackLight)lcd.backlight();
  else lcd.noBacklight();
  
  ModeBackLight=!ModeBackLight;
  
  
}
