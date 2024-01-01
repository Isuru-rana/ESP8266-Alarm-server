# ESP8266 Alarm Server

## Overview
The ESP8266 Alarm Server is designed for Hesilicon and Hesillicon-based NVR devices, such as Smonet, that utilize port 15002 as their alarm server port. The server captures alarm messages in JSON format and forwards them to an MQTT broker through the Wi-Fi protocol.

## Disclaimer
This code serves as an example program for ESP microcontrollers and is intended solely for educational purposes. Using this code as a commercial or fixed solution is not recommended.

## System Requirements
- ESP8266 or ESP32 microcontroller
- Wi-Fi connectivity
- MQTT broker
- Hesilicon or Hesillicon-based NVR devices with port 15002 for alarm messages

## Important Notes
- The program requires significant processing power. While ESP8266 is capable, it may take approximately 20 to 30 seconds to process one message.
- Messages received during the processing time will not be processed until the current processing is complete.
- Consider using ESP32 to reduce processing time, but it's not recommended for resource-intensive tasks.

## Getting Started
1. Clone this repository to your local environment.
2. Configure the necessary parameters in the code, such as Wi-Fi credentials and MQTT broker details.
3. Upload the code to your ESP8266 or ESP32 device.
4. Monitor the console for debugging information.

## Contributing
Feel free to contribute to the project by submitting bug reports, feature requests, or pull requests. However, keep in mind the educational and experimental nature of this code.

## License
This project is licensed under the [MIT License](LICENSE).

---

**Note:** Always follow ethical and legal guidelines when working with surveillance or alarm systems. This project is meant for educational purposes only.
