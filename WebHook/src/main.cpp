#include <ESP8266WiFi.h>
#include "Arduino.h"
#include <FastLED.h>

// How many leds in your strip?
#define NUM_LEDS 8
void coracao(uint16_t color256);

// For led chips like WS2812, which have a data line, ground, and power, you just
// need to define DATA_PIN.  For led chipsets that are SPI based (four wires - data, clock,
#define DATA_PIN D3

// Define the array of leds
CRGB leds[NUM_LEDS];
 
const char* ssid     = "Casa 2G"; // Net Virtua 169 2g
const char* password = "012996009191"; 
 
//Host e chave disponíveis na plataforma IFTTT.
const char* host = "maker.ifttt.com";
 
//Led indica que a mensagem foi disparada ao IFTTT.
 
#define BUTTON_PIN D7
 
const char* eventName1 = "/trigger/button_pressed/with/key/briI0wuHkLJTMEcWOUnXe4";
const char* eventName2 = "/trigger/button_released/with/key/briI0wuHkLJTMEcWOUnXe4";

void button_pressed();
void button_released();
void connectToWifi();
 
void setup() {

// pinMode(D6,OUTPUT);
// digitalWrite(D6, LOW);

FastLED.addLeds<NEOPIXEL, DATA_PIN>(leds, NUM_LEDS);  // GRB ordering is assumed
delay(50);
 
//Porta serial configurada em 115200 baud rate.
Serial.begin(74880);
delay(10);
 
// set button pin as an input
pinMode(BUTTON_PIN, INPUT);
 
connectToWifi();
}
void loop()
{
 
//Assim que o botao for pressionado o SMS é disparado.
if(digitalRead(BUTTON_PIN) == LOW){
  button_pressed();
  delay(5000);
  button_released();
}

coracao(32000);
FastLED.show();

}

void connectToWifi()
{
 
//Serial monitor indica comunicacao entre Wi-Fi e ESP8266.
Serial.print("Conectado em:");
Serial.println(ssid);
WiFi.begin(ssid, password);
 
while (WiFi.status() != WL_CONNECTED) {
delay(500);
Serial.print(".");
}
 
Serial.println("");
Serial.println("WiFi conectado");
Serial.println("Endereco IP: ");
Serial.println(WiFi.localIP());
}

void button_pressed()
{
// Turn the LED on, then pause
for (size_t i = 0; i < NUM_LEDS/2; i++)
{
  leds[i] = CRGB::Red; //cor fabi 123457
  leds[7-i] = CRGB::Red;
  FastLED.show();
  delay(100);
}

Serial.print("conectado em: ");
Serial.println(host);
WiFiClient client;
 
//Estabelece comunicacao através da porta 80 - HTTP.
const int httpPort = 80;
//Caso a conexao nao seja estabelecida entre ESP82666 e IFTTT.
if (!client.connect(host, httpPort)) {
Serial.println("Falha de Conexao");
return;
}
//Dispara eventName através da chave de comunicacao do IFTTT.
String url = eventName1;
 
Serial.print("URL: ");
Serial.println(url);
client.print(String("GET ") + url + " HTTP/1.1\r\n" +
"Host: " + host + "\r\n" +
"Conexao: Fechada\r\n\r\n");
int timeout = millis() + 5000;
while (client.available() == 0) {
if (timeout - millis() < 0) {
Serial.println(">>> Tempo Excedido !");
client.stop();
return;
}
}
while(client.available()){
String line = client.readStringUntil('\r');
Serial.print(line);
}
Serial.println();
Serial.println("Fechando Conexao");
 
delay(1000);
}

void button_released()
{

Serial.print("conectado em: ");
Serial.println(host);
WiFiClient client;
 
//Estabelece comunicacao através da porta 80 - HTTP.
const int httpPort = 80;
//Caso a conexao nao seja estabelecida entre ESP82666 e IFTTT.
if (!client.connect(host, httpPort)) {
Serial.println("Falha de Conexao");
return;
}
//Dispara eventName através da chave de comunicacao do IFTTT.
String url = eventName2;

Serial.print("URL: ");
Serial.println(url);
client.print(String("GET ") + url + " HTTP/1.1\r\n" +
"Host: " + host + "\r\n" +
"Conexao: Fechada\r\n\r\n");
int timeout = millis() + 5000;
while (client.available() == 0) {
if (timeout - millis() < 0) {
Serial.println(">>> Tempo Excedido !");
client.stop();
return;
}
}
while(client.available()){
String line = client.readStringUntil('\r');
Serial.print(line);
}
Serial.println();
Serial.println("Fechando Conexao");
 
delay(1000);
// Now turn the LED off, then pause
for (size_t i = 0; i < NUM_LEDS/2; i++)
{
  leds[i] = CRGB::Black;
  leds[7-i] = CRGB::Black;
  FastLED.show();
  delay(100);
} 
}

void coracao(uint16_t color256) 
{
  static uint16_t sPseudotime = 0;
  static uint16_t sLastMillis = 0;
 
  uint8_t sat8 = beatsin88( 87, 220, 250);
  uint8_t brightdepth = beatsin88( 341, 96, 254);
  uint16_t brightnessthetainc16 = beatsin88( 203, (25 * 256), (40 * 256));
  uint8_t msmultiplier = beatsin88(147, 23, 60);

  uint16_t hue16 = color256;
  
  uint16_t ms = millis();
  uint16_t deltams = ms - sLastMillis ;
  sLastMillis  = ms;
  sPseudotime += deltams * msmultiplier;
  uint16_t brightnesstheta16 = sPseudotime;
  
  for( uint16_t i = 0 ; i < NUM_LEDS; i++) {
    uint8_t hue8 = hue16 / 256;

    brightnesstheta16  += brightnessthetainc16;
    uint16_t b16 = sin16( brightnesstheta16  ) + 32768;

    uint16_t bri16 = (uint32_t)((uint32_t)b16 * (uint32_t)b16) / 65536;
    uint8_t bri8 = (uint32_t)(((uint32_t)bri16) * brightdepth) / 65536;
    bri8 += (255 - brightdepth);
    
    CRGB newcolor = CHSV( hue8, sat8, bri8);
    
    uint16_t pixelnumber = i;
    pixelnumber = (NUM_LEDS-1) - pixelnumber;
    
    nblend( leds[pixelnumber], newcolor, 64);
  }
}