#include <WiFi.h>
#include <HTTPClient.h>
#include <IRremoteESP8266.h>
#include <ESPmDNS.h>
#include <IRrecv.h>
#include <IRutils.h>

// const char* ssid = "TIM-24326654";// soggiorno
const char* ssid = "TIM-24326654_EXT";// notte
// const char* ssid = "TIM-24326654_TENDA";//tavernetta
const char* password = "T9ZDHXACUfdTUC33DcTCASsz";
const String serverPath = "http://myhomesmart.altervista.org/";
#define ADDR "tappa" 
const uint16_t IR_PIN = 14;
IRrecv irrecv(IR_PIN);decode_results results;

// Rimuoviamo la costante fissa myboard perché la passeremo dinamicamente
// const String myboard = ""; 

void update_tappa(String protocollo, String mycommand) {
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
    String fullURL = serverPath + "update_tappa.php";
    http.begin(fullURL);
    http.addHeader("Content-Type", "application/x-www-form-urlencoded");

    // Ora 'board' conterrà il nome del protocollo del telecomando
    String postData = "board=" + protocollo + "&command=" + mycommand;
    
    Serial.print("HTTP postData: ");
    Serial.println(postData);

    int httpCode = http.POST(postData);
    String payload = http.getString();

    Serial.print("HTTP Response Code: ");
    Serial.println(httpCode);
    Serial.print("Risposta Server: ");
    Serial.println(payload);
    http.end();
  } else {
    Serial.println("WiFi disconnesso, impossibile inviare dati.");
  }
}

void setup() {
  Serial.begin(115200);
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
      Serial.print("Protocollo: ");
      Serial.println(remote_type);
      Serial.print("Comando IR: 0x");
      Serial.println(final_command, HEX);

      // Invio al Database: passiamo sia il protocollo che il comando
      update_tappa(remote_type, String(final_command, HEX));
      
      // Il delay di 5 secondi impedisce invii multipli accidentali
      delay(5000);
    }

    irrecv.resume(); // Ricevi il prossimo segnale
  }
}
