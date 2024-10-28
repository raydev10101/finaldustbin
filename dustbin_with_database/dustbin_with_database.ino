#include <SPI.h>
#include <MFRC522.h>
#include <ESP32Servo.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include <WiFiManager.h>
#include <time.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
LiquidCrystal_I2C lcd(0x27, 16, 2);  //sometimes the adress is not 0x3f. Change to 0x27 if it dosn't work.

const char* ntpServer = "pool.ntp.org";  // NTP server
const long gmtOffset_sec = 3600;         // Timezone offset in seconds (e.g., GMT+1 = 3600)
const int daylightOffset_sec = 3600;     // Daylight saving offset in seconds (usually 3600)

WiFiManager wm;

#define WASTE_BIN_IS_FULL "WASTE BIN IS FULL"
#define WASTE_BIN_IS_OPEN "WASTE BIN IS OPEN"
#define WASTE_BIN_IS_CLOSED "WASTE BIN IS CLOSED"
#define WASTE_BIN_IS_EMPTY "WASTE BIN IS EMPTY"
const char BaseURL[] =  "http://192.168.43.166:8080/";

typedef struct {
  char username[30];
  char password[30];
  char phoneNumber[30];
  int id;
  float balance;
} LoggedInUser;

LoggedInUser loggedInUser;


#define RST_PIN 17                 //Pin 9 is for the RC522 reset
#define SS_PIN 16                  //Pin 10 is the SS (SDA) of RC522 module
MFRC522 mfrc522(SS_PIN, RST_PIN);  //Create anew RC522 object
Servo motor_1;

//Variables
boolean DustBin_opened = false;
boolean first_read = false;
boolean normal_mode = true;
boolean countdown = false;
int timer = 0;
int user_added = 0;
int add_ID_counter = 0;

//I/O_1.
int buzzer_pin = 4;
int outside_close = 0;  //pin for the button of the outside open
int inside_close = 0;   //pin for the button of the inside close
int inside_open = 0;    //pin for the button of the inside open

void setup() {
  Serial.begin(115200);
  setupWifiConfigurer();
  setupTime();  // Initialize time sync with NTP server

  // Example calls to test functionality
  sendLoginRequest("Samuel", "14106622");
  sendSignupRequest("USERNAME", "PASSWORD", "PHONE_NUMBER");

  SPI.begin();                  //Start a new SPI bus
  mfrc522.PCD_Init();           // Startt the MFRC522
  motor_1.attach(13);           //Set digital pin D3 to be the PWM signal for the servo motor
  pinMode(buzzer_pin, OUTPUT);  //Set digital pin D7 to be the buzzer OUTPUT

  //Config of the i2c LCD screen
  lcd.init();
  lcd.backlight();
  lcd.setCursor(0, 0);
  lcd.print("Place card here!");
  lcd.setCursor(0, 1);
  lcd.print("                ");
  motor_1.writeMicroseconds(0);  //Put the servo in to the close position

  sendLogMessage(WASTE_BIN_IS_OPEN, loggedInUser.username);  // Log a message
  delay(200);
  sendLogMessage(WASTE_BIN_IS_FULL,loggedInUser.username);
  delay(200);
  sendLogMessage(WASTE_BIN_IS_EMPTY,loggedInUser.username);
  delay(200);
  sendLogMessage(WASTE_BIN_IS_CLOSED,loggedInUser.username);
  fetchLogs("2024-01-01T00:00:00", "2025-01-01T00:00:00", loggedInUser.username);  /// PARAM 1 -> TIME FROM, PARAM 2 -> TIME TO param3 
}

byte ActualUID[4];  //This will store the ID each time we read a new ID code

byte USER1[4] = { 0xB3, 0x68, 0x17, 0x04 };  //Master ID code Change it for yor tag. First use the READ exampel and check your ID

byte USER2[4] = { 0x00, 0x00, 0x00, 0x00 };  //Empty ID of USER2
byte USER3[4] = { 0x00, 0x00, 0x00, 0x00 };  //Empty ID of USER3
byte USER4[4] = { 0x00, 0x00, 0x00, 0x00 };  //Empty ID of USER4
byte USER5[4] = { 0x00, 0x00, 0x00, 0x00 };  //Empty ID of USER5

