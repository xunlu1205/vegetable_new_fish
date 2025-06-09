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

對應接腳
馬達控制腳位（ENA, IN1, IN2, ENB, IN3, IN4）
感測器腳位（IR_L, IR_R, Trig, Echo）
LED 與蜂鳴器腳位（LED_L, LED_R, pinBuzzer）
狀態列舉 enum State {...} 與狀態變數 state
控制用的時間變數（如 lastToggleTime, turnStartTime）
音階陣列 doremi[]
曲譜索引 happybirthday[] 與節奏 meter[]
控制播放的變數 noteIndex, noteStartTime, playingNote

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

核心函式



四、安裝與上傳
1.下載或複製本專案至 Arduino IDE。
2.選擇對應開發板 (Arduino Uno) 與序列埠。
3.點擊「上傳」編譯並燒錄至主控板。


五、參數調整
| 參數項目         | 數值（單位）                 | 用途說明                              |
| ------------ | ---------------------- | --------------------------------- |
| 前進速度     | 200（PWM）             | `MOVE_FORWARD` 狀態下，兩側馬達以 200 速度前進 |
| 左轉速度     | 200（PWM）             | 左輪不動、右輪轉，轉速為 200                  |
| 右轉速度     | 200（PWM）             | 右輪不動、左輪轉，轉速為 200                  |
| 轉彎持續時間   | 1000（毫秒）          | 左轉或右轉持續 1 秒                          |
| 紅外線偵測條件 | IR_L / IR_R == LOW`   | 感測器讀取 LOW 代表偵測到黑線（道路線）       |
| 超音波觸發距離  | < 15（公分）          | 若障礙物距離小於 15 cm，進入停止與警示模式    |
| LED 閃爍間隔 | 500（毫秒）              | 當進入 STOP 狀態時，左右 LED 每 0.5 秒交替閃爍  |
| 音符節拍長度   | 60000 / meter[n]（毫秒） | 根據節奏陣列，決定生日快樂歌每個音符的時值     |
| 蜂鳴器延遲時間  | 10（毫秒）              | 每個音符之間的停頓延遲                     |
