# AWS IoT Samples

This repository contains some examples on how to use AWS IoT and communicate with ESP32 devices. We are using Maker Feather AIoT S3 as our IoT device. 

Reference:

* [Maker Feather AIoT S3](https://sg.cytron.io/p-v-maker-feather-aiot-s3-simplifying-aiot-with-esp32?r=1)
* [Getting Started with Maker Feather AIoT S3 (Arduino IDE)](https://sg.cytron.io/tutorial/getting-started-with-Maker-Feather-AIoT-S3-(ArduinoIDE)?r=1)
* [ESP32 - AWS IoT Tutorial](https://esp32io.com/tutorials/esp32-aws-iot)


## Setting Up AWS IoT Core
Follow the steps in [ESP32 - AWS IoT Tutorial](https://esp32io.com/tutorials/esp32-aws-iot) to setup AWS IoT Core. This comprises of the following steps:

1. Create a **Thing** in AWS IoT and set the **Thing name**. 
1. Create a policy and generate the credentials. You should download all the files containing your certificate and the private keys.
   * AmazonRootCA1.pem
   * xxxx-certificate.pem.crt
   * xxxx-private.pem.key

You can open these files using a text editors. You will need to copy some of the information here to the file `secrets.h` inside the `arduino` folder in this repository. 

## Installing Arduino Libraries
Follow the steps in [ESP32 - AWS IoT Tutorial](https://esp32io.com/tutorials/esp32-aws-iot) to install the required Arduino library. This comprises the following steps:

1. Open the file `mtqq_test.ino` inside the `arduino` directory. 
1. Open the Library Manager in Arduino IDE.
1. Search for `MQTT` by `Joel Gaehwiler`. Click install.
1. Search for `ArduinoJson` by `Benoit Blanchon`. Click install.

## Editing the Credentials

Open and edit the file `secrets.h` inside `arduino` directory in this repository. The file contains something like the following.

```c
#include <pgmspace.h>

#define SECRET
#define THINGNAME "Your THING name in AWS IOT"

const char WIFI_SSID[] = "Your SSID";
const char WIFI_PASSWORD[] = "Your wifi password";
const char AWS_IOT_ENDPOINT[] = "YOUR_END_POINT.amazonaws.com";

// Amazon Root CA 1
static const char AWS_CERT_CA[] PROGMEM = R"EOF(
)EOF";

// Device Certificate
static const char AWS_CERT_CRT[] PROGMEM = R"KEY(
)KEY";

// Device Private Key
static const char AWS_CERT_PRIVATE[] PROGMEM = R"KEY(
)KEY";
```

You need to edit the following:

* Edit the name of your **Thing** under `THINGNAME`.
* Edit your WiFi SSID under `WIFI_SSID` by replacing the string `"Your SSID"`. 
* Edit your WiFi password under `WIFI_PASSWORD` by replacing the string `"Your wifi password"`. 
* Find your endpoint by going to AWS IoT console and select Connect -> Domain Configuration from the left menu. Find the field under "Domain name". It should look something like `xxxx.region-name.amazonaws.com`. 
* Edit the endpoint name under `AWS_IOT_ENDPOINT` by replacing the string `"YOUR_END_POINT.amazonaws.com"`. 
* Open the downloaded files and copy the certificate into the fields:
  * Amazon Root CA 1
  * Device Certificate
  * Device Private Key

## Test Sending Data From ESP32

1. Make sure your WiFi hotspot is on with the correct SSID and password as the one you set inside `secrets.h`. 
1. Compile and upload the file `mtqq_test.ino`. You will see something like the following.

```
[0;32mI (619) esp_image: segment 0: paddr=00010020 vaddr=3c0c00ESP32 connecting to Wi-Fi
...........................
ESP32 connecting to AWS IOT
ESP32  - AWS IoT Connected!
- topic: esp32/esp32-to-aws
- payload:{"timestamp":50607,"data":0}
sent:
- topic: esp32/esp32-to-aws
- payload:{"timestamp":54610,"data":0}
...
```

You can check if you receive any data from the AWS console. Refer to the tutorial in [ESP32 - AWS IoT Tutorial](https://esp32io.com/tutorials/esp32-aws-iot).

1. Go to AWS IoT Core console.
1. From the left menu, find Test -> MQTT test client.
1. Under "Subscribe a topic" tab, type the following string into the "Topic filter": `esp32/esp32-to-aws`. 
1. Click "Subscribe" button.

You should be able to see the data coming in. 

## Test Reading Digital Data From ESP32

In this example, we make use of two digital sensors:

- Touch sensor as a digital button
- PIR sensor for motion detection

Before running the code, you need to connect the two sensors. We make use of the "left" Maker Port for the Touch sensor.

- Red wire to VCC
- Black wire to GND
- Blue wire to OUT

The blue wire of the left Maker Port is connected to Digital Pin 4 (D4). 

We make sue of the "top" Maker Port for the PIR sensor.

1. Orient the PIR sensor such that you see the bottom part where the 3-pin connectors come out facing you.
1. Connect Red wire to left most pin (VCC).
1. Connect Black wire to right most pin (GND).
1. Connect the Blue wire to the middle pin (OUT).

The Blue wire of the top Maker Port is connected to Digital Pin 6 (D6). 

Once you are done, you can do the following steps to test it.

1. Make sure your Wifi Hotspot is turned on to upload to AWS IoT.
1. you can upload the code `mtqq_digital_read.ino` to your Arduino IDE and upload the code. 

You should see the data from the Serial Monitor as something below.

```
- topic: esp32/esp32-to-aws
- payload:{"timestamp":608554,"touch":1,"motion":0}
sent:
```

When you touch the Touch Sensor, you will see the value `1`. When you touch it again the second time, you will see it will toggle to `0`. Similarly, when you move your hand slowly in front of the PIR sensor, it will change to `1` temporarily and back to `0`. 


## Reading IoT Message From Another Device

If you do the previous section, you will see that you can read the sensor data from AWS IoT monitor. You can also read these data in any other devices such as your local computer. To run this test.

1. Do all the steps in the previous section and upload `mtqq_test.ino` to your ESP32.
1. Open `receive_message.py` in your local computer editor. 
1. Download the AWS certificate and edit the following part in that file.

```python
endpoint = "YOUR_ENDPOINT.amazonaws.com"
cert_filepath = "PATH-TO-YOUR-certificate.pem.crt"
key_filepath = "PATH-TO-YOUR-private.pem.key"
client_id = "basicPubSub"

message_topic = "esp32/esp32-to-aws"
```

Make sure you type in the same `message_topic` with that you put into `mtqq_test.ino` under `AWS_IOT_PUBLISH_TOPIC`. 

```c
#define AWS_IOT_PUBLISH_TOPIC "esp32/esp32-to-aws"
#define AWS_IOT_SUBSCRIBE_TOPIC "esp32/aws-to-esp32"
```

Note that in this case our ESP32 is the producer while our local computer is the consumer. This means that the topic inside our arduino code is called **PUBLISH_TOPIC**. On the other hand, our local computer Python's code will be the one **SUBSCRIBE**-ing this topic.

You can run the python code from the root directory of this repository as follows.

```python
python scripts/receive_message.py
```

You will see something like the following.

```shell
==== Received message from topic 'esp32/esp32-to-aws': {"timestamp": 116085, "data": 0} ====
116085 0
looping waiting for message.
looping waiting for message.
looping waiting for message.
looping waiting for message.
```

Currently, the arduino is reading the data from analog port 0.

```c
  message["timestamp"] = millis();
  message["data"] = analogRead(0); // Or you can read data from other sensors
```

## Reading Data from IoT Sensors and Store to Database

Connect the ESP32 as discussed in the section "Test Reading Digital Data From ESP32". 

1. Open and upload `mtqq_digital_read.ino` to your ESP32 device.
1. In your local computer, run the Python's script `read_sensors.py`. 

```shell
python scripts/read_sensors.py
```

You should see output similar to the following.

```shell
New database created and table initialized.
==== Starting client ====
Lifecycle Connection Attempt
Connecting to endpoint: 'a3fjs7l9x4f669-ats.iot.ap-southeast-1.amazonaws.com' with client ID'basicPubSub'
Lifecycle Connection Success with reason code:<ConnectReasonCode.SUCCESS: 0>

==== Subscribing to topic 'esp32/esp32-to-aws' ====
Suback received with reason code:[<SubackReasonCode.GRANTED_QOS_1: 1>]


==== Received message from topic 'esp32/esp32-to-aws': {"timestamp":20013,"touch":0,"motion":0} ====

timestamp: 20013, Touch toogle: 0, Motion sensor: 0
Inserted: 20013 0 0
```

There are two data that you can take a look at. 

- Touch sensor
- Motin sensor

When you press the Touch sensor once, it will toggle the `touch` value to 1. When you press the second time, it will toggle back to 0.

```shell
==== Received message from topic 'esp32/esp32-to-aws': {"timestamp":28019,"touch":1,"motion":0} ====

timestamp: 28019, Touch toogle: 1, Motion sensor: 0
Inserted: 28019 1 0

==== Received message from topic 'esp32/esp32-to-aws': {"timestamp":32022,"touch":0,"motion":0} ====

timestamp: 32022, Touch toogle: 0, Motion sensor: 0
Inserted: 32022 0 0
```

Similarly, if you put your hand in front of the motion sensor, it will change to 1 and go back to 0.

```shell
==== Received message from topic 'esp32/esp32-to-aws': {"timestamp":36025,"touch":0,"motion":1} ====

timestamp: 36025, Touch toogle: 0, Motion sensor: 1
Inserted: 36025 0 1
```


