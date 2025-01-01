#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// OLED 顯示器的 RESET 腳位
#define OLED_RESET 4
// 建立 OLED 顯示器和 LED 條的物件
Adafruit_SSD1306 display(128, 64, &Wire, OLED_RESET);

// 定義按鍵引腳
#define BTN_UP    A0
#define BTN_DOWN  A1
#define BTN_ENTER A2
#define BTN_EXIT  A3

// 存儲當前按鍵值
unsigned char keyValue = 0;

// 定義結構來管理菜單邏輯
typedef struct
{
  unsigned char index;  // 當前菜單索引
  unsigned char up;     // 按上鍵後的菜單索引
  unsigned char down;   // 按下鍵後的菜單索引
  unsigned char enter;  // 按確認鍵後的菜單索引
  unsigned char exit;   // 按退出鍵後的菜單索引
  void (*operation)(void); // 該菜單的操作函數
} KEY_TABLE;

// 當前菜單索引和函數指標
unsigned char funIndex = 0;
void (*current)(void);

// 菜單函數宣告
void menu11(void);
void menu12(void);
void menu21(void);
void menu22(void);

// 定義菜單邏輯表
KEY_TABLE table[4] =
{
// up down enter exit operation
  {0, 0, 2, 1, 0, menu11},  // 菜單 0
  {1, 0, 1, 3, 0, menu12},  // 菜單 1
  {2, 2, 2, 0, 0, menu21},  // 菜單 2
  {3, 2, 2, 0, 0, menu22},  // 菜單 3
};

// 菜單 11：主菜單顯示內容
void menu11(void)
{
  display.clearDisplay();
  display.setTextSize(2);
  display.setCursor(15, 2);
  display.println(F("-> 1"));
  display.setCursor(2, 25);
  display.println(F("   2"));
  display.display();
}

// 菜單 12：EEPROM 菜單顯示內容
void menu12(void)
{
  display.clearDisplay();
  display.setTextSize(2);
  display.setCursor(15, 2);
  display.println(F("   1"));
  display.setCursor(2, 25);
  display.println(F("-> 2"));
  display.display();
}

// 菜單 21：燈光選單 - 自動模式
void menu21(void)
{
  display.clearDisplay();
  display.setTextSize(2);
  display.setCursor(15, 2);
  display.println(F("   3"));
  display.display();
}

// 菜單 22：燈光選單 - 正常模式
void menu22(void)
{
  display.clearDisplay();
  display.setTextSize(2);
  display.setCursor(15, 2);
  display.println(F("   4"));
  display.display();
}

// 按鍵掃描函數：檢測按鍵並返回按鍵值
unsigned char keyScan(void)
{
  static unsigned char keyUp = 1; // 記錄按鍵是否處於彈起狀態

  if (keyUp) // 如果按鍵處於彈起狀態，檢測按鍵是否被按下
  {
    if (digitalRead(BTN_UP) == LOW)
    {
      keyUp = 0;
      Serial.println("Button UP pressed");
      return 4;  // 上鍵
    }
    else if (digitalRead(BTN_DOWN) == LOW)
    {
      keyUp = 0;
      Serial.println("Button DOWN pressed");
      return 3;  // 下鍵
    }
    else if (digitalRead(BTN_ENTER) == LOW)
    {
      keyUp = 0;
      Serial.println("Button ENTER pressed");
      return 1;  // 確認鍵
    }
    else if (digitalRead(BTN_EXIT) == LOW)
    {
      keyUp = 0;
      Serial.println("Button EXIT pressed");
      return 2;  // 退出鍵
    }
  }

  // 如果所有按鍵都處於彈起狀態，恢復按鍵檢測功能
  if (digitalRead(BTN_UP) == HIGH && 
      digitalRead(BTN_DOWN) == HIGH && 
      digitalRead(BTN_ENTER) == HIGH && 
      digitalRead(BTN_EXIT) == HIGH)
  {
    keyUp = 1;  // 可以檢測新的按鍵
  }

  return 0;  // 沒有按鍵被按下
}

void setup()
{
  // 初始化按鍵引腳
  pinMode(BTN_UP, INPUT_PULLUP);
  pinMode(BTN_DOWN, INPUT_PULLUP);
  pinMode(BTN_ENTER, INPUT_PULLUP);
  pinMode(BTN_EXIT, INPUT_PULLUP);

  // 初始化串口和 OLED 顯示器
  Serial.begin(9600);
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println(F("SSD1306 allocation failed"));
    for (;;);
  }


  // 初始化菜單
  menu11();
}

void loop()
{
  // 獲取按鍵值
  keyValue = keyScan();

  if (keyValue != 0) // 如果有按鍵被按下
  {
    // 根據按鍵值更新菜單索引
    switch (keyValue)
    {
      case 1: funIndex = table[funIndex].enter; break;  // 確認鍵
      case 2: funIndex = table[funIndex].exit;  break;  // 退出鍵
      case 3: funIndex = table[funIndex].down;  break;  // 下鍵
      case 4: funIndex = table[funIndex].up;    break;  // 上鍵
    }

    // 更新當前菜單並執行對應的操作
    current = table[funIndex].operation;
    (*current)();
  }
}