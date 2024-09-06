// //#include <Arduino.h>
// /**
//  * @file streams-url_mp3-i2s.ino
//  * @author Phil Schatzmann
//  * @brief decode MP3 stream from url and output it on I2S
//  * @version 0.1
//  * @date 2021-96-25
//  *
//  * @copyright Copyright (c) 2021
//  */

// // install https://github.com/pschatzmann/arduino-libhelix.git

// #include "AudioTools.h"
// #include "AudioLibs/AudioA2DP.h"
// #include "AudioCodecs/CodecMP3Helix.h"

// #include <Arduino.h>

// #include <main.h>
// #include <wss.h>
// #include <record.h>
// #include <http.h>

// #include "esp_system.h"  // 包含 esp_random() 函数

// #include "AudioTools.h"
// #include "AudioLibs/AudioA2DP.h"
// #include "AudioCodecs/CodecMP3Helix.h"

// uint32_t randomNumber;
// int randomInRange;

// A2DPStream out2BT;

// URLStream url_audio_strm("ling-test","50505050");
// ///I2SStream i2s; // final output of decoded stream(AudioStream*)
// EncodedAudioStream dec( &out2BT, new MP3DecoderHelix()); // &i2s Decoding stream
// StreamCopy copier(dec, url_audio_strm); // copy url to decoder


// void setup(){
//     delay(200);
//     Serial.begin(115200);

//       randomNumber = esp_random();

//   randomInRange = 1000 + (randomNumber % (9999 - 1000 + 1));

//   WiFi.begin(ssid, password);
//   while (WiFi.status() != WL_CONNECTED) {
//       delay(1000);
//       Serial.println("Connecting to WiFi...");
//   }
//   Serial.println("Connected to WiFi");
  
//   record_init();
//     Serial.println("record success");
//   //audioplayer_init();
//    // Serial.println("audio success");
  



//     AudioLogger::instance().begin(Serial, AudioLogger::Info);
//     auto cfg = out2BT.defaultConfig(TX_MODE);
//     cfg.name = "ES2BT";
//     //cfg.auto_reconnect = true;  // if this is use we just quickly connect to the last device ignoring cfg.name
//     out2BT.begin(cfg);
//     Serial.println("cfg end");
// /*    // setup i2s
//     auto config = i2s.defaultConfig(TX_MODE);
//     // you could define e.g your pins and change other settings
//     config.pin_ws = 32;//10;
//     config.pin_bck = 27;//11;
//     config.pin_data = 33;//12;
//     //config.mode = I2S_STD_FORMAT;
//     i2s.begin(config);  */

//     // setup I2S based on sampling rate provided by decoder
//     dec.begin();
//     Serial.println("dec end");
// // mp3 radio
//     // good net mp3~ url.begin("http://stream.srg-ssr.ch/m/rsj/mp3_128","audio/mp3");
//    // url_audio_strm.begin("http://lhttp.qtfm.cn/live/1649/64k.mp3","audio/mp3");
//     ///url.begin("http://stream.srg-ssr.ch/m/rsj/mp3_128","audio/mp3");
//   wss_init();
// }

// void loop(){
//     copier.copy();

//     if(urlDownloadFinishedFlag==1 && endOfMP3Flag == 1) {
//       client.poll();
//       wss_loop();
//   }

//         if(wss_got){ // new mp3 in~
//         set_false_wss_got();
//         url_audio_strm.end();
//         delay(200);
//         url_audio_strm.begin( get_wss_mp3(), "audio/mp3" );
//     }

//   record_loop();
// //   audioplayer_loop();
// }
