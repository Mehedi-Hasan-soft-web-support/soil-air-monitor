# soil-air-monitor: 🌿 Smart Agriculture Monitoring System using ESP32, DHT11, DS18B20, BH1750, and ThingSpeak
https://mehedi-hasan-soft-web-support.github.io/soil-air-monitor/

## 🌐 Live Web Preview

[![Live Preview](https://img.shields.io/badge/View%20Live%20Dashboard-blue?style=for-the-badge&logo=google-chrome)](https://mehedi-hasan-soft-web-support.github.io/soil-air-monitor/)


## 📲 How to Use

1. **Wire all sensors and modules** according to the pin connection table above.
2. **Install required libraries** via the Arduino Library Manager:
   - `DHT sensor library` by Adafruit
   - `Adafruit Unified Sensor`
   - `DallasTemperature`
   - `OneWire`
   - `BH1750`
   - `LiquidCrystal_I2C`
   - `ThingSpeak`
3. **Upload the code** to your ESP32 board.
4. **Open Serial Monitor** at `115200` baud rate.
5. **View live data on**:
   - 🖥️ **Serial Monitor**
   - 📟 **LCD Display** (3 rotating sensor modes)
   - 🌐 **ThingSpeak Dashboard**

---

## 📷 Screenshots *(Optional)*

You can add screenshots such as:

- 🔌 **Wiring Diagram**
- 📟 **LCD Display Output**
- 📈 **ThingSpeak Channel Graphs**
- 🖥️ **Serial Monitor Output**

---

## 🛠️ Future Improvements

- 🔁 Replace **DHT11** with **DHT22** or **BME280** for improved accuracy and reliability.
- ☔ Add a **rainfall sensor** and integrate **OpenWeatherMap** API.
- 📡 Implement **MQTT protocol** for real-time updates to platforms like Node-RED or Home Assistant.
- 📱 Create a **mobile app dashboard** using **Blynk**, **MIT App Inventor**, or **Node-RED dashboard**.

---

## 📃 License

This project is licensed under the **MIT License**.

> Feel free to use, modify, and distribute with proper attribution.

---

**Made with 💡 by Mehedi Hasan**
