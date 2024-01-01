# ESP8266 Alarm Server

## Overview

The ESP8266 Alarm Server is a code designed for Hisilicon and Hisillicon-based NVR devices, such as those from Smonet, that use port 15002 as their alarm server port. The code captures alarm server messages in JSON format and forwards them to an MQTT broker through the Wi-Fi protocol.

## Important Notes

- **Processing Power Requirement:**
  - This program is designed for devices with sufficient processing power, such as certain NVR devices. It is not recommended for use with microcontrollers such as ESP8266 or ESP32 due to their limited processing capabilities.

- **Processing Time:**
  - The ESP8266 may take around 20 to 30 seconds to process one message. During this processing time, other messages will not be processed until the current operation is complete.

- **ESP32 Consideration:**
  - While using an ESP32 may reduce processing time compared to the ESP8266, it is still not recommended for use due to potential limitations.

- **Educational Purpose:**
  - This code is intended solely for educational purposes. It is not recommended for use as a commercial or fixed solution. The provided functionality may have inherent limitations and should not be relied upon for critical applications.

## Getting Started

To use this code, follow these steps:

1. Clone the repository to your local machine.
2. Configure the necessary parameters in the code (e.g., Wi-Fi credentials, MQTT broker details).
3. Upload the code to your ESP8266 device using the Arduino IDE or another compatible platform.

## Contributing

Contributions to enhance the code or address specific issues are welcome. Please follow the [contribution guidelines](CONTRIBUTING.md) for more details.

## License

This project is licensed under the [MIT License](LICENSE), which means you are free to use, modify, and distribute the code for educational and non-commercial purposes.

## Disclaimer

This code is provided "as is" and without warranty. The author is not responsible for any damages or issues arising from the use of this code in a production environment.

