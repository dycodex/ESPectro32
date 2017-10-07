/*
 * WiFiManager.cpp
 *
 *  Created on: Jun 4, 2017
 *      Author: andri
 */

#include "WiFiManager.h"
#include <string>
#include <sstream>
#include "soc/soc.h"

#define NVS_NAMESPACE "DCX_WIFI_STA"
#define NVS_STA_SSID_NAME_KEY 	"STA_SSID_NAME"
#define NVS_STA_SSID_PASS_KEY 	"STA_SSID_PASS"
#define NVS_STA_CONFGRD_KEY 	  	"STA_CONFGRD"

#define IOT_CHECK(tag, a, ret)  if(!(a)) {                                             	\
        ESP_LOGE(tag,"%s:%d (%s)", __FILE__, __LINE__, __FUNCTION__);      				\
        return (ret);                                                                   \
}

#define ERR_ASSERT(tag, param)  	IOT_CHECK(tag, (param) == ESP_OK, ESP_FAIL)
#define POINT_ASSERT(tag, param) 	IOT_CHECK(tag, (param) != NULL, ESP_FAIL)
#define RES_ASSERT(tag, res, ret) 	IOT_CHECK(tag, (res) != pdFALSE, ret)

#define WIFI_CONNECTED_EVT	BIT0
#define WIFI_STOP_REQ_EVT 	BIT1

// Debug tag in esp log
static const char* TAG = "WiFiMgr";
// Create an event group to handle different WiFi events.
static EventGroupHandle_t s_wifi_event_group = NULL;
// Mutex to protect WiFi connect
static xSemaphoreHandle s_wifi_mux = NULL;

// Smart Config related
#define SC_DONE_EVT        	BIT2//BIT0
#define SC_STOP_REQ_EVT 		BIT3//BIT1
//static xSemaphoreHandle s_sc_mux = NULL;
//static EventGroupHandle_t s_sc_event_group = NULL;
static smartconfig_status_t currentSmartConfigStatus_ = SC_STATUS_WAIT;

WiFiManager::WiFiManager():
	Task(0, "wifiManagerTask", 2048, 10)
{
}

WiFiManager::~WiFiManager() {
	if (savedStaConfig == NULL) {
		free(savedStaConfig);
		savedStaConfig = NULL;
	}
}

static esp_err_t wifi_event_handler(void *ctx, system_event_t *event)
{
	WiFiManager *mgr = (WiFiManager*)ctx;
	mgr->notifyEvent(event);


    return ESP_OK;
}

void WiFiManager::notifyEvent(system_event_t* event) {
	switch (event->event_id) {
		case SYSTEM_EVENT_STA_START:
			currentStatus_ = WIFI_STATUS_STA_CONNECTING;
			ESP_LOGI(TAG, "SYSTEM_EVENT_STA_START\n");
			break;
//		case SYSTEM_EVENT_STA_CONNECTED:
//			currentStatus_ = WIFI_STATUS_STA_CONNECTING;
//			ESP_LOGI(TAG, "SYSTEM_EVENT_STA_CONNECTED\n");
//			ESP_LOGI(TAG, "SYSREM_EVENT_STA_GOT_IP: %s\n", getStationIpAddress().c_str());
//			break;
		case SYSTEM_EVENT_STA_CONNECTED:
		case SYSTEM_EVENT_STA_GOT_IP:
			currentStatus_ = WIFI_STATUS_STA_CONNECTED;
			ESP_LOGI(TAG, "SYSTEM_EVENT_STA_CONNECTED\n");
			//ESP_LOGI(TAG, "SYSREM_EVENT_STA_GOT_IP: %s\n", getStationIpAddress().c_str());

			// Set event bit to sync with other tasks.
			xEventGroupSetBits(s_wifi_event_group, WIFI_CONNECTED_EVT);

			//Save WiFi
			if (newConnection_) {
				wifi_config_t local_wifi_config;
				esp_wifi_get_config(ESP_IF_WIFI_STA, &local_wifi_config);
				saveStaConfig(&local_wifi_config.sta);
			}

			if (wifiConnectedCallback_) {
				wifiConnectedCallback_(newConnection_);
			}

			break;
		case SYSTEM_EVENT_STA_DISCONNECTED:
			currentStatus_ = WIFI_STATUS_STA_DISCONNECTED;
			ESP_LOGI(TAG, "SYSTEM_EVENT_STA_DISCONNECTED\n");
			esp_wifi_connect();
			// Clear event bit so WiFi task knows the disconnect-event
			xEventGroupClearBits(s_wifi_event_group, WIFI_CONNECTED_EVT);

			if (wifiDisconnectedCallback_) {
				wifiDisconnectedCallback_();
			}

			break;
		default:
			//printf("Get default WiFi event: %d\n", event->event_id);
			if (wifiEventCallback_) {
				wifiEventCallback_(event);
			}
			break;
	}

}

