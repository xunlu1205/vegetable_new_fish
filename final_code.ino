// --- 馬達控制腳位設定區塊 ---
const int ENA = 3, IN1 = 2, IN2 = 4; // 左輪控制
const int ENB = 5, IN3 = 6, IN4 = 7; // 右輪控制

// --- 紅外線循跡感測腳位設定區塊 ---
const int IR_L = 8, IR_R = 9; // 左右紅外線循跡感測器（LOW表示偵測到白線）

// --- LED 腳位設定區塊 ---
const int LED_L = 13;
const int LED_R = 12;

// --- 超音波避障感測器腳位設定區塊 ---
const int Trig = 10, Echo = 11;

// --- 蜂鳴器腳位（DAC輸出） ---
const int pinBuzzer = A0; // 類比腳位 A0

// --- 狀態列舉定義 ---
enum State { IDLE, TURN_RIGHT, TURN_LEFT, MOVE_FORWARD, STOP_MOTORS };
State state = IDLE;

// --- 控制變數 ---
unsigned long lastToggleTime = 0;
bool motorsOn = false;
unsigned long turnStartTime = 0;
const unsigned long turnDuration = 800;

bool ledToggle = false;
bool turnLedToggle = false;

// 生日快樂歌設定（使用頻率）
int doremi[] = {262, 294, 330, 349, 392, 440, 494, 523, 587, 659, 698, 784, 880, 988};

int happybirthday[] = {
  5,5,6,5,8,7,
  5,5,6,5,9,8,
  5,5,12,10,1,7,6,
  11,11,10,8,9,8
};

int meter[] = {
  1,1,2,2,2,4,
  1,1,2,2,2,4,
  1,1,2,2,2,2,2,
  1,1,2,2,2,4
};

int melodyLength = sizeof(happybirthday) / sizeof(int);
int noteIndex = 0;
unsigned long noteStartTime = 0;
bool playingNote = false;

void setup() {
  pinMode(ENA, OUTPUT); pinMode(IN1, OUTPUT); pinMode(IN2, OUTPUT);
  pinMode(ENB, OUTPUT); pinMode(IN3, OUTPUT); pinMode(IN4, OUTPUT);
  pinMode(IR_L, INPUT); pinMode(IR_R, INPUT);
  pinMode(LED_L, OUTPUT); pinMode(LED_R, OUTPUT);
  pinMode(Trig, OUTPUT); pinMode(Echo, INPUT);
  pinMode(pinBuzzer, OUTPUT);

  Serial.begin(9600);
  noteStartTime = millis();
}