void loop() {

  if (normal_mode) {

    if (countdown) {
      if (add_ID_counter > 300) {
        countdown = false;
        first_read = false;
        add_ID_counter = 0;
        lcd.setCursor(0, 0);
        lcd.print("New ID  canceled");
        lcd.setCursor(0, 1);
        lcd.print("                ");
        digitalWrite(buzzer_pin, HIGH);
        delay(1000);
        digitalWrite(buzzer_pin, LOW);
        delay(4000);
        lcd.setCursor(0, 0);
        lcd.print("Place card here!");
        lcd.setCursor(0, 1);
        lcd.print("                ");
      }

      if (add_ID_counter == 50) {
        lcd.setCursor(15, 1);
        lcd.print("5");
        digitalWrite(buzzer_pin, HIGH);
        delay(100);
        digitalWrite(buzzer_pin, LOW);
      }

      if (add_ID_counter == 100) {
        lcd.setCursor(15, 1);
        lcd.print("4");
        digitalWrite(buzzer_pin, HIGH);
        delay(100);
        digitalWrite(buzzer_pin, LOW);
      }

      if (add_ID_counter == 150) {
        lcd.setCursor(15, 1);
        lcd.print("3");
        digitalWrite(buzzer_pin, HIGH);
        delay(100);
        digitalWrite(buzzer_pin, LOW);
      }

      if (add_ID_counter == 200) {
        lcd.setCursor(15, 1);
        lcd.print("2");
        digitalWrite(buzzer_pin, HIGH);
        delay(100);
        digitalWrite(buzzer_pin, LOW);
      }

      if (add_ID_counter == 250) {
        lcd.setCursor(15, 1);
        lcd.print("1");
        digitalWrite(buzzer_pin, HIGH);
        delay(100);
        digitalWrite(buzzer_pin, LOW);
      }


      add_ID_counter = add_ID_counter + 1;
      delay(10);
    }



    // Check if there are any new ID card in front of the sensor
    if (mfrc522.PICC_IsNewCardPresent()) {
      //Select the found card
      if (mfrc522.PICC_ReadCardSerial()) {
        // We store the read ID into 4 bytes with a for loop
        for (byte i = 0; i < mfrc522.uid.size; i++) {
          ActualUID[i] = mfrc522.uid.uidByte[i];
        }

        //Compare the UID and check if the new iD is on the user listz


        if (first_read) {
          if (compareArray(ActualUID, USER1)) {
            countdown = false;
            add_ID_counter = 0;
            digitalWrite(buzzer_pin, HIGH);
            delay(300);
            digitalWrite(buzzer_pin, LOW);
            normal_mode = false;
            lcd.setCursor(0, 0);
            lcd.print("Place New ID in:");
            lcd.setCursor(0, 1);
            lcd.print("       3        ");
            digitalWrite(buzzer_pin, HIGH);
            delay(100);
            digitalWrite(buzzer_pin, LOW);
            delay(1000);
            lcd.setCursor(0, 1);
            lcd.print("       2        ");
            digitalWrite(buzzer_pin, HIGH);
            delay(100);
            digitalWrite(buzzer_pin, LOW);
            delay(1000);
            lcd.setCursor(0, 1);
            lcd.print("       1       ");
            digitalWrite(buzzer_pin, HIGH);
            delay(100);
            digitalWrite(buzzer_pin, LOW);
            delay(1000);
            lcd.setCursor(0, 1);
            lcd.print("      NOW!      ");
            digitalWrite(buzzer_pin, HIGH);
            delay(500);
            digitalWrite(buzzer_pin, LOW);


          } else {
            first_read = false;
          }
        }

        if (!first_read) {
          if (compareArray(ActualUID, USER1)) {
            lcd.setCursor(0, 0);
            lcd.print(" Access granted ");
            lcd.setCursor(0, 1);
            lcd.print("  MASTER  USER  ");
            digitalWrite(buzzer_pin, HIGH);  //use analogWrite(buzzer_pin,200); for non active buzzer
            delay(300);
            digitalWrite(buzzer_pin, LOW);  //use analog write for non active buzzer
            motor_1.writeMicroseconds(1500);
            delay(10000);
            motor_1.writeMicroseconds(0);
            DustBin_opened = true;
            first_read = true;
            countdown = true;
            delay(1000);
            lcd.setCursor(0, 0);
            lcd.print("Put  MASTER card");
            lcd.setCursor(0, 1);
            lcd.print("for new ID     6");
            digitalWrite(buzzer_pin, HIGH);
            delay(100);
            digitalWrite(buzzer_pin, LOW);

          } else if (compareArray(ActualUID, USER2)) {
            lcd.setCursor(0, 0);
            lcd.print(" Access granted ");
            lcd.setCursor(0, 1);
            lcd.print("     USER 2     ");
            digitalWrite(buzzer_pin, HIGH);  //use analogWrite(buzzer_pin,200); for non active buzzer
            delay(300);
            digitalWrite(buzzer_pin, LOW);  //use analog write for non active buzzer
            motor_1.writeMicroseconds(1500);
            delay(10000);
            motor_1.writeMicroseconds(0);
            DustBin_opened = true;
            first_read = true;
            delay(1000);
            lcd.setCursor(0, 0);
            lcd.print("Last use: USER2");
            lcd.setCursor(0, 1);
            lcd.print("  DustBin OPENED  ");
            sendLogMessage(WASTE_BIN_IS_OPEN,loggedInUser.username);  // Log a message

          } else if (compareArray(ActualUID, USER3)) {
            lcd.setCursor(0, 0);
            lcd.print(" Access granted ");
            lcd.setCursor(0, 1);
            lcd.print("     USER 3     ");
            digitalWrite(buzzer_pin, HIGH);  //use analogWrite(buzzer_pin,200); for non active buzzer
            delay(300);
            digitalWrite(buzzer_pin, LOW);  //use analog write for non active buzzer
            motor_1.writeMicroseconds(1500);
            delay(10000);
            motor_1.writeMicroseconds(0);
            DustBin_opened = true;
            first_read = true;
            delay(1000);
            lcd.setCursor(0, 0);
            lcd.print("Last use: USER3");
            lcd.setCursor(0, 1);
            lcd.print("  DustBin OPENED  ");
            sendLogMessage(WASTE_BIN_IS_OPEN,loggedInUser.username);  // Log a message
          } else if (compareArray(ActualUID, USER4)) {
            lcd.setCursor(0, 0);
            lcd.print(" Access granted ");
            lcd.setCursor(0, 1);
            lcd.print("     USER 4     ");
            digitalWrite(buzzer_pin, HIGH);  //use analogWrite(buzzer_pin,200); for non active buzzer
            delay(300);
            digitalWrite(buzzer_pin, LOW);  //use analog write for non active buzzer
            motor_1.writeMicroseconds(1500);
            delay(10000);
            motor_1.writeMicroseconds(0);
            DustBin_opened = true;
            first_read = true;
            delay(1000);
            lcd.setCursor(0, 0);
            lcd.print("Last use: USER4");
            lcd.setCursor(0, 1);
            lcd.print("  DustBin OPENED  ");
            sendLogMessage(WASTE_BIN_IS_OPEN,loggedInUser.username);  // Log a message
          } else if (compareArray(ActualUID, USER5)) {
            lcd.setCursor(0, 0);
            lcd.print(" Access granted ");
            lcd.setCursor(0, 1);
            lcd.print("     USER 5     ");
            digitalWrite(buzzer_pin, HIGH);  //use analogWrite(buzzer_pin,200); for non active buzzer
            delay(300);
            digitalWrite(buzzer_pin, LOW);  //use analog write for non active buzzer
            motor_1.writeMicroseconds(1500);
            delay(10000);
            motor_1.writeMicroseconds(0);
            DustBin_opened = true;
            first_read = true;
            delay(1000);
            lcd.setCursor(0, 0);
            lcd.print("Last use: USER5");
            lcd.setCursor(0, 1);
            lcd.print("  DustBin OPENED  ");
            sendLogMessage(WASTE_BIN_IS_OPEN,loggedInUser.username);  // Log a message
          } else {
            lcd.setCursor(0, 0);
            lcd.print(" Access  denied ");
            lcd.setCursor(0, 1);
            lcd.print("   UNKNOWN ID   ");
            analogWrite(buzzer_pin, 200);  //use analogWrite(buzzer_pin,200); for non active buzzer
            delay(1500);
            analogWrite(buzzer_pin, LOW);  //use analog write for non active buzzer
            //motor_1.write(15);
            DustBin_opened = false;
            first_read = false;
            delay(1000);
            lcd.setCursor(0, 0);
            lcd.print("Place card here!");
            lcd.setCursor(0, 1);
            lcd.print("  DustBin CLOSED  ");
          }
        }



        // Terminamos la lectura de la tarjeta tarjeta  actual
        mfrc522.PICC_HaltA();
      }
    }
  }  //end  normal mode

  if (!normal_mode) {
    // Revisamos si hay nuevas tarjetas  presentes
    if (mfrc522.PICC_IsNewCardPresent()) {
      //Seleccionamos una tarjeta
      if (mfrc522.PICC_ReadCardSerial()) {
        // Enviamos serialemente su UID

        for (byte i = 0; i < mfrc522.uid.size; i++) {

          ActualUID[i] = mfrc522.uid.uidByte[i];
        }

        //Compare the read ID and the stored USERS
        if (user_added == 4) {
          lcd.setCursor(0, 0);
          lcd.print("  User list is  ");
          lcd.setCursor(0, 1);
          lcd.print("      FULL      ");
          digitalWrite(buzzer_pin, HIGH);  //use analogWrite(buzzer_pin,200); for non active buzzer
          delay(1000);
          digitalWrite(buzzer_pin, LOW);  //use analog write for non active buzzer
          lcd.setCursor(0, 0);
          lcd.print("Place card here!");
          lcd.setCursor(0, 1);
          lcd.print("                ");
        }

        if (user_added == 3) {
          USER5[0] = ActualUID[0];
          USER5[1] = ActualUID[1];
          USER5[2] = ActualUID[2];
          USER5[3] = ActualUID[3];
          user_added = user_added + 1;
          lcd.setCursor(0, 0);
          lcd.print("New user stored ");
          lcd.setCursor(0, 1);
          lcd.print("   as  USER 5   ");
          digitalWrite(buzzer_pin, HIGH);  //use analogWrite(buzzer_pin,200); for non active buzzer
          delay(300);
          digitalWrite(buzzer_pin, LOW);  //use analog write for non active buzzer
          delay(1000);
          normal_mode = true;
          first_read = false;
          delay(1000);
          lcd.setCursor(0, 0);
          lcd.print("Place card here!");
          lcd.setCursor(0, 1);
          lcd.print("                ");
        }

        if (user_added == 2) {
          USER4[0] = ActualUID[0];
          USER4[1] = ActualUID[1];
          USER4[2] = ActualUID[2];
          USER4[3] = ActualUID[3];
          user_added = user_added + 1;
          lcd.setCursor(0, 0);
          lcd.print("New user stored ");
          lcd.setCursor(0, 1);
          lcd.print("   as  USER 4   ");
          digitalWrite(buzzer_pin, HIGH);  //use analogWrite(buzzer_pin,200); for non active buzzer
          delay(300);
          digitalWrite(buzzer_pin, LOW);  //use analog write for non active buzzer
          delay(1000);
          normal_mode = true;
          first_read = false;
          delay(1000);
          lcd.setCursor(0, 0);
          lcd.print("Place card here!");
          lcd.setCursor(0, 1);
          lcd.print("                ");
        }

        if (user_added == 1) {
          USER3[0] = ActualUID[0];
          USER3[1] = ActualUID[1];
          USER3[2] = ActualUID[2];
          USER3[3] = ActualUID[3];
          user_added = user_added + 1;
          lcd.setCursor(0, 0);
          lcd.print("New user stored ");
          lcd.setCursor(0, 1);
          lcd.print("   as  USER 3   ");
          digitalWrite(buzzer_pin, HIGH);  //use analogWrite(buzzer_pin,200); for non active buzzer
          delay(300);
          digitalWrite(buzzer_pin, LOW);  //use analog write for non active buzzer
          delay(1000);
          normal_mode = true;
          first_read = false;
          delay(1000);
          lcd.setCursor(0, 0);
          lcd.print("Place card here!");
          lcd.setCursor(0, 1);
          lcd.print("                ");
        }

        if (user_added == 0) {
          USER2[0] = ActualUID[0];
          USER2[1] = ActualUID[1];
          USER2[2] = ActualUID[2];
          USER2[3] = ActualUID[3];
          user_added = user_added + 1;
          lcd.setCursor(0, 0);
          lcd.print("New user stored ");
          lcd.setCursor(0, 1);
          lcd.print("   as  USER 2   ");
          digitalWrite(buzzer_pin, HIGH);  //use analogWrite(buzzer_pin,200); for non active buzzer
          delay(300);
          digitalWrite(buzzer_pin, LOW);  //use analog write for non active buzzer
          delay(1000);
          normal_mode = true;
          first_read = false;
          delay(1000);
          lcd.setCursor(0, 0);
          lcd.print("Place card here!");
          lcd.setCursor(0, 1);
          lcd.print("                ");
        }
      }
    }
  }  //end  ID add mode
}

