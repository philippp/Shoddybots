#include "IRReader.h"

IRReader ir_reader;

uint32_t loop_count = 0;

void setup(void) {
  Serial.begin(9600);
}

void loop(void) {
  loop_count++;
  if (loop_count % 100 == 0) {
    Serial.print("Control Loop #");
    Serial.println(loop_count, DEC);
  }
  RemoteButton button;
  if (ir_reader.GetButton(&button)) {
    Serial.print("Button #");
    Serial.print(button, DEC);
    Serial.println(" pressed.");
  }
}
