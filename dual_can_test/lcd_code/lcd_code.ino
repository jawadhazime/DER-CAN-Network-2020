#include "FlexCAN_T4.h"
#include <LiquidCrystal.h>
FlexCAN_T4 <CAN2, RX_SIZE_256, TX_SIZE_16> Can0; //dataCAN

// LCD Meta -------------------------------------------------
LiquidCrystal lcd(12, 11, 5, 4, 3, 2);
int wheelSpeed;

void DisplayTempSpeed(int wheelSpeed){
  lcd.setCursor(0,0);
  lcd.print("SPEED: ");
  lcd.setCursor(6, 0);
  lcd.print(wheelSpeed);
  delay(500);
  return;
}

// CAN Meta -------------------------------------------------
struct message {
  CAN_message_t msg;
} dis;

void canSniff(const CAN_message_t &msg) {
  Serial.print(" MB "); Serial.print(msg.mb);
  Serial.print(" OVERRUN: "); Serial.print(msg.flags.overrun);
  Serial.print(" LEN: "); Serial.print(msg.len);
  Serial.print(" EXT: "); Serial.print(msg.flags.extended);
  Serial.print(" TS: "); Serial.print(msg.timestamp);
  Serial.print(" ID: "); Serial.print(msg.id, HEX);
  Serial.print(" Buffer: ");
  for ( uint8_t i = 0; i < msg.len; i++ ) {
    Serial.print(msg.buf[i], HEX); Serial.print(" ");
  } Serial.println();
  
  dis.msg = msg;
  dis.msg.id = 0x999; // wheel speed id
}

void setup(void) {
  // Can Set-up -------------------------------------------------
  Serial.begin(115200); delay(400);
  pinMode(6, OUTPUT); digitalWrite(6, LOW); // enable transceiver
  Can0.begin();
  Can0.setClock(CLK_60MHz);
  Can0.setBaudRate(500000);
  Can0.setMaxMB(16); // up to 64 max for T4, not important in FIFO mode, unless you want to use additional mailboxes with FIFO
  Can0.enableFIFO();
  Can0.enableFIFOInterrupt();
  Can0.onReceive(canSniff);
  Can0.mailboxStatus();

  // LCD Set-up -------------------------------------------------
  lcd.begin(8, 2);// Switch on the LCD screen
  lcd.print("DashBoard");
  lcd.setCursor(0,1);
  lcd.print("d");
  delay(1500);
}

void loop() {
  // CAN Loop -------------------------------------------------
  Can0.events();
  static uint32_t timeout = millis();
  if ( millis() - timeout > 1000 ) { // send random frame every 1000ms
    lcd.clear();
    wheelSpeed = (int)dis.msg.buf[0];
    DisplayTempSpeed(wheelSpeed);
  }
}