esp_err_t WiFiManager::begin(wifi_mode_t mode, bool autoConnect) {
	currentMode_ = mode;

#if DEBUG_EN
    esp_log_level_set(TAG, ESP_LOG_DEBUG);
#endif
    if (s_wifi_mux == NULL) {
        s_wifi_mux = xSemaphoreCreateMutex();
        POINT_ASSERT(TAG, s_wifi_mux);
    }
    tcpip_adapter_init();
    // hoop WiFi event handler
    ERR_ASSERT(TAG, esp_event_loop_init(wifi_event_handler, this));
    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT()
    // Init WiFi
    ERR_ASSERT(TAG, esp_wifi_init(&cfg));
    ERR_ASSERT(TAG, esp_wifi_set_storage(WIFI_STORAGE_RAM));
    esp_wifi_set_mode(mode);
    esp_wifi_start();
    // Init event group
    s_wifi_event_group = xEventGroupCreate();
    POINT_ASSERT(TAG, s_wifi_event_group);

//    xEventGroupClearBits(s_wifi_event_group, WIFI_CONNECTED_EVT);

    if (savedStaConfig == NULL) {
    		savedStaConfig = (wifi_sta_config_t*)malloc(sizeof(wifi_sta_config_t));
    }

    if (autoConnect) {
		esp_err_t loadRes = loadSavedStaConfig(savedStaConfig);
		if (loadRes == ESP_OK) {
				//if (autoConnect) {
					//ESP_LOGI(TAG, "SSID %s, PASS %s", savedStaConfig->ssid, savedStaConfig->password);
					return connectToAP((const char*)savedStaConfig->ssid, (const char*)savedStaConfig->password);
				//}
		}
		else {
				ESP_LOGI(TAG, "No WiFi credentials stored. Call connectToAP or start SmartConfig");
				//if (autoConnect) {
					//start smart config?
					startSmartConfig();
				//}
		}
    }

    return ESP_OK;
}

esp_err_t WiFiManager::saveStaConfig(wifi_sta_config_t *sta_config) {
	esp_err_t ret;
	nvs_handle my_handle;

	ESP_LOGI(TAG, "Saving WiFi creds to NVS");

	ret = nvs_open(NVS_NAMESPACE, NVS_READWRITE, &my_handle);
	if (ret != ESP_OK) {
		ESP_LOGE(TAG, "Error (%d) opening NVS!\n", ret);
	}

	char *ssid = (char*)sta_config->ssid;
	char *pass = (char*)sta_config->password;
	if (strlen(ssid) > 0) {
		ret = nvs_set_str(my_handle, NVS_STA_SSID_NAME_KEY, ssid);
	}

	if (strlen(pass) > 0) {
		ret = nvs_set_str(my_handle, NVS_STA_SSID_PASS_KEY, pass);
	}

	return ret;
}

