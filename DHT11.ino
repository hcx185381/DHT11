#include <WiFi.h>
#include <PubSubClient.h>  // 添加MQTT库
#include <Wire.h>
#include <Adafruit_SSD1306.h>
#include <DHT.h>

// ===== WiFi 配置 =====
const char* ssid = "TP-LINK_C0AE";
const char* password = "12345678";

// ===== MQTT 配置（使用国内节点，与网页端一致）=====
const char* mqtt_server = "broker-cn.emqx.io";  // 国内专用节点
const int mqtt_port = 1883;                      // MQTT over TCP端口
const char* temp_topic = "home/esp32c3/temperature";
const char* hum_topic = "home/esp32c3/humidity";

// ===== 引脚定义 =====
#define DHTPIN 5
#define DHTTYPE DHT11

// ===== OLED 定义 =====
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 32
#define OLED_ADDR 0x3C
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);
bool oled_ok = false;

DHT dht(DHTPIN, DHTTYPE);
WiFiClient espClient;
PubSubClient client(espClient);  // MQTT客户端

float temperature = 0;
float humidity = 0;
unsigned long lastRead = 0;
const long interval = 5000;  // 5秒读取一次（减轻MQTT服务器压力）

void updateDisplay();  // 声明显示函数

void setup() {
  Serial.begin(115200);
  delay(1000);

  // ---------- 初始化 I2C ----------
  Wire.begin(6, 7);
  Wire.setClock(100000);

  // ---------- 初始化 OLED ----------
  if (!display.begin(SSD1306_SWITCHCAPVCC, OLED_ADDR)) {
    Serial.println(F("OLED 初始化失败！"));
    oled_ok = false;
  } else {
    oled_ok = true;
    display.clearDisplay();
    delay(1);
    display.clearDisplay();
    display.setTextSize(1);
    display.setTextColor(SSD1306_WHITE);
    display.display();
    delay(1000);
  }

  dht.begin();

  // ---------- 连接 WiFi ----------
  WiFi.begin(ssid, password);
  Serial.print("连接 WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\n连接成功！IP地址: ");
  Serial.println(WiFi.localIP());

  // ---------- 配置 MQTT 服务器 ----------
  client.setServer(mqtt_server, mqtt_port);
}

void loop() {
  // 维持MQTT连接
  if (!client.connected()) {
    reconnectMQTT();
  }
  client.loop();

  // 定时读取并发布数据
  if (millis() - lastRead >= interval) {
    readSensorAndPublish();
    lastRead = millis();
  }
}

// 重新连接MQTT
void reconnectMQTT() {
  while (!client.connected()) {
    Serial.print("尝试连接MQTT服务器...");
    // 创建唯一的客户端ID
    String clientId = "ESP32C3_" + String(random(0xffff), HEX);
    if (client.connect(clientId.c_str())) {
      Serial.println("已连接");
    } else {
      Serial.print("失败，错误代码=");
      Serial.print(client.state());
      Serial.println(" 5秒后重试");
      delay(5000);
    }
  }
}

void readSensorAndPublish() {
  float h = dht.readHumidity();
  float t = dht.readTemperature();

  if (isnan(h) || isnan(t)) {
    Serial.println("DHT11 读取失败");
    if (oled_ok) {
      display.clearDisplay();
      delay(1);
      display.clearDisplay();
      display.setCursor(0, 0);
      display.println("Sensor Error");
      display.display();
    }
    return;
  }

  temperature = t;
  humidity = h;

  // 更新OLED显示
  if (oled_ok) {
    updateDisplay();
  }

  // 发布到MQTT
  char tempStr[8], humStr[8];
  dtostrf(temperature, 1, 1, tempStr);
  dtostrf(humidity, 1, 1, humStr);
  client.publish(temp_topic, tempStr);
  client.publish(hum_topic, humStr);

  Serial.printf("已发布 - 温度: %s°C, 湿度: %s%%\n", tempStr, humStr);
}

// 美化 OLED 显示函数（与之前相同）
void updateDisplay() {
  display.clearDisplay();
  delay(1);
  display.clearDisplay();

  display.drawRect(0, 0, 128, 32, SSD1306_WHITE);

  display.setCursor(4, 4);
  display.print("T:");
  display.setCursor(20, 4);
  display.print(temperature, 1);
  display.print(" C");

  int tempBarHeight = map(constrain(temperature, 0, 40), 0, 40, 0, 16);
  display.fillRect(80, 16 - tempBarHeight, 6, tempBarHeight, SSD1306_WHITE);
  display.drawRect(80, 0, 6, 16, SSD1306_WHITE);

  display.setCursor(4, 18);
  display.print("H:");
  display.setCursor(20, 18);
  display.print(humidity, 1);
  display.print(" %");

  int humBarWidth = map(constrain(humidity, 0, 100), 0, 100, 0, 20);
  display.fillRect(80, 24, humBarWidth, 6, SSD1306_WHITE);
  display.drawRect(80, 24, 20, 6, SSD1306_WHITE);

  display.display();
}
