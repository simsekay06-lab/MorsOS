# 🕵️‍♂️ MorsOS

> A discreet, screenless multi-tool and Morse code communication device powered by an **ESP32-C3 Super Mini**, designed for mobile-restricted environments.

---

## ✨ Features

- **Screenless Interface:** Minimalist design interacting solely through a built-in LED and a single tactile `BOOT` button using Morse code ritim/patterns.
- **5 Versatile Operating Modes:**
  1. **Stopwatch (S):** Measures elapsed time and transmits the result back via Morse code.
  2. **Wi-Fi Scanner (W):** Scans surrounding wireless networks and transmits the total count.
  3. **Dice Roller (Z):** Generates a random number (1-6) like a digital dice.
  4. **Wi-Fi Beacon / Captive Portal (B):** Broadcasts an open access point (`MorsOS_C3_Unit`) equipped with a custom retro **Rickroll** captive portal and browser-based synthesizer.
  5. **Morse Trainer (E):** An interactive tutor that generates random letters, challenges you to input the correct Morse sequence, and provides feedback.

---

## 🛠️ Hardware Requirements

- **Microcontroller:** ESP32-C3 Super Mini
- **Power Source:** Portable Powerbank (for mobile deployment)
- **Interface:** Built-in LED (GPIO 8) & BOOT Button (GPIO 9)

---

## 🚀 Getting Started

1. Clone or download this repository.
2. Open the `.ino` file using **Arduino IDE**.
3. Ensure you have the **ESP32 Board Package** installed and select your ESP32-C3 board.
4. Upload the sketch to your device.

---

## 🎧 The Rickroll Captive Portal
When activated in **Beacon Mode**, any mobile device connecting to the `MorsOS_C3_Unit` network will be automatically redirected to a retro terminal interface. Tapping the button triggers a browser-based Web Audio API synthesizer playing the legendary tune!

---

## 📜 License
This project is open-source and available under the [MIT License](LICENSE).
