COMPONENT_ADD_INCLUDEDIRS := lib/Commons lib/FreeRTOS-Wrapper lib/Adafruit-GFX lib/Adafruit_IS31FL3731 lib/Network lib/WS281x src examples-idf
#COMPONENT_PRIV_INCLUDEDIRS := cores/esp32/libb64
COMPONENT_SRCDIRS := lib/Commons lib/FreeRTOS-Wrapper lib/Adafruit-GFX lib/Adafruit_IS31FL3731 lib/Network lib/WS281x src 
CXXFLAGS += -fno-rtti -Wno-sign-compare -Wno-reorder -fpermissive