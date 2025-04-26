#include <Wire.h>
#include <Adafruit_SSD1306.h>
#include <Adafruit_GFX.h>
#include <WiFi.h>
#include <DHT.h>

// Definícia pinov
#define DHTPIN 4           // Pin pre DHT11
#define DHTTYPE DHT11      // Typ senzora DHT11
DHT dht(DHTPIN, DHTTYPE);

// OLED nastavenia
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

// WiFi nastavenia
const char* ssid = "Tvoje_SSID";
const char* password = "Tvoje_Heslo";

// Funkcia na zistenie posledného čísla v IP adrese
String getLastOctetOfIP(IPAddress ip) {
  return String(ip[3]);
}

void setup() {
  // Spustenie sériovej komunikácie
  Serial.begin(115200);

  // Pripojenie k Wi-Fi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Pripájanie k Wi-Fi...");
  }
  Serial.println("Pripojený k Wi-Fi");

  // Inicializácia OLED displeja
  if(!display.begin(SSD1306_I2C_ADDRESS, SCREEN_WIDTH, SCREEN_HEIGHT, &Wire)) {
    Serial.println(F("Chyba pri inicializácii OLED"));
    while(true);
  }

  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0,0);

  // Inicializácia DHT senzorov
  dht.begin();
}

void loop() {
  // Získanie teploty a vlhkosti z DHT11
  float temp = dht.readTemperature();
  float humidity = dht.readHumidity();

  // Monitorovanie pripojených zariadení cez softAP režim
  int currentConnectedDevices = WiFi.softAPgetStationNum();

  // Získanie údajov o prenosoch cez Wi-Fi (Rx a Tx)
  long currentRxBytes = WiFi.getRxBytes(); // Prijaté bajty
  long currentTxBytes = WiFi.getTxBytes(); // Odoslané bajty

  // Výpočet spotrebovaných MB
  float rxMB = currentRxBytes / 1024.0 / 1024.0;
  float txMB = currentTxBytes / 1024.0 / 1024.0;

  // Hľadanie zariadenia s najväčším prenosom
  IPAddress deviceIP = WiFi.softAPIP(); // Príklad IP zariadenia (môžete to upravit pre reálny scenár)
  String lastOctet = getLastOctetOfIP(deviceIP); // Získanie posledného čísla v IP adrese

  // Vypísať na OLED displej
  display.clearDisplay();
  display.setCursor(0, 0);
  display.print("Aktuálny prenos: ");
  display.print(rxMB + txMB, 2);
  display.print(" MB");

  display.setCursor(0, 10);
  display.print("Zariadenie: Xiaomi ");
  display.print(rxMB, 2);
  display.print(" MB");

  display.setCursor(0, 20);
  display.print("IP Zariadenia: ");
  display.print(deviceIP);
  
  display.setCursor(0, 30);
  display.print("Počet zariadení: ");
  display.print(currentConnectedDevices);

  display.display();

  delay(2000); // Aktualizácia každé 2 sekundy
}
