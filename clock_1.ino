#include <ESP8266WiFi.h>
#include <Adafruit_NeoPixel.h>
#include <WiFiUdp.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClient.h>
#define PIXEL_PER_SEGMENT 1 // Number of LEDs in each Segment
#define PIXEL_DIGITS 4 // Number of connected Digits
#define PIXEL_PIN 4 // GPIO Pin
#define PIXEL_DASH 1 // Binary segment

#define TIME_FORMAT 12 // 12 = 12 hours format || 24 = 24 hours format

Adafruit_NeoPixel strip = Adafruit_NeoPixel((PIXEL_PER_SEGMENT * 7 * PIXEL_DIGITS) + (PIXEL_DASH * 1), PIXEL_PIN, NEO_GRB + NEO_KHZ800);

// set Wi-Fi SSID and password
const char* ssid = "XXXX";
const char* password = "XXXX";

int period = 1000; //Update frequency
unsigned long time_now = 0;

// set default brightness
int Brightness = 255;
// current temperature, updated in loop()

int first, second, third, fourth, R, G, B, brightness, buzzer;
//Digits array
byte digits[12] = {
    //abcdefg
    0b0111111, // 0
    0b0100001, // 1
    0b1110110, // 2
    0b1110011, // 3
    0b1101001, // 4
    0b1011011, // 5
    0b1011111, // 6
    0b0110001, // 7
    0b1111111, // 8
    0b1111011, // 9
    0b1100110, // C
    0b1100011, // F
};

//Clear all the Pixels
void clearDisplay()
{
    for (int i = 0; i < strip.numPixels(); i++) {
        strip.setPixelColor(i, strip.Color(0, 0, 0));
    }
    strip.show();
}

void setup()
{
    Serial.begin(115200);
    strip.begin();
    strip.show();

    WiFi.begin(ssid, password);
    Serial.print("Connecting.");
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }
    Serial.println("connected");
    delay(10);
}

void loop()
{
 while (millis() > time_now + period) {
    if (WiFi.status() == WL_CONNECTED) { // check WiFi connection status
        Serial.println("Connected");

        WiFiClient client;
        HTTPClient http;
        if (http.begin(client, "http://192.168.1.27/clock_data.php")) { // HTTP

            Serial.print("[HTTP] GET...\n");
            // start connection and send HTTP header
            int httpCode = http.GET();

            // httpCode will be negative on error
            if (httpCode > 0) {
                // HTTP header has been send and Server response header has been handled
                Serial.printf("[HTTP] GET... code: %d\n", httpCode);

                // file found at server
                if (httpCode == HTTP_CODE_OK || httpCode == HTTP_CODE_MOVED_PERMANENTLY) {
                    String payload = http.getString();
                    Serial.println(payload);
                    first = atoi(getValue(payload, ':', 0).c_str());
                    second = atoi(getValue(payload, ':', 1).c_str());
                    third = atoi(getValue(payload, ':', 2).c_str());
                    fourth = atoi(getValue(payload, ':', 3).c_str());
                    R = atoi(getValue(payload, ':', 4).c_str());
                    G = atoi(getValue(payload, ':', 5).c_str());
                    B = atoi(getValue(payload, ':', 6).c_str());
                    Brightness = atoi(getValue(payload, ':', 7).c_str());
                    buzzer = atoi(getValue(payload, ':', 8).c_str());
                    
                }
            }
            else {
                Serial.printf("[HTTP] GET... failed, error: %s\n", http.errorToString(httpCode).c_str());
            }

            http.end();
        }
        else {
            Serial.printf("[HTTP} Unable to connect\n");
        }
    }

   
        time_now = millis();
        disp_Time_N(); // Show Time
    }
}

void disp_Time_N()
{
    Serial.println("clearing display");
    clearDisplay();
    Serial.println("cleared");
    //writeDigit(1, first);
    //writeDigit(0, 9);
    writeDigit(0, first);
    writeDigit(1, second);
    writeDigit(2, third);
    writeDigit(3, fourth);
    //writeDigit(4, Second / 10);
    //writeDigit(5, Second % 10);
    disp_Dash();
    strip.show();
}

int sflag = 0;
void disp_Dash()
{
    int dash = 14;
    sflag == 0 ? strip.setPixelColor(dash, strip.Color(100, 0, 0)) : strip.setPixelColor(dash, strip.Color(0, 0, 0));
    sflag = !sflag;
}

void writeDigit(int index, int val)
{
    byte digit = digits[val];
    int margin;
    if (index == 0 || index == 1)
        margin = 0;
    if (index == 2 || index == 3)
        margin = 1;
    if (index == 4 || index == 5)
        margin = 2;
    for (int i = 6; i >= 0; i--) {
        int offset = index * (PIXEL_PER_SEGMENT * 7) + i * PIXEL_PER_SEGMENT + margin * 1;
        uint32_t color;
        if (digit & 0x01 != 0) {
            if (index == 0 || index == 1)
                color = strip.Color(R, G, B);
            if (index == 2 || index == 3)
                color = strip.Color(R, G, B);
            if (index == 4 || index == 5)
                color = strip.Color(0, 0, Brightness);
        }
        else
            color = strip.Color(0, 0, 0);

        for (int j = offset; j < offset + PIXEL_PER_SEGMENT; j++) {
            strip.setPixelColor(j, color);
        }
        digit = digit >> 1;
    }
}

String getValue(String data, char separator, int index)
{
    int found = 0;
    int strIndex[] = { 0, -1 };
    int maxIndex = data.length() - 1;

    for (int i = 0; i <= maxIndex && found <= index; i++) {
        if (data.charAt(i) == separator || i == maxIndex) {
            found++;
            strIndex[0] = strIndex[1] + 1;
            strIndex[1] = (i == maxIndex) ? i + 1 : i;
        }
    }
    return found > index ? data.substring(strIndex[0], strIndex[1]) : "";
}
