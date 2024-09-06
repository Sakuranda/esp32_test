

Develop Log:
```
Writing at 0x001fca6a... (98 %)
Writing at 0x00201f7e... (100 %)
Wrote 2062720 bytes (1277654 compressed) at 0x00010000 in 32.6 seconds (effective 505.8 kbit/s)...
Hash of data verified.

Leaving...
Hard resetting via RTS pin...
--- Terminal on COM17 | 115200 8-N-1
--- Available filters and text transformations: colorize, debug, default, direct, esp32_exception_decoder, hexlify, log2file, nocontrol, printable, send_on_enter, time
--- More details at https://bit.ly/pio-monitor-filters
--- Quit: Ctrl+C | Menu: Ctrl+T | Help: Ctrl+T followed by Ctrl+H
ets Jun  8 2016 00:22:57

rst:0x1 (POWERON_RESET),boot:0x33 (SPI_FAST_FLASH_BOOT)
configsip: 0, SPIWP:0xee
clk_drv:0x00,q_drv:0x00,d_drv:0x00,cs0_drv:0x00,hd_drv:0x00,wp_drv:0x00
mode:DIO, clock div:2
load:0x3fff0030,len:1184
load:0x40078000,len:13232
load:0x40080400,len:3028
entry 0x400805e4
PSRAM initialized successfully!
Total PSRAM: 4192107 bytes
Free PSRAM: 4191863 bytes
Free heap: 147452 bytes
[I] A2DPStream.h : 134 - Connecting to ES32_BT_DEV666
[D] Allocator.h : 68 - Allocated 15361
a2dp_buffer.resize
Free PSRAM after Wi-Fi init: 4176483 bytes
Free heap after Wi-Fi init: 147452 bytes
[I] A2DPStream.h : 149 - Starting a2dp_source...
source()
Free PSRAM after Wi-Fi init: 4176483 bytes
Free heap after Wi-Fi init: 146904 bytes
start_raw
Free PSRAM after Wi-Fi init: 4172099 bytes
Free heap after Wi-Fi init: 60356 bytes
[D] A2DPStream.h : 173 - waiting for connection
[D] A2DPStream.h : 173 - waiting for connection
[D] A2DPStream.h : 308 - static void audio_tools::A2DPStream::a2dp_state_callback(esp_a2d_connection_state_t, void*)
[W] A2DPStream.h : 313 - ==> state: Connecting
[D] A2DPStream.h : 173 - waiting for connection
[D] A2DPStream.h : 308 - static void audio_tools::A2DPStream::a2dp_state_callback(esp_a2d_connection_state_t, void*)
[W] A2DPStream.h : 313 - ==> state: Connected
[I] A2DPStream.h : 176 - a2dp_source is connected...
cfg end
Free PSRAM after Wi-Fi init: 4172099 bytes
Free heap after Wi-Fi init: 48296 bytes
[D] AudioIO.h : 30 - void audio_tools::TransformationReader<T>::begin(T*, Stream*) [with T = audio_tools::ReformatBaseStream]
[D] AudioEncoded.h : 140 - virtual bool audio_tools::EncodedAudioOutput::begin()
[D] AudioEncoded.h : 142 - virtual bool audio_tools::EncodedAudioOutput::begin()
[D] AudioCodecsBase.h : 27 - virtual void audio_tools::AudioDecoder::setAudioInfo(audio_tools::AudioInfo)
[D] CodecMP3Helix.h : 82 - virtual bool audio_tools::MP3DecoderHelix::begin()
[D] MetaDataFilter.h : 30 - void audio_tools::MetaDataFilter<Decoder>::begin() [with Decoder = libhelix::MP3DecoderHelix]
dec end
Free PSRAM after Wi-Fi init: 4140955 bytes
Free heap after Wi-Fi init: 48296 bytes
E (9152) wifi:Expected to init 4 rx buffer, actual is 2
E (9154) wifi_init: Failed to deinit Wi-Fi driver (0x3001)
E (9155) wifi_init: Failed to deinit Wi-Fi (0x3001)
[  4334][E][WiFiGeneric.cpp:685] wifiLowLevelInit(): esp_wifi_init 257
[  4352][E][WiFiSTA.cpp:227] begin(): STA enable failed!
Free PSRAM after Wi-Fi init: 4140955 bytes
Free heap after Wi-Fi init: 28216 bytes
Connecting to WiFi...
Connecting to WiFi...
Connecting to WiFi...
Connecting to WiFi...
Connecting to WiFi...
```