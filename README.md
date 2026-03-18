# 🌧️ ESP32 Rain Detection System with AWS IoT Core

## Overview

This project implements a cloud-connected rain detection system using an ESP32-C6 microcontroller and AWS IoT Core.

The system reads analog data from a rain sensor and determines whether the surface is wet or dry. The processed data is then transmitted securely to AWS IoT Core using the MQTT protocol over TLS, enabling real-time remote monitoring.

![Image (1) (1)](https://github.com/user-attachments/assets/dd2e3395-11ea-474f-941a-7fe5543c1959)

---

## Features

* Real-time rain detection using analog sensor input
* Secure communication using MQTT over TLS
* Cloud-based monitoring via AWS IoT Core
* JSON-formatted data transmission
* Scalable IoT system architecture

---

## System Architecture

Rain Sensor → ESP32 → WiFi → AWS IoT Core

The rain sensor generates an analog signal based on moisture. The ESP32 processes this data and sends it to AWS IoT Core through a secure WiFi connection.

---

## Hardware Components

* ESP32-C6 microcontroller
* Rain sensor module (analog output)
* Jumper wires

---

## Hardware Connections

| Rain Sensor | ESP32 |
| ----------- | ----- |
| VCC         | 3.3V  |
| GND         | GND   |
| AO          | GPIO1 |

---

## MQTT Topics

| Topic              | Purpose             |
| ------------------ | ------------------- |
| rain/sensor/data   | Publish sensor data |
| rain/sensor/status | Device status       |
| rain/sensor/cmd    | Receive commands    |

---

## Example Output

```json
{
  "device": "esp32-rain-sensor",
  "analog": 2100,
  "wet": true
}
```

---

## Software Requirements

* Arduino IDE
* ESP32 board package
* Required libraries:

  * WiFi.h
  * WiFiClientSecure.h
  * PubSubClient.h

---

## Configuration

Sensitive data such as WiFi credentials and AWS certificates are stored in a separate file (`secrets.h`) and are not included in this repository for security reasons.

To run this project:

1. Create a file named `secrets.h`
2. Use the provided `secrets_example.h` as a template
3. Fill in your:

   * WiFi SSID and password
   * AWS IoT endpoint
   * Certificates and private key

---

## How It Works

1. ESP32 connects to WiFi
2. Establishes secure connection to AWS IoT Core
3. Reads analog value from rain sensor
4. Determines wet/dry condition using a threshold
5. Sends data to AWS every 5 seconds
6. Data can be monitored via AWS MQTT test client

---

## Limitations

* No physical output (cloud-based only)
* Sensor readings require calibration
* No long-term data storage

---

## Future Improvements

* Add LED or buzzer for local output
* Store data using AWS DynamoDB
* Create a dashboard for visualization
* Add more environmental sensors

---

## License

This project is licensed under the MIT License.

---

## References

* ESP32 AWS IoT Tutorial