void loop() {
  unsigned long currentMillis = millis();

  // 超音波避障偵測
  digitalWrite(Trig, LOW);
  delayMicroseconds(2);
  digitalWrite(Trig, HIGH);
  delayMicroseconds(10);
  digitalWrite(Trig, LOW);

  long dur = pulseIn(Echo, HIGH, 30000);
  float dist = 0;
  if (dur > 0) {
    dist = dur / 58.0;
    Serial.print("距離: "); Serial.print(dist); Serial.println(" cm");
  }

  if (dist > 0 && dist < 15 && state != STOP_MOTORS) {
    Serial.println("前方有障礙物，停止！");
    stopMotors();
    digitalWrite(LED_L, HIGH);
    digitalWrite(LED_R, HIGH);
    delay(1000);
    digitalWrite(LED_L, LOW);
    digitalWrite(LED_R, LOW);
    state = STOP_MOTORS;
    motorsOn = false;
  }

  // 紅外線循跡感測器讀取
  int l = digitalRead(IR_L);
  int r = digitalRead(IR_R);

  switch (state) {
    case IDLE:
      Serial.print("IR_L = "); Serial.print(l);
      Serial.print(", IR_R = "); Serial.println(r);

      if (l == LOW && r == HIGH) {
        Serial.println("開始右轉");
        turnStartTime = currentMillis;
        state = TURN_RIGHT;
        motorsOn = false;
        turnLedToggle = false;
        digitalWrite(LED_L, LOW);
        digitalWrite(LED_R, LOW);

      } else if (l == HIGH && r == LOW) {
        Serial.println("開始左轉");
        turnStartTime = currentMillis;
        state = TURN_LEFT;
        motorsOn = false;
        turnLedToggle = false;
        digitalWrite(LED_L, LOW);
        digitalWrite(LED_R, LOW);

      } else if (l == LOW && r == LOW) {
        Serial.println("前進");
        state = MOVE_FORWARD;
        motorsOn = false;
        digitalWrite(LED_L, LOW);
        digitalWrite(LED_R, LOW);

      } else if (l == HIGH && r == HIGH) {
        Serial.println("停止");
        stopMotors();
        motorsOn = false;
        state = STOP_MOTORS;
        digitalWrite(LED_L, LOW);
        digitalWrite(LED_R, LOW);
      }
      break;

    case TURN_RIGHT:
      if (currentMillis - turnStartTime >= turnDuration) {
        Serial.println("右轉完成，回到IDLE");
        stopMotors();
        motorsOn = false;
        digitalWrite(LED_L, LOW);
        digitalWrite(LED_R, LOW);
        state = IDLE;
      } else {
        if (currentMillis - lastToggleTime >= 200) {
          lastToggleTime = currentMillis;
          motorsOn = !motorsOn;
          turnLedToggle = !turnLedToggle;

          if (motorsOn) {
            startRightTurn(); Serial.println("右轉馬達開");
          } else {
            stopMotors(); Serial.println("右轉馬達停");
          }

          digitalWrite(LED_L, turnLedToggle ? HIGH : LOW);
          digitalWrite(LED_R, LOW);
        }
      }
      break;

    case TURN_LEFT:
      if (currentMillis - turnStartTime >= turnDuration) {
        Serial.println("左轉完成，回到IDLE");
        stopMotors();
        motorsOn = false;
        digitalWrite(LED_L, LOW);
        digitalWrite(LED_R, LOW);
        state = IDLE;
      } else {
        if (currentMillis - lastToggleTime >= 200) {
          lastToggleTime = currentMillis;
          motorsOn = !motorsOn;
          turnLedToggle = !turnLedToggle;

          if (motorsOn) {
            startLeftTurn(); Serial.println("左轉馬達開");
          } else {
            stopMotors(); Serial.println("左轉馬達停");
          }

          digitalWrite(LED_R, turnLedToggle ? HIGH : LOW);
          digitalWrite(LED_L, LOW);
        }
      }
      break;

    case MOVE_FORWARD:
      if (!(l == LOW && r == LOW)) {
        Serial.println("前進感測器條件不符，停止並回IDLE");
        stopMotors();
        digitalWrite(LED_L, LOW);
        digitalWrite(LED_R, LOW);
        motorsOn = false;
        state = IDLE;
      } else {
        if (currentMillis - lastToggleTime >= 100) {
          lastToggleTime = currentMillis;
          motorsOn = !motorsOn;
          ledToggle = !ledToggle;

          if (motorsOn) {
            moveForward(); Serial.println("前進馬達開");
          } else {
            stopMotors(); Serial.println("前進馬達停");
          }

          if (ledToggle) {
            digitalWrite(LED_L, HIGH);
            digitalWrite(LED_R, LOW);
          } else {
            digitalWrite(LED_L, LOW);
            digitalWrite(LED_R, HIGH);
          }
        }
      }
      break;

    case STOP_MOTORS:
      stopMotors();
      digitalWrite(LED_L, LOW);
      digitalWrite(LED_R, LOW);
      motorsOn = false;
      state = IDLE;
      break;
  }

  // --- 非阻塞生日快樂歌播放（偵測兩個感測器同時HIGH就停止唱並重置） ---
  if (l == HIGH && r == HIGH) {
    // 兩個HIGH時停止並重置
    noTone(pinBuzzer);
    noteIndex = 0;
    playingNote = false;
    noteStartTime = currentMillis;
  } else {
    // 其他情況持續播放（轉彎時不重置）
    playHappyBirthdayNonBlocking(currentMillis);
  }
}

// 馬達控制函式
void startRightTurn() {
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, HIGH);
  analogWrite(ENA, 200);

  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);
  analogWrite(ENB, 200);
}

void startLeftTurn() {
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);
  analogWrite(ENA, 200);

  digitalWrite(IN3, LOW);
  digitalWrite(IN4, HIGH);
  analogWrite(ENB, 200);
}

void moveForward() {
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);
  analogWrite(ENA, 200);

  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);
  analogWrite(ENB, 200);
}

void stopMotors() {
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, LOW);
  analogWrite(ENA, 0);

  digitalWrite(IN3, LOW);
  digitalWrite(IN4, LOW);
  analogWrite(ENB, 0);
}

// 非阻塞播放生日快樂歌函式
void playHappyBirthdayNonBlocking(unsigned long currentMillis) {
  if (!playingNote) {
    // 開始新音符
    int freq = doremi[happybirthday[noteIndex] - 1];
    int duration = meter[noteIndex] * 150;

    playToneStart(freq);

    noteStartTime = currentMillis;
    playingNote = true;
  }

  // 檢查是否該停止目前音符
  int duration = meter[noteIndex] * 150;
  if (currentMillis - noteStartTime >= duration) {
    playToneStop();

    noteIndex++;
    if (noteIndex >= melodyLength) {
      noteIndex = 0;
    }
    playingNote = false;
  }
}

void playToneStart(int frequency) {
  tone(pinBuzzer, frequency);
}

void playToneStop() {
  noTone(pinBuzzer);
}