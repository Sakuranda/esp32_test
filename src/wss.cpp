//#include <wss.h>
//
//#include <http.h>
//#include <random>
//
//
//extern int randomInRange;
//
//using namespace websockets;
//
//WebsocketsClient client;
//
//const char* ssid = "ling-test";
//char* message_id = "mvp04-message-wddd";
//const char* session_id = "mvp04-session-wddd";
//char* url = "";
//bool flag = 1;
//


// DELETE PART FUNCTION FOR PRIVATE DEVELOPMENT




//const char* fakeurl = ;
//
//volatile bool url_change_flag = 0;
//volatile bool endOfWebstreamFlag = false;
//volatile bool firstFlag = false;
//bool urlDownloadFinishedFlag  = 1;
//volatile bool urlRecvFlag = 0;
//
//char urlWaitForDownloadIndex = 0;
//const char *urlWaitForDownload[] = {"","","","",""};
//int playindex = 0;
//
//uint64_t urlPlayStartTime=0;
//uint64_t urlPlayEndTime=0;
//uint64_t urlLastGetTime = 0;
//uint64_t startSendFlag = 0;
//uint64_t urlNowGetTime=0;
//
//char processEndIndex = 0;
//
//bool urlProcessRecvSuccessFlag = 0;
//
//const int SIZE = 20;
//char* urlGetArray[SIZE];
//int urlGetIndex = 0;
//
//int urlProcessIndex = 0;
//int urlDownloadIndex = 0;
//int urlPlayIndex = 0;
//bool wss_url_got = 0;
//
//
//bool wss_got(){
//  return wss_url_got;
//}
//
//void set_false_wss_got(){
//  wss_url_got = 0;
//}
//
//char* get_wss_mp3(){
//  return urlGetArray[urlGetIndex];
//}
//
//char* constCharToChar(const char* constStr) {
//    // Determine length of constStr
//    size_t len = strlen(constStr);
//
//    // Allocate memory for char array (don't forget +1 for null terminator)
//    char* charArray = new char[len + 1];
//
//    // Copy constStr to charArray
//    strcpy(charArray, constStr);
//
//    return charArray;
//}
//
//
//
//void wss_init(void) {
//     /* Connect with wss */
//  client.onMessage(onMessageCallback);
//  client.setCACert(ssl_cert);
//
//  memset(urlWaitForDownload, 0, sizeof(urlWaitForDownload));
//
//   /*
//  URL Format: (Python)
//
//  key = "FK3HRefdoeRGBl7BvlME"
//  token = "Bearer eyJ0eXAiOiJKV1QiLCJhbGciOiJIUzI1NiJ9.eyJ0aW1lU3RhbXAiOjE3MjE2NDMzMjkyODIsImRldmljZV9pZCI6InRlc3QiLCJ1c2VyX2lkIjoidGVzdCIsImV4cCI6MTcyNDIzNTMyOX0.2AGbGIsNJV0hPrb0F3MJ5QU-_ahFDAI2Ds1YpdE-sSg"
//
//  params = [
//      f"x-key={key}",
//      "x-service=chat",
//      "x-version=1.0",
//      f"x-time={int(time.time())}",
//  ]
//  plain_text = "&".join(params)
//  sign = hashlib.md5(plain_text.encode()).hexdigest()
//  params.append(f"x-sign={sign}")
//  params.append("x-client-type=web")
//  params.append("x-client-id=TESTCLIENT")
//  params.append(f"authorization={urllib.parse.quote(token)}")
//
//  url = f"wss://lingos.test.ling.space/ws/chat?{'&'.join(params)}"
//
//  */
//
//
//  bool connected = client.connect(fakeurl);
//  if (connected) {
//    Serial.println("Connected");
//  } else {
//    Serial.println("Connected Failed");
//  }
//
//    //std::string base_id = "esp32test-wwd";
//    // 将基础字符串和随机数拼接起来
//    std::string sessionStr(session_id);
//    std::string full_id = std::string(sessionStr) + "-" + std::to_string(randomInRange);
//    //randomNumber
//  /* Send Session */
//  DynamicJsonDocument doc(200);
//  doc["uuid"] = full_id;
//  doc["event"] = "session";
//  JsonObject data = doc.createNestedObject("data");
//  JsonObject attributes = data.createNestedObject("attributes");
//  attributes["ip_id"] = 5;
//  attributes["nickname"] = "esp32-testss";
//  attributes["needs_tts"] = true;
//
//  String message;
//  serializeJson(doc, message); // json generation will package to func in the future
//  client.send(message.c_str());
//  Serial.println(message.c_str());
//
//
//
//  // sendMessage();
//
//}
//
//void url_download(char* urlInput) {
//
//    //path
//    char path[20];
//    memset(path, 0, sizeof(path));
//    snprintf(path, sizeof(path), "%s%d%s", "/file", urlDownloadIndex, ".mp3");
//
//
//    if(http_download_mp3(urlInput,path) == HTTP_STATUS::HTTP_DOWNLOAD_MP3_SUCCESS) {
//        Serial.println("Download MP3 success");
//        //audioplayer_connecttoFS(SD, path);
//        urlDownloadFinishedFlag = 1;
//        Serial.println("Connect MP3 success");
//        Serial.printf("Played File%d.mp3",urlDownloadIndex);
//    } else {
//        Serial.println("Download MP3 Error!");
//    }
//
//}
//
//void url_play() {
//
//    //path
//    char path[20];
//    memset(path, 0, sizeof(path));
//    snprintf(path, sizeof(path), "%s%d%s", "/file", urlPlayIndex, ".mp3");
//
//
//    audioplayer_connecttoFS(SD, path);
//    //return 0;
//}
//
//void wss_loop() {
//
//  //memset(urlnow, 0, sizeof(urlnow));
//
//  //------------------云端VAD自动发送语音&回传---------------------------
//
//
//
//  //-----------------------------------------------------------------
//
//
//
//
//  //-------------------SEND-------------------------
//    if (endOfWebstreamFlag && send_flag) {
//    //if (1) {
//
//        startSendFlag = esp_timer_get_time();
//        sendPCMData("/recording.pcm");
//        endOfWebstreamFlag = 0;
//        send_flag = 0;
//        delay(100);
//
//
//        //message_id = "message-099";
//    }
//
//
//    if(url_change_flag && playFinishedFlag) {
//      url_change_flag = 0;
//      //audioplayer_connecttohost(url);
//
//    }
//
//    //--------------------------------------------------------
//
//
//
//
//    //--------------------------------------------
//    //urlnetworkplay
////    if((endOfWebStreamFlag == 1) && (urlNetworkPlayIndex < urlGetIndex) && (urlGetArray[urlDownloadIndex+1] != nullptr)) {
////
////    }
//
//
//    //--------------------------------------------------------
//
//
////----------------------------------------------
//    // if((urlDownloadIndex < urlGetIndex) && urlGetArray[urlDownloadIndex+1] != nullptr) {
//    //     //是否需要判别url格式？是否含HTTP？  strncmp(urlWaitForDownload[i], "http", 4) == 0)
//    //     urlDownloadIndex++;
//
//    //     uint64_t urlDownloadStartTime = esp_timer_get_time();
//
//    //     Serial.printf("Download URL : %s",urlGetArray[urlDownloadIndex]);
//    //     urlDownloadFinishedFlag = 0;
//    //     url_download(urlGetArray[urlDownloadIndex]);
//
//    //     uint64_t urlDownloadEndTime = esp_timer_get_time();
//
//    //     uint64_t runTime = (urlDownloadEndTime - urlDownloadStartTime) / 1000;
//    //     Serial.print("Download程序运行时长（毫秒）：");
//    //     Serial.println(runTime);
//
//    // }
//
//    // if(endOfMP3Flag == 1 && (urlDownloadIndex > urlPlayIndex) ) {
//    //     endOfMP3Flag = 0;
//    //     urlPlayStartTime = esp_timer_get_time();
//    //     urlPlayIndex++;
//
//    //     Serial.printf("Play URL : %s",urlGetArray[urlPlayIndex]);
//    //     url_play();
//    // }
////---------------------------------------------------------
//
//}
//
