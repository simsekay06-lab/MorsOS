#include <WiFi.h>
#include <WebServer.h>
#include <DNSServer.h>

WebServer server(80);
DNSServer dnsServer;
const byte DNS_PORT = 53;

// Captive Portal için Rickroll Web Sayfası
const char RICKROLL_PAGE[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html lang="tr">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>RICKROLLED!</title>
    <style>
        body {
            background-color: #0b0b0b;
            color: #00ff66;
            font-family: 'Courier New', Courier, monospace;
            text-align: center;
            padding-top: 50px;
            margin: 0;
        }
        h1 {
            font-size: 2.5rem;
            color: #ff0055;
            text-shadow: 0 0 10px #ff0055;
        }
        p {
            font-size: 1.2rem;
            color: #fff;
        }
        .box {
            border: 2px dashed #00ff66;
            padding: 20px;
            display: inline-block;
            margin-top: 20px;
            background: #111;
            box-shadow: 0 0 20px rgba(0, 255, 102, 0.2);
        }
        button {
            background: #00ff66;
            color: #000;
            border: none;
            padding: 14px 28px;
            font-size: 1.2rem;
            font-weight: bold;
            cursor: pointer;
            margin-top: 20px;
            border-radius: 4px;
        }
        button:hover {
            background: #ff0055;
            color: #fff;
        }
    </style>
</head>
<body>
    <div class="box">
        <h1>LOL 2026 RICKROLLED LANDIN!</h1>
        <p>Okulda serbest takılalım dedin ama... <br><b>Never gonna give you up!</b></p>
        <p style="font-size: 0.9rem; color: #888;">MorsOS Ajan Ağındasın.</p>
        <button onclick="playRickroll()">Bağlantıyı kes</button>
    </div>

    <script>
        function playRickroll() {
            const AudioContext = window.AudioContext || window.webkitAudioContext;
            if (!AudioContext) {
                alert("Tarayıcınız ses motorunu desteklemiyor.");
                return;
            }
            const ctx = new AudioContext();

            // Never Gonna Give You Up - Giriş Melodisi Notaları (Frekans ve Süre)
            // D, E, G, E, B, B, A ...
            const notes = [
                {f: 293.66, d: 200}, // D4
                {f: 329.63, d: 200}, // E4
                {f: 392.00, d: 200}, // G4
                {f: 329.63, d: 200}, // E4
                {f: 493.88, d: 400}, // B4
                {f: 493.88, d: 400}, // B4
                {f: 440.00, d: 600}, // A4
                
                {f: 293.66, d: 200}, // D4
                {f: 329.63, d: 200}, // E4
                {f: 392.00, d: 200}, // G4
                {f: 329.63, d: 200}, // E4
                {f: 523.25, d: 400}, // C5
                {f: 493.88, d: 400}, // B4
                {f: 440.00, d: 600}  // A4
            ];

            let time = ctx.currentTime;
            notes.forEach(note => {
                let osc = ctx.createOscillator();
                let gain = ctx.createGain();

                osc.type = 'square'; // Klasik 8-bit / dijital tat
                osc.frequency.setValueAtTime(note.f, time);

                gain.gain.setValueAtTime(0.1, time);
                gain.gain.exponentialRampToValueAtTime(0.001, time + (note.d / 1000) - 0.05);

                osc.connect(gain);
                gain.connect(ctx.destination);

                osc.start(time);
                osc.stop(time + (note.d / 1000));

                time += note.d / 1000 + 0.05; // Nota arası minik boşluk
            });
        }
    </script>
</body>
</html>
)rawliteral";

void handleRoot() {
  server.send(200, "text/html", RICKROLL_PAGE);
}

void handleNotFound() {
  server.sendHeader("Location", String("http://192.168.4.1/"), true);
  server.send(302, "text/plain", "");
}

const int LED_PIN = 8;   
const int BTN_PIN = 9;   // Kart üzerindeki BOOT butonu
const bool LED_INVERTED = true; 

const int DOT_MS = 400;
const int DASH_MS = DOT_MS * 3;

