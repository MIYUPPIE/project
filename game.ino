#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64

#define JOY_X A0
#define JOY_Y A1
#define JOY_BUTTON 2

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1, 400000UL);

const int boxSize = 20;
const int boxMargin = 4;
const int gameSpeed = 1000; // Time between each box movement (in milliseconds)
const int numMoves = 10;    // Number of moves to remember
const int maxLevel = 5;     // Maximum level of the game

// Array to store the sequence of box movements
int moves[numMoves];

// Variables to track player's progress
int playerIndex = 0;
bool isGameOver = false;
int level = 1;
int scoreC = 0; // Score for each level
int scoreR = 0; // Total cumulative score across all levels
int positiveMoves = 0; // Number of positive moves in the current level
String playerName = "";

void setup() {
  pinMode(JOY_BUTTON, INPUT_PULLUP);
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0);
  display.println("WELCOME TO");
  display.println();
  display.println("MEMORY");
  display.println("GAME");
  display.println();

  display.setTextSize(1);
  display.clearDisplay();
  display.display();
  delay(2000);
    // Clear the display
  display.clearDisplay();

  // Draw the brain icon
  drawBrainIcon();

  // Display the content
  display.display();
  delay(2000);
  testJoystick();
  getPlayerName();
  display.clearDisplay();
}
void drawBrainIcon() {
  // Draw the left hemisphere
  display.fillCircle(40, 32, 20, SSD1306_WHITE);
  display.drawCircle(40, 32, 21, SSD1306_WHITE);
  display.fillRect(35, 12, 10, 40, SSD1306_WHITE);

  // Draw the right hemisphere
  display.fillCircle(88, 32, 20, SSD1306_WHITE);
  display.drawCircle(88, 32, 21, SSD1306_WHITE);
  display.fillRect(83, 12, 10, 40, SSD1306_WHITE);

  // Draw the connecting lines
  display.drawLine(48, 32, 78, 32, SSD1306_WHITE);
  display.drawLine(40, 16, 48, 32, SSD1306_WHITE);
  display.drawLine(88, 32, 96, 16, SSD1306_WHITE);
}
void loop() {
  while (!isGameOver) {
    playGame();
    if (!isGameOver) {
      displayLevelUp();
      delay(2000);
      level++;
    }
  }

  displayGameOver();
  delay(2000);
  resetGame();
}

void testJoystick() {
  display.clearDisplay();
  display.setCursor(0, 0);
  display.println("Test Joystick");
  display.println();
  display.println("Push the joystick");
  display.println("in each direction:");
  display.println();

  while (digitalRead(JOY_BUTTON) == HIGH) {
    int x = analogRead(JOY_X);
    int y = analogRead(JOY_Y);

    display.setCursor(0, 40);
    if (y < 70) {
      display.println("UP");
    } else {
      display.println("        ");
    }

    display.setCursor(60, 40);
    if (y > 700) {
      display.println("DOWN");
    } else {
      display.println("        ");
    }

    display.setCursor(0, 50);
    if (x > 700) {
      display.println("RIGHT");
    } else {
      display.println("        ");
    }

    display.setCursor(60, 50);
    if (x < 70) {
      display.println("LEFT");
    } else {
      display.println("        ");
    }

    display.display();
    delay(100);
  }

  display.clearDisplay();
  display.display();
}
void getPlayerName() {
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0);
  display.println("Enter your name:");
  display.display();

  String inputName = "";
  int currentCharIndex = 0; // Track the index of the current character being selected

  const String characters = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789";

  while (true) {
    int joyX = analogRead(JOY_X);
    int joyY = analogRead(JOY_Y);

    if (joyX < 70) { // Move to the next letter on right button press
      currentCharIndex++;
      if (currentCharIndex >= characters.length()) {
        currentCharIndex = 0; // Wrap around to the first character
      }
      delay(200);
    }
    if (joyX > 700) { // Move to the previous letter on left button press
      currentCharIndex--;
      if (currentCharIndex < 0) {
        currentCharIndex = characters.length() - 1; // Wrap around to the last character
      }
      delay(200);
    }
    if (joyY < 70) { // Select the current letter on up button press
      if (inputName.length() < 4) {
        inputName += characters[currentCharIndex];
      }
      delay(200);
    }
    if (joyY > 700) { // Remove the last character on down button press
      if (inputName.length() > 0) {
        inputName.remove(inputName.length() - 1);
      }
      delay(200);
    }

    // Exit name input loop when the name reaches the desired length
    if (inputName.length() >= 4) {
      break;
    }

    // Check for invalid characters
    if (characters.indexOf(inputName[currentCharIndex]) < 0) {
      inputName[currentCharIndex] = '\0';
    }

    display.clearDisplay();
    display.setTextSize(1);
    display.setTextColor(SSD1306_WHITE);
    display.setCursor(0, 0);
    display.println("Enter your name:");
    display.println(inputName);

    // Display the current character being selected with a ">" symbol
    display.print("=");
    display.println(characters[currentCharIndex]);
    display.display();
  }

  playerName = inputName;
}
void displayInstructions() {
  display.clearDisplay();
  display.setCursor((SCREEN_WIDTH - 84) / 2, 10);
  display.print("Memory Game");
  display.setCursor(10, 20);
  display.print("Player: " + playerName);
  display.setCursor(10, 30);
  display.print("Level: " + String(level));
  display.setCursor(0, 40);
  display.print("Remember and repeat");
  display.setCursor(10, 50);
  display.print("the box movements");
  display.display();
  delay(3000);
  display.clearDisplay();
  display.display();
}