esp_err_t WiFiManager::loadSavedStaConfig(wifi_sta_config_t *saved_sta_config) {

    esp_err_t ret;
    nvs_handle my_handle;

    size_t required_size;
    char* sta_ssid = "";
    char* sta_password = "";

    ESP_LOGI(TAG, "Opening Non-Volatile Storage (NVS)\n");

    ret = nvs_open(NVS_NAMESPACE, NVS_READWRITE, &my_handle);

    // TODO factory RESET.
    // nvs_erase_key(my_handle, "sta_ssid");
    // nvs_erase_key(my_handle, "sta_password");

    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Error (%d) opening NVS\n", ret);
        return ESP_FAIL;
    }

    ret = nvs_get_str(my_handle, NVS_STA_SSID_NAME_KEY, NULL, &required_size);
    switch (ret) {
        case ESP_OK:
            sta_ssid = (char*)malloc(required_size);

            ret = nvs_get_str(my_handle, NVS_STA_SSID_NAME_KEY, sta_ssid, &required_size);
            strncpy((char *)saved_sta_config->ssid, sta_ssid, required_size);
            free(sta_ssid);

            ESP_LOGI(TAG, "Read wifi ssid from NVS %s\n", sta_ssid);
            break;
        case ESP_ERR_NVS_NOT_FOUND:
            ESP_LOGE(TAG, "The ssid NVS value is not initialized yet!\n");
            break;
        default:
            ESP_LOGE(TAG, "NVS Error (%d) reading!\n", ret);
    }

    required_size = 0;

    ret = nvs_get_str(my_handle, NVS_STA_SSID_PASS_KEY, NULL, &required_size);
    switch (ret) {
        case ESP_OK:
            sta_password = (char*)malloc(required_size);

            ret = nvs_get_str(my_handle, NVS_STA_SSID_PASS_KEY, sta_password, &required_size);
            strncpy((char *)saved_sta_config->password, sta_password, required_size);
            free(sta_password);

            ESP_LOGI(TAG, "Read wifi password from NVS\n");
            break;
        case ESP_ERR_NVS_NOT_FOUND:
            ESP_LOGE(TAG, "The password NVS value is not initialized yet!\n");
            break;
        default:
            ESP_LOGE(TAG, "NVS Error (%d) reading!\n", ret);
    }

    if (strlen(sta_ssid) == 0 || strlen(sta_password) == 0) {
        return ESP_FAIL;
    }

    return ESP_OK;
}

esp_err_t WiFiManager::connectToAP(const char* ssid, const char* pwd, uint32_t ticks_to_wait) {

	if (savedStaConfig != NULL && strlen((const char*)savedStaConfig->ssid) > 0) {
		if ((strcmp((const char*)savedStaConfig->ssid, ssid) != 0) || (strcmp((const char*)savedStaConfig->password, pwd) != 0)) {
			ESP_LOGI(TAG, "Connecting to a new WiFi SSID");
			newConnection_ = true;
		}
		else {
			newConnection_ = false;
		}
	}

	connectTimeout_ = ticks_to_wait;

	// Take mutex
	BaseType_t res = xSemaphoreTake(s_wifi_mux, ticks_to_wait);
	RES_ASSERT(TAG, res, ESP_ERR_TIMEOUT);

	// Clear stop event bit
	xEventGroupClearBits(s_wifi_event_group, WIFI_STOP_REQ_EVT);
	wifi_config_t wifi_config;
	::memset(&wifi_config, 0, sizeof(wifi_config_t));
	// Connect router
	strcpy((char *)wifi_config.sta.ssid, ssid);
	strcpy((char *)wifi_config.sta.password, pwd);
	esp_wifi_set_config(ESP_IF_WIFI_STA, &wifi_config);
	esp_wifi_start();
	esp_wifi_connect();

//	// Wait event bits
//	EventBits_t uxBits;
//	uxBits = xEventGroupWaitBits(s_wifi_event_group, WIFI_CONNECTED_EVT | WIFI_STOP_REQ_EVT, false, false, ticks_to_wait);
//	esp_err_t ret;
//	// WiFi connected event
//	if (uxBits & WIFI_CONNECTED_EVT) {
//		ESP_LOGI(TAG, "WiFi connected");
//		ret = ESP_OK;
//	}
//	// WiFi stop connecting event
//	else if (uxBits & WIFI_STOP_REQ_EVT) {
//		ESP_LOGI(TAG, "WiFi connecting stop.");
//		// Clear stop event bit
//		xEventGroupClearBits(s_wifi_event_group, WIFI_STOP_REQ_EVT);
//		ret = ESP_FAIL;
//	}
//	// WiFi connect timeout
//	else {
//		esp_wifi_stop();
//		ESP_LOGW(TAG, "WiFi connect fail");
//		ret = ESP_ERR_TIMEOUT;
//	}
//	xSemaphoreGive(s_wifi_mux);

	ESP_LOGW(TAG, "WiFi start connecting...");

	struct timeval tv = {0};
	struct timezone *tz = {0};
	gettimeofday(&tv, &tz);
	lastConnectingCheckMicros_ = tv.tv_usec + tv.tv_sec * 1000000;

	return ESP_OK;//ret;
}

