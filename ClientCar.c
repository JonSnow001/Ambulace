
#include<ESP8266WiFi.h>
#include<Wire.h>
#include<SoftwareSerial.h>
#include<TinyGPS.h>
#include<SPI.h>


const int MPU_addr=0x68;
int16_t AcX,AcY,AcZ,Tmp,GyX,GyY,GyZ;
const int trigPin = 9;
const int echoPin = 10;
long duration;
int distance;
#define SHOCK_PIN 2
float lat;
float lon;
SoftwareSerial gpsSerial(3,4);
TinyGPS gps;


int shockvar;

const char *ssid =  "Kingsman"; // We are setting up wifi connection due to unavailability of GSM module.
const char *pass =  "thesecretservice";

WiFiClient client;

void setup()
{
       Serial.begin(9600);
       delay(10);

       Serial.println("Connecting to ");
       Serial.println(ssid);

       WiFi.begin(ssid, pass);
       while (WiFi.status() != WL_CONNECTED)
          {
            delay(500);
            Serial.print(".");
          }
      Serial.println("");
      Serial.println("WiFi connected");

      Wire.begin();
      Wire.beginTransmission(MPU_addr);
      Wire.write(0x6B);
      Wire.write(0);
      Wire.endTransmission(true);

      pinMode(trigPin, OUTPUT);
      pinMode(echoPin, INPUT);

      pinMode(LED_BUILTIN, OUTPUT);
      pinMode(SHOCK_PIN, INPUT);
}

}

void loop()
{
  Wire.beginTransmission(MPU_addr);
 Wire.write(0x3B);
 Wire.endTransmission(false);
 Wire.requestFrom(MPU_addr,14,true);

 AcX=Wire.read()<<8|Wire.read();
 AcY=Wire.read()<<8|Wire.read();
 AcZ=Wire.read()<<8|Wire.read();

 Tmp=Wire.read()<<8|Wire.read();
 GyX=Wire.read()<<8|Wire.read();
 GyY=Wire.read()<<8|Wire.read();
 GyZ=Wire.read()<<8|Wire.read();

 Serial.print("AcX = "); Serial.print(AcX);
 Serial.print(" | AcY = "); Serial.print(AcY);
 Serial.print(" | AcZ = "); Serial.print(AcZ);
 Serial.print(" | Tmp = "); Serial.print(Tmp/340.00+36.53);
 Serial.print(" | GyX = "); Serial.print(GyX);
 Serial.print(" | GyY = "); Serial.print(GyY);
 Serial.print(" | GyZ = "); Serial.println(GyZ);

 delay(333);

digitalWrite(trigPin, LOW);
delayMicroseconds(2);
digitalWrite(trigPin, HIGH);
delayMicroseconds(10);
digitalWrite(trigPin, LOW);
duration = pulseIn(echoPin, HIGH);
distance= duration*0.034/2;
Serial.print("Distance: ");
Serial.println(distance);

if (digitalRead(SHOCK_PIN)) {
    digitalWrite(LED_BUILTIN, HIGH);
    shockvar = 1;
    delay(2000);
  }
  else {
    digitalWrite(LED_BUILTIN, LOW);
    shockvar = 0;
  }
  while(gpsSerial.available()){
    if(gps.encode(gpsSerial.read()))
    {
    gps.f_get_position(&lat,&lon);

   }
  }

  String latitude = String(lat,6);
    String longitude = String(lon,6);
  Serial.println(latitude+";"+longitude);
  delay(1000);


  if(AcX==0 && GyZ>>10 && shockvar==1 && distance << 5){
        aa = latitude;
        bb = longitude;


      data = "latitude" + aa + "longitude" + bb;

      if (client.connect("192.168.101.61/centralserver/server.php",80)) {
        client.println("POST /add.php HTTP/1.1");
        client.println("192.168.101.61/centralserver/server.php");
        client.println("Content-Type: application/x-www-form-urlencoded");
        client.print("Content-Length: ");
        client.println(data.length());
        client.println("Accident Detected at");
        client.print(data);
        client.print("Please respond immediately");
      }

      if (client.connected()) {
        client.stop();
      }

      delay(300000);
}
}
