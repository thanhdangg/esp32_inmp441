# 📡 ESP32: Gửi dữ liệu âm thanh, nhiệt độ & độ ẩm lên server

Dự án này sử dụng ESP32, micro I2S INMP441, và cảm biến DHT22 để:
- Ghi âm 2 giây âm thanh (mỗi 10 giây)
- Đọc nhiệt độ & độ ẩm
- Gửi tất cả dữ liệu lên một server Node.js thông qua HTTP

---

## 🧰 Thiết bị cần chuẩn bị

| Thiết bị         | Mô tả                        |
|------------------|-----------------------------|
| ESP32 DevKit     | Bảng mạch chính             |
| Micro INMP441    | Giao tiếp qua I2S           |
| DHT22            | Cảm biến nhiệt độ & độ ẩm   |
| Dây nối          | Dây jumper nối mạch         |
| Máy tính         | Có cài sẵn Arduino IDE      |

---

## 🖥️ Bước 1: Cài đặt Arduino IDE & ESP32

1. Tải và cài đặt Arduino IDE.
2. Mở Arduino IDE → File → Preferences.
3. Trong **Additional Board URLs**, thêm:
```
    http://arduino.esp8266.com/stable/package_esp8266com_index.json
    https://raw.githubusercontent.com/espressif/arduino-esp32/gh-pages/package_esp32_index.json
```

4. Vào Tools → Board → Boards Manager.
5. Tìm và cài **esp32 by Espressif Systems**.
6. Vào Tools → Board và chọn: **ESP32 Dev Module**.

---

## 🧾 Bước 2: Cài đặt Drive (ESP32/ESP8266 USB Drivers – CP210x USB to UART Bridge)
- Tải file drive tại đây: https://www.silabs.com/developer-tools/usb-to-uart-bridge-vcp-drivers?tab=downloads
- Giải nén và cài đặt theo các bước xuất hiện trên màn hình

------

## 🧾 Bước 3: Cài đặt thư viện

Vào Tools → Manage Libraries, cài các thư viện:
- "DHT sensor library" (Adafruit)

- "WiFi" (ESP32)

- "HTTPClient" (ESP32)

---

## ⚙️ Bước 4: Kết nối phần cứng

### 🧭 Kết nối I2S Microphone (INMP441)

| INMP441 Pin | ESP32 Pin |
|-------------|-----------|
| VCC         | 3.3V      |
| GND         | GND       |
| WS          | GPIO25    |
| SCK         | GPIO32    |
| SD          | GPIO26    |

### 🌡️ Kết nối DHT22

| DHT22 Pin | ESP32 Pin |
|-----------|-----------|
| VCC       | 3.3V      |
| GND       | GND       |
| DATA      | GPIO14    |

---

## 📥 Bước 5: Tải mã nguồn

1. Mở file .ino trong Arduino IDE
2. Chỉnh sửa các thông số:
```
const char* ssid = "Bee";            // Tên WiFi
const char* password = "12345678";  // Mật khẩu WiFi
```

## 🔌 Bước 5: Biên dịch và nạp chương trình vào ESP32
1.  Kết nối ESP32 với máy tính qua cáp USB
2. Chọn board: Tools > Board > ESP32 Arduino > ESP32 Dev Module board
3. Chọn cổng COM: Tools > Port (chọn cổng ESP32)
4. Nạp code: Nhấn nút Upload (mũi tên sang phải)
5. Giữ nút **BOOT** trên ESP32 trong khi code đang nạp code (Output của IDE sẽ hiện Connecting.....) cho đến khi xuất hiện dòng 
```
Writing at 0x00001000... (100 %)  
```
thì thả ra. Sau khi code nạp xong sẽ xuất hiện dòng: 
```
Wrote 1050624 bytes (679096 compressed) at 0x00010000 in 15.4 seconds (effective 547.5 kbit/s)...
Hash of data verified.

Leaving...
Hard resetting via RTS pin...
``` 
thì tiếp tục bấm nút **RST** (Reset) để bắt đầu chạy chương trình trên ESP32.

# ✨ Giải thích hoạt động
- Kết nối WiFi khi khởi động

- Mỗi 8 giây thực hiện:

    1. Thu âm 2 giây (16000 samples/giây)

    2. Đọc giá trị DHT22

    3. Gửi dữ liệu âm thanh và cảm biến qua HTTP POST

# 📤 Dữ liệu gửi lên server
ESP32 sẽ gửi mỗi 10 giây:

- File âm thanh .wav dài 2 giây (dưới dạng nhị phân)
- Header HTTP:
    - X-Temp: nhiệt độ
    - X-Hum: độ ẩm

# 🧪 Kiểm tra kết quả
- Mở Serial Monitor trong Arduino IDE
- Chọn baud rate: 921600
- Kiểm tra log kết nối WiFi và các lần gửi dữ liệu