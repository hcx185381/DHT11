# DHT11
markdown
# ESP32-C3 温湿度监测站 (DHT11 + OLED + MQTT)

本项目基于 ESP32-C3 Super Mini 开发板，使用 DHT11 传感器采集环境温湿度，通过 0.91 寸 OLED 屏幕本地显示，并利用 MQTT 协议将数据发布到公共 Broker，实现全球远程访问。同时提供美观的 Web 页面，通过浏览器实时查看数据。

## ✨ 功能特性

- **本地 OLED 显示**：实时显示温度和湿度，带美化界面（边框、温度计条形图、湿度条形图）。
- **远程 Web 监测**：通过手机/电脑浏览器访问 GitHub Pages 上的静态页面，即可实时查看数据（需 MQTT 支持）。
- **MQTT 云端发布**：将温湿度数据发布到公共 MQTT Broker（如 `broker-cn.emqx.io`），实现全球访问。
- **自动重连**：WiFi 或 MQTT 断开时自动重连。
- **低功耗设计**：ESP32-C3 功耗优化，适合长期运行。

## 🛠️ 硬件清单

| 组件                  | 数量 | 备注                         |
|-----------------------|------|------------------------------|
| ESP32-C3 Super Mini   | 1    | 主控                         |
| DHT11 温湿度传感器    | 1    | 可测量温度和湿度             |
| 0.91 寸 OLED 显示屏   | 1    | I2C 接口，128×32 分辨率      |
| 面包板 + 杜邦线       | 若干 | 用于连接                     |
| 10µF 电解电容（可选） | 1    | 用于稳定 OLED 电源（防花屏） |

## 🔌 硬件接线

### DHT11 传感器
| DHT11 引脚 | 连接到 ESP32-C3 |
|------------|-----------------|
| VCC        | 3V3             |
| GND        | GND             |
| DATA       | GPIO5           |

### OLED 显示屏
| OLED 引脚 | 连接到 ESP32-C3 |
|-----------|-----------------|
| VCC       | 3V3             |
| GND       | GND             |
| SCL (SCK) | GPIO7           |
| SDA       | GPIO6           |

**注意**：如果 OLED 花屏，可在 VCC 和 GND 之间并联一个 10µF 电解电容（正极接 VCC，负极接 GND）。

## 💻 软件准备

### 1. 安装 Arduino IDE
从 [arduino.cc](https://www.arduino.cc/en/software) 下载并安装 Arduino IDE（建议 1.8.19 或 2.x 版本）。

### 2. 配置 ESP32-C3 开发环境
- 打开 Arduino IDE，进入 **文件 → 首选项**，在“附加开发板管理器网址”中添加：
https://jihulab.com/esp-mirror/espressif/arduino-esp32/-/raw/gh-pages/package_esp32_index_cn.json

text
- 进入 **工具 → 开发板 → 开发板管理器**，搜索 `ESP32`，安装 **esp32 by Espressif Systems**（选择带 `cn` 后缀的版本，如 3.3.7-cn）。

### 3. 安装所需库
通过 **项目 → 加载库 → 管理库**，搜索并安装以下库：
- `Adafruit SSD1306` by Adafruit
- `Adafruit GFX` by Adafruit（SSD1306 的依赖）
- `DHT sensor library` by Adafruit
- `PubSubClient` by Nick O‘Leary（用于 MQTT）

### 4. 下载本仓库代码
```bash
git clone https://github.com/你的用户名/你的仓库名.git
🚀 烧录代码
用 USB 线将 ESP32-C3 连接到电脑。

打开 Arduino IDE，选择开发板：工具 → 开发板 → ESP32 Arduino → ESP32C3 Dev Module。

设置端口：工具 → 端口，选择正确的 COM 口。

重要设置：在 工具 菜单中，确保 USB CDC On Boot 设为 Enabled，Flash Mode 设为 DIO。

打开本仓库中的 .ino 文件（如 ESP32_DHT11_OLED_MQTT.ino）。

在代码开头修改 WiFi 信息和 MQTT Broker 地址（如果需要）：

cpp
const char* ssid = "你的WiFi名称";
const char* password = "你的WiFi密码";
const char* mqtt_server = "broker-cn.emqx.io";  // 国内节点，更稳定
点击 上传 按钮。等待编译并烧录完成。

📱 使用说明
本地 OLED 显示
烧录成功后，OLED 屏幕会显示带有边框的温湿度信息，右侧有温度计条形图和湿度条形图。

数据每 5 秒更新一次。

远程 Web 访问
将仓库中的 index.html 文件部署到 GitHub Pages 或其他静态托管服务。

打开浏览器访问该页面，页面会自动连接 MQTT Broker 并订阅温湿度主题。

如果一切正常，页面会显示“✅ 已连接到云端”，并实时更新温湿度数据。

MQTT 主题
温度主题：home/esp32c3/temperature

湿度主题：home/esp32c3/humidity
你可以用任何 MQTT 客户端（如 MQTTX）订阅这些主题查看数据。

📁 文件说明
ESP32_DHT11_OLED_MQTT.ino – Arduino 主程序，包含传感器读取、OLED 显示、MQTT 发布。

index.html – 独立的 Web 页面源码，通过 MQTT over WebSocket 实时显示数据。

README.md – 本文档。

❓ 常见问题
Q1: OLED 不亮或花屏
检查接线是否正确，特别是 SDA/SCL 是否接反。

确认代码中已添加 Wire.begin(6, 7); 和 Wire.setClock(100000);。

尝试在 VCC 和 GND 间并联 10µF 电解电容。

运行 I2C 扫描程序确认 OLED 地址（通常为 0x3C 或 0x3D）。

Q2: DHT11 读数始终为 0 或失败
检查 DATA 线是否接在 GPIO5。

确保传感器供电正常（3.3V 足够，但部分模块可能需要 5V）。

在代码中增加读取间隔（至少 1 秒，建议 2 秒）。

尝试外接 4.7kΩ 上拉电阻到 DATA 引脚。

Q3: 网页无法连接 MQTT
确认使用的 Broker 地址是否正确，国内推荐 broker-cn.emqx.io。

检查浏览器控制台（F12）是否有错误信息。

尝试更换 Broker（如 test.mosquitto.org）或 WebSocket 端口（如 8084）。

Q4: ESP32 无法连接 WiFi
检查 WiFi 名称和密码是否正确。

确认路由器是否开启 DHCP。

尝试靠近路由器，或重启 ESP32。

📄 许可证
本项目采用 MIT 许可证，详情见 LICENSE 文件。

欢迎 Star 和 Fork！ 如有问题，请提交 Issue。

text

---

## 三、恢复 OLED 显示的方法

如果你需要恢复 OLED 显示，请检查你的代码中是否包含以下关键部分：

```cpp
#include <Wire.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 32
#define OLED_ADDR 0x3C
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);
bool oled_ok = false;

void setup() {
  Wire.begin(6, 7);
  Wire.setClock(100000);
  if (!display.begin(SSD1306_SWITCHCAPVCC, OLED_ADDR)) {
    Serial.println("OLED 初始化失败");
    oled_ok = false;
  } else {
    oled_ok = true;
    display.clearDisplay();
    display.setTextSize(1);
    display.setTextColor(SSD1306_WHITE);
    display.display();
  }
}

void loop() {
  if (oled_ok) {
    // 更新显示的函数
    updateDisplay();
  }
}
