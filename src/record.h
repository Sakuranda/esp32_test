//#ifndef __RECORD_H
//#define __RECORD_H
//
//
//#include <Arduino.h>
////#include "Audio.h"
//#include <file.h>
//#include <Wire.h>
//#include <driver/i2s.h>
//
//// I2S pins configuration
//#define I2S_BCLK_PIN    4  // Bit clock pin
//#define I2S_LRC_PIN     22  // Left-right clock pin
//#define I2S_DIN_PIN     34  // Data in pin
//
//#define SAMPLE_RATE     16000   // 采样率 16 kHz
//#define BITS_PER_SAMPLE I2S_BITS_PER_SAMPLE_16BIT  // 采样位深度 16-bit
//#define CHANNELS        1       // 单通道
//
//
//// esp32 Taobao board Speaker
//#define I2S_DOUT_SPK      33//din
//#define I2S_BCLK_SPK      27//bclk
//#define I2S_LRC_SPK       32//brc
//
//
//// 13录音，io14播放
//#define anniu1 13
//#define anniu2 14
//#define LED 2
//
//extern bool send_flag;
//extern bool playFinishedFlag;
//extern bool endOfMP3Flag;
//extern bool lastPlayEndFlag;
//extern volatile bool endOfMP3ProcessPlayFlag;
//
//extern uint64_t urlPlayStartTime;
//extern uint64_t urlPlayEndTime;
//extern uint64_t urlPlayLastTime;
//extern char processEndIndex;
//
//void record_init(void);
//void record_loop(void);
//void audioplayer_init(void);
//
//void audioplayer_connecttohost(char* url);
//void audioplayer_connecttoFS(fs::FS file, char* path);
//
//void audioplayer_loop(void);
//
//
//bool isProcessEndIndexMatch(const std::string& str);
//
///* Audio库CALLBACK */
//// optional
//void audio_info(const char *info){
//    Serial.print("info        "); Serial.println(info);
//
//    std::string infostring(info);
//
//    std::string keyword = "Audio-Length: ";
//    size_t pos = infostring.find(keyword);
//    if (pos != std::string::npos) {
//        std::string value = infostring.substr(pos + keyword.length());
//
//        int length = std::stoi(value);
//
//        length /= 2; //163200length 对应8s160ms
//
//        int audiotime_s = length / 10000;
//        int audiotime_ms = (length % 10000) / 10 ;
//        Serial.printf(" 提取到的音频时长为：%d秒 %d毫秒", audiotime_s, audiotime_ms);
//    }
//
//}
//void audio_id3data(const char *info){  //id3 metadata
//    Serial.print("id3data     ");Serial.println(info);
//}
//void audio_eof_mp3(const char *info){  //end of file
//    Serial.print("eof_mp3     ");Serial.println(info);
//    //lastPlayEndFlag = 1;
//    endOfMP3Flag = 1;
//    urlPlayEndTime = esp_timer_get_time();
//
//    uint64_t runTime = (urlPlayEndTime - urlPlayStartTime) / 1000;
//
//    Serial.print("Play程序运行时长（毫秒）：");
//    Serial.println(runTime);
//
//    uint64_t Timenow = esp_timer_get_time();
//    uint64_t time_p = (Timenow - urlPlayLastTime) / 1000;
//
//    Serial.printf("距离上次播放完毕时长（毫秒）：%d", time_p);
//    urlPlayLastTime = Timenow;
//
//    std::string infostring(info);
//
//    if(isProcessEndIndexMatch(infostring)) {
//        endOfMP3ProcessPlayFlag = 1;
//        if(endOfMP3ProcessPlayFlag) {
//            digitalWrite(LED, LOW);
//            Serial.println("endOfMP3ProcessPlayFlag");
//        }
//    } else {
//        endOfMP3ProcessPlayFlag = 0;
//    }
//
//}
//void audio_showstation(const char *info){
//    Serial.print("station     ");Serial.println(info);
//}
//void audio_showstreamtitle(const char *info){
//    Serial.print("streamtitle ");Serial.println(info);
//}
//void audio_bitrate(const char *info){
//    Serial.print("bitrate     ");Serial.println(info);
//}
//void audio_commercial(const char *info){  //duration in sec
//    Serial.print("commercial  ");Serial.println(info);
//}
//void audio_icyurl(const char *info){  //homepage
//    Serial.print("icyurl      ");Serial.println(info);
//}
//void audio_lasthost(const char *info){  //stream URL played
//    Serial.print("lasthost    ");Serial.println(info);
//}
//void audio_eof_speech(const char *info){
//    Serial.print("eof_speech  ");Serial.println(info);
//}
//
//#endif