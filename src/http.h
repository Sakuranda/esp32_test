#ifndef __HTTP_H
#define __HTTP_H


enum class HTTP_STATUS : char {
    HTTP_DOWNLOAD_MP3_SUCCESS = 1,
    HTTP_DOWNLOAD_MP3_ERROR,
    // 可以继续添加其他的HTTP状态
};

HTTP_STATUS http_download_mp3(char* urlInput, char* filepath);

#endif