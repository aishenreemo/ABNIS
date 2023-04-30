#define ON_PIN 2
#define YELLOW_PIN 3
#define RED_PIN 4
#define SOUND_PIN A0

#define ON_BTN_CD 500
#define ON_BTN_PIN 8

#define BLINK_DELAY 50
#define YELLOW_THRESHOLD 55
#define RED_THRESHOLD 60

struct button_t {
    unsigned long last;
    unsigned long cooldown;
    int pin;
};

struct button_t sensitivity;
struct button_t on;

int turned_on;
int sound_level;

int sound_read();

void setup() {
    Serial.begin(9600);
    turned_on = LOW;

    on.last = ON_BTN_CD;
    on.cooldown = ON_BTN_CD;
    on.pin = ON_BTN_PIN;

    pinMode(on.pin, INPUT_PULLUP);

    pinMode(SOUND_PIN, INPUT);
    pinMode(RED_PIN, OUTPUT);
    pinMode(YELLOW_PIN, OUTPUT);
    pinMode(ON_PIN, OUTPUT);
}

void loop() {
    // listen to ON button
    if (digitalRead(on.pin) == LOW && millis() - on.last >= on.cooldown) {
        digitalWrite(ON_PIN, !turned_on);
        turned_on = !turned_on;
        on.last = millis();
    }

    if (!turned_on) return;

    sound_level = sound_read();

    Serial.print("Sound level: ");
    Serial.print(sound_level);
    Serial.println(" dB");

    if (sound_level >= RED_THRESHOLD) {
        digitalWrite(RED_PIN, HIGH);
        delay(BLINK_DELAY * 5);
        digitalWrite(RED_PIN, LOW);
    } else if (sound_level >= YELLOW_THRESHOLD) {
        digitalWrite(YELLOW_PIN, HIGH);
        delay(BLINK_DELAY * 5);
        digitalWrite(YELLOW_PIN, LOW);
    }
}

int sound_read() {
    float range = 0;
    int sample_window = 50;

    unsigned int sample;
    unsigned int signal_max = 0;
    unsigned int signal_min = 1024;

    unsigned long start = millis();
    while (millis() - start < sample_window) {
        sample = analogRead(SOUND_PIN);

        if (sample >= 1024) continue;

        if (sample > signal_max) {
            signal_max = sample;
        } else if (sample < signal_min) {
            signal_min = sample;
        }
    }

    range = signal_max - signal_min;
    return map(range, 20, 200, 49.5, 90);
}
