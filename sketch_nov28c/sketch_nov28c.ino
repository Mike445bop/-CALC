#include <stdint.h>
#include <stdlib.h>

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

int button1 = 17;
int button2 = 16;
int button3 = 19;
int button4 = 18;
int button5 =  5;
int button6 =  4;
int button7 = 36;
int button8 = 39;
int button0 = 34;

const int buttonPins[] = { button1, button2, button3, button4, button5, button6, button7, button8, button0};
int taille = sizeof(buttonPins)/sizeof(buttonPins[0]);

const char buttonValues[] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9'};
//fonctionnement bouton pour calc
//switch pour + ou -
int switch1 = 26;
int val;
char plusOuMoins;
bool calculationComplete = false;
bool isAnswerCorrect = false;
int userInputIndex1 = 0;
int userInputIndex2 = 0;
int userInputIndex3 = 0;
char userInput[2];
char userInput1[2]; 
char operation = '\0';
int firstOperand = 0, secondOperand = 0;
bool enteringFirstOperand = true;  // Start with entering the first operand
bool enteringSecondOperand = true;  // Initially, not entering the second operand
bool enteringAnswer = true;
// SPI Interface
uint8_t _SCL; // 5
uint8_t _SDI; // 4
uint8_t _CS; // 3

// RS232 Interface
uint8_t _TX; // 2

//I2C Interface
uint8_t _SDA; // 4

enum Interface{
  I2C,
  SPI,
  RS232
};

Interface _interface;

void setup() 
{ 
  Serial.begin(115200);
  initLCD_SPI(22, 23, 21);
  pinMode(switch1, INPUT);
  for(int i=0; i < taille; i++)
  {
    pinMode(buttonPins[i], INPUT);
  }  
  // Uncomment desired interface. Currently configured for SPI.
  

  writeString((unsigned char*)"Newhaven Display----");
  setCursor(0x40);
  writeString((unsigned char*)" - Character LCD");
  setCursor(0x14);
  writeString((unsigned char*)" - Serial LCD");
  setCursor(0x54);
  writeString((unsigned char*)"  -> I2C, SPI, RS232");
  setBrightness(0x08);
  setContrast(0x28);
  putData_SPI(0x69);
  delay(3000);
  clearScreen();
    // Display initial message
  setCursor(0x40);
  writeString((unsigned char*)"Calculator Ready");
  setCursor(0x14); // Move to second row
  writeString((unsigned char*)"Press buttons...");
  delay(5000);
}

void loop() {
  
  if (!calculationComplete) {
    for (int i = 0; i < taille; i++) {
      if (digitalRead(buttonPins[i]) == HIGH) {
        delay(400); // Debounce delay
        char button = buttonValues[i];
        
        processInput(button);
        updateDisplay();
        break;
      }
    }
  } else {
    // Display result
    displayResult();
    delay(4000); // Wait before resetting
    resetCalculator();
  }
}

