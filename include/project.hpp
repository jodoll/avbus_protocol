#if !defined(UNO) && !defined(ESP32)
#define ESP32
#endif

#if defined(ESP32)
#define WEBSERVER
#endif

#ifndef WIFI_SSID
#define WIFI_SSID "ssid"
#endif

#ifndef WIFI_PASSWORD
#define WIFI_PASSWORD "password"
#endif