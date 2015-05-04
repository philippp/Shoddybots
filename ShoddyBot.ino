#include "IRReader.h"
#include "UltraRanger.h"

IRReader ir_reader;
UltraRanger ultra_ranger;

uint32_t loop_count = 0;

void setup(void) {
  Serial.begin(9600);
  pinMode(10, INPUT);
}

void loop(void) {
  loop_count++;
  if (loop_count % 1600 == 0) {
    Serial.print("Control Loop #");
    Serial.println(loop_count, DEC);
  }
  RemoteButton button;
  if (ir_reader.GetButton(&button)) {
    Serial.print("Button #");
    Serial.print(button, DEC);
    Serial.println(" pressed.");
  }
  if (loop_count % 4 == 0) {
    Serial.println(ultra_ranger.GetRangeInInches());
  }
}
