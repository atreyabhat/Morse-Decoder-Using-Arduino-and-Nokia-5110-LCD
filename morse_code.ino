
#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Adafruit_PCD8544.h>
#include <LedControl.h>

unsigned long signal_len,t1,t2;   //time for which button is pressed
int inputPin = 2;                 //input pin for push button

String code = "";                 //string in which one alphabet is stored
char x;

int DIN = 12;                     //LED matrix Connections
int CS =  11;
int CLK = 10;

byte dot[8]=     {0x00,0x00,0x3C,0x3C,0x3C,0x3C,0x00,0x00};          //The co ord for LED matrix
byte dash[8]=     {0x00,0x18,0x18,0x18,0x18,0x18,0x18,0x00};



// pin 7 - Serial clock out (SCLK)
// pin 6 - Serial data out (DIN)
// pin 5 - Data/Command select (D/C)
// pin 4 - LCD chip select (CS)
// pin 3 - LCD reset (RST)
Adafruit_PCD8544 display = Adafruit_PCD8544(7, 6, 5, 4, 3);



LedControl lc=LedControl(DIN,CLK,CS,0);

void setup() {
  Serial.begin(9600);
  pinMode(inputPin, INPUT_PULLUP); //internal pullup resistor is used to simplify the circuit


lc.shutdown(0,false);       //The MAX72XX is in power-saving mode on startup
 lc.setIntensity(0,5);      // Set the brightness to maximum value
 lc.clearDisplay(0);    

 display.begin();
  // init done

  // you can change the contrast around to adapt the display
  // for the best viewing!
  display.setContrast(50);

  
  delay(2000);
  display.clearDisplay();   // clears the screen and buffer
 display.setTextSize(2);
  display.setTextColor(BLACK);
  display.setCursor(0,0);

  
}

void loop()
{
NextDotDash:
  while (digitalRead(inputPin) == HIGH) {}
  t1 = millis();                            //time at button press
  while (digitalRead(inputPin) == LOW) {}
  t2 = millis();                            //time at button release
  signal_len = t2 - t1;                     //time for which button is pressed
  if (signal_len > 60)                      //to account for switch debouncing
  {
    code += readio();                       //function to read dot or dash
  }
  while ((millis() - t2) < 500)           //if time between button press greater than 0.5sec, skip loop and go to next alphabet
  {     
    if (digitalRead(inputPin) == LOW)
    {
      goto NextDotDash;
    }
  }
  convertor();                          //function to decipher code into alphabet
}

char readio()
{
  if (signal_len < 600 && signal_len > 50)
  {
     printByte(dot); 
     delay(50);
      lc.clearDisplay(0);   

    return '.';    
        //if button press less than 0.6sec, it is a dot
  }
  else if (signal_len > 600)
  {
     printByte(dash); 
          delay(50);
           lc.clearDisplay(0);   


    return '-';  
         //if button press more than 0.6sec, it is a dash
  }
}

void convertor()
{
  static String letters[] = {".-", "-...", "-.-.", "-..", ".", "..-.", "--.", "....", "..", ".---", "-.-", ".-..", "--", "-.", "---", ".--.", "--.-",
                             ".-.", "...", "-", "..-", "...-", ".--", "-..-", "-.--", "--..", "E"
                            };
  int i = 0;
  if (code == ".-.-.-")
  {
    Serial.print(".");    
    display.print(" ");
    display.display();
    
  }
  else
  {
    while (letters[i] != "E")  //loop for comparing input code with letters array
    {
      if (letters[i] == code)
      {
        Serial.print(char('A' + i));
        char x=char('A'+i);

  display.print(x);
    display.display();


        break;
      }
      i++;
    }
    if (letters[i] == "E")
    {
      Serial.println("<Wrong input>"); 
      
      //if input code doesn't match any letter, error
    }
  }
  code = "";                            //reset code to blank string
}


void printByte(byte character [])
{
  int i = 0;
  for(i=0;i<8;i++)
  {
    lc.setRow(0,i,character[i]);
  }
}
