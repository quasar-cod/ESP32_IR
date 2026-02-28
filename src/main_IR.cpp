#include <WiFi.h>
#include <HTTPClient.h>
#include <IRremoteESP8266.h>
#include <ArduinoJSON.h>
#include <ESPmDNS.h>
#include <IRrecv.h>
#include <IRutils.h>

// const char* ssid = "TIM-24326654";// soggiorno
const char* ssid = "TIM-24326654_EXT";// notte
// const char* ssid = "TIM-24326654_TENDA";//tavernetta
const char* password = "T9ZDHXACUfdTUC33DcTCASsz";
const String serverPath = "http://myhomesmart.altervista.org/";
#define ADDR "IR" 

String action;
String payload;
String postData;
HTTPClient http;
int httpCode;
// activity/board received from server
String last_activity;
String activity;
String board;
const uint16_t IR_PIN = 14;
IRrecv irrecv(IR_PIN);decode_results results;

void get_command(String mycommand) {
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
    String fullURL = serverPath + "get_command.php";
    http.begin(fullURL);
    http.addHeader("Content-Type", "application/x-www-form-urlencoded");

    // Ora 'board' conterrà il nome del protocollo del telecomando
    String postData = "command=" + mycommand;
    Serial.println("---------------");  
    Serial.println("get_command.php");  
    Serial.print("postData: ");
    Serial.println(postData);

    int httpCode = http.POST(postData);
    payload = http.getString();

    Serial.print("httpCode: ");
    Serial.println(httpCode);
    Serial.print("payload: ");
    Serial.println(payload);
    http.end();
  } else {
    Serial.println("WiFi disconnesso, impossibile inviare dati.");
  }
  // allocate a JsonDocument with enough capacity for the expected
  // payload (small, two strings).  Using a local document avoids
  // problems with an uninitialised or zero‑capacity global object.
  JsonDocument doc;
  DeserializationError error = deserializeJson(doc, payload);
  if (error) {
    // print the error to help diagnose failures
    Serial.print(F("Failed to parse JSON: "));
    Serial.println(error.c_str());
    // optionally dump the payload for debugging
    Serial.print(F("raw payload: "));
    Serial.println(payload);
    return;
  }
  board = doc["board"].as<String>();
  activity = doc["activity"].as<String>();
  last_activity = doc["last_activity"].as<String>();
  Serial.print("board: ");
  Serial.println(board);
  Serial.print("activity: ");
  Serial.println(activity);
  Serial.print("last_activity: ");
  Serial.println(last_activity);
}

void update_activity(){
  // compute next action based on last activity; ensure we always set something
  if (last_activity == "UP" || last_activity == "DOWN") {
    action = "OFF";
  } else if (last_activity == "OFF") {
    action = activity;
  } else {
    // fallback in case last_activity was empty or unexpected
    action = activity;
  }
  payload = "";
  postData = "board=";
  postData += board;
  postData += "&activity=";
  postData += action;
  Serial.println("---------------");
  Serial.println("update_activity.php");  
  Serial.println(postData);
  String fullURL = serverPath + "update_activity.php";
  http.begin(fullURL);
  http.addHeader("Content-Type", "application/x-www-form-urlencoded"); 
  httpCode = http.POST(postData);
  payload = http.getString();
  Serial.print("httpCode: ");
  Serial.println(httpCode);
  Serial.print("payload: ");
  Serial.println(payload);
  http.end();
}

void setup() {
  Serial.begin(230400);
  // Connessione WiFi
  WiFi.begin(ssid, password);
  Serial.print("Connessione WiFi in corso");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWiFi Connesso!");
  
  Serial.println("Abilito dns");
  if (MDNS.begin(ADDR)){
    Serial.println("Abilitato");
  }
  
  irrecv.enableIRIn();
  Serial.println("Ricevitore IR pronto.");
}

void loop() {
  if (irrecv.decode(&results)) {
    
    // Otteniamo il nome del protocollo come stringa (es: "RC5", "NEC", "SONY")
    String remote_type = typeToString(results.decode_type);
    uint32_t final_command = 0;

    // Gestione differenziata in base al protocollo
    if (results.decode_type == RC5) {
      // Pulizia Toggle Bit per RC5
      final_command = results.value & 0x7FF; 
    } else {
      // Per altri protocolli (es. quello nuovo che comprerai) prendiamo il valore intero
      final_command = results.value;
    }

    if (final_command != 0 && final_command != 0xFFFFFFFFFFFFFFFF) {
      Serial.println("---------------");
      Serial.print("Protocollo: ");
      Serial.println(remote_type);
      Serial.print("Comando IR: 0x");
      Serial.println(final_command, HEX);

      // Invio al Database: passiamo sia il protocollo che il comando
      get_command(String(final_command, HEX));
      update_activity();
      // Il delay di 5 secondi impedisce invii multipli accidentali
      delay(2000);
    }

    irrecv.resume(); // Ricevi il prossimo segnale
  }
}
