#include <LiquidCrystal.h>

// Initialize LCD with pins: RS, EN, D4, D5, D6, D7
LiquidCrystal lcd(22, 23, 24, 25, 26, 27);

// Multipliers for payoff
const int HIGH_MULTIPLIER = 50;
const int LOW_MULTIPLIER = 10;

// BCD input pins (12 BCD bits for 3 digits)
int bcdPins[12] = {2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13};

int bcdToDecimal(int bcd[4]) {
  int decimal = 0;
  for (int i = 0; i < 4; i++) {
    decimal |= (bcd[i] << (3 - i));
  }
  return decimal;
}

void setup() {
  Serial.begin(9600);
  lcd.begin(16, 2);

  for (int i = 0; i < 12; i++) {
    pinMode(bcdPins[i], INPUT);
  }

  lcd.setCursor(0, 0);
  lcd.print("..Slot Machine..");
  delay(2000);
  lcd.clear();
}

void loop() {
  int bet = 0;
  int digits[3] = {0, 0, 0};
  int payoff = 0;

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Waiting for bet");
  Serial.println("Enter bet (0 to 3):");

  while (!Serial.available());
  bet = Serial.parseInt();
  Serial.read();

  while (bet < 0 || bet > 3) {
    Serial.println("Invalid bet. Enter 0 to 3:");
    while (!Serial.available());
    bet = Serial.parseInt();
    Serial.read();
  }

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Bet Placed: ");
  lcd.print(bet);
  delay(2000);

  // Read BCD inputs
  for (int i = 0; i < 3; i++) {
    int bcd[4];
    for (int j = 0; j < 4; j++) {
      bcd[j] = digitalRead(bcdPins[i * 4 + j]);
    }
    digits[i] = bcdToDecimal(bcd);
  }

  // Calculate payoff
  payoff = calculatePayoff(digits[0], digits[1], digits[2], bet);

  // Display result
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Digits: ");
  lcd.print(digits[0]);
  delay(500);
  lcd.print(" ");
  lcd.print(digits[1]);
  delay(500);
  lcd.print(" ");
  lcd.print(digits[2]);
  delay(500);
  lcd.setCursor(0, 1);
  lcd.print("Payoff: ");
  lcd.print(payoff);

  Serial.print("Digits: ");
  Serial.print(digits[0]); Serial.print(" ");
  Serial.print(digits[1]); Serial.print(" ");
  Serial.println(digits[2]);
  Serial.print("Payoff: ");
  Serial.println(payoff);

  Serial.println("Press any key to play again...");
  while (!Serial.available());
  Serial.read();
}

int calculatePayoff(int a, int b, int c, int bet) {
  int base = 0;

  if (a == 0 && b == 0 && c == 0) {
    return 1000;
  } else if ((a == b && a == 0) || (a == c && a == 0) || (b == c && c == 0)) {
    return 200;
  } else if (a == b && b == c) {
    base = a * HIGH_MULTIPLIER;
  } else if (a == b || a == c) {
    base = a * LOW_MULTIPLIER;
  } else if (b == c) {
    base = b * LOW_MULTIPLIER;
  }

  return base * (bet + 1);
}
