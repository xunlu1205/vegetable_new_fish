# vegetable_new_fish
本專案結合紅外線循跡與超音波避障，讓 Arduino 控制的自走車能在黑線路徑上行駛並自動避開前方障礙物。

一、專案特色

循跡 (Line Following)：雙紅外線感測器偵測黑線，依偏差自動前進、左轉、右轉。
避障 (Obstacle Avoidance)：超音波模組測量前方距離，低於設定閾值時自動後退並轉向。
後退 (Reversing)：遇障退避後，再進行方向修正後返回循跡。
LED 狀態指示：前進、轉彎與停止時的指示燈提示。
最大亮點是可以唱生日快樂歌

二、硬體需求
Arduino Uno (或相容開發板)
L298N 雙 H 橋馬達驅動板
2x 紅外線循跡感測器 (IR modules)
1x HC-SR04 超音波感測器
2x 直流馬達 + 車輪套件
2x LED 指示燈
1x 蜂鳴器
連接線、電源模組

三、軟體結構
setup() 初始化
    ↓
loop()
  ├─ 超音波感測判斷障礙物 → STOP_MOTORS
  ├─ 紅外線判斷左右轉、前進
  └─ 狀態機控制馬達與 LED
         ├─ TURN_LEFT / TURN_RIGHT
         ├─ MOVE_FORWARD
         └─ STOP_MOTORS → 播放音樂


四、安裝與上傳
1.下載或複製本專案至 Arduino IDE。
2.選擇對應開發板 (Arduino Uno) 與序列埠。
3.點擊「上傳」編譯並燒錄至主控板。

五、參數調整
| 分類     | 參數                                     | 說明         |
|-----    | ---------------------------------------- | ----------- |
| 馬達控制 | `ENA~IN4`                                | 控制左右輪動作     |
| 感測器   | `IR_L`, `IR_R`, `Trig`, `Echo`           | 紅外線循跡與避障    |
| 輸出裝置 | `LED_L`, `LED_R`, `pinBuzzer`            | 顯示與聲音       |
| 動作狀態 | `State`、`state`                         | 控制行為流程      |
| 時間參數 | `turnDuration`, `lastToggleTime`         | 控制動作持續與閃爍節奏 |
| 音樂參數 | `doremi[]`, `happybirthday[]`, `meter[]` | 音階與節奏控制     |
