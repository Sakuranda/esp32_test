//#ifndef __WSS_H
//#define __WSS_H
//
//#include <Arduino.h>
//#include <WiFi.h>
//#include <ArduinoWebsockets.h>
//#include <ArduinoJson.h>
//
//#include <base64.h>
//#include <file.h>
//
//#include <record.h>
//
//using namespace websockets;
//
//extern const char* ssid;
//extern const char* password;
//extern WebsocketsClient client;
//extern int playindex;
//extern bool urlDownloadFinishedFlag;
//
///*----------------------------------------------------*/
//
//
//String createJson(const char* uuid, const char* event, const char* key = nullptr, const char* value = nullptr);
//void sendMessage(const char* id);
//void sendPCMData(const char* filePath);
//void onMessageCallback(WebsocketsMessage message);
//void wss_init(void);
//void wss_loop(void);
//char* get_wss_mp3();
//
//bool wss_got();
//
//void set_false_wss_got();
//#endif