void WiFiManager::disconnect() {
	esp_wifi_stop();
	//xEventGroupSetBits(s_wifi_event_group, WIFI_STOP_REQ_EVT);

	xEventGroupSetBits(s_wifi_event_group, WIFI_STOP_REQ_EVT | SC_STOP_REQ_EVT);
}

tcpip_adapter_ip_info_t WiFiManager::getStationIpInfo() {
	tcpip_adapter_ip_info_t ipInfo;
	tcpip_adapter_get_ip_info(TCPIP_ADAPTER_IF_STA, &ipInfo);

	return ipInfo;
}

bool WiFiManager::waitForConnection() {
	EventBits_t uxBits = xEventGroupWaitBits(s_wifi_event_group, WIFI_CONNECTED_EVT, false, false, connectTimeout_);
	return (uxBits & WIFI_CONNECTED_EVT);
}

void WiFiManager::run() {

	struct timeval tv = {0};
	struct timezone *tz = {0};
	gettimeofday(&tv, &tz);
	uint64_t now = tv.tv_usec + tv.tv_sec * 1000000;
	uint64_t delta = now - lastConnectingCheckMicros_;

	bool timeoutReached = delta > (connectTimeout_*1000);
	//bool timeoutReached = (currentStatus_ != WIFI_STATUS_STA_SMARTCONFIG)? (delta > (connectTimeout_*1000)): (delta > (4*connectTimeout_*1000));

	// Wait event bits
	EventBits_t uxBits;
	uxBits = xEventGroupWaitBits(s_wifi_event_group, SC_DONE_EVT | SC_STOP_REQ_EVT | WIFI_CONNECTED_EVT | WIFI_STOP_REQ_EVT, false, false, 500);//connectTimeout_);

	esp_err_t ret;

	// WiFi connected event
	//if (uxBits & WIFI_CONNECTED_EVT) {
	if (uxBits & (WIFI_CONNECTED_EVT || SC_DONE_EVT)) {
		//ESP_LOGI(TAG, "WiFi connected");
		ret = ESP_OK;
	}
	// WiFi stop connecting event
	//else if (uxBits & WIFI_STOP_REQ_EVT) {
	else if (uxBits & (WIFI_STOP_REQ_EVT || SC_STOP_REQ_EVT)) {
		ESP_LOGI(TAG, "WiFi connecting stop.");
		// Clear stop event bit
		//xEventGroupClearBits(s_wifi_event_group, WIFI_STOP_REQ_EVT);
		xEventGroupClearBits(s_wifi_event_group, (WIFI_STOP_REQ_EVT | SC_STOP_REQ_EVT));
		ret = ESP_FAIL;
	}
	// WiFi connect timeout
//	else {
//		esp_wifi_stop();
//		ESP_LOGW(TAG, "WiFi connect fail");
//		ret = ESP_ERR_TIMEOUT;
//	}
	else {
		//Connecting...
		if (timeoutReached) {
			//timeout
			ESP_LOGW(TAG, "WiFi connect TIMEOUT");

			//esp_wifi_stop();
			xSemaphoreGive(s_wifi_mux); //so that sc start

			if (currentStatus_ != WIFI_STATUS_STA_SMARTCONFIG) {
				//start smart config
				currentStatus_ = WIFI_STATUS_STA_SMARTCONFIG;
				lastConnectingCheckMicros_ = now;

				startSmartConfig();
			}
			else {
				esp_smartconfig_stop();
				esp_wifi_stop();
			}
		}
		else {
			//printf(".");

			//connecting callback
			if (wifiConnectingCallback_) {
				wifiConnectingCallback_(delta, currentStatus_);
			}
		}
	}

	if (uxBits) {
		xSemaphoreGive(s_wifi_mux);
	}
}

void WiFiManager::runAsync(void* taskData) {
	for(;;) {
		run();
		vTaskDelay(1/portTICK_PERIOD_MS);
	}
}

