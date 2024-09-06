//#include <Arduino.h>
/**
 * @file streams-url_mp3-i2s.ino
 * @author Phil Schatzmann
 * @brief decode MP3 stream from url and output it on I2S
 * @version 0.1
 * @date 2021-96-25
 *
 * @copyright Copyright (c) 2021
 */

// install https://github.com/pschatzmann/arduino-libhelix.git

#include "AudioTools.h"
#include "AudioLibs/AudioA2DP.h"
#include "AudioCodecs/CodecMP3Helix.h"
#include "esp_wifi.h"
#include <Arduino.h>
#include <esp_pm.h>
#include <esp_wifi.h>
#include <esp_wifi_types.h>
//#include <main.h>
//#include <wss.h>
//#include <record.h>
//#include <http.h>
//
//#include "esp_system.h"  

//uint32_t randomNumber;
//int randomInRange;

A2DPStream out2BT;

URLStream url_audio_strm("ling-test","50505050");
///I2SStream i2s; // final output of decoded stream(AudioStream*)
EncodedAudioStream dec( &out2BT, new MP3DecoderHelix()); // &i2s Decoding stream
StreamCopy copier(dec, url_audio_strm); // copy url to decoder



void* __wrap_malloc(size_t size) {
    void* ptr = ps_malloc(size);

    if (ptr == nullptr) {
        ptr = (void*)heap_caps_malloc(size, MALLOC_CAP_DEFAULT);
    }

    return ptr;
}


void* __real_malloc(size_t size) {
    return malloc(size);
}

void setup(){
    delay(200);
    Serial.begin(115200);



    multi_heap_info_t heapInfo;
    heap_caps_get_info(&heapInfo, MALLOC_CAP_DEFAULT);


    Serial.println("Heap Memory Info:");
    Serial.print("Total Free Bytes: ");
    Serial.println(heapInfo.total_free_bytes);
    Serial.print("Total Allocated Bytes: ");
    Serial.println(heapInfo.total_allocated_bytes);
    Serial.print("Largest Free Block: ");
    Serial.println(heapInfo.largest_free_block);
    Serial.print("Minimum Free Heap Ever: ");
    Serial.println(heapInfo.minimum_free_bytes);


    if (psramInit()) {
        Serial.println("PSRAM initialized successfully!");
    } else {
        Serial.println("PSRAM initialization failed.");
    }
  //audioplayer_init();
   // Serial.println("audio success");

    Serial.printf("Total PSRAM: %u bytes\n", ESP.getPsramSize());
    Serial.printf("Free PSRAM: %u bytes\n", ESP.getFreePsram());
    Serial.printf("Free heap: %u bytes\n", ESP.getFreeHeap());



    multi_heap_info_t heapInfo1;
    heap_caps_get_info(&heapInfo1, MALLOC_CAP_DEFAULT);


    Serial.println("Heap Memory Info:");
    Serial.print("Total Free Bytes: ");
    Serial.println(heapInfo1.total_free_bytes);
    Serial.print("Total Allocated Bytes: ");
    Serial.println(heapInfo1.total_allocated_bytes);
    Serial.print("Largest Free Block: ");
    Serial.println(heapInfo1.largest_free_block);
    Serial.print("Minimum Free Heap Ever: ");
    Serial.println(heapInfo1.minimum_free_bytes);

//    WiFi.mode(WIFI_STA);
//    WiFi.begin("ling-test", "50505050");
//
//    while (WiFi.status() != WL_CONNECTED) {
//        delay(1000);
//        Serial.print(".");
//    }
//    Serial.println("Connected!");
//
//    if (esp_wifi_set_ps(WIFI_PS_MIN_MODEM) == ESP_OK) {
//        Serial.println("WIFI_PS_MODEM Success");
//    } else {
//        Serial.println("WIFI_PS_MODEM Failed");
//    }
//


    Serial.println("Heap Memory Info:");
    Serial.print("Total Free Bytes: ");
    Serial.println(heapInfo1.total_free_bytes);
    Serial.print("Total Allocated Bytes: ");
    Serial.println(heapInfo1.total_allocated_bytes);
    Serial.print("Largest Free Block: ");
    Serial.println(heapInfo1.largest_free_block);
    Serial.print("Minimum Free Heap Ever: ");
    Serial.println(heapInfo1.minimum_free_bytes);

    Serial.printf("Free PSRAM after Wi-Fi init: %u bytes\n", ESP.getFreePsram());
    Serial.printf("Free heap after Wi-Fi init: %u bytes\n", ESP.getFreeHeap());


    AudioLogger::instance().begin(Serial, AudioLogger::Debug);
    auto cfg = out2BT.defaultConfig(TX_MODE);
    cfg.name = "ES32_7BT";
    //cfg.auto_reconnect = true;  // if this is use we just quickly connect to the last device ignoring cfg.name
    out2BT.begin(cfg);
    Serial.println("cfg end");

    Serial.printf("Free PSRAM after Wi-Fi init: %u bytes\n", ESP.getFreePsram());
    Serial.printf("Free heap after Wi-Fi init: %u bytes\n", ESP.getFreeHeap());
/*    // setup i2s
    auto config = i2s.defaultConfig(TX_MODE);
    // you could define e.g your pins and change other settings
    config.pin_ws = 32;//10;
    config.pin_bck = 27;//11;
    config.pin_data = 33;//12;
    //config.mode = I2S_STD_FORMAT;
    i2s.begin(config);  */

    // setup I2S based on sampling rate provided by decoder
    dec.begin();
    Serial.println("dec end");

    Serial.printf("Free PSRAM after Wi-Fi init: %u bytes\n", ESP.getFreePsram());
    Serial.printf("Free heap after Wi-Fi init: %u bytes\n", ESP.getFreeHeap());









// mp3 radio
url_audio_strm.setPowerSave(true);
    // good net mp3~ url.begin("http://stream.srg-ssr.ch/m/rsj/mp3_128","audio/mp3");
    url_audio_strm.begin("http://stream.srg-ssr.ch/m/rsj/mp3_128","audio/mp3");
    ///url.begin("http://stream.srg-ssr.ch/m/rsj/mp3_128","audio/mp3");
//        randomNumber = esp_random();
//
//  randomInRange = 1000 + (randomNumber % (9999 - 1000 + 1));
//
//
//
//  record_init();
//    Serial.println("record success");
//
//  wss_init();
}

void loop(){
    copier.copy();

//if(wss_got){ // new mp3 in~
//        set_false_wss_got();
//        url_audio_strm.end();
//        delay(200);
//        url_audio_strm.begin( get_wss_mp3(), "audio/mp3" );
//    }


//    if(urlDownloadFinishedFlag==1 && endOfMP3Flag == 1) {
//      client.poll();
//      wss_loop();
//  }
//
//
//
//  record_loop();
//   audioplayer_loop();
}
