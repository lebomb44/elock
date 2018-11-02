#include <Fifo_U16.h>
#include <HT12E.h>

#define WATCHDOG_DELAY 10000

#define LED_pin 13
#define RELAY_POWER1_pin 7
#define RELAY_POWER2_pin 8
#define RELAY_OPEN_CLOSE_pin 10
#define RF_IN_pin 2

HT12E ht12e;

uint32_t watchDog = 0;

void setup()
{
  ht12e.init();
  watchDog = 0;
  // initialize serial communications and wait for port to open:
  Serial.begin(115200);
  pinMode(LED_pin, OUTPUT);
  digitalWrite(LED_pin, LOW);
  pinMode(RELAY_POWER1_pin, OUTPUT);
  digitalWrite(RELAY_POWER1_pin, LOW);
  pinMode(RELAY_POWER2_pin, OUTPUT);
  digitalWrite(RELAY_POWER2_pin, LOW);
  pinMode(RELAY_OPEN_CLOSE_pin, OUTPUT);
  digitalWrite(RELAY_OPEN_CLOSE_pin, LOW);
  pinMode(RF_IN_pin, INPUT);
  Serial.println("Starting electrical jack !");
  Serial.println("Ready for commands");
}

void loop()
{
  ht12e.run();
  if(true == ht12e.rxCodeIsReady()) {
    digitalWrite(LED_pin, HIGH);
    Serial.print(ht12e.rxGetCode(), HEX);Serial.print(" : ");
    Serial.print(ht12e.rxGetAddress(), HEX);Serial.print("-");
    Serial.println(ht12e.rxGetData(), HEX);
    delay(200);
    ht12e.purge();
    if(0x5956 == ht12e.rxGetAddress()) {
      if(0x59 == ht12e.rxGetData()) {
        digitalWrite(RELAY_OPEN_CLOSE_pin, HIGH);
        delay(500);
        digitalWrite(RELAY_POWER1_pin, HIGH);
        digitalWrite(RELAY_POWER2_pin, HIGH);
        Serial.println("Closing...");
        watchDog = WATCHDOG_DELAY;
      }
      if(0x69 == ht12e.rxGetData()) {
        digitalWrite(RELAY_OPEN_CLOSE_pin, LOW);
        delay(500);
        digitalWrite(RELAY_POWER1_pin, HIGH);
        digitalWrite(RELAY_POWER2_pin, HIGH);
        Serial.println("Opening...");
        watchDog = WATCHDOG_DELAY;
      }
    }
    ht12e.rxRelease();
  }
  digitalWrite(LED_pin, LOW);
  if(0 < watchDog) { watchDog--; }
  if(1 == watchDog) {
    Serial.println("...command finished");
  }
  if(0 == watchDog) {
    digitalWrite(RELAY_OPEN_CLOSE_pin, LOW);
    digitalWrite(RELAY_POWER1_pin, LOW);
    digitalWrite(RELAY_POWER2_pin, LOW);
  }
  delay(1);
}

