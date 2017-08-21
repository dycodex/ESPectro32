/*
 * AppSetting.h
 *
 *  Created on: Jun 13, 2017
 *      Author: andri
 */

#ifndef MAIN_APPSETTING_H_
#define MAIN_APPSETTING_H_

#include "esp_system.h"
#include "esp_partition.h"
#include "nvs_flash.h"
#include "nvs.h"
#include "esp_log.h"
#include <string.h>

#define APPSETTING_DEBUG_PRINT(...)  ESP_LOGI("APPSETTING", __VA_ARGS__);
#define APPSETTING_INFO_PRINT(...)   ESP_LOGI("APPSETTING", __VA_ARGS__);

#define APPSETTING_NAMESPACE		"DCX_APP_SETTING"
#define APPSETTING_KEY			"SETTING_BLOB"

#define SETTING_DEFAULT_AP_NAME             "ESPectro32_000"
#define SETTING_DEFAULT_AP_PASS             "11223344"
#define SETTING_FORCE_RESET                 false

class AppSettingStorage {
public:
	AppSettingStorage();
	virtual ~AppSettingStorage();

	//generic properties
	bool wifiConfigured = false;
	uint16_t UTC = 7;

//	char ssidName[32+1];
//	char ssidPass[32+1];
	char apName[32+1];
	char apPass[32+1];
	char ipAddress[16];

	esp_err_t begin(char *namesp = APPSETTING_NAMESPACE);
	esp_err_t init();
	esp_err_t load();
	esp_err_t save(char *key = APPSETTING_KEY);
	void printVals();

	//convenience
//	esp_err_t saveWiFiSettings(const char *sName, const char *sPass, const char *sIpAddress);

protected:
	nvs_handle nvsHandle_ = 0;
	void reset();
};

extern AppSettingStorage AppSetting;

#endif /* MAIN_APPSETTING_H_ */
