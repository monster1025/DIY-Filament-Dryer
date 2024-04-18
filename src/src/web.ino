//web
#ifdef ESP32
#include <WiFi.h>
#include <AsyncTCP.h>
#elif defined(ESP8266)
#include <ESP8266WiFi.h>
#include <ESPAsyncTCP.h>
#endif
#include "ESPAsyncWebServer.h"
#include <FS.h>
#include <LittleFS.h> 
  
AsyncWebServer server(80);
int WiFi_status = WL_IDLE_STATUS;
int wifi_debug_printed = 0;

void setup_web() {
    WiFi.begin(ssid, password);
    wifi_debug_printed = 0;

    LittleFS.begin();
}

void task_setup_web() {
    int wifi_status = WiFi.status();
    if (wifi_status != WL_CONNECTED){
        Serial.print("WiFi Status: ");
        Serial.println(wifi_status);
        wifi_debug_printed = 0;
    } else if (wifi_status == WL_CONNECTED && wifi_debug_printed == 0) {
        Serial.print("WiFi IP: ");
        Serial.println(WiFi.localIP());
        wifi_debug_printed = 1;
        

        setupWebServer();
        server.begin();
    }
}

void setupWebServer(void)
{
    server.onNotFound([](AsyncWebServerRequest *request) {
        Serial.println("404:");
        Serial.println(request->url());
        request->send(404);
    });

    // // send a file when /index is requested
    server.on("/index.html", HTTP_ANY, [](AsyncWebServerRequest *request) {
        request->send(LittleFS, "/index.html");
    });

    // send a file when /index is requested
    server.on("/", HTTP_ANY, [](AsyncWebServerRequest *request) {
        request->send(LittleFS, "/index.html");
    });

    server.serveStatic("/img/", LittleFS, "/img/");
    server.serveStatic("/css/", LittleFS, "/css/");
    server.serveStatic("/js/", LittleFS, "/js/");
    server.serveStatic("/webfonts/", LittleFS, "/webfonts/");

    // Get dry box status
    server.on("/status", HTTP_GET, [](AsyncWebServerRequest *request) {
        char buff[200] = {0};
        int len;
        len =
            snprintf(buff, 200,
                     "{\"status\":%d, \"target_temp_in\":%f, "
                     "\"max_temp_heater\":%f, "
                     "\"temp_in\":%f, \"temp_heater\":%f, \"humid_in\":%f, "
                     "\"fan_speed\":%d, \"temp_out\":%f, \"humid_out\":%d}",
                     box_status, target_temperature_in, max_temperature_heater,
                     temperature_in, temperature_heater, humidity_in, fan_duty,
                     temperature_out, humidity_out);

        if (len)
        {
            request->send(200, "text/plain", buff);
        }
        else
        {
            request->send(500, "text/plain",
                          "{\"status\": \"Internal server error\"}");
        }
    });

    //Turn OFF dry box
    server.on("/off", HTTP_GET, [](AsyncWebServerRequest *request) {
        box_status = 0;
        set_fan_duty(0);
        set_heater_duty(HEATER_DUTY_OFF);
        target_temperature_in = 0;
        max_temperature_heater = 0;
        request->send(200, "text/plain", "{\"status\": \"OK\"}");
    });

    // Turn ON dry box and set target temperature, max heater temperature and fan speed
    server.on("/set", HTTP_GET, [](AsyncWebServerRequest *request) {
        // Check if temperature, heater temperature and fan speed arguments are present
        if (request->hasParam("temperature") && request->hasParam("heater") &&
            request->hasParam("fanspeed"))
        {
            int32_t temperature = 0;
            int32_t heater = 0;
            int32_t fanspeed = 0;

            temperature = request->getParam("temperature")->value().toInt();
            heater = request->getParam("heater")->value().toInt();
            fanspeed = request->getParam("fanspeed")->value().toInt();

#if DEF_DEUG_WEB_API
            Serial.println((String) "Target temp: " + temperature +
                           "C Heater: " + heater + "C Fan Speed: " + fanspeed);
#endif

            // Check drybox temperature and heater temperature limit
            if ( (temperature <= LIMIT_TEMP_IN_MAX) && (heater <= LIMIT_TEMP_HEATER_MAX) )
            {
                target_temperature_in = temperature;
                max_temperature_heater = heater;
                change_target_temperature(temperature);
                set_fan_duty(fanspeed);
                box_status = 1;
                request->send(200, "text/plain", "{\"status\": \"OK\"}");
                return;
            }
            // If either of them are out of range, return bad request status
            else
            {
                request->send(400, "text/plain", "{\"status\": \"Bad request. Limit error!\"}");
                return;
            }

        }
        // Not all arguments are present in the request
        else
        {
            request->send(400, "text/plain", "{\"status\": \"Bad request\"}");
            return;
        }
    });
}