//Compare the 4 bytes of the users and the received ID
boolean compareArray(byte array1[], byte array2[]) {
  if (array1[0] != array2[0]) return (false);
  if (array1[1] != array2[1]) return (false);
  if (array1[2] != array2[2]) return (false);
  if (array1[3] != array2[3]) return (false);
  return (true);
}
// Structure to hold logged-in user details
// Function to connect to Wi-Fi using WiFiManager
void setupWifiConfigurer() {
  if (!wm.autoConnect("IoT_Dustbin")) {
    Serial.println("Failed to connect and hit timeout");
    ESP.restart();
  } else {
    Serial.print("Local IP: ");
    Serial.println(WiFi.localIP());
  }
}
// Function to initialize and sync time using NTP server
void setupTime() {
  configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
  Serial.println("Waiting for time synchronization...");
  delay(2000);
  struct tm timeinfo;
  if (!getLocalTime(&timeinfo)) {
    Serial.println("Failed to obtain time");
    return;
  }
  Serial.println(&timeinfo, "Current time: %Y-%m-%d %H:%M:%S");
}
// Function to send a login request
void sendLoginRequest(const String& username, const String& password) {
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
    http.begin(String(BaseURL) +"api/auth/login");
    http.addHeader("Content-Type", "application/json");
    // Create the JSON object for the request body
    StaticJsonDocument<200> doc;
    doc["username"] = username;
    doc["password"] = password;
    // Serialize the JSON document to a string
    String requestBody;
    serializeJson(doc, requestBody);
    // Send HTTP POST request
    int httpResponseCode = http.POST(requestBody);
    Serial.printf("HTTP Status Code = %d\n", httpResponseCode);
    if (httpResponseCode == 200) {
      String response = http.getString();
      Serial.println("Response Body: " + response);
      StaticJsonDocument<200> responseDoc;
      DeserializationError error = deserializeJson(responseDoc, response);
      if (!error) {
        strcpy(loggedInUser.username, responseDoc["username"]);
        strcpy(loggedInUser.password, responseDoc["password"]);
        strcpy(loggedInUser.phoneNumber, responseDoc["phoneNumber"]);
        loggedInUser.id= responseDoc["id"];
        loggedInUser.balance = responseDoc["balance"];
      } else {
        Serial.println("Failed to parse response");
      }
    } else if (httpResponseCode == 400) {
      Serial.println("Bad Request: User Already Exists");
    } else {
      Serial.println("Error on sending POST: " + String(httpResponseCode));
    }
    http.end();
  } else {
    Serial.println("WiFi Disconnected");
  }
}

