// Blynk Authentication
#define BLYNK_TEMPLATE_ID "TMPL6EwKOEBnr"
#define BLYNK_TEMPLATE_NAME "Test Asap"
#define BLYNK_AUTH_TOKEN "Izfya3DGmK9jjWjJhFk8wjXwpOuAENGY" 

#include <WiFi.h>
#include <WifiClient.h>
#include <BlynkSimpleEsp32.h>
#include <HTTPClient.h>
#include <SPI.h>
#include <Wire.h>
#include "DHT.h"
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);



#define BLYNK_PRINT Serial

// Konfigurasi Wi-Fi
const char* ssid = "TP-Link_8B92";
const char* password = "92015678";

BlynkTimer timer;

String url;

WiFiClient client;

// Pin ESP32

#define BUZZER_PIN 33
#define GAS_PIN 32
#define LED_MERAH 26
#define LED_HIJAU 25
#define DHT11PIN 4

DHT dht(DHT11PIN, DHT11);

// Nilai ambang batas deteksi gas
int gasThreshold = 0;

char _buffer[8];

const unsigned char epd_bitmap_Untitled [] PROGMEM = {
  0xff, 0xff, 0xff, 0xfc, 0xff, 0x80, 0x03, 0xfc, 0xff, 0x00, 0x01, 0xfc, 0xfe, 0x3f, 0xf8, 0xfc,
  0xfe, 0x78, 0x78, 0xfc, 0xfe, 0x78, 0x78, 0xfc, 0xfe, 0x7c, 0xf8, 0xfc, 0xfe, 0x7c, 0x78, 0xfc,
  0xfe, 0x7c, 0x78, 0xfc, 0xfc, 0x00, 0x00, 0xfc, 0xf0, 0x00, 0x00, 0x3c, 0xe0, 0x00, 0x00, 0x1c,
  0xe0, 0x00, 0x00, 0x1c, 0xc0, 0x00, 0x00, 0x0c, 0xc0, 0x00, 0x00, 0x0c, 0xc0, 0x00, 0x00, 0x0c,
  0xc0, 0x00, 0x00, 0x0c, 0xc0, 0x00, 0x00, 0x0c, 0xc0, 0x00, 0x00, 0x0c, 0xc0, 0x03, 0x80, 0x0c,
  0xc0, 0x07, 0xc0, 0x0c, 0xc0, 0x0c, 0xe0, 0x0c, 0xc0, 0x18, 0x70, 0x0c, 0xc0, 0x38, 0x38, 0x0c,
  0xc0, 0x70, 0x18, 0x0c, 0xc0, 0x60, 0x0c, 0x0c, 0xc0, 0xc0, 0x0c, 0x0c, 0xc1, 0xc0, 0x06, 0x0c,
  0xc1, 0x80, 0x06, 0x0c, 0xc1, 0x80, 0x03, 0x0c, 0xc1, 0x00, 0x03, 0x0c, 0xc3, 0x00, 0x03, 0x0c,
  0xc3, 0x00, 0x03, 0x0c, 0xc1, 0x80, 0x03, 0x0c, 0xc1, 0x80, 0x07, 0x0c, 0xc1, 0xc0, 0x06, 0x0c,
  0xc0, 0xc0, 0x0c, 0x0c, 0xc0, 0x70, 0x3c, 0x0c, 0xc0, 0x38, 0x70, 0x0c, 0xc0, 0x1f, 0xe0, 0x0c,
  0xc0, 0x07, 0x80, 0x0c, 0xc0, 0x00, 0x00, 0x0c, 0xc0, 0x00, 0x00, 0x0c, 0xc0, 0x79, 0x38, 0x0c,
  0xc0, 0x41, 0xb0, 0x0c, 0xc0, 0x5b, 0x98, 0x0c, 0xc0, 0x4a, 0x44, 0x0c, 0xc0, 0x3c, 0x78, 0x0c,
  0xc0, 0x00, 0x00, 0x0c, 0xc0, 0x00, 0x00, 0x0c, 0xc0, 0x00, 0x00, 0x0c, 0xc0, 0x00, 0x00, 0x0c, 
  0xe0, 0x00, 0x00, 0x1c, 0xe0, 0x00, 0x00, 0x1c, 0xf0, 0x00, 0x00, 0x3c, 0xff, 0xff, 0xff, 0xfc, 
  0xfc, 0x00, 0x01, 0xfc, 0xfc, 0x00, 0x01, 0xfc, 0xff, 0xff, 0xff, 0xfc, 0xff, 0xff, 0xff, 0xfc

};

const int epd_bitmap_allArray_LEN = 1 ;
const unsigned char* epd_bitmap_allArray[1] = {
  epd_bitmap_Untitled
};

uint8_t buffer[SCREEN_WIDTH * SCREEN_HEIGHT / 8];