void generateMoves() {
  for (int i = 0; i < numMoves; i++) {
    moves[i] = random(4);
  }
}

void displaySequence() {
  display.clearDisplay();

  int move = moves[playerIndex];
  int x, y;

  switch (move) {
    case 0: // Down
      x = (SCREEN_WIDTH - boxSize) / 2;
      y = boxMargin;
      break;
    case 1: // Up
      x = (SCREEN_WIDTH - boxSize) / 2;
      y = SCREEN_HEIGHT - boxSize - boxMargin;
      break;
    case 2: // Right
      x = boxMargin;
      y = (SCREEN_HEIGHT - boxSize) / 2;
      break;
    case 3: // Left
      x = SCREEN_WIDTH - boxSize - boxMargin;
      y = (SCREEN_HEIGHT - boxSize) / 2;
      break;
  }

  display.fillRect(x, y, boxSize, boxSize, SSD1306_WHITE);
  display.display();
  delay(gameSpeed);
  display.fillRect(x, y, boxSize, boxSize, SSD1306_BLACK);
  display.display();
  delay(gameSpeed / 2); // Short pause between movements

  display.clearDisplay();
  display.display();
}

void displayScore() {
  display.setCursor(10, 30);
  display.print("Score (C): ");
  display.print(scoreC);
  display.setCursor(10, 40);
  display.print("\nScore (R): ");
  display.print(scoreR);
  display.display();
}

void playerTurn() {
  int move = -1;
  int levelScoreC = 0; // Score for the current level
  bool playerHasMoved = false;

  while (!playerHasMoved) {
    int joyX = analogRead(JOY_X);
    int joyY = analogRead(JOY_Y);

    if (joyX > 700) { // Move to the right on right button press
      move = 3; // Right
      playerHasMoved = true;
    } else if (joyX < 70) { // Move to the left on left button press
      move = 2; // Left
      playerHasMoved = true;
    } else if (joyY > 700) { // Move up on up button press
      move = 1; // Up
      playerHasMoved = true;
    } else if (joyY < 70) { // Move down on down button press
      move = 0; // Down
      playerHasMoved = true;
    } else if (digitalRead(JOY_BUTTON) == LOW) {
      move = -1;
      playerHasMoved = true;
    }

    // Check if the player has made a valid move
    if (playerHasMoved) {
      break;
    }

    delay(200);
  }

  int correctMove = moves[playerIndex];

  if (move == correctMove) {
    playerIndex++;
    positiveMoves++;
    levelScoreC++; // Increment the score for the current level
    scoreR++;   
    scoreC++;   // Increment the total cumulative score
    delay(500);

    if (playerIndex >= numMoves) {
      scoreC = 0; // Add the current level's score to the total score
      playerIndex = 0;
      level++;
      if (level > maxLevel) {
        // Game is over, display the game over screen
        isGameOver = true;
        return;
      } else {
        // Level up, generate new moves and continue playing
        generateMoves();
        display.clearDisplay();
        display.setCursor(10, 10);
        display.print("Level Up!");
        display.display();
        displayScore();
        delay(3000);
        scoreC = 0;
      }
    } else {
      display.clearDisplay();
      display.setCursor(10, 10);
      display.print("Correct!");
      display.setCursor(10, 20);
      display.print("Level: ");
      display.print(level);
      display.display();
      displayScore();
      display.print("Level: ");
      display.print(level);
      delay(2000);
    }

    display.clearDisplay();
    display.display();
  } else {
    // Game over, display the game over screen
    isGameOver = true;
  }
}

void displayGameOver() {
  display.clearDisplay();
  display.setCursor((SCREEN_WIDTH - 72) / 2, 10);
  display.print("Game Over");
  display.setCursor((SCREEN_WIDTH - 48) / 2, 30);
  display.print("Level: ");
  display.print(level);
  display.setCursor(10, 40);
  display.print("Score (C): ");
  display.print(scoreC);
  display.setCursor(10, 50);
  display.print("\nScore (R): ");
  display.print(scoreR);
  display.display();
}

void resetGame() {
  playerIndex = 0;
  isGameOver = false;
  level = 1;
  scoreC = 0;
  scoreR = 0;
}

void resetScoreC() {
  scoreC = 0;
}

void playGame() {
  generateMoves();
  displayInstructions();
  positiveMoves = 0; // Reset positiveMoves count to zero at the beginning of each level
  resetScoreC(); // Reset scoreC to zero at the beginning of each level
  while (!isGameOver) {
    displaySequence();
    playerTurn();
  }
}

void displayLevelUp() {
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor((SCREEN_WIDTH - 52) / 2, SCREEN_HEIGHT / 2 - 10);
  display.println("Level Up!");
  display.setCursor((SCREEN_WIDTH - 32) / 2, SCREEN_HEIGHT / 2 + 10);
  display.print("Level ");
  display.println(level + 1);
  display.display();
}