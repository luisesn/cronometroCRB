#include <TimerOne.h>
#include <Wire.h>
#include <MultiFuncShield.h>

volatile int LDR;
volatile unsigned int clockMilliSeconds = 0;
volatile byte clockCentiSeconds = 0;
volatile byte clockSeconds = 0;
volatile bool coche = false;
volatile int pasada = 0;

static byte LDR_MIN = 180;
static byte numeroVueltas = 4;
volatile int vuelta[15];
volatile int total = 0;

boolean isr = false;

void setup()
{
  Timer1.initialize(1000); // Inicializamos Timer1 a mil microsegundos -> 1 milisegundo
  MFS.userInterrupt = clockISR;
  MFS.initialize(&Timer1);
  MFS.blinkDisplay(DIGIT_ALL);
  MFS.beep(10);
  Serial.begin(9600);
}

void loop()
{
  displayTime(clockSeconds, clockCentiSeconds);
  if (isr) ejecutar_isr();
}

void ejecutar_isr() {
  LDR = analogRead(A5);
  if (LDR < LDR_MIN) { // Haz cortado
    if (coche) { // Ya ha arrancado la carrera
      vuelta[pasada] = clockSeconds * 100 + clockCentiSeconds;
      clockMilliSeconds = 0;
      clockCentiSeconds = 0;
      clockSeconds = 0;

      MFS.beep(10);
      char time[5];
      sprintf(time, "%04d", pasada);
      MFS.write(time, 1);
      delay(500);
      pasada++;
      if (pasada == numeroVueltas) { // SE ACABO
        total=0;
        for (int i = 0; i < numeroVueltas; i++) total += vuelta[i];
        sprintf(time, "%04d", total);
        while (1) {
          MFS.write("Fin");
          delay(1000);
          MFS.write(time, 1);
          delay(1000);
        }
      }
    } else {
      MFS.blinkDisplay(DIGIT_ALL,0);
      coche = true; // Arrancamos la carrera!
      pasada = 0;
    }
  } else {
    if (coche) { // Ya ha arrancado la carrera
      //MFS.beep(20, 2, 8);
    } else {
      clockMilliSeconds = 0;
      clockCentiSeconds = 0;
      clockSeconds = 0;
    }
  }

  isr = false;
}

//---------------------------------------------------------
void displayTime (byte seconds, byte centiseconds)
{
  char time[5];
  sprintf(time, "%04d", (seconds * 100) + centiseconds);
  MFS.write(time, 1);
}

//--------------------------------------------------------------------------------------------------
void clockISR ()
{
  isr = true;
  if (coche == true) // Si hay coche sumamos un milisegundo al reloj
  {
    clockMilliSeconds++;
    if (clockMilliSeconds >= 10)
    {
      clockMilliSeconds = 0;
      clockCentiSeconds++;

      if (clockCentiSeconds >= 100)
      {
        clockCentiSeconds = 0;
        clockSeconds++;
        Serial.println(coche);
        Serial.println(LDR);

        if (clockSeconds >= 100)
        {
          clockCentiSeconds = 0;
          clockCentiSeconds = 0;
          clockSeconds = 0;
        }
      }
    }
  }
}