void setup() {
  Serial.begin(9600);

  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  display.clearDisplay();

  dht.begin();

  pinMode(GAS_PIN, INPUT); 

  pinMode(BUZZER_PIN, OUTPUT);
  digitalWrite(BUZZER_PIN, LOW);
  
  pinMode(LED_MERAH, OUTPUT);
  digitalWrite(LED_MERAH, LOW);

  pinMode(LED_HIJAU, OUTPUT);
  digitalWrite(LED_HIJAU, LOW);

  // Mulai koneksi Wi-Fi
  Serial.println();
  Serial.println();
  Serial.println("Connecting To ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi Connected");
  Serial.println("IP Address : ");
  Serial.println(WiFi.localIP());

  // Mulai koneksi ke Blynk
  Blynk.begin(BLYNK_AUTH_TOKEN, ssid, password);
  timer.setInterval(1000L, getSendData);
}
  

void loop() {

  Blynk.run();
  timer.run();
}
void getSendData(){

  gasThreshold = analogRead(GAS_PIN);
  Blynk.virtualWrite(V0, gasThreshold);
  Serial.print("Nilai Gas :");
  Serial.print(gasThreshold);
  Serial.print("\t");

  int humi = dht.readHumidity();
  int temp = dht.readTemperature();
  Blynk.virtualWrite(V2, temp);
  Serial.print("Temperature: ");
  Serial.print(temp);
  Serial.print("ºC ");
  Serial.print("Humidity: ");
  Serial.println(humi);
  delay(1000);

display.clearDisplay();

  if (gasThreshold > 800) {
    Serial.println("Gas Terdeteksi"); // Nyalakan buzzer
    tone(BUZZER_PIN, 1200, 800);
    digitalWrite(LED_MERAH, HIGH);
    digitalWrite(LED_HIJAU, LOW);
    delay(500);
    kirimwa("Bahaya!!! Telah ditemukan gas di tempat anda. \n Nilai Gas : " + String(gasThreshold) + "\n     Segera Evakuasi dari tempat tersebut.");
    display.setTextSize(3);
    display.setTextColor(WHITE);
    display.setCursor(11,4);
    display.println("DANGER"); 

    display.setTextSize(2);
    display.setTextColor(WHITE);
    display.setCursor(11,32);
    display.println("Gas High");

    display.display();

  } else if (temp >= 35){
    Serial.println("Suhu Tinggi");
    tone(BUZZER_PIN, 1200);
    digitalWrite(LED_MERAH, HIGH);
    digitalWrite(LED_HIJAU, LOW);
    delay(500);
    kirimwa("Peringatan!!! Suhu di tempat anda mencapai " + String(temp) + " °C. Segera Ambil Tindakan.");

  }else {
    Serial.println("Gas Tidak Terdeteksi / Suhu Normal");
    noTone(BUZZER_PIN); // Matikan buzzer
    digitalWrite(LED_MERAH, LOW);
    digitalWrite(LED_HIJAU, HIGH);

 
  display.fillRect(64, 10, 62, 20, BLACK);
  display.display();
  display.setTextSize(2);
  display.setTextColor(WHITE);
  display.setCursor(64,10);
  display.print(gasThreshold);
  display.display();

  display.drawBitmap(10,4, epd_bitmap_Untitled,30,60,1);
  display.display();

  display.drawLine(64,32,124,32, WHITE);
  display.display();

  display.setTextSize(2);
  display.setTextColor(WHITE);
  display.setCursor(64,40);
  display.print(temp);
  display.setCursor(80,40);
  display.print(" C");
  display.display();
    
  }

  delay(300); // Delay antara pembacaan sensor

}


void kirimwa(String pesan){
 
  url = "http://api.callmebot.com/whatsapp.php?phone=6281944315336&text="+urlencode(pesan)+"&apikey=8116944";
  postData();
}

void postData(){
  int httpCode;
  HTTPClient http;
  http.begin(client, url);
  httpCode = http.POST(url);
  if(httpCode==200){
    Serial.println("Notifikasi WhatsApp Berhasil Terkirim");
  }
  else{
    Serial.println("Notifikasi WhatsApp Gagal Terkirim");
  }
  http.end();
}


String urlencode(String str){

  String encodedString="";
  char c;
  char code0, code1, code2;
  for(int i=0; i<str.length(); i++){
    c = str.charAt(i);
    if(c == ' '){
      encodedString += '+';
    }
    else if(isalnum(c)){
      encodedString += c;
    }
    else{
      code1 = (c & 0xf) + '0';
      if((c & 0xf) > 9) {
        code1 = (c & 0xf) - 10 + 'A';
      }
      c = (c>>4)&0xf;
      code0 = c+'0';
      if(c > 9){
        code0 = c -10 + 'A';
      }
      code2 = '\0';
      encodedString += '%';
      encodedString += code0;
      encodedString += code1;
    }
    yield();
      
  }
  Serial.println(encodedString);
  return encodedString;
}
