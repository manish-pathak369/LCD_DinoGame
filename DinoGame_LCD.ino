#include <Wire.h>
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x27, 16, 2);

const int jumpButton = 2;

int dinoPos = 0;          // 0 = ground, 1 = jumping
int cactusPos = 15;       // Cactus position
int score = 0;
boolean gameRunning = true;
unsigned long jumpStartTime = 0;
const int JUMP_DURATION = 600;  // Jump time in milliseconds

byte dino[8] = {
  B00000,
  B00000,
  B00111,
  B00101,
  B10111,
  B10100,
  B01110,
  B00000
};

byte cactus[8] = {
  B00100,
  B10100,
  B10100,
  B10100,
  B10100,
  B10111,
  B01100,
  B00100
};

void setup() {
  lcd.init();
  lcd.backlight();
  
  lcd.createChar(0, dino);
  lcd.createChar(1, cactus);
  
  pinMode(jumpButton, INPUT_PULLUP);
  
  lcd.setCursor(0, 1);
  lcd.write(byte(0));
}

void loop() {
  if (gameRunning) {
    // Handle jumping
    if (digitalRead(jumpButton) == LOW && dinoPos == 0) {
      dinoPos = 1;
      jumpStartTime = millis();
      lcd.setCursor(0, 1);
      lcd.print(" ");
      lcd.setCursor(0, 0);
      lcd.write(byte(0));
    }
    
    // Check if jump should end
    if (dinoPos == 1 && millis() - jumpStartTime >= JUMP_DURATION) {
      dinoPos = 0;
      lcd.setCursor(0, 0);
      lcd.print(" ");
      lcd.setCursor(0, 1);
      lcd.write(byte(0));
    }
    
    // Move cactus
    static unsigned long lastMove = 0;
    if (millis() - lastMove >= 200) {  // Control cactus speed
      lcd.setCursor(cactusPos, 1);
      lcd.print(" ");
      cactusPos--;
      
      if (cactusPos < 0) {
        cactusPos = 15;
        score++;
      }
      
      lcd.setCursor(cactusPos, 1);
      lcd.write(byte(1));
      lastMove = millis();
    }
    
    // Improved collision detection
    if (cactusPos == 0 && dinoPos == 0) {
      gameRunning = false;
      gameOver();
    }
    
    // Update score
    lcd.setCursor(12, 0);
    lcd.print("S:");
    if (score < 10) lcd.print(" ");  // Padding for single digits
    lcd.print(score);
  }
}

void gameOver() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Game Over!");
  lcd.setCursor(0, 1);
  lcd.print("Score: ");
  lcd.print(score);
  
  while (digitalRead(jumpButton) == HIGH) {
    delay(100);
  }
  
  // Reset game
  score = 0;
  cactusPos = 15;
  dinoPos = 0;
  lcd.clear();
  lcd.setCursor(0, 1);
  lcd.write(byte(0));
  gameRunning = true;
}
