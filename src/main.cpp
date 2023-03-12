#include <Arduino.h>
#include<vector>

//Classes definition
class Button                                                  //Button object definition
{
  public:
    uint8_t pin;                                              //Button pin
    bool val_act, val_ant, cambioact, cambioanterior, orden;  //Filter variables
    
    //API
    Button(uint8_t PPIN)                                      //Constructor by PIN
    {
      pin = PPIN;
      val_ant = 1;
      orden = 0;
    }
};

class LED                                                     //LED structure definition 
{
  public:
    uint8_t pin;                                              //LED pin
    byte state;                                               //State of LED

    //API
    LED(){}                                                   //LED Constructor by default
    LED(uint8_t pin, byte state): pin(pin), state(state){}    //LED Constructor with parameters

    LED& operator =(const LED& L)                             //LED Operator 
    {
      if (this != &L)
      {
        this -> state = L.state;
        this -> pin = L.pin ;
      }
      return(*this);
    } 
};

class modes_t                                                  //Modes structure definition
{
  public:
    LED led_1,                                                 //LED variables
        led_2,
        led_3,
        led_4,
        led_5,
        led_6,
        led_7,
        led_8;
    //API
    modes_t(){}                                                 //Modes Constructor By Default
    modes_t(byte state1, byte state2, byte state3, byte state4, 
            byte state5, byte state6, byte state7, byte state8) //Modes Constructor With Parameters
    {
      led_1.pin = 12; led_1.state = state1;
      led_2.pin = 13; led_2.state = state2;
      led_3.pin = 14; led_3.state = state3;
      led_4.pin = 27; led_4.state = state4;
      led_5.pin = 16; led_5.state = state5;
      led_6.pin = 17; led_6.state = state6;
      led_7.pin = 18; led_7.state = state7;
      led_8.pin = 19; led_8.state = state8;
    }
    modes_t& operator =(const modes_t& A)                       //Modes '=' operator
    {
      if(this != &A)
      {
        this -> led_1 = A.led_1;
        this -> led_2 = A.led_2;
        this -> led_3 = A.led_3;
        this -> led_4 = A.led_4;
        this -> led_5 = A.led_5;
        this -> led_6 = A.led_6;
        this -> led_7 = A.led_7;
        this -> led_8 = A.led_8;
      }
      return(*this);
    }
};

//Button definition
Button  button_speed_up(25),
        button_speed_down(32),
        button_effect(33);

//Modes definition
modes_t mode_1(
              0b00000001,
              0b00000010,
              0b00000100,
              0b00001000,
              0b00010000,
              0b00100000,
              0b01000000,
              0b10000000
      ),
      mode_2(
              0b10000000,
              0b01000000,
              0b00100000,
              0b00010000,
              0b00001000,
              0b00000100,
              0b00000010,
              0b00000001
      ),
      mode_3(
              0b01000001,
              0b10000010,
              0b00000101,
              0b00001010,
              0b00010100,
              0b00101000,
              0b01010000,
              0b10100000
      ),
      mode_4(
              0b01010101,
              0b10101010,
              0b01010101,
              0b10101010,
              0b01010101,
              0b10101010,
              0b01010101,
              0b10101010
      ),
      mode_5(
              0b00010001,
              0b00100010,
              0b01000100,
              0b10001000,
              0b10001000,
              0b01000100,
              0b00100010,
              0b00010001
      ),
      mode_6(
              0b10001000,
              0b01000100,
              0b00100010,
              0b00010001,
              0b00010001,
              0b00100010,
              0b01000100,
              0b10001000
      ),
      mode_7(
              0b00010001,
              0b00110011,
              0b01110111,
              0b11111111,
              0b11111111,
              0b01110111,
              0b00110011,
              0b00010001
      );
modes_t mode_v = mode_1;                    //Mode variable
                               
//Global variables
hw_timer_t * timer = NULL;                  //Pointer to timer
int conti = 15, cont = 15;                  //Counter variables
int effectOrder = 1;                        //Number of effect variable
byte  masc = 0b00000001,                    //Mask definition
      masc_ini = masc;                      //Initial Mask def. 

//Function declaration
void mascType();
void IRAM_ATTR buttonManagement();       
void lightsEffects();

//Code that only runs once
void setup()
{
  //Baud rate
  Serial.begin(115200);

  //LEDs pin definition
  pinMode(mode_v.led_1.pin, OUTPUT);
  pinMode(mode_v.led_2.pin, OUTPUT);
  pinMode(mode_v.led_3.pin, OUTPUT);
  pinMode(mode_v.led_4.pin, OUTPUT);
  pinMode(mode_v.led_5.pin, OUTPUT);
  pinMode(mode_v.led_6.pin, OUTPUT);
  pinMode(mode_v.led_7.pin, OUTPUT);
  pinMode(mode_v.led_8.pin, OUTPUT);
  
  //Buttons pins definition
  pinMode(button_effect.pin,INPUT_PULLUP);
  pinMode(button_speed_down.pin,INPUT_PULLUP);
  pinMode(button_speed_up.pin,INPUT_PULLUP);

  //Timer definition
  timer = timerBegin(0, 80, true);                            //Initiation of timer
  timerAttachInterrupt(timer, &buttonManagement, true);       //Relate function with timer
  timerAlarmWrite(timer, 50000, true);                        //Specify time betweem interrupts
  timerAlarmEnable(timer);                                    //Enable timer

}

