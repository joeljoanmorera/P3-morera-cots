#include <Arduino.h>

//Button structures definition
struct Button 
{
  uint8_t pin;                                              //Pin del boton
  bool val_act, val_ant, cambioact, cambioanterior, orden; 
	
  Button(uint8_t PPIN)
  {
    pin = PPIN;
    val_ant = 0;
    cambioanterior = 0;
  }
};

//LED structure definition
struct LED 
{
  const int pin;
  byte state;
};

//Button structs definition
Button button_speed_up(25);
Button button_speed_down(32);
Button button_effect(33);

//LED structs definition
LED led_1 = {12, 0b00000001};
LED led_2 = {13, 0b00000010};
LED led_3 = {14, 0b00000100};
LED led_4 = {27, 0b00001000};
LED led_5 = {16, 0b00010000};
LED led_6 = {17, 0b00100000};
LED led_7 = {18, 0b01000000};
LED led_8 = {19, 0b10000000};

//Global variables
hw_timer_t * timer = NULL;
int conti = 100, cont = 100;
int effectOrder = 1;

//Effects
byte  effect_1 = 0b00000001,
      effect_2 = 0b00010001,
      effect_3 = 0b01010101,
      effect_4 = 0b00000101,
      masc = effect_1,
      masc_ini = masc;

//Function declaration
void mascType();
void IRAM_ATTR buttonManagement();
void lightsEffects();

void setup()
{
  //Baud rate
  Serial.begin(115200);

  //LEDs
  pinMode(led_1.pin, OUTPUT);
  pinMode(led_2.pin, OUTPUT);
  pinMode(led_3.pin, OUTPUT);
  pinMode(led_4.pin, OUTPUT);
  pinMode(led_5.pin, OUTPUT);
  pinMode(led_6.pin, OUTPUT);
  pinMode(led_7.pin, OUTPUT);
  pinMode(led_8.pin, OUTPUT);

  //Buttons
  pinMode(button_effect.pin,INPUT_PULLUP);
  pinMode(button_speed_down.pin,INPUT_PULLUP);
  pinMode(button_speed_up.pin,INPUT_PULLUP);

  //Timer
  timer = timerBegin(0, 80, true);                            //Iniciamos el timer
  timerAttachInterrupt(timer, &buttonManagement, true);       //Relacionamos el timer a la funcion
  timerAlarmWrite(timer, 1000, true);                        //Especificar el valor del contador en el que se genera la interrupcion
  timerAlarmEnable(timer);
}

//Codigo que se ejeucta multiples veces
void loop() 
{
  if (button_effect.orden)
  {
    effectOrder += 1;
    if (effectOrder > 4) effectOrder = 1;
    mascType();
    Serial.print("Cambio estado a: ");
    Serial.println(effectOrder);
    button_effect.orden = 0;
  }

  if (button_speed_up.orden)
  {
    conti -= 25;
    if(conti == 0)conti = 25;
    Serial.print("Velocidad asciende a: ");
    Serial.println(conti);
    button_speed_up.orden = 0;
  } 

  if (button_speed_down.orden)
  {
    conti += 25;
    Serial.print("Velocidad desciende a: ");
    Serial.println(conti);
    button_speed_down.orden = 0;
  } 

  delay(500);
}

void lightsEffects()
{
  if(cont-- <= 0)
  {
    if(masc == 0)masc = masc_ini;

    digitalWrite(led_1.pin,masc & led_1.state);
    digitalWrite(led_2.pin,masc & led_2.state);
    digitalWrite(led_3.pin,masc & led_3.state);
    digitalWrite(led_4.pin,masc & led_4.state);
    digitalWrite(led_5.pin,masc & led_5.state);
    digitalWrite(led_6.pin,masc & led_6.state);
    digitalWrite(led_7.pin,masc & led_7.state);
    digitalWrite(led_8.pin,masc & led_8.state);

    masc *= 2;
    cont = conti;
  }
  
}

void mascType()
{
  switch(effectOrder)
  {
    case 1:
      masc = effect_1;
      masc_ini = effect_1;
      break;
    case 2:
      masc = effect_2;
      masc_ini = effect_2;
      break;
    case 3:
      masc = effect_3;
      masc_ini = effect_3;
      break;
    case 4:
      masc = effect_4;
      masc_ini = effect_4;
      break;
  }
}

void IRAM_ATTR buttonManagement()
{
  lightsEffects();
      
  button_speed_up.val_act = digitalRead(button_speed_up.pin); 
	button_speed_down.val_act = digitalRead(button_speed_down.pin);
	button_effect.val_act = digitalRead(button_effect.pin); 

	button_speed_up.cambioact = button_speed_up.val_ant ^ button_speed_up.val_act;    
	button_speed_down.cambioact = button_speed_down.val_ant ^ button_speed_down.val_act;    
	button_effect.cambioact = button_effect.val_ant ^ button_effect.val_act;     

  if (button_speed_up.cambioact == 1 && button_speed_up.cambioanterior == 1)  
  {
    button_speed_up.orden = 1;                  
    
    button_speed_up.val_ant = button_speed_up.val_act;                                                           
    button_speed_up.cambioanterior = 0;                            
    return;                                         
  }

  if (button_speed_down.cambioact == 1 && button_speed_down.cambioanterior == 1)  
  {
    button_speed_down.orden = 1;                                      
                   
    button_speed_down.val_ant = button_speed_down.val_act;                                                           
    button_speed_down.cambioanterior = 0;                            
    return;                                         
  }

  if (button_effect.cambioact == 1 && button_effect.cambioanterior == 1)  
  {
    button_effect.orden = 1;                                      
                                                          
    button_effect.val_ant = button_effect.val_act;                                                           
    button_effect.cambioanterior = 0;                            
    return;                                         
  }

  button_speed_up.cambioanterior = button_speed_up.cambioact;  
  button_speed_down.cambioanterior = button_speed_down.cambioact;
	button_effect.cambioanterior = button_effect.cambioact;
}