// Function to send a signup request
void sendSignupRequest(const String& username, const String& password, const String& phoneNumber) {
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
    http.begin(String(BaseURL)+"api/auth/signup");
    http.addHeader("Content-Type", "application/json");

    // Create the JSON object for the request body
    StaticJsonDocument<200> doc;
    doc["username"] = username;
    doc["password"] = password;
    doc["phoneNumber"] = phoneNumber;

    // Serialize the JSON document to a string
    String requestBody;
    serializeJson(doc, requestBody);

    // Send HTTP POST request
    int httpResponseCode = http.POST(requestBody);
    Serial.printf("HTTP Status Code = %d\n", httpResponseCode);

    if (httpResponseCode == 200) {
      Serial.println("Signup successful.");
    } else if (httpResponseCode == 400) {
      Serial.println("Bad Request: Signup failed.");
    } else {
      Serial.println("Error on sending POST: " + String(httpResponseCode));
    }
    http.end();
  } else {
    Serial.println("WiFi Disconnected");
  }
}

// Function to send a log message with the current date and time
void sendLogMessage(const String& message, const String& username) {
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
    http.begin(String(BaseURL) +"api/log");
    http.addHeader("Content-Type", "application/json");

    // Get the current time
    struct tm timeinfo;
    if (!getLocalTime(&timeinfo)) {
      Serial.println("Failed to obtain time");
      return;
    }

    // Format the dateTime as a string
    char dateTime[20];
    strftime(dateTime, sizeof(dateTime), "%Y-%m-%dT%H:%M:%S", &timeinfo);

    // Create the JSON object for the request body
    StaticJsonDocument<200> doc;
    doc["dateTime"] = dateTime;
    doc["message"] = message;
    doc["username"] = username;

    // Serialize the JSON document to a string
    String requestBody;
    serializeJson(doc, requestBody);

    // Send HTTP POST request
    int httpResponseCode = http.POST(requestBody);
    Serial.printf("HTTP Status Code = %d\n", httpResponseCode);

    if (httpResponseCode == 200) {
      Serial.println("Log message sent successfully.");
    } else if (httpResponseCode == 400) {
      Serial.println("Bad Request: Log message failed.");
    } else {
      Serial.println("Error on sending POST: " + String(httpResponseCode));
    }
    http.end();
  } else {
    Serial.println("WiFi Disconnected");
  }
}

