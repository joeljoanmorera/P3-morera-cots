#include <Arduino.h>

//Button structures definition
struct Button 
{
  uint8_t pin;                         //Pin del boton
  bool val_act, val_ant, cambioact, cambioanterior, orden; 
	
  Button(uint8_t PPIN)
  {
    pin = PPIN;
  }                              //Variable de pulsado
};

//Button structs definition
Button button_velo_up(33);
Button button_velo_down(32);
Button button_rutina(34);

//LED structure definition
struct led 
{
  const int pin;
  byte state;
};

//LED structs definition
led led_1 = {12, 0b00000001};
led led_2 = {13, 0b00000010};
led led_3 = {14, 0b00000100};
led led_4 = {27, 0b00001000};
led led_5 = {16, 0b00010000};
led led_6 = {17, 0b00100000};
led led_7 = {18, 0b01000000};
led led_8 = {19, 0b10000000};

//Global variables
  //Timer 1 variables
hw_timer_t * timer = NULL;
portMUX_TYPE timerMux = portMUX_INITIALIZER_UNLOCKED;
volatile int interruptCounter;
  //Timer 2 variables
hw_timer_t * timer_led = NULL;
portMUX_TYPE timerMux_leds= portMUX_INITIALIZER_UNLOCKED;
volatile int interruptCounterForLed;

//
int conti = 100, cont = 100;
int delay_v = 200;
int effectOrder = 1;

//Effects
byte effect_1 = 0b00000001;
byte effect_2 = 0b00010001;
byte effect_3 = 0b01010101;
byte effect_4 = 0b00000101;
byte masc = effect_1, masc_ini = masc;;

//Function declaration
void mascType();
void IRAM_ATTR buttonManagement();
void lightsEffects();

void setup()
{
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
  pinMode(button_rutina.pin,INPUT_PULLUP);
  pinMode(button_velo_down.pin,INPUT_PULLUP);
  pinMode(button_velo_up.pin,INPUT_PULLUP);

  //TIMER Buttons
  timer = timerBegin(0, 80, true);                            //Iniciamos el timer
  timerAttachInterrupt(timer, &buttonManagement, true);       //Relacionamos el timer a la funcion
  timerAlarmWrite(timer, 10000, true);                        //Especificar el valor del contador en el que se genera la interrupcion
  timerAlarmEnable(timer);
  
  /*
  //Timer LEDS
  timer_led = timerBegin(1, 80, true);                            //Iniciamos el timer
  timerAttachInterrupt(timer_led, &lightsEffects, true);       //Relacionamos el timer a la funcion
  timerAlarmWrite(timer_led, 100000, true);                        //Especificar el valor del contador en el que se genera la interrupcion
  timerAlarmEnable(timer_led);*/
  
}

//Codigo que se ejeucta multiples veces
void loop() 
{
  //if (interruptCounter > 0)
  //{
    Serial.println("Bucle principal");
    //Critical section
    //portENTER_CRITICAL(&timerMux);
    //interruptCounter--;
    //portEXIT_CRITICAL(&timerMux);

    if (button_rutina.orden)
    {
      effectOrder += effectOrder;
      if (effectOrder > 4) effectOrder = 1;
      mascType();
      Serial.println("Cambio estado 200!");
    }
      if (button_velo_up.orden)
    {
      //delay_v=delay_v/2;
      conti -= 100;
      if(conti == 0)conti = 100;
    } 
    if (button_velo_down.orden)
    {
      //delay_v=delay_v*2;
      conti += 100;
    } 
    delay(500);
    
    Serial.println(conti);
  //}
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

  portENTER_CRITICAL_ISR(&timerMux); 
	interruptCounter++;             
  portEXIT_CRITICAL_ISR(&timerMux);
      
  button_velo_up.val_act = digitalRead(button_velo_up.pin); 
	button_velo_down.val_act = digitalRead(button_velo_down.pin);
	button_rutina.val_act = digitalRead(button_rutina.pin); 

	button_velo_up.cambioact = button_velo_up.val_ant ^ button_velo_up.val_act;    
	button_velo_down.cambioact = button_velo_down.val_ant ^ button_velo_down.val_act;    
	button_rutina.cambioact = button_rutina.val_ant ^ button_rutina.val_act;     

  if (button_velo_up.cambioact == 1 && button_velo_up.cambioanterior == 1)  
  {
    button_velo_up.orden = 1;                  
    
    Serial.println("Boton up speed!");
    button_velo_up.val_ant = button_velo_up.val_act;                                                           
    button_velo_up.cambioanterior = 0;                            
    return;                                         
  }

  if (button_velo_down.cambioact == 1 && button_velo_down.cambioanterior == 1)  
  {
    button_velo_down.orden = 1;                                      
    
    Serial.println("Boton down speed!");                 
    button_velo_down.val_ant = button_velo_down.val_act;                                                           
    button_velo_down.cambioanterior = 0;                            
    return;                                         
  }

  if (button_rutina.cambioact == 1 && button_rutina.cambioanterior == 1)  
  {
    button_rutina.orden = 1;                                      
               
    Serial.println("Boton cambio estado!");                                             
    button_rutina.val_ant = button_rutina.val_act;                                                           
    button_rutina.cambioanterior = 0;                            
    return;                                         
  }

  button_velo_up.cambioanterior = button_velo_up.cambioact;  
  button_velo_down.cambioanterior = button_velo_down.cambioact;
	button_rutina.cambioanterior = button_rutina.cambioact;
}