#include <LiquidCrystal.h>
#include <FHT.h>

// Pins for LCD
const int rs = 12, en = 11, d4 = 5, d5 = 4, d6 = 3, d7 = 2;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

// Pins for microphone
const int microphonePin = A0;

// Constants for FHT
#define LOG_OUT 1 // use the log output function
#define FHT_N 256 // set to 256 point FHT
#define FHT_SAMPLE_RATE 10000 // set to 10kHz sample rate

// Global variables
volatile int fht_log_out[FHT_N]; // FHT log output buffer
int fht_peak_index = -1; // Index of peak in FHT spectrum
float fht_peak_frequency = 0; // Frequency of the detected peak

void setup() {
  lcd.begin(16, 2);
  lcd.setCursor(0, 0);
  lcd.print("Current Note:");
  pinMode(microphonePin, INPUT);
  Serial.begin(9600);

  fht_window();
}

void loop() {
  fht_run();

  fht_peak_index = fht_find_peak();

  fht_peak_frequency = fht_peak_index * (FHT_SAMPLE_RATE / FHT_N);

  char note = map_frequency_to_note(fht_peak_frequency);

  lcd.setCursor(0, 1);
  lcd.print("           "); 
  lcd.setCursor(0, 1);
  lcd.print(note);

  delay(1000); 
}

int fht_find_peak() {
  int max_index = 0;
  for (int i = 0; i < FHT_N / 2; i++) {
    if (fht_log_out[i] > fht_log_out[max_index]) {
      max_index = i;
    }
  }
  return max_index;
}

char map_frequency_to_note(float frequency) {
  float string_frequencies[6] = {82.41, 110.00, 146.83, 196.00, 246.94, 329.63};
  char notes[7] = {'E', 'A', 'D', 'G', 'B', 'E'};
  
  int nearest_index = 0;
  float min_difference = abs(frequency - string_frequencies[0]);
  for (int i = 1; i < 6; i++) {
    float difference = abs(frequency - string_frequencies[i]);
    if (difference < min_difference) {
      min_difference = difference;
      nearest_index = i;
    }
  }

  char note = notes[nearest_index];

  return note;
}