//Code that runs repeatedly
void loop() 
{
  if (button_effect.orden)                                    //If next effect order 
  {
    effectOrder++;                                            //Increase effect
    if (effectOrder > 7) effectOrder = 1;                     //If last effect, then return to the first one
    mascType();                                               //Call maskType() function
    Serial.print("Cambio estado a: ");                        //Print effect number
    Serial.println(effectOrder);
    button_effect.orden = 0;                                  //Button order to 0
  }

  if (button_speed_up.orden)                                  //If speed up order
  {
    conti -= 1;                                               //Decrease counter
    if(conti == 0)conti = 1;                                  //Counter >= 1
    Serial.print("Contador desciende a: ");                   //Print counter value
    Serial.println(conti);                                   
    button_speed_up.orden = 0;                                //Button order to 0
  } 

  if (button_speed_down.orden)                                //If speed down order            
  {                        
    conti += 1;                                               //Increase counter
    Serial.print("Contador asciende a: ");                    //Print counter value
    Serial.println(conti);                     
    button_speed_down.orden = 0;                              //Button order to 0
  } 

  delay(500);
}

//Ligths effects function
void lightsEffects()
{
  if(cont-- <= 0)                                               //If counter it's decrease to 0, lights effects are executed
  {
    if(masc == 0)masc = masc_ini;                               //If mask is equal to 0, then it's restarted

    digitalWrite(mode_v.led_1.pin, masc & mode_v.led_1.state);  //LEDs value is decided as the 'AND' operation between its state and the mask
    digitalWrite(mode_v.led_2.pin, masc & mode_v.led_2.state);
    digitalWrite(mode_v.led_3.pin, masc & mode_v.led_3.state);
    digitalWrite(mode_v.led_4.pin, masc & mode_v.led_4.state);
    digitalWrite(mode_v.led_5.pin, masc & mode_v.led_5.state);
    digitalWrite(mode_v.led_6.pin, masc & mode_v.led_6.state);
    digitalWrite(mode_v.led_7.pin, masc & mode_v.led_7.state);
    digitalWrite(mode_v.led_8.pin, masc & mode_v.led_8.state);

    masc *= 2;                                                  //We move mask'values one position
    cont = conti;                                               //Counter reset
  } 
}

//Effects management
void mascType()
{
  switch(effectOrder)
  {
    case 1:             //if effectOrder is equal to 1, then mode_1 is selected
      mode_v = mode_1;
      break;
    case 2:             //if effectOrder is equal to 2, then mode_2 is selected
      mode_v = mode_2;
      break;
    case 3:             //if effectOrder is equal to 3, then mode_3 is selected
      mode_v = mode_3;
      break;
    case 4:             //if effectOrder is equal to 4, then mode_4 is selected
      mode_v = mode_4;
      break;
    case 5:             //if effectOrder is equal to 5, then mode_5 is selected
      mode_v = mode_5;
      break;
    case 6:             //if effectOrder is equal to 6, then mode_6 is selected
      mode_v = mode_6;
      break;
    case 7:             //if effectOrder is equal to 7, then mode_7 is selected
      mode_v = mode_7;
      break;
  }
}

//Timer function for button management and lights effects
void IRAM_ATTR buttonManagement()
{
  lightsEffects();                                                                      //Lights effects
  
  //Read the value of the button
  button_speed_up.val_act = digitalRead(button_speed_up.pin); 
	button_speed_down.val_act = digitalRead(button_speed_down.pin);
	button_effect.val_act = digitalRead(button_effect.pin); 

  //XOR of actual value and last value
	button_speed_up.cambioact = button_speed_up.val_ant ^ button_speed_up.val_act;    
	button_speed_down.cambioact = button_speed_down.val_ant ^ button_speed_down.val_act;    
	button_effect.cambioact = button_effect.val_ant ^ button_effect.val_act;     

  //If both status changes are equal to 1
  if (button_speed_up.cambioact == 1 && button_speed_up.cambioanterior == 1)  
  {
    button_speed_up.orden = 1;                                                //Order to 1
    
    button_speed_up.val_ant = button_speed_up.val_act;                        //Last value equal to actual value
    button_speed_up.cambioanterior = 0;                                       // Last status change equal to 0
    return;                                         
  }

  if (button_speed_down.cambioact == 1 && button_speed_down.cambioanterior == 1)  
  {
    button_speed_down.orden = 1;                                              //Order to 1
                   
    button_speed_down.val_ant = button_speed_down.val_act;                    //Last value equal to actual value                                                    
    button_speed_down.cambioanterior = 0;                                     // Last status change equal to 0               
    return;                                         
  }

  if (button_effect.cambioact == 1 && button_effect.cambioanterior == 1)  
  {
    button_effect.orden = 1;                                                //Order to 1        
                                                          
    button_effect.val_ant = button_effect.val_act;                          //Last value equal to actual value                                                           
    button_effect.cambioanterior = 0;                                       // Last status change equal to 0                       
    return;                                         
  }
  
  //Last status change is equal to acutal change
  button_speed_up.cambioanterior = button_speed_up.cambioact;  
  button_speed_down.cambioanterior = button_speed_down.cambioact;
	button_effect.cambioanterior = button_effect.cambioact;
}