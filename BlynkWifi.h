#ifndef BlynkWifi_h
#define BlynkWifi_h

#include <BlynkApiArduino.h>
#include <Blynk/BlynkProtocol.h>
#include <Adapters/BlynkArduinoClient.h>
#include <ESP8266WiFi.h>
#include <WiFiManager.h>
#include <ESP8266mDNS.h>
#include <ArduinoOTA.h>
#include <EEPROM.h>

// Blynk
#define BLYNK_TOKEN_SIZE    32
#define BLYNK_ADDR_SIZE     32
#define BLYNK_SERVER_ADDR   "iot.vnptphuyen.vn"
#define BLYNK_SERVER_PORT   8084

#define WIFI_RECONNECT_TIME 60000 //60s
#define WIFI_CONNECT_TIMEOUT 60000 //60s


#define DEVICE_NAME_MAX   30

struct blynk_server_t {
  char Token[BLYNK_TOKEN_SIZE + 1];
  char Host[BLYNK_ADDR_SIZE + 1];
  int Port;
};


class BlynkWifi
  : public BlynkProtocol<BlynkArduinoClient>
{
    typedef BlynkProtocol<BlynkArduinoClient> Base;

  public:
    BlynkWifi(BlynkArduinoClient& transp)
      : Base(transp)
    {}

    //------------------------------------
    void init(const char *hostName) {
      // Set Hardware ID
      int len = strlen(hostName);
      strcpy(_deviceID, hostName); _deviceID[len] = '_';
      String(ESP.getChipId(), HEX).toCharArray(_deviceID + len + 1, 7);
      _deviceID[len + 8] = 0;
      BLYNK_LOG2(BLYNK_F("Hardware ID: "), _deviceID);

      readBlynkConfig();
      _1stWifiConnected = false;
      if (WiFi.SSID().length() > 0) {
        connectWifi();
      }
      else configWifi();
    }
    //------------------------------------
    void handle() {
      if (WiFi.status() == WL_CONNECTED) {
        if (_1stWifiConnected) {
          Base::run();
          ArduinoOTA.handle();
        }
        else {
          _1stWifiConnected = true;
          Base::begin(_blynkServer.Token);
          this->conn.begin(_blynkServer.Host, _blynkServer.Port);
          initOTA();
          BLYNK_LOG1(getWifiInfo());
        }
      }
      else if (millis() - _wifiConnectTime > WIFI_CONNECT_TIMEOUT) {
        BLYNK_LOG1(BLYNK_F("Reconnect Wifi"));
        connectWifi();
      }
    }
    //------------------------------------
    void resetWifi() {
      _wifiManager.resetSettings();
      ESP.restart();
    }
    //------------------------------------
    String getWifiInfo() {
      String wifiInfo = "\nWifi Info";
      wifiInfo += "\n\tAP-Name: " + String(WiFi.SSID());
      wifiInfo += "\n\tSignal: " + String(WiFi.RSSI());
      wifiInfo += "\n\tIP-Addr: " + WiFi.localIP().toString();
      wifiInfo += "\n\tGateway: " + WiFi.gatewayIP().toString();
      return wifiInfo;
    }
    //------------------------------------
    const char* getDeviceID() {
      return _deviceID;
    }
    //------------------------------------
    void connectWifi() {
      BLYNK_LOG2(BLYNK_F("Connect Wifi -> "), WiFi.SSID());
      _wifiConnectTime = millis();
      WiFi.mode(WIFI_STA);
      ETS_UART_INTR_DISABLE();
      wifi_station_disconnect();
      ETS_UART_INTR_ENABLE();
      WiFi.begin();
    }
    //------------------------------------
    void configWifi() {
      WiFiManagerParameter paraBlynkToken("Token", "Token of Blynk Project", _blynkServer.Token, BLYNK_TOKEN_SIZE + 1);
      WiFiManagerParameter paraBlynkServer("Host", "Blynk server address", _blynkServer.Host, BLYNK_ADDR_SIZE + 1);
      WiFiManagerParameter paraBlynkPort("Port", "Blynk server port", "8084", 5);
      _wifiManager.addParameter(&paraBlynkToken);
      _wifiManager.addParameter(&paraBlynkServer);
      _wifiManager.addParameter(&paraBlynkPort);

      _wifiManager.autoConnect(_deviceID);

      // Save Blynk Config
      strcpy(_blynkServer.Token, paraBlynkToken.getValue());
      strcpy(_blynkServer.Host, paraBlynkServer.getValue());
      _blynkServer.Port = String(paraBlynkPort.getValue()).toInt();
      writeBlynkConfig();
    }
    //------------------------------------
    String getBlynkConfig() {
      String configStr = "Blynk Settings:";
      configStr += "\n\tServer: " + String(_blynkServer.Host) + ":" + String(_blynkServer.Port);
      configStr += "\n\tToken : " + String(_blynkServer.Token);
      return configStr;
    }

  private:

    //----------------------------------------------
    void readBlynkConfig() {
      EEPROM.begin(sizeof(blynk_server_t));
      EEPROM.get(0, _blynkServer);
      EEPROM.end();
      _blynkServer.Token[BLYNK_TOKEN_SIZE] = 0;
      _blynkServer.Host[BLYNK_ADDR_SIZE] = 0;

      BLYNK_LOG1(getBlynkConfig());
    }
    //----------------------------------------------
    void writeBlynkConfig() {
      BLYNK_LOG1(BLYNK_F("Write Blynk Config"));
      BLYNK_LOG1(getBlynkConfig());

      EEPROM.begin(sizeof(blynk_server_t));
      EEPROM.put(0, _blynkServer);
      EEPROM.commit();
      EEPROM.end();
    }
    //----------------------------------------------

    void initOTA() {
      ArduinoOTA.setHostname(_deviceID);

#ifdef OTA_DEBUG
      ArduinoOTA.onStart([]() {
        OTA_DEBUG.println("\nOTA Start");
      });

      ArduinoOTA.onEnd([]() {
        OTA_DEBUG.println("\nOTA End");
      });

      ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
        OTA_DEBUG.printf("Progress: %u%%\r", (progress / (total / 100)));
      });

      ArduinoOTA.onError([](ota_error_t error) {
        OTA_DEBUG.printf("Error[%u]: ", error);
        if (error == OTA_AUTH_ERROR) OTA_DEBUG.println("Auth Failed");
        else if (error == OTA_BEGIN_ERROR) OTA_DEBUG.println("Begin Failed");
        else if (error == OTA_CONNECT_ERROR) OTA_DEBUG.println("Connect Failed");
        else if (error == OTA_RECEIVE_ERROR) OTA_DEBUG.println("Receive Failed");
        else if (error == OTA_END_ERROR) OTA_DEBUG.println("End Failed");
      });
#endif

      ArduinoOTA.begin();
    }
    //------------------------------------

    char _deviceID[DEVICE_NAME_MAX];
    WiFiManager _wifiManager;
    blynk_server_t _blynkServer;

    bool _1stWifiConnected;
    unsigned long _wifiConnectTime;
};

static WiFiClient _blynkWifiClient;
static BlynkArduinoClient _blynkTransport(_blynkWifiClient);
BlynkWifi Blynk(_blynkTransport);

#include <BlynkWidgets.h>

#endif