int currentMode = 0;
const int TOTAL_MODES = 5;
const char modeChars[] = "SWZBE"; 
const char* modeNames[] = {
  "S - Sayac / Kronometre",
  "W - Wi-Fi Tarayici",
  "Z - Zar Atma",
  "B - Wi-Fi Beacon (Yayin)",
  "E - Mors Egitmeni"
};

const char* morseLetters[] = {
  ".-", "-...", "-.-.", "-..", ".", "..-.", "--.", "....", "..", ".---", // A-J
  "-.-", ".-..", "--", "-.", "---", ".--.", "--.-", ".-.", "...", "-",  // K-T
  "..-", "...-", ".--", "-..-", "-.--", "--.."                         // U-Z
};
const char* morseNumbers[] = {
  "-----", ".----", "..---", "...--", "....-", // 0-4
  ".....", "-....", "--...", "---..", "----."  // 5-9
};

void setLed(bool on) {
  if (LED_INVERTED) digitalWrite(LED_PIN, on ? LOW : HIGH);
  else digitalWrite(LED_PIN, on ? HIGH : LOW);
}

void blinkCode(const char* code) {
  Serial.print("   [LED Mors]: ");
  Serial.println(code);
  
  for (int i = 0; code[i] != '\0'; i++) {
    setLed(true);
    delay(code[i] == '.' ? DOT_MS : DASH_MS);
    setLed(false);
    delay(DOT_MS);
  }
  delay(DOT_MS * 2);
}

void sendMorseChar(char c) {
  c = toupper(c);
  Serial.print("-> Karakter Okunuyor: ");
  Serial.println(c);
  
  if (c >= 'A' && c <= 'Z') blinkCode(morseLetters[c - 'A']);
  else if (c >= '0' && c <= '9') blinkCode(morseNumbers[c - '0']);
}

void sendMorseText(String text) {
  Serial.print(">> Metin Okunuyor: ");
  Serial.println(text);
  for (int i = 0; i < text.length(); i++) {
    if (text[i] == ' ') delay(DOT_MS * 4);
    else sendMorseChar(text[i]);
  }
}

int readButton() {
  if (digitalRead(BTN_PIN) == LOW) {
    unsigned long pressTime = millis();
    while (digitalRead(BTN_PIN) == LOW) {
      delay(10);
    }
    unsigned long duration = millis() - pressTime;
    if (duration > 50 && duration < 500) {
      Serial.println("[BUTON] Kisa Basis (.)");
      return 1;
    }
    if (duration >= 500) {
      Serial.println("[BUTON] Uzun Basis (-)");
      return 2;
    }
  }
  return 0;
}

// --- MOD FONKSİYONLARI ---

void runStopwatch() {
  Serial.println("\n--- SAYAC BASLADI (Durdurmak icin butona bas) ---");
  setLed(true); delay(80); setLed(false); delay(80);
  setLed(true); delay(80); setLed(false);
  
  unsigned long start = millis();
  while (digitalRead(BTN_PIN) == HIGH) {
    delay(10);
  }
  while (digitalRead(BTN_PIN) == LOW) delay(10);
  
  int seconds = (millis() - start) / 1000;
  Serial.print("Gecen Sure: ");
  Serial.print(seconds);
  Serial.println(" saniye.");
  
  delay(400);
  sendMorseText(String(seconds));
}

void runWifiScanner() {
  Serial.println("\n--- WI-FI TARANIYOR ---");
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();
  delay(100);
  int n = WiFi.scanNetworks();
  
  Serial.print("Bulunan Ag Sayisi: ");
  Serial.println(n);
  for (int i = 0; i < n; i++) {
    Serial.print("  ");
    Serial.print(i + 1);
    Serial.print(": ");
    Serial.print(WiFi.SSID(i));
    Serial.print(" (");
    Serial.print(WiFi.RSSI(i));
    Serial.println(" dBm)");
  }
  
  sendMorseText(String(n));
}

void runDice() {
  int roll = random(1, 7);
  Serial.print("\n--- ZAR ATILDI: ");
  Serial.print(roll);
  Serial.println(" ---");
  sendMorseText(String(roll));
}

