/*
 * ex_sdcard.hpp
 *
 *  Created on: Sep 2, 2017
 *      Author: andri
 */

#ifndef COMPONENTS_ESPECTRO32_EXAMPLES_IDF_EX_SDCARD_HPP_
#define COMPONENTS_ESPECTRO32_EXAMPLES_IDF_EX_SDCARD_HPP_

#include <esp_log.h>
#include "FS.h"
#include "SD.h"
#include "SPI.h"
#include <ESPectro32_Board.h>

const static char *TAG_EX_SDCARD ="SDCARD";

static void list_dir(fs::FS &fs, const char * dirname, uint8_t levels) {

	ESP_LOGI(TAG_EX_SDCARD, "Listing directory: %s\n", dirname);

	File root = fs.open(dirname);
	if(!root){
		ESP_LOGE(TAG_EX_SDCARD, "Failed to open directory");
		return;
	}
	if(!root.isDirectory()){
		ESP_LOGE(TAG_EX_SDCARD, "Not a directory");
		return;
	}

	File file = root.openNextFile();
	while(file){
		if(file.isDirectory()){
			ESP_LOGE(TAG_EX_SDCARD, "  DIR : %s", file.name());
			if(levels){
				list_dir(fs, file.name(), levels -1);
			}
		} else {
			ESP_LOGE(TAG_EX_SDCARD, "  FILE : %s, SIZE : %d", file.name(), file.size());
		}
		file = root.openNextFile();
	}
}

static void load_ex_sdcard(){

	if (!ESPectro32.beginSDCard()) {
		return;
	}

	uint8_t cardType = SD.cardType();

	if (cardType == CARD_NONE){
		ESP_LOGE(TAG_EX_SDCARD, "No SD card attached");
		return;
	}

	ESP_LOGI(TAG_EX_SDCARD, "SD Card type: %s", ((cardType == CARD_MMC)? "MMC": (cardType == CARD_SD)? "SDSC": "SDHC"));

	uint64_t cardSize = SD.cardSize() / (1024 * 1024);
	ESP_LOGI(TAG_EX_SDCARD, "SD Card Size: %lluMB", cardSize);

	list_dir(SD, "/", 0);
}


#endif /* COMPONENTS_ESPECTRO32_EXAMPLES_IDF_EX_SDCARD_HPP_ */
