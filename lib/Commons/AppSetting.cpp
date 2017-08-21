/*
 * AppSetting.cpp
 *
 *  Created on: Jun 13, 2017
 *      Author: andri
 */

#include "AppSetting.h"

AppSettingStorage::AppSettingStorage() {

}

AppSettingStorage::~AppSettingStorage() {
}

esp_err_t AppSettingStorage::begin(char *namesp) {
	esp_err_t err = nvs_flash_init();
	if (err == ESP_ERR_NVS_NO_FREE_PAGES) {
		// NVS partition was truncated and needs to be erased
		const esp_partition_t* nvs_partition = esp_partition_find_first(
				ESP_PARTITION_TYPE_DATA, ESP_PARTITION_SUBTYPE_DATA_NVS, NULL);
		assert(nvs_partition && "partition table must have an NVS partition");
		ESP_ERROR_CHECK( esp_partition_erase_range(nvs_partition, 0, nvs_partition->size) );
		// Retry nvs_flash_init
		err = nvs_flash_init();
	}
	ESP_ERROR_CHECK( err );

	// Open
	err = nvs_open(namesp, NVS_READWRITE, &nvsHandle_);
	return err;
}

esp_err_t AppSettingStorage::load() {
	if (nvsHandle_ == 0) {
		APPSETTING_DEBUG_PRINT("AppSetting not init-ed");
		return ESP_FAIL;
	}

	esp_err_t err;

	if (SETTING_FORCE_RESET) {
		err = init();
	}

	size_t size = sizeof(AppSettingStorage);
	err = nvs_get_blob(nvsHandle_, APPSETTING_KEY, this, &size);

	if (size == 0) {
		APPSETTING_DEBUG_PRINT("Nothing saved yet!\n");
	} else {
		APPSETTING_DEBUG_PRINT("Stored %d", size);
	}

	if (err != ESP_OK) {
		err = init();
	}

	return err;
}

void AppSettingStorage::reset() {
	wifiConfigured = false;
	UTC = 7;

	strcpy(apName, SETTING_DEFAULT_AP_NAME);
	apName[sizeof(SETTING_DEFAULT_AP_NAME) - 1] = '\0';

	strcpy(apPass, SETTING_DEFAULT_AP_PASS);
	apPass[sizeof(SETTING_DEFAULT_AP_PASS) - 1] = '\0';

//	memset(ssidName, 0, sizeof(ssidName) - 1);
//	ssidName[sizeof(ssidName) - 1] = '\0';
//
//	memset(ssidPass, 0, sizeof(ssidPass) - 1);
//	ssidPass[sizeof(ssidName) - 1] = '\0';
}

esp_err_t AppSettingStorage::init() {
	if (nvsHandle_ == 0) {
		APPSETTING_DEBUG_PRINT("AppSetting not init-ed");
		return ESP_FAIL;
	}

	//stuff.init();
	reset();
	return save();
}

esp_err_t AppSettingStorage::save(char *key) {
	if (nvsHandle_ == 0) {
		APPSETTING_DEBUG_PRINT("AppSetting not init-ed");
		return ESP_FAIL;
	}

	size_t size = sizeof(AppSettingStorage);
	esp_err_t err = nvs_set_blob(nvsHandle_, key, this, size);

	return err;
}

void AppSettingStorage::printVals() {
	//APPSETTING_DEBUG_PRINT("configured: %d", stuff.config.configured);
	APPSETTING_DEBUG_PRINT("wifiConfigured: %d", wifiConfigured);
//	APPSETTING_DEBUG_PRINT("configInitialized: %d", stuff.config.configInitialized);
	APPSETTING_DEBUG_PRINT("apName: %s", apName);
	APPSETTING_DEBUG_PRINT("apPass: %s", apPass);
}

AppSettingStorage AppSetting;

//esp_err_t AppSettingStorage::saveWiFiSettings(const char* sName, const char* sPass,
//		const char* sIpAddress) {
//
//	strcpy(ssidName, sName);
//	strcpy(ssidPass, sPass);
//	strcpy(ipAddress, sIpAddress);
//
//	return save();
//}
