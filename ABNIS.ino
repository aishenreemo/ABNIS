#define ON_PIN 2
#define SOUND_PIN A0
#define TRIGGER_PIN 13

#define SENS_BTN_CD 500
#define SENS_BTN_PIN 3

#define ON_BTN_CD 500
#define ON_BTN_PIN 4

#define LEVEL_COUNT 3

#define BLINK_DELAY 50

const int levels[LEVEL_COUNT] = {
    700,
    540,
    530,
};

struct button_t {
    unsigned long last;
    unsigned long cooldown;
    int pin;
};

struct button_t sensitivity;
struct button_t on;

int turned_on;
int level_cur;
int level_ptr;
int sound_level;

void setup() {
    Serial.begin(9600);
    turned_on = LOW;

    level_ptr = 0;
    level_cur = levels[level_ptr];

    sensitivity.last = SENS_BTN_CD;
    sensitivity.cooldown = SENS_BTN_CD;
    sensitivity.pin = SENS_BTN_PIN;

    on.last = ON_BTN_CD;
    on.cooldown = ON_BTN_CD;
    on.pin = ON_BTN_PIN;

    pinMode(sensitivity.pin, INPUT_PULLUP);
    pinMode(on.pin, INPUT_PULLUP);

    pinMode(SOUND_PIN, INPUT);
    pinMode(TRIGGER_PIN, OUTPUT);
    pinMode(ON_PIN, OUTPUT);
}

void loop() {
    // listen to ON button
    if (digitalRead(on.pin) == LOW && millis() - on.last >= on.cooldown) {
        digitalWrite(ON_PIN, !turned_on);
        turned_on = !turned_on;
        on.last = millis();
    }

    // listen to SENSITIVITY button
    if (turned_on && digitalRead(sensitivity.pin) == LOW && millis() - sensitivity.last >= sensitivity.cooldown) {
        level_ptr += 1;
        level_ptr %= LEVEL_COUNT;
        level_cur = levels[level_ptr];

        for (int i = 0; i < level_ptr + 1; i++) {
            digitalWrite(TRIGGER_PIN, HIGH);
            delay(BLINK_DELAY);
            digitalWrite(TRIGGER_PIN, LOW);
            delay(BLINK_DELAY);
        }

        sensitivity.last = millis();
    }

    if (!turned_on) return;

    sound_level = analogRead(SOUND_PIN);

    if (sound_level >= level_cur) {
        digitalWrite(TRIGGER_PIN, HIGH);
        delay(BLINK_DELAY * 5);
        digitalWrite(TRIGGER_PIN, LOW);
    }
}
