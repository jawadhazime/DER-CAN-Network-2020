#include "FlexCAN_T4.h"
FlexCAN_T4 <CAN2, RX_SIZE_256, TX_SIZE_16> Can0; //dataCAN

struct message {
  CAN_message_t msg;
  int num;
} ws, lcd;

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
}

CAN_message_t canPass(const CAN_message_t &msg) {
  return msg;
}


void setup(void) {
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
}

void loop() {
  Can0.events();

  static uint32_t timeout = millis();
  if ( millis() - timeout > 100 ) { // send random frame every 100ms
    canSniff(ws.msg);

    lcd.msg = canPass(ws.msg);
    
    lcd.msg.id = 0x0999; // wheel speed id

    lcd.msg.buf[0] = ws.msg.buf[0];
    lcd.msg.buf[1] = ws.msg.buf[1];
    lcd.msg.buf[2] = ws.msg.buf[2];
    lcd.msg.buf[3] = ws.msg.buf[3];
    lcd.msg.buf[4] = ws.msg.buf[4];
    lcd.msg.buf[5] = ws.msg.buf[5];
    lcd.msg.buf[6] = ws.msg.buf[6];
    lcd.msg.buf[7] = ws.msg.buf[7];

    Can0.write(lcd.msg);
    timeout = millis();
    canSniff(lcd.msg);
  }
}
