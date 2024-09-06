//#include <record.h>
//
//#include <wss.h>
//
//#define SD_CS          5//sd
//#define SPI_MOSI      23//sd
//#define SPI_MISO      19//sd
//#define SPI_SCK       18//sd
//
////Audio audio;
//File filerecord;
//
//const int recordingTimeSec = 5; // 录制时间  秒
//// 判断按钮是否被按过
//int key1 = 0;
//int key2 = 0;
//
//bool key_down = 0;
//bool key_up = 0;
//bool key_also_down = 0;
//
//bool key_state = 1;
//bool last_key_state = 1;
//
//bool send_flag = 0;
//bool playFinishedFlag = 1;
//bool endOfMP3Flag = 1;
//bool lastPlayEndFlag = 1;
//
//volatile bool record_flag = 0;
//volatile bool endOfMP3ProcessPlayFlag = 0;
//size_t bytes_written = 0;
//uint8_t i2s_data[64];
//
//uint64_t urlPlayLastTime = 0;
//
//void record_init(void) {
//    pinMode(anniu1, INPUT_PULLUP);  //定义按钮
//    pinMode(anniu2, INPUT_PULLUP);
//    pinMode(LED, OUTPUT);
//    digitalWrite(LED, HIGH);
//
//        // 初始化 I2S 接口
//    i2s_config_t i2s_config = {
//        .mode = (i2s_mode_t)(I2S_MODE_MASTER | I2S_MODE_RX),  // I2S 接收模式
//        .sample_rate = SAMPLE_RATE,
//        .bits_per_sample = BITS_PER_SAMPLE,
//        .channel_format = I2S_CHANNEL_FMT_ONLY_LEFT,  // 单声道
//        .communication_format = I2S_COMM_FORMAT_I2S_MSB,
//        .intr_alloc_flags = ESP_INTR_FLAG_LEVEL1,
//        .dma_buf_count = 8,
//        .dma_buf_len = 64
//    };
//    i2s_pin_config_t pin_config = {
//        .bck_io_num = I2S_BCLK_PIN,
//        .ws_io_num = I2S_LRC_PIN,
//        .data_out_num = I2S_PIN_NO_CHANGE,
//        .data_in_num = I2S_DIN_PIN
//    };
//    i2s_driver_install(I2S_NUM_1, &i2s_config, 0, NULL);
//    i2s_set_pin(I2S_NUM_1, &pin_config);
//
//    delay(500); // 等待 I2S 初始化完成
//}
//
//void record() {
//
//
//
//    if (key_down) {
//        key_down = 0;
//        // 打开文件以写入录音数据
//        filerecord = SD.open("/recording.pcm", FILE_WRITE);
//            if (!filerecord) {
//                Serial.println("Failed to open file for writing.");
//                return;
//            }
//
//        Serial.println("Recording started...");
//        digitalWrite(LED, LOW);
//    }
//
//
//    if (key_also_down) {
//        // 录制音频数据
//
//        //uint32_t recordingStartTime = millis();
//
//        key_also_down = 0;
//        // 从 I2S 接口读取音频数据
//        auto sz = i2s_read(I2S_NUM_0, &i2s_data, sizeof(i2s_data), &bytes_written, portMAX_DELAY);
//        char* p =(char*)(i2s_data);
//        for(int i=0;i<sz;i++)
//        {
//            i2s_data[i] *= 20;  //提高声音
//        }
//        // 将音频数据写入文件
//        filerecord.write(i2s_data, bytes_written);
//    }
//
//
//
//    if (key_up) {
//        key_up = 0;
//        // 关闭文件
//        filerecord.close();
//
//        Serial.println("Recording finished.");
//        digitalWrite(LED, HIGH);
//        delay(10);  // 延迟 5 秒后再次录制（可选）
//
//        send_flag = 1; //send message
//    }
//
//
//}
//
//void record_loop() {
//
//    record();
//
//
//   // digitalRead(anniu2)  0 按下   1 松开
//
//     key_state = digitalRead(anniu2);
//
//     if (key_state == 0 && last_key_state == 1) {
//         key_down = 1;
//     } else if (key_state == 1 && last_key_state == 0) {
//         key_up = 1;
//     } else if (key_state == 0 && last_key_state == 0) {
//         key_also_down = 1;
//     }
//
//     last_key_state = key_state;
//
//}
//
//bool isProcessEndIndexMatch(const std::string& str)
// {
//    // 定义后缀
//    std::string suffix = ".mp3";
//
//    // 确保字符串足够长以包含后缀
//    if (str.length() <= suffix.length()) {
//        return false;
//    }
//
//    // 检查后缀
//    size_t suffixPos = str.rfind(suffix);
//    if (suffixPos == std::string::npos || suffixPos + suffix.length() != str.length()) {
//        return false;  // 字符串不以 ".mp3" 结尾
//    }
//
//    // 提取文件名部分（不包括后缀）
//    std::string fileNamePart = str.substr(0, suffixPos);
//
//    // 提取数字部分
//    std::string numberPart = fileNamePart.substr(4);  // 从 "file" 之后开始提取
//    if (numberPart.empty()) {
//        return false;  // 文件名部分为空
//    }
//
//    // 确保数字部分只包含数字
//    for (char c : numberPart) {
//        if (!isdigit(c)) {
//            return false;  // 文件名部分包含非数字字符
//        }
//    }
//
//    // 转换为整数并比较
//    int fileIndex = std::stoi(numberPart);
//    return fileIndex == processEndIndex;
//}
//
//void audioplayer_init(void) {
//  pinMode(SD_CS, OUTPUT);
//  digitalWrite(SD_CS, HIGH);
//  SPI.begin(SPI_SCK, SPI_MISO, SPI_MOSI);
//  Serial.begin(115200);
//  SD.begin(SD_CS);
//
//  //SD init
//  audio.setPinout(I2S_BCLK_SPK, I2S_LRC_SPK, I2S_DOUT_SPK);
//  audio.setVolume(20); // default 0...21
//}
//
//void audioplayer_connecttohost(char* url) {
//  audio.connecttohost(url);        // mp3
//}
//
//void audioplayer_connecttoFS(fs::FS file, char* path) {
//    audio.connecttoFS(SD, path);
//}
//
//void audioplayer_loop(void) {
//  audio.loop();
//}
//
