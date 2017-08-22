Examples
========

This folder contains examples for ESP-IDF framework.

## Using

Say you want to use `ex_wifi_conn_anim.hpp` example, which shows how to connect to WiFi and show connecting animation on on-board Led Matrix of ESPectro32. in `main.cpp` file of ESP-IDF project (take a look the example [here](https://github.com/dycodex/ESPectro32-IDF)), you should do:

```#include <ex_wifi_conn_anim.hpp>```

Then, inside `app_main()` method, just call the function in the example file:

```load_ex_wifi_conn_anim();```

So, the complete code may look like:
```
#include <ex_wifi_conn_anim.hpp>

void app_main(void)
{
	initArduino();
	Serial.begin(115200);

	ESPectro32.begin();

	//load examples
	load_ex_wifi_conn_anim();
}
```