void processInput(char button) {
  if (button >= '0' && button <= '9') 
  {
    if (enteringFirstOperand) {
      // Build first operand
      // prendre la lettre et la mettre dans une string
      firstOperand = firstOperand * 10 + (button - '0');
      additionOrSustraction(switch1);
      userInput[userInputIndex1++] = button;
      userInput[userInputIndex1] = '\0';
      writeString((unsigned char*)"Input: ");
      writeString((unsigned char*)userInput);
      delay(1000);
      //enteringFirstOperand = false
    } 
    else if (enteringSecondOperand)
    {
      // Build second operand
      
      secondOperand = secondOperand * 10 + (button - '0');
      userInput[userInputIndex2++] = button;
      userInput[userInputIndex2] = '\0';
      writeString((unsigned char*)"Input: ");
      writeString((unsigned char*)userInput);
      delay(1000);
    }
    else if(enteringAnswer)
    {
      //build the answer
      
      enteringAnswer = enteringAnswer * 10 + (button - '0');
      
      userInput1[userInputIndex3++] = button;
      userInput1[userInputIndex3] = '\0';
      writeString((unsigned char*)"answer : ");
      writeString((unsigned char*)userInput);
      delay(1000);
    }
  }
  if(btnEnter == HIGH)
  {
    //verifier quel operend
    //if firstOperand
      // parse la string en int
    //else if secondOperand
     // parse la string en int
    // else calculer    
  }
  /*if (enteringFirstOperand && userInputIndex1 >= sizeof(userInput) - 1) {
    // Finish first operand input
    enteringFirstOperand = false;
    userInputIndex1 = 0; // Reset index for the second operand
    userInput[0] = '\0';
    userInput1[0];  // Clear the input buffer
    delay(500); // Wait for 5 seconds
    setCursor(0x00);
    clearScreen();
    writeString((unsigned char*)"Enter 2nd operand:");
    delay(500);
    enteringSecondOperand = true;
  } 
  
  else if (enteringSecondOperand && userInputIndex2 >= sizeof(userInput) - 1) 
  {
    enteringSecondOperand = false;
    userInputIndex2 = 0; // Reset index for the second operand
    userInput[0] = '\0'; // Clear the input buffer
    userInput1[0] = '\0';// Clear the input buffer
    delay(5000); // Wait for 5 seconds
    setCursor(0x00);
    writeString((unsigned char*)"Enter 2nd operand:");
    enteringAnswer = true;
  }
  else if (enteringAnswer && userInputIndex2 >= sizeof(userInput) - 1)
  {
  // Finish second operand input
  enteringSecondOperand = false;
  calculationComplete = true;
  }*/
}
void additionOrSustraction(int switch1)
{
  int switchState = digitalRead(switch1);
  Serial.print("Operation: add\n");
    
    if (switchState == LOW) {
      operation = '+';  // Set operation to addition
      Serial.print("Operation: +");
      setCursor(0x40);
      writeString((unsigned char*)"op :");
      write(operation);
    } 
    else 
    {
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
  int correctAnswer = (operation == '+') ? (firstOperand + secondOperand) : (firstOperand - secondOperand);

  int userAnswer = atoi(userInput);

  setCursor(0x00);
  if (userAnswer == correctAnswer) {
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
  userInputIndex1 = 0;
  userInputIndex2 = 0;
  userInput[0] = '\0';
  userInput1[0] = '\0';
  calculationComplete = false;
  enteringFirstOperand = true;
  enteringSecondOperand = false;
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
 void initLCD_SPI(uint8_t SCL, uint8_t SDI, uint8_t CS)
{
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
void startCondition()
{
  clearSDA();
  clearSCL();
}
void stopCondition()
{
  setSCL();
  setSDA();
}
void setSDA()
{
  digitalWrite(_SDA, HIGH);
  
}
void clearSDA()
{
  digitalWrite(_SDA, LOW);
  
}
void setSCL()
{
  digitalWrite(_SCL, HIGH);
  
}
void clearSCL()
{
  digitalWrite(_SCL, LOW);

}
void setCS()
{
  digitalWrite(_CS, HIGH);
  delay(1);
}
void write(uint8_t data)
{
  clearCS();
  putData_SPI(data);
  delayMicroseconds(10);
  setCS();
  delayMicroseconds(20);
}
void clearCS()
{
  digitalWrite(_CS, LOW);
  delay(1);
}
void writeString(unsigned char* data)
{
  // Iterate through data until null terminator is found.
  while(*data != '\0')
  {
    write(*data);
    data++; // Increment pointer.
  }
}
void putData_SPI(uint8_t data)
{
  // Write data byte MSB first -> LSB last
  for(int i = 7; i >= 0; i--)
  {
    clearSCL();

    digitalWrite(_SDI, (data >> i) & 0x01);
    
    setSCL();
  }
}
void clearScreen()
{
  write(0xFE);
  write(0x51);
  delay(2);
}
void setCursor(uint8_t position)
{
  write(0xFE);
  write(0x45);
  write(position);
}
void setBrightness(uint8_t brightness)
{
  write(0xFE);
  write(0x53);
  write(brightness);
}
void setContrast(uint8_t contrast)
{
  write(0xFE);
  write(0x52);
  write(contrast);
}