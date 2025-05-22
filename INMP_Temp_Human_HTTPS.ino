#include <driver/i2s.h>
#include <DHT.h>
#include <WiFi.h>
#include <HTTPClient.h>

#define DHTPIN 14    
#define DHTTYPE DHT22
DHT dht(DHTPIN, DHTTYPE);

#define I2S_BCK 32
#define I2S_WS 25
#define I2S_DI 26

const char* ssid = "Bee";
const char* password = "12345678";
const char* serverUrl = "http://13.213.71.17:5001/upload";

const float TEMP_THRESHOLD = 2.0;   
const float HUMIDITY_THRESHOLD = 5.0; 
float lastTemp = 0;
float lastHumidity = 0;
unsigned long lastSendTime = 0;

const i2s_config_t i2s_config = {
    .mode = (i2s_mode_t)(I2S_MODE_MASTER | I2S_MODE_RX),
    .sample_rate = 16000,
    .bits_per_sample = I2S_BITS_PER_SAMPLE_32BIT,
    .channel_format = I2S_CHANNEL_FMT_ONLY_LEFT,
    .communication_format = I2S_COMM_FORMAT_STAND_I2S,
    .intr_alloc_flags = ESP_INTR_FLAG_LEVEL1,
    .dma_buf_count = 8,
    .dma_buf_len = 512
};

const i2s_pin_config_t pin_config = {
    .bck_io_num = I2S_BCK,
    .ws_io_num = I2S_WS,
    .data_out_num = -1,
    .data_in_num = I2S_DI
};

// Audio buffer: 2s * 16000Hz = 32000 samples
int16_t audio_buffer[32000];

void setup() {
    Serial.begin(921600);

    // Sensor
    dht.begin();

    // WiFi
    WiFi.begin(ssid, password);
    Serial.println("");
    Serial.print("WiFi connecting");
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }
    Serial.println("");
    Serial.println("WiFi connected");

    // I2S
    i2s_driver_install(I2S_NUM_0, &i2s_config, 0, NULL);
    i2s_set_pin(I2S_NUM_0, &pin_config);
}

void loop() {
    size_t bytes_read;
    int32_t raw_sample;

    // Record 2 seconds
    for (int i = 0; i < 32000; i++) {
        i2s_read(I2S_NUM_0, &raw_sample, sizeof(raw_sample), &bytes_read, portMAX_DELAY);
        raw_sample >>= 16;
        audio_buffer[i] = (int16_t)raw_sample;
    }

    float temp = dht.readTemperature();
    float hum = dht.readHumidity();
    Serial.print("Humidity: ");
    Serial.print(hum);
    Serial.print("%\t");
    Serial.print("Temperature: ");
    Serial.print(temp);
    Serial.println("°C");  

    if (WiFi.status() == WL_CONNECTED) {
        WiFiClient client;
        HTTPClient http;
        http.begin(client, serverUrl);
        http.addHeader("Content-Type", "application/octet-stream");
        http.addHeader("X-Temp", String(temp));
        http.addHeader("X-Hum", String(hum));

        http.POST((uint8_t*)audio_buffer, sizeof(audio_buffer));
        http.end();
    }

    delay(8000);
}
