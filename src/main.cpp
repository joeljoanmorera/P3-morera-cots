#include <Arduino.h>

//Creation of semaphore
SemaphoreHandle_t mutex;

//Other variables
int LED = 23;

//Code from the task that sets the LED on.
void TaskSetLedON (void *pvParameters)
{
	for(;;)
	{	
		xSemaphoreTake(mutex, portMAX_DELAY);
		digitalWrite(LED, HIGH);
		Serial.println("LED state: ON");	
		xSemaphoreGive(mutex);
		vTaskDelay(pdMS_TO_TICKS(2000));
	}
}

//Code from the task that sets the LED off.
void TaskSetLedOFF (void *pvParameters)
{
	for(;;)
	{		
		xSemaphoreTake(mutex, portMAX_DELAY);	
		digitalWrite(LED, LOW);
		Serial.println("LED state: OFF");
		xSemaphoreGive(mutex);	
		vTaskDelay(pdMS_TO_TICKS(2000));
	}
}

//Codigo que solo se ejecuta una vez
void setup()
{
	// Transmision rate
	Serial.begin(115200);

	//Set LED pin as OUTPUT
	pinMode(LED,OUTPUT);
	
	// Tasks creation
	xTaskCreate(TaskSetLedON, "LED_ON", 10000, NULL, 0, NULL);
	xTaskCreate(TaskSetLedOFF, "LED_OFF", 10000, NULL, 0, NULL);
	
	//Semaphore creation
	mutex = xSemaphoreCreateMutex();
	if(mutex == NULL)Serial.println("ERROR: Mutex cannot be created");
}

//Codigo que se ejeucta multiples veces
void loop() {}
