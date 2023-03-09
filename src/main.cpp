#include <Arduino.h>


SemaphoreHandle_t mutex;
int totalInterruptCounter;
hw_timer_t * timer = NULL;
portMUX_TYPE timerMux = portMUX_INITIALIZER_UNLOCKED;
int delay_v=200;
int effectOrder=1;
volatile int interruptCounter;



struct Button 
{
    uint8_t PIN;                         //Pin del boton
    bool val_act, val_ant, cambioact, cambioanterior, orden; 
	
Button(uint8_t PPIN)
{
	PIN = PPIN;
}                              //Variable de pulsado
};

Button button_velo_up(33);
Button button_velo_down(32);
Button button_rutina(34);

void IRAM_ATTR onTimer()
{
    portENTER_CRITICAL_ISR(&timerMux); 
	interruptCounter++;             
    portEXIT_CRITICAL_ISR(&timerMux);
    
    
    button_velo_up.val_act = digitalRead(button_velo_up.PIN); 
	button_velo_down.val_act = digitalRead(button_velo_down.PIN);
	button_rutina.val_act = digitalRead(button_rutina.PIN); 


	button_velo_up.cambioact = button_velo_up.val_ant ^ button_velo_up.val_act;    
	button_velo_down.cambioact = button_velo_down.val_ant ^ button_velo_down.val_act;    
	button_rutina.cambioact = button_rutina.val_ant ^ button_rutina.val_act;     

    if (button_velo_up.cambioact == 1 && button_velo_up.cambioanterior == 1)  
    {
      button_velo_up.orden = 1;                                      
                         
                                                      
      button_velo_up.val_ant = button_velo_up.val_act;                                                           
      button_velo_up.cambioanterior = 0;                            
      return;                                         
    }

	 if (button_velo_down.cambioact == 1 && button_velo_down.cambioanterior == 1)  
    {
      button_velo_down.orden = 1;                                      
                         
                                                      
      button_velo_down.val_ant = button_velo_down.val_act;                                                           
      button_velo_down.cambioanterior = 0;                            
      return;                                         
    }

	 if (button_rutina.cambioact == 1 && button_rutina.cambioanterior == 1)  
    {
      button_rutina.orden = 1;                                      
                         
                                                      
      button_rutina.val_ant = button_rutina.val_act;                                                           
      button_rutina.cambioanterior = 0;                            
      return;                                         
    }


    button_velo_up.cambioanterior = button_velo_up.cambioact;  
    button_velo_down.cambioanterior = button_velo_down.cambioact;
	button_rutina.cambioanterior = button_rutina.cambioact;

}


void setup()
{
	Serial.begin(115200);                             //Velocidad de transmision
    timer = timerBegin(0, 80, true);                  //Iniciamos el timer
    timerAttachInterrupt(timer, &onTimer, true);      //Relacionamos el timer a la funcion
    timerAlarmWrite(timer, 10000, true);              //Especificar el valor del contador en el que se genera la interrupcion
    timerAlarmEnable(timer);
}

//Codigo que se ejeucta multiples veces
void loop() 
{

  if (interruptCounter > 0) {
 
    portENTER_CRITICAL(&timerMux);
    interruptCounter--;
    portEXIT_CRITICAL(&timerMux);
 
    totalInterruptCounter++;

	if (button_rutina.orden)
	{
	 effectOrder=effectOrder++;    
	}
    if (button_velo_up.orden)
	{
     delay_v=delay_v/2;
	} 
	if (button_velo_down.orden)
	{
     delay_v=delay_v*2;
	}
   
 
  }

  }



