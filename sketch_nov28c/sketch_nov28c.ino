#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#define STARTUP_DELAY 500


#define SLAVE_ADDRESS 0x28
void setCursor(uint8_t position);
void startCondition();
void stopCondition();
void setSDA();
void clearSDA();
void clearSCL();
void setWriteMode();
void write(uint8_t data);
void writeString(unsigned char* data);
void putData_SPI(uint8_t data);
void clearScreen();
void setCS();
void clearCS();
int operateurSwitch();
void processInput(char button);
void updateDisplay();
void displayResult();
void resetCalculator();
void additionOrSustraction(bool switch1);
void determineAnswer();

// Pins
const int button1 = 17;
const int button2 = 16;
const int button3 = 19;
const int button4 = 18;
const int button5 = 5;
const int button6 = 4;
const int button7 = 36;
const int button8 = 39;
const int button0 = 34;
const int switch1 = 26;

const int buttonPins[] = { button1, button2, button3, button4, button5, button6, button7, button8, button0 };
const char buttonValues[] = { '0', '1', '2', '3', '4', '5', '6', '7', '8', '9' };

int taille = sizeof(buttonPins) / sizeof(buttonPins[0]);
int firstOperand = 0;
int secondOperand = 0;
int answer = 0;
int goodAnswer = 0;
// Bools
bool enteringFirstOperand = true;
bool enteringSecondOperand = true; 
bool enteringAnswer = false;
bool calculationComplete = false;
bool isAnswerCorrect = false;
bool secondDigitAnswer = false;
// Char
char plusOuMoins;
char operation = '\0';
// SPI Interface
uint8_t _SCL;  // 5
uint8_t _SDI;  // 4
uint8_t _CS;   // 3

// RS232 Interface
uint8_t _TX;  // 2

//I2C Interface
uint8_t _SDA;  // 4

enum Interface {
  I2C,
  SPI,
  RS232
};

Interface _interface;

void setup() {
  Serial.begin(115200);
  initLCD_SPI(22, 23, 21);
  pinMode(switch1, INPUT);
  for (int i = 0; i < taille; i++) {
    pinMode(buttonPins[i], INPUT);
  }
  setBrightness(0x08);
  setContrast(0x28);
  putData_SPI(0x69);
  delay(10);
  clearScreen();
  // Display initial message
  setCursor(0x40);
  writeString((unsigned char*)"Calculator Ready");
  setCursor(0x14);  // Move to second row
  writeString((unsigned char*)"enter firstoperand...");
  delay(5000);
  clearScreen();
}

void loop() {
  if (!calculationComplete) {
    //Passe a travers tous les boutton
    for (int i = 0; i < taille; i++) {
      //voit quel boutton est cliquer
      if (digitalRead(buttonPins[i]) == HIGH) {
        delay(400);  // Debounce delay
        char button = buttonValues[i];
        processInput(button);
        updateDisplay();
        break;
      }
    }
  } else {
    // Display result
    displayResult();
    delay(4000);  // Wait before resetting
    resetCalculator();
  }
}
void determineAnswer()
{
   goodAnswer = (operation == '+') ? (firstOperand + secondOperand) : (firstOperand - secondOperand);
}
void processInput(char button) 
{
  if (button >= '0' && button <= '9') {
    if (enteringAnswer) {
      if (goodAnswer > 9 && secondDigitAnswer) 
      {
        answer = (button - '0') + 10;
        calculationComplete = true;
      } 
      else if(goodAnswer<10)
      {  //build the answer
        answer = button - '0';
        writeString((unsigned char*)"answer : ");
        writeString((unsigned char*)button);
        delay(1000);
        calculationComplete = true;
      }
      secondDigitAnswer = true;
    }

    if (enteringFirstOperand) {
      clearScreen();
      firstOperand = button - '0';
      setCursor(0x14);
      additionOrSustraction(switch1);
      setCursor(0x40);
      writeString((unsigned char*)"Input: ");
      writeString((unsigned char*)button);
      delay(5000);
      setCursor(0x00);
      clearScreen();
      writeString((unsigned char*)"Enter 2nd operand:");
      delay(500);
      enteringFirstOperand = false;
    } else if (enteringSecondOperand){
      clearScreen();
      secondOperand = button - '0';
      setCursor(0x40);
      writeString((unsigned char*)"Input: ");
      writeString((unsigned char*)button);
      delay(5000);
      setCursor(0x00);
      writeString((unsigned char*)"Enter Answer:");
      enteringAnswer = true;
      determineAnswer();
      enteringSecondOperand = false;
    }
  }
}
void additionOrSustraction(int switch1) {
  int switchState = digitalRead(switch1);
  Serial.print("Operation: add\n");

  if (switchState == LOW) {
    operation = '+';  // Set operation to addition
    Serial.print("Operation: +");
    setCursor(0x40);
    writeString((unsigned char*)"op :");
    write(operation);
  } else {
    operation = '-';  // Set operation to subtraction
    Serial.print("Operation: -");
    setCursor(0x40);
    writeString((unsigned char*)"op :");
    write(operation);
  }
}
void updateDisplay() {
  clearScreen();
  setCursor(0x00);
}