std::string WiFiManager::getStationIpAddress() {
	tcpip_adapter_ip_info_t ipInfo = getStationIpInfo();
	std::stringstream s;

	ip4_addr1_16(&ipInfo.ip);

	s << (int) ip4_addr1_16(&ipInfo.ip) << '.' << (int) ip4_addr2_16(&ipInfo.ip) << '.' << (int) ip4_addr3_16(&ipInfo.ip) << '.' << (int) ip4_addr4_16(&ipInfo.ip);

	return s.str();
}

static void sc_callback(smartconfig_status_t status, void *pdata)
{
	wifi_config_t *wifi_config = (wifi_config_t*)pdata;

    switch (status) {
        case SC_STATUS_WAIT:
        	currentSmartConfigStatus_ = SC_STATUS_WAIT;
            ESP_LOGI(TAG, "SC_STATUS_WAIT\n");
            break;
        case SC_STATUS_FIND_CHANNEL:
        	currentSmartConfigStatus_ = SC_STATUS_FIND_CHANNEL;
            ESP_LOGI(TAG, "SC_STATUS_FIND_CHANNEL\n");
            break;
        case SC_STATUS_GETTING_SSID_PSWD:
        	currentSmartConfigStatus_ = SC_STATUS_GETTING_SSID_PSWD;
            ESP_LOGI(TAG, "SC_STATUS_GETTING_SSID_PSWD\n");
            break;
        case SC_STATUS_LINK:
        	currentSmartConfigStatus_ = SC_STATUS_LINK;
            ESP_LOGI(TAG, "SC_STATUS_LINK\n");
            ESP_LOGI(TAG, "SSID:%s\n", wifi_config->sta.ssid);
            ESP_LOGI(TAG, "PASSWORD:%s\n", wifi_config->sta.password);
            esp_wifi_set_mode(WIFI_MODE_STA);
            esp_wifi_set_config(ESP_IF_WIFI_STA, wifi_config);
            esp_wifi_connect();
            break;
        case SC_STATUS_LINK_OVER:
        	currentSmartConfigStatus_ = SC_STATUS_LINK_OVER;
            ESP_LOGI(TAG, "SC_STATUS_LINK_OVER\n");
            if (pdata != NULL) {
                uint8_t phone_ip[4] = { 0 };
                MEMCPY(phone_ip, (uint8_t* )pdata, 4);
                ESP_LOGI(TAG, "Phone ip: %d.%d.%d.%d\n", phone_ip[0], phone_ip[1], phone_ip[2], phone_ip[3]);
            }
            //xEventGroupSetBits(s_sc_event_group, SC_DONE_EVT);
            xEventGroupSetBits(s_wifi_event_group, SC_DONE_EVT);
            break;
        default:
            break;
    }
}

esp_err_t WiFiManager::startSmartConfig(smartconfig_type_t sc_type, uint32_t ticks_to_wait) {


//	if (s_sc_event_group == NULL) {
//		s_sc_event_group = xEventGroupCreate();
//		POINT_ASSERT(TAG, s_sc_event_group);
//	}

	//smartconfig
//	if (s_sc_mux == NULL) {
//		s_sc_mux = xSemaphoreCreateMutex();
//		POINT_ASSERT(TAG, s_sc_mux);
//	}

//	portBASE_TYPE res = xSemaphoreTake(s_sc_mux, ticks_to_wait);
	portBASE_TYPE res = xSemaphoreTake(s_wifi_mux, ticks_to_wait);
	if (res != pdPASS) {
		return ESP_ERR_TIMEOUT;
	}

	ESP_LOGI(TAG, "STARTING SmartConfig\n");

//	xEventGroupClearBits(s_sc_event_group, SC_STOP_REQ_EVT);
	xEventGroupClearBits(s_wifi_event_group, SC_STOP_REQ_EVT);

	ESP_ERROR_CHECK( esp_smartconfig_set_type(sc_type) );
	//ERR_ASSERT(TAG, esp_smartconfig_fast_mode(true));
	ESP_ERROR_CHECK( esp_smartconfig_start(sc_callback) );

	return ESP_OK;
}


