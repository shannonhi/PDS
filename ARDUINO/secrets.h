#ifndef SECRETS_H
#define SECRETS_H

// Wi-Fi
#define WIFI_SSID "your_wifi"
#define WIFI_PASSWORD "your_password"

// AWS IoT Core endpoint
#define AWS_IOT_ENDPOINT "your-endpoint"

// MQTT topics
#define AWS_PUBLISH_TOPIC "rain/sensor/data"
#define AWS_SUBSCRIBE_TOPIC "rain/sensor/cmd"
#define AWS_STATUS_TOPIC "rain/sensor/status"

// Amazon Root CA 1
static const char AWS_ROOT_CA[] PROGMEM = R"EOF(
-----BEGIN CERTIFICATE-----
-----END CERTIFICATE-----
)EOF";

// Device certificate
static const char AWS_CERT_CRT[] PROGMEM = R"KEY(
-----BEGIN CERTIFICATE-----
-----END CERTIFICATE-----
)KEY";

// Device private key
static const char AWS_PRIVATE_KEY[] PROGMEM = R"KEY(
-----BEGIN RSA PRIVATE KEY-----
-----END RSA PRIVATE KEY-----
)KEY";

#endif