bool apActive = false;
void toggleWifiBeacon() {
  if (!apActive) {
    WiFi.mode(WIFI_AP);
    WiFi.softAP("MorsOS_C3_Unit");
    
    dnsServer.start(DNS_PORT, "*", WiFi.softAPIP());
    server.on("/", HTTP_GET, handleRoot);
    server.on("/favicon.ico", []() { server.send(204); });
    server.onNotFound(handleNotFound);
    server.begin();

    apActive = true;
    Serial.println("\n--- RICKROLLED CAPTIVE PORTAL AKTIF (SSID: MorsOS_C3_Unit) ---");
    sendMorseText("ON");
  } else {
    dnsServer.stop();
    server.stop();
    WiFi.softAPdisconnect(true);
    apActive = false;
    Serial.println("\n--- WI-FI YAYINI KAPATILDI ---");
    sendMorseText("OFF");
  }
}

void runMorseTrainer() {
  char target = 'A' + random(0, 26);
  Serial.println("\n--- MORS EGITMENI ---");
  Serial.print("Hedef Harf: ");
  Serial.print(target);
  Serial.print(" (Beklenen Kod: ");
  Serial.print(morseLetters[target - 'A']);
  Serial.println(")");
  
  sendMorseChar(target);
  Serial.println("Simdi butona basarak kodu gir (2 sn beklenecek)...");
  
  String userInput = "";
  unsigned long lastAction = millis();
  
  while (millis() - lastAction < 2000) {
    int btn = readButton();
    if (btn == 1) {
      userInput += ".";
      Serial.print("Girilen: "); Serial.println(userInput);
      lastAction = millis();
    } else if (btn == 2) {
      userInput += "-";
      Serial.print("Girilen: "); Serial.println(userInput);
      lastAction = millis();
    }
  }
  
  String expected = String(morseLetters[target - 'A']);
  if (userInput == expected) {
    Serial.println("SONUC: DOGRU! Tebrikler.");
    for(int i=0; i<3; i++) { setLed(true); delay(70); setLed(false); delay(70); }
  } else {
    Serial.print("SONUC: YANLIS! Senin girdigin: '");
    Serial.print(userInput);
    Serial.print("' | Dogrusu: '");
    Serial.print(expected);
    Serial.println("'");
    setLed(true); delay(800); setLed(false); delay(400);
    sendMorseChar(target);
  }
}

void setup() {
  Serial.begin(115200);
  pinMode(LED_PIN, OUTPUT);
  pinMode(BTN_PIN, INPUT_PULLUP);
  setLed(false);
  randomSeed(analogRead(0));
  
  delay(1500);
  Serial.println("\n=========================================");
  Serial.println("       MorsOS (Ekransiz M5Stick) Hazir!   ");
  Serial.println("=========================================");
  
  sendMorseText("OK");
  delay(300);
  Serial.print("\nAktif Mod: ");
  Serial.println(modeNames[currentMode]);
  sendMorseChar(modeChars[currentMode]);
}

void loop() {
  if (apActive) {
    dnsServer.processNextRequest();
    server.handleClient();
  }

  int action = readButton();
  
  if (action == 1) { 
    currentMode = (currentMode + 1) % TOTAL_MODES;
    Serial.print("\nMod Degisti -> ");
    Serial.println(modeNames[currentMode]);
    sendMorseChar(modeChars[currentMode]);
  } 
  else if (action == 2) { 
    Serial.print("\nMod Baslatiliyor -> ");
    Serial.println(modeNames[currentMode]);
    delay(200);
    
    if (currentMode == 0) runStopwatch();
    else if (currentMode == 1) runWifiScanner();
    else if (currentMode == 2) runDice();
    else if (currentMode == 3) toggleWifiBeacon();
    else if (currentMode == 4) runMorseTrainer();
    
    // Eğer beacon modu açıksa döngüde server çalışmaya devam etmeli, kapandıysa menüye dön
    if (!apActive) {
      delay(500);
      Serial.print("\nMenuye Donuldu. Aktif Mod: ");
      Serial.println(modeNames[currentMode]);
      sendMorseChar(modeChars[currentMode]);
    }
  }
}