// Function to fetch logs with a specified date range
void fetchLogs(const String& dateTimeFrom, const String& dateTimeTo, const String & username) {
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
    // Construct URL with date range parameters
    String url = String(BaseURL)+"api/log";
    url += "?dateTimeFrom=" + dateTimeFrom;
    url += "&dateTimeTo=" + dateTimeTo;
    url += "&username="+username;
    http.begin(url);
    int httpResponseCode = http.GET();
    Serial.printf("HTTP Status Code = %d\n", httpResponseCode);
    if (httpResponseCode == 200) {
      String response = http.getString();
      Serial.println("Response Body: " + response);
      StaticJsonDocument<512> doc;
      DeserializationError error = deserializeJson(doc, response);
      if (!error) {
        bool status = doc["status"];
        String message = doc["message"];
        Serial.println("Status: " + String(status ? "true" : "false"));
        Serial.println("Message: " + message);
        // Get the array of logs
        JsonArray data = doc["data"].as<JsonArray>();
        for (JsonObject log : data) {
          String logMessage = log["logMessage"].as<String>();
          String logDateTime = log["logDateTime"].as<String>();
          // Put in a tabular form here

          Serial.println("Log Message: " + logMessage);
          Serial.println("Log DateTime: " + logDateTime);
        }
      } else {
        Serial.println("Failed to parse response");
      }
    } else {
      Serial.println("Error on sending GET: " + String(httpResponseCode));
    }
    http.end();
  } else {
    Serial.println("WiFi Disconnected");
  }
}

void sendPurchaseRequest(double valueAdded, const String& username) {
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
    http.begin(String(BaseURL)+"api/auth/purchase");
    http.addHeader("Content-Type", "application/x-www-form-urlencoded");

    String postData = "valueAdded=" + String(valueAdded) + "&username=" + username;

    int httpResponseCode = http.POST(postData);

    if (httpResponseCode == 200) {
      Serial.println("Purchase successful.");
    } else {
      Serial.print("Purchase failed, error code: ");
      Serial.println(httpResponseCode);
    }

    http.end();
  } else {
    Serial.println("WiFi Disconnected");
  }
}
