# soil-air-monitor: 🌿 Smart Agriculture Monitoring System using ESP32, DHT11, DS18B20, BH1750, and ThingSpeak
https://mehedi-hasan-soft-web-support.github.io/soil-air-monitor/
# 🌿 Smart Agriculture Monitoring System using ESP32, DHT11, DS18B20, BH1750, and ThingSpeak

## 📜 Description

This project implements a complete **IoT-based Smart Agriculture Monitoring System** using an ESP32 microcontroller. It continuously monitors:

- 🌡️ **Air temperature and humidity** using **DHT11**
- 🌱 **Soil temperature** using **DS18B20**
- 💧 **Soil moisture** using a capacitive analog sensor
- ☀️ **Ambient light intensity** using **BH1750**

The system displays the readings on a 16x2 I2C LCD, and sends the data to **ThingSpeak Cloud** every 15 seconds. A **buzzer and LED** provide real-time alerts for successful or failed data uploads.

---

## 🧠 Features

- 📶 WiFi-enabled ESP32
- 🌡️ Measures both air and soil temperature
- 💧 Monitors humidity and soil moisture level
- ☀️ Reads ambient light level
- 📊 Sends data to [ThingSpeak IoT Cloud](https://thingspeak.com/)
- 🖥️ Displays values on I2C LCD in 3 rotating modes
- 🔔 Alerts via buzzer and LED for upload status
- ✅ Sensor validation, fallback values, and error handling

---

## 🔌 Components Used

| Component         | Model / Notes                       |
|------------------|--------------------------------------|
| Microcontroller   | ESP32 Dev Module                    |
| Temp & Humidity   | DHT11                               |
| Soil Temperature  | DS18B20                             |
| Soil Moisture     | Capacitive Analog Soil Sensor       |
| Light Sensor      | BH1750 (I2C)                         |
| Display           | 16x2 I2C LCD (`0x27` address)        |
| Buzzer & LED      | Standard active buzzer + LED         |
| Cloud Platform    | ThingSpeak                          |

---

## 🧰 Pin Connections

| ESP32 Pin   | Sensor / Module          |
|-------------|--------------------------|
| GPIO4       | DHT11 Data               |
| GPIO5       | Buzzer                   |
| GPIO2       | LED                      |
| GPIO36 (A0) | Soil Moisture (Analog)   |
| GPIO18      | DS18B20                  |
| GPIO21      | SDA (I2C - BH1750 & LCD) |
| GPIO22      | SCL (I2C - BH1750 & LCD) |

---

## 🌐 ThingSpeak Setup

1. Sign up at [ThingSpeak](https://thingspeak.com/).
2. Create a new channel and enable 6 or more fields.
3. Replace the following in your code:
   ```cpp
   const char* ssid = "Your_WiFi_Name";
   const char* password = "Your_WiFi_Password";
   unsigned long channelID = YOUR_CHANNEL_ID;
   const char* writeAPIKey = "YOUR_WRITE_API_KEY";
