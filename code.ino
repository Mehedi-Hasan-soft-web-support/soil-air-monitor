
#include <WiFi.h>
#include <ThingSpeak.h>
#include <DHT.h>
#include <Wire.h>
#include <BH1750.h>
#include <LiquidCrystal_I2C.h>
#include <OneWire.h>
#include <DallasTemperature.h>

// WiFi credentials
const char* ssid = "Me";
const char* password = "mehedi113";

// ThingSpeak channel details
unsigned long channelID = 3019178;
const char* writeAPIKey = "L57V2MFOMXD5NU3J";

// Pin definitions
#define DHT_PIN 4
#define DHT_TYPE DHT11          // Changed from DHT22 to DHT11
#define LED_PIN 2
#define BUZZER_PIN 5
#define SOIL_MOISTURE_PIN 36    // Analog pin for capacitive soil moisture sensor (A0 = GPIO36)
#define DS18B20_PIN 18          // Digital pin for DS18B20 temperature sensor

// Sensor & Display Setup
DHT dht(DHT_PIN, DHT_TYPE);
BH1750 lightMeter;
LiquidCrystal_I2C lcd(0x27, 16, 2); // LCD address and size

// DS18B20 Setup
OneWire oneWire(DS18B20_PIN);
DallasTemperature ds18b20(&oneWire);

WiFiClient client;

// Timing variables for continuous sensor reading
unsigned long lastReadTime = 0;
const unsigned long readInterval = 3000; // Read sensors every 3 seconds

// DHT11 specific timing (changed from DHT22 timing)
unsigned long lastDHTRead = 0;
const unsigned long dhtInterval = 2000; // DHT11 needs 2 seconds minimum (not 5)

// Timing variables for ThingSpeak updates
unsigned long lastThingSpeakTime = 0;
const unsigned long thingSpeakInterval = 15000; // Update ThingSpeak every 15 seconds

// Display cycling variables
unsigned long lastDisplayChange = 0;
const unsigned long displayInterval = 3000; // Change display every 3 seconds
int displayMode = 0; // 0: Temp/Humidity, 1: Light/Soil, 2: DS18B20/Soil%

// Soil moisture calibration values (adjust based on your sensor)
const int soilMoistureMin = 2800; // Value in air (completely dry) - typical for capacitive sensors
const int soilMoistureMax = 1200;  // Value in water (completely wet) - typical for capacitive sensors

// Variables to store last valid DHT11 readings
float lastValidTemperature = 25.0;  // Default fallback value
float lastValidHumidity = 50.0;     // Default fallback value
bool dhtEverWorked = false;

void setup() {
  Serial.begin(115200);
  Serial.println("\n=== Enhanced IoT Monitoring System ===");
  Serial.println("Starting system with DHT11 sensor...");
  
  // Setup pins
  pinMode(LED_PIN, OUTPUT);
  pinMode(BUZZER_PIN, OUTPUT);
  
  // Initial LED test
  digitalWrite(LED_PIN, HIGH);
  delay(500);
  digitalWrite(LED_PIN, LOW);
  
  // Start sensors and display
  Serial.println("Initializing DHT11...");
  dht.begin();
  delay(1000); // DHT11 stabilization time
  
  // Test DHT11 connection multiple times
  Serial.println("Testing DHT11 connection...");
  bool dhtConnected = false;
  for (int i = 0; i < 5; i++) {
    float testTemp = dht.readTemperature();
    float testHum = dht.readHumidity();
    
    Serial.print("DHT11 test attempt ");
    Serial.print(i + 1);
    Serial.print(": ");
    
    if (!isnan(testTemp) && !isnan(testHum) && testTemp > -40 && testTemp < 80 && testHum >= 0 && testHum <= 100) {
      Serial.println("SUCCESS");
      Serial.print("Temperature: ");
      Serial.print(testTemp);
      Serial.print("°C, Humidity: ");
      Serial.print(testHum);
      Serial.println("%");
      dhtConnected = true;
      dhtEverWorked = true;
      lastValidTemperature = testTemp;
      lastValidHumidity = testHum;
      break;
    } else {
      Serial.println("FAILED");
      Serial.print("Raw values - Temp: ");
      Serial.print(testTemp);
      Serial.print(", Hum: ");
      Serial.println(testHum);
    }
    delay(2000);
  }
  
  if (!dhtConnected) {
    Serial.println("DHT11 CONNECTION FAILED!");
    Serial.println("Check connections:");
    Serial.println("- VCC to 3.3V or 5V");
    Serial.println("- DATA to GPIO 4");
    Serial.println("- GND to Ground");
    Serial.println("- Add 10k pullup resistor between DATA and VCC");
    Serial.println("Continuing with other sensors...");
  }
  
  Serial.println("Initializing DS18B20...");
  ds18b20.begin();
  delay(1000);
  
  // Test DS18B20
  ds18b20.requestTemperatures();
  delay(750); // Wait for conversion
  float testSoilTemp = ds18b20.getTempCByIndex(0);
  if (testSoilTemp != DEVICE_DISCONNECTED_C && testSoilTemp > -50 && testSoilTemp < 100) {
    Serial.print("DS18B20 connected successfully: ");
    Serial.print(testSoilTemp);
    Serial.println("°C");
  } else {
    Serial.println("DS18B20 connection failed!");
  }
  
  // Initialize I2C with explicit pins for ESP32
  Serial.println("Initializing I2C...");
  Wire.begin(21, 22);  // SDA = 21, SCL = 22
  
  // Initialize light sensor
  Serial.println("Initializing BH1750...");
  if (lightMeter.begin()) {
    Serial.println("BH1750 initialized successfully");
    float testLight = lightMeter.readLightLevel();
    Serial.print("Light level: ");
    Serial.print(testLight);
    Serial.println(" lx");
  } else {
    Serial.println("Error initializing BH1750");
  }
  
  Serial.println("Initializing LCD...");
  lcd.init();
  lcd.backlight();
  
  // Test soil moisture sensor
  int testSoilRaw = analogRead(SOIL_MOISTURE_PIN);
  Serial.print("Soil moisture sensor raw value: ");
  Serial.println(testSoilRaw);
  
  lcd.setCursor(0, 0);
  lcd.print("Connecting WiFi");
  lcd.setCursor(0, 1);
  lcd.print("Please wait...");
  
  Serial.print("Connecting to WiFi");
  WiFi.begin(ssid, password);
  int wifiTimeout = 0;
  while (WiFi.status() != WL_CONNECTED && wifiTimeout < 30) {
    delay(1000);
    Serial.print(".");
    wifiTimeout++;
  }
  
  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("\nWiFi connected successfully!");
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("WiFi Connected");
    lcd.setCursor(0, 1);
    lcd.print(WiFi.localIP());
    delay(2000);
  } else {
    Serial.println("\nWiFi connection failed!");
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("WiFi Failed");
    lcd.setCursor(0, 1);
    lcd.print("Check credentials");
    delay(2000);
  }
  
  lcd.clear();
  ThingSpeak.begin(client);
  
  Serial.println("=== System Ready ===");
  Serial.println("Sensors: DHT11, BH1750, Soil Moisture, DS18B20");
  Serial.println("Starting monitoring loop...");
  Serial.println("========================");
}