void displayResult() {
  delay(500);
  clearScreen();
  setCursor(0x00);
  if (answer == goodAnswer) {
    writeString((unsigned char*)"Correct Answer!");
    delay(100);
  } else {
    writeString((unsigned char*)"Incorrect!");
    setCursor(0x40);
    writeString((unsigned char*)"Try Again.");
  }
}

void resetCalculator() {
  firstOperand = 0;
  secondOperand = 0;
  operation = '\0';
  calculationComplete = false;
  enteringFirstOperand = true;
  enteringAnswer = false;

  clearScreen();
  writeString((unsigned char*)"Calculator Ready");
  setCursor(0x40);
  writeString((unsigned char*)"Enter 1st operand:");
}
/**
 * @brief Initialize selected IO ports for I2C.
 * 
 * @param SCL Serial clock pin assigment.
 * @param SDA Serial data pin assignment.
 * @return none
 */
void initLCD_SPI(uint8_t SCL, uint8_t SDI, uint8_t CS) {
  _interface = SPI;

  // Store pin assignments globally
  _SCL = SCL;
  _SDI = SDI;
  _CS = CS;

  // Set IO modes
  pinMode(CS, OUTPUT);
  pinMode(SCL, OUTPUT);
  pinMode(SDI, OUTPUT);

  // Set pin states
  digitalWrite(CS, HIGH);
  digitalWrite(SCL, HIGH);

  // Wait for display to power ON
  delay(STARTUP_DELAY);
  clearScreen();
}
void startCondition() {
  clearSDA();
  clearSCL();
}
void stopCondition() {
  setSCL();
  setSDA();
}
void setSDA() {
  digitalWrite(_SDA, HIGH);
}
void clearSDA() {
  digitalWrite(_SDA, LOW);
}
void setSCL() {
  digitalWrite(_SCL, HIGH);
}
void clearSCL() {
  digitalWrite(_SCL, LOW);
}
void setCS() {
  digitalWrite(_CS, HIGH);
  delay(1);
}
void write(uint8_t data) {
  clearCS();
  putData_SPI(data);
  delayMicroseconds(10);
  setCS();
  delayMicroseconds(20);
}
void clearCS() {
  digitalWrite(_CS, LOW);
  delay(1);
}
void writeString(unsigned char* data) {
  // Iterate through data until null terminator is found.
  while (*data != '\0') {
    write(*data);
    data++;  // Increment pointer.
  }
}
void putData_SPI(uint8_t data) {
  // Write data byte MSB first -> LSB last
  for (int i = 7; i >= 0; i--) {
    clearSCL();

    digitalWrite(_SDI, (data >> i) & 0x01);

    setSCL();
  }
}
void clearScreen() {
  write(0xFE);
  write(0x51);
  delay(2);
}
void setCursor(uint8_t position) {
  write(0xFE);
  write(0x45);
  write(position);
}
void setBrightness(uint8_t brightness) {
  write(0xFE);
  write(0x53);
  write(brightness);
}
void setContrast(uint8_t contrast) {
  write(0xFE);
  write(0x52);
  write(contrast);
}