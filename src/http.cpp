#include <http.h>

#include <file.h>
#include <HTTPClient.h>

bool flaggg = 1;

void http_init(void) {
    //wifi
    //sd
}



HTTP_STATUS http_download_mp3(char* urlInput, char* filepath) {
    if(1)
        flaggg = 0;
    else
        return HTTP_STATUS::HTTP_DOWNLOAD_MP3_ERROR;

    HTTPClient http;
    HTTP_STATUS status = HTTP_STATUS::HTTP_DOWNLOAD_MP3_ERROR;

    if (http.begin(urlInput)) {
        int httpCode = http.GET();

        if (httpCode > 0) {
            File file = SD.open(filepath, FILE_WRITE);
            if (file) {
                WiFiClient *stream = http.getStreamPtr();
                size_t size = http.getSize();
                uint8_t buff[2560];  // 减小缓冲区大小以节省内存

                while (http.connected() && (size > 0 || size == -1)) {
                    size_t len = stream->available();
                    if (len > 0) {
                        int c = stream->readBytes(buff, min(len, sizeof(buff)));
                        file.write(buff, c);
                        if (size > 0) {
                            size -= c;
                        }
                    } else {
                        delay(10); // 增加适当的延迟
                    }
                }
                file.close();
                Serial.println("File downloaded successfully");
                status = HTTP_STATUS::HTTP_DOWNLOAD_MP3_SUCCESS;
            } else {
                Serial.println("Failed to open file for writing");
            }
        } else {
           Serial.printf("HTTP GET failed, error: %s\n", http.errorToString(httpCode).c_str());
       }
        http.end();
    } else {
        Serial.println("Failed to begin HTTP connection");
    }

    return status;
}


//HTTP_STATUS http_download_mp3(char* urlInput, char* filepath) {
//    if(1)
//        flaggg = 0;
//    else
//        return HTTP_STATUS::HTTP_DOWNLOAD_MP3_ERROR;
//
//    HTTPClient http;
//    HTTP_STATUS status = HTTP_STATUS::HTTP_DOWNLOAD_MP3_ERROR;
//
//    if (http.begin(urlInput)) {
//        int httpCode = http.GET();
//
//        if (httpCode > 0) {
//            File file = SD.open(filepath, FILE_WRITE);
//            if (file) {
//                WiFiClient *stream = http.getStreamPtr();
//                size_t size = http.getSize();
//                uint8_t buff[2560];  // 减小缓冲区大小以节省内存
//
//                while (http.connected() && (size > 0 || size == -1)) {
//                    size_t len = stream->available();
//                    if (len > 0) {
//                        int c = stream->readBytes(buff, min(len, sizeof(buff)));
//                        file.write(buff, c);
//                        if (size > 0) {
//                            size -= c;
//                        }
//                    } else {
//                        delay(10); // 增加适当的延迟
//                    }
//                }
//                file.close();
//                Serial.println("File downloaded successfully");
//                status = HTTP_STATUS::HTTP_DOWNLOAD_MP3_SUCCESS;
//            } else {
//                Serial.println("Failed to open file for writing");
//            }
//        } else {
//            Serial.printf("HTTP GET failed, error: %s\n", http.errorToString(httpCode).c_str());
//        }
//        http.end();
//    } else {
//        Serial.println("Failed to begin HTTP connection");
//    }
//
//    return status;
//}



//HTTP_STATUS http_download_mp3(char* urlInput, char* filepath) {
//    if(1)
//        flaggg = 0;
//    else
//        return HTTP_STATUS::HTTP_DOWNLOAD_MP3_ERROR;
//
//    HTTPClient http;
//    HTTP_STATUS status = HTTP_STATUS::HTTP_DOWNLOAD_MP3_ERROR;
//
//    if (http.begin(urlInput)) {
//        int httpCode = http.GET();
//
//        if (httpCode > 0) {
//
//                WiFiClient *stream = http.getStreamPtr();
//                size_t size = http.getSize();
//                uint8_t buff[128];  // 减小缓冲区大小以节省内存
//
//                while (http.connected() && (size > 0 || size == -1)) {
//                    size_t len = stream->available();
//                    if (len > 0) {
//                        int c = stream->readBytes(buff, min(len, sizeof(buff)));
//
//                        if (size > 0) {
//                            size -= c;
//                        }
//                    }
//
//
//                    //Serial.println("File downloaded successfully");
//
//                }
//            status = HTTP_STATUS::HTTP_DOWNLOAD_MP3_SUCCESS;
//        }
//        else {
//            Serial.printf("HTTP GET failed, error: %s\n", http.errorToString(httpCode).c_str());
//        }
//        http.end();
//    } else {
//        Serial.println("Failed to begin HTTP connection");
//    }
//
//    return status;
//}
