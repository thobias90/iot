# My IoT Project

- Connect to a Network
- Publish MQTT packets
- Subscribe to topic
- Monitor PushButton
- Drive a LED

## How to build this project

Follow detailed instructions on how to get-started with ESP-IDF.

Select the instructions depending on Espressif chip installed on your development board:

- [ESP32 Getting Started Guide](https://docs.espressif.com/projects/esp-idf/en/stable/get-started/index.html)


## Project folder contents

ESP-IDF projects are built using CMake. The project build configuration is contained in `CMakeLists.txt` files that provide set of directives and instructions describing the project's source files and targets (executable, library, or both).

Below is short explanation of remaining files in the project folder.

```
├── CMakeLists.txt
├── main
|   ├── wifi
│   │   ├── wifi_connect.c
│   │   └── wifi_connect.h
│   ├── CMakeLists.txt
│   └── iot_main.c
└── README.md                  This is the file you are currently reading
```

For more information on structure and contents of ESP-IDF projects, please refer to Section [Build System](https://docs.espressif.com/projects/esp-idf/en/latest/esp32/api-guides/build-system.html) of the ESP-IDF Programming Guide.