void loop() {
  unsigned long currentTime = millis();
  
  // Read sensors and update display continuously
  if (currentTime - lastReadTime >= readInterval) {
    lastReadTime = currentTime;
    
    // Initialize sensor values
    float temperature = NAN;
    float humidity = NAN;
    bool dhtReadSuccess = false;
    
    // Read DHT11 with proper timing and validation
    if (currentTime - lastDHTRead >= dhtInterval) {
      lastDHTRead = currentTime;
      
      // Try reading DHT11 multiple times with validation
      for (int attempt = 0; attempt < 3; attempt++) {
        temperature = dht.readTemperature();
        humidity = dht.readHumidity();
        
        // Validate readings (DHT11 ranges: -40 to 80°C, 0-100% RH)
        if (!isnan(temperature) && !isnan(humidity) && 
            temperature > -40 && temperature < 80 && 
            humidity >= 0 && humidity <= 100) {
          
          // Additional sanity check - readings shouldn't change too drastically
          if (dhtEverWorked) {
            if (abs(temperature - lastValidTemperature) > 20 || 
                abs(humidity - lastValidHumidity) > 30) {
              Serial.print("DHT11 reading seems incorrect (attempt ");
              Serial.print(attempt + 1);
              Serial.println("), retrying...");
              delay(1000);
              continue;
            }
          }
          
          dhtReadSuccess = true;
          dhtEverWorked = true;
          lastValidTemperature = temperature;
          lastValidHumidity = humidity;
          break;
        } else {
          Serial.print("DHT11 read attempt ");
          Serial.print(attempt + 1);
          Serial.print(" failed - Temp: ");
          Serial.print(temperature);
          Serial.print(", Hum: ");
          Serial.println(humidity);
          delay(1000);
        }
      }
      
      // If all attempts failed, use last valid readings
      if (!dhtReadSuccess && dhtEverWorked) {
        Serial.println("Using last valid DHT11 readings");
        temperature = lastValidTemperature;
        humidity = lastValidHumidity;
        dhtReadSuccess = true; // Mark as success to use cached values
      }
    } else {
      // Use cached values if we're not due for a new reading
      if (dhtEverWorked) {
        temperature = lastValidTemperature;
        humidity = lastValidHumidity;
        dhtReadSuccess = true;
      }
    }
    
    // Read other sensors
    float light = lightMeter.readLightLevel();
    if (light < 0) light = 0; // Handle negative readings
    
    // Read soil moisture sensor
    int soilMoistureRaw = analogRead(SOIL_MOISTURE_PIN);
    int soilMoisturePercent = map(soilMoistureRaw, soilMoistureMin, soilMoistureMax, 0, 100);
    soilMoisturePercent = constrain(soilMoisturePercent, 0, 100);
    
    // Read DS18B20 temperature sensor
    ds18b20.requestTemperatures();
    delay(100); // Small delay for sensor reading
    float soilTemperature = ds18b20.getTempCByIndex(0);
    
    // Validate DS18B20 reading
    if (soilTemperature == DEVICE_DISCONNECTED_C || 
        soilTemperature < -50 || soilTemperature > 100) {
      soilTemperature = -999; // Error indicator
    }
    
    // Cycle through different display modes
    if (currentTime - lastDisplayChange >= displayInterval) {
      lastDisplayChange = currentTime;
      displayMode = (displayMode + 1) % 3;
    }
    
    // Display values on LCD based on current mode
    lcd.clear();
    switch (displayMode) {
      case 0: // Air Temperature and Humidity
        lcd.setCursor(0, 0);
        if (dhtReadSuccess) {
          lcd.print("Air T:");
          lcd.print(temperature, 1);
          lcd.print("C");
        } else {
          lcd.print("Air T: Error");
        }
        lcd.setCursor(0, 1);
        if (dhtReadSuccess) {
          lcd.print("Humidity:");
          lcd.print(humidity, 1);
          lcd.print("%");
        } else {
          lcd.print("Humidity: Error");
        }
        break;
        
      case 1: // Light and Soil Moisture Raw
        lcd.setCursor(0, 0);
        lcd.print("Light:");
        lcd.print(light, 0);
        lcd.print(" lx");
        lcd.setCursor(0, 1);
        lcd.print("Soil Raw:");
        lcd.print(soilMoistureRaw);
        break;
        
      case 2: // Soil Temperature and Moisture Percentage
        lcd.setCursor(0, 0);
        if (soilTemperature != -999) {
          lcd.print("Soil T:");
          lcd.print(soilTemperature, 1);
          lcd.print("C");
        } else {
          lcd.print("Soil T: Error");
        }
        lcd.setCursor(0, 1);
        lcd.print("Soil M:");
        lcd.print(soilMoisturePercent);
        lcd.print("%");
        break;
    }
    
    // Print all values to Serial Monitor
    Serial.println("=== Sensor Readings ===");
    if (dhtReadSuccess) {
      Serial.print("Air Temperature: ");
      Serial.print(temperature, 1);
      Serial.println(" °C");
      Serial.print("Humidity: ");
      Serial.print(humidity, 1);
      Serial.println(" %");
    } else {
      Serial.println("Air Temperature: DHT11 Error");
      Serial.println("Humidity: DHT11 Error");
    }
    Serial.print("Light: ");
    Serial.print(light, 1);
    Serial.println(" lx");
    Serial.print("Soil Temperature: ");
    if (soilTemperature != -999) {
      Serial.print(soilTemperature, 1);
      Serial.println(" °C");
    } else {
      Serial.println("Error");
    }
    Serial.print("Soil Moisture Raw: ");
    Serial.println(soilMoistureRaw);
    Serial.print("Soil Moisture: ");
    Serial.print(soilMoisturePercent);
    Serial.println(" %");
    Serial.println("========================");
    
    // Update ThingSpeak at the specified interval
    if (currentTime - lastThingSpeakTime >= thingSpeakInterval && WiFi.status() == WL_CONNECTED) {
      lastThingSpeakTime = currentTime;
      
      Serial.println("Updating ThingSpeak...");
      
      // Send to ThingSpeak (using all 8 available fields)
      if (dhtReadSuccess) {
        ThingSpeak.setField(1, temperature);        // Air temperature
        ThingSpeak.setField(2, humidity);           // Humidity
      }
      ThingSpeak.setField(3, light);              // Light level
      ThingSpeak.setField(4, soilMoisturePercent); // Soil moisture percentage
      ThingSpeak.setField(5, soilMoistureRaw);    // Soil moisture raw value
      if (soilTemperature != -999) {
        ThingSpeak.setField(6, soilTemperature);  // Soil temperature
      }
      
      int code = ThingSpeak.writeFields(channelID, writeAPIKey);
      if (code == 200) {
        Serial.println("ThingSpeak Update: SUCCESS");
        // Visual and audio feedback
        digitalWrite(LED_PIN, HIGH);
        digitalWrite(BUZZER_PIN, HIGH);
        delay(200);
        digitalWrite(BUZZER_PIN, LOW);
        delay(200);
        digitalWrite(BUZZER_PIN, HIGH);
        delay(200);
        digitalWrite(BUZZER_PIN, LOW);
        digitalWrite(LED_PIN, LOW);
      } else {
        Serial.print("ThingSpeak Update ERROR: ");
        Serial.println(code);
        // Error indication
        for (int i = 0; i < 3; i++) {
          digitalWrite(LED_PIN, HIGH);
          delay(100);
          digitalWrite(LED_PIN, LOW);
          delay(100);
        }
      }
    }
  }
  
  // Check WiFi connection and reconnect if necessary
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("WiFi disconnected, attempting to reconnect...");
    WiFi.reconnect();
  }
}
