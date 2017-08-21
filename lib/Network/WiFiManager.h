/*
 * WiFiManager.h
 *
 *  Created on: Jun 4, 2017
 *      Author: andri
 */

#ifndef COMPONENTS_NETWORK_SRC_WIFIMANAGER_H_
#define COMPONENTS_NETWORK_SRC_WIFIMANAGER_H_


#include <string.h>
#include <functional>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "freertos/event_groups.h"
#include "freertos/semphr.h"
#include "esp_system.h"
#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_event_loop.h"
#include "esp_log.h"
#include "esp_smartconfig.h"
#include "lwip/ip_addr.h"
#include "tcpip_adapter.h"
#include "sys/time.h"
#include "Task.h"
#include "nvs_flash.h"
#include "nvs.h"

#define WIFIMANAGER_DEFAULT_TIMEOUT 60000/portTICK_PERIOD_MS

class WiFiManager: public Task {
public:

	typedef enum {
	    WIFI_STATUS_STA_DISCONNECTED,           /**< ESP32 station disconnected */
	    WIFI_STATUS_STA_CONNECTING,             /**< ESP32 station connecting */
	    WIFI_STATUS_STA_CONNECTED,              /**< ESP32 station connected */
		WIFI_STATUS_STA_SMARTCONFIG
	} Status_t;

	typedef std::function<void(system_event_t *event)> WifiEventCallback;
	typedef std::function<void(bool)> WifiConnectedCallback;
	typedef std::function<void()> WifiDisconnectedCallback;
	typedef std::function<void(uint64_t elapsedTime, Status_t status)> WifiConnectingCallback;

	WiFiManager();
	virtual ~WiFiManager();

	esp_err_t begin(wifi_mode_t mode, bool autoConnect = true);
	esp_err_t connectToAP(const char *ssid, const char *pwd, uint32_t ticks_to_wait = WIFIMANAGER_DEFAULT_TIMEOUT);
	//esp_err_t connectAsync(const char *ssid, const char *pwd, uint32_t ticks_to_wait);
	esp_err_t startSmartConfig(smartconfig_type_t sc_type = SC_TYPE_ESPTOUCH, uint32_t ticks_to_wait = WIFIMANAGER_DEFAULT_TIMEOUT);

	void disconnect();
	void run();
	void runAsync(void *taskData);

	tcpip_adapter_ip_info_t getStationIpInfo();
	std::string getStationIpAddress();

	void onWiFiEvent(WifiEventCallback cb) {
		wifiEventCallback_ = cb;
	}

	void onWiFiConnected(WifiConnectedCallback cb) {
		wifiConnectedCallback_ = cb;
	}

	void onWiFiDisconnected(WifiDisconnectedCallback cb) {
		wifiDisconnectedCallback_ = cb;
	}

	void onWiFiConnecting(WifiConnectingCallback cb) {
		wifiConnectingCallback_ = cb;
	}

	void notifyEvent(system_event_t *event);

	bool waitForConnection();

private:
	wifi_mode_t currentMode_ = WIFI_MODE_NULL;

	WifiEventCallback wifiEventCallback_ = nullptr;
	WifiConnectedCallback wifiConnectedCallback_ = nullptr;
	WifiDisconnectedCallback wifiDisconnectedCallback_ = nullptr;
	WifiConnectingCallback wifiConnectingCallback_ = nullptr;

	Status_t currentStatus_ = WIFI_STATUS_STA_DISCONNECTED;
	uint32_t connectTimeout_ = 0;

	uint64_t lastConnectingCheckMicros_ = 0;

	esp_err_t loadSavedStaConfig(wifi_sta_config_t *saved_sta_config);
	esp_err_t saveStaConfig(wifi_sta_config_t *sta_config);
	bool newConnection_ = false;
	wifi_sta_config_t *savedStaConfig = NULL;
};

#endif /* COMPONENTS_NETWORK_SRC_WIFIMANAGER_H_ */
