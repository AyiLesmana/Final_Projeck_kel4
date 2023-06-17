// Blynk Authentication
#define BLYNK_TEMPLATE_ID "TMPL6EwKOEBnr"
#define BLYNK_TEMPLATE_NAME "Test Asap"
#define BLYNK_AUTH_TOKEN "Izfya3DGmK9jjWjJhFk8wjXwpOuAENGY" 

#include <WiFi.h>
#include <WifiClient.h>
#include <BlynkSimpleEsp32.h>
#include <HTTPClient.h>


#define BLYNK_PRINT Serial
// Konfigurasi Wi-Fi
const char* ssid = "TP-Link_8B92";
const char* password = "92015678";

BlynkTimer timer;

String url;

WiFiClient client;

// Pin ESP32

#define BUZZER_PIN 25
#define GAS_PIN 34
#define LED_MERAH 35
#define LED_HIJAU 32
#define TEMP_PIN 26

// Nilai ambang batas deteksi gas
int gasThreshold = 0;
float temperature = 0.0;

void setup() {
  Serial.begin(9600);
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

  temperature = analogRead(TEMP_PIN);
  Blynk.virtualWrite(V1, temperature);
  Serial.print("Temperature : ");
  Serial.print(temperature);
  Serial.print(" °C\t");

  if (gasThreshold > 800) {
    Serial.println("Gas Terdeteksi"); // Nyalakan buzzer
    tone(BUZZER_PIN, 1200, 800);
    digitalWrite(LED_MERAH, HIGH);
    digitalWrite(LED_HIJAU, LOW);
    kirimwa("Bahaya!!! Telah ditemukan asap di tempat anda. Segera Evakuasi dari tempat tersebut.");

  } else if (temperature >= 50){
    Serial.println("Suhu Tinggi");
    tone(BUZZER_PIN, 1200, 800);
    digitalWrite(LED_MERAH, HIGH);
    digitalWrite(LED_HIJAU, LOW);
    kirimwa("Peringatan!!! Suhu di tempat anda mencapai " + String(temperature) + " °C. Segera Ambil Tindakan.");

  }else {
    Serial.println("Gas Tidak Terdeteksi / Suhu Normal");
    noTone(BUZZER_PIN); // Matikan buzzer
    digitalWrite(LED_MERAH, LOW);
    digitalWrite(LED_HIJAU, HIGH);
    
  }

  delay(5000); // Delay antara pembacaan sensor
}

float readTemperature(){
  int sensorValue = analogRead(TEMP_PIN);
  float voltage = sensorValue * (3.3 / 4095); 
  float temperature = voltage * (10 + 0.0);
  return temperature;

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









