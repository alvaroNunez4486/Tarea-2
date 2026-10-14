#include <Arduino.h>

// DEFINICIÓN DE PINES Y CONSTANTES
const uint8_t PIN_TRIG         = 2;  // Disparo del HC-SR04
const uint8_t PIN_ECHO         = 3;  // Eco del HC-SR04
const uint8_t PIN_BUZZER       = 4;  // Zumbador pasivo
const uint8_t PIN_BOTON_GRAVE  = 5;  // Pulsador Rango Grave
const uint8_t PIN_BOTON_MEDIO  = 6;  // Pulsador Rango Medio
const uint8_t PIN_BOTON_AGUDO  = 7;  // Pulsador Rango Agudo


// FRECUENCIAS POR RANGO (en Hz - Octavas musicales)
const int FREC_GRAVE_MIN = 131, FREC_GRAVE_MAX = 262; // Do3 a Do4
const int FREC_MEDIO_MIN = 262, FREC_MEDIO_MAX = 523; // Do4 a Do5
const int FREC_AGUDO_MIN = 523, FREC_AGUDO_MAX = 1047;// Do5 a Do6


// LÍMITES DE ZONA ÚTIL (en cm)
const int DIST_MIN_CM = 5;
const int DIST_MAX_CM = 30;


// ESTADOS Y RANGOS
enum RangoTonal { GRAVE, MEDIO, AGUDO };
RangoTonal rangoActual = MEDIO; // Rango inicial por defecto


// CONTROL DE TIEMPOS NO BLOQUEANTES (millis)
unsigned long ultimoTiempoUS = 0;
const unsigned long INTERVALO_US = 50; // Medir distancia cada 50 ms

unsigned long ultimoTiempoRebote = 0;
const unsigned long DELAY_REBOTE = 50; // Antirrebote de 50 ms


// FILTRO DE PROMEDIO MÓVIL
const int MUESTRAS = 5;
int lecturas[MUESTRAS] = {0, 0, 0, 0, 0};
int indiceMuestra = 0;


// SETUP: CONFIGURACIÓN INICIAL
void setup() {
  // Configuración de pines del sensor y zumbador
  pinMode(PIN_TRIG, OUTPUT);
  pinMode(PIN_ECHO, INPUT);
  pinMode(PIN_BUZZER, OUTPUT);

  // Configuración de pulsadores con INPUT_PULLUP interna
  pinMode(PIN_BOTON_GRAVE, INPUT_PULLUP);
  pinMode(PIN_BOTON_MEDIO, INPUT_PULLUP);
  pinMode(PIN_BOTON_AGUDO, INPUT_PULLUP);

  // Inicializar comunicación para monitoreo
  Serial.begin(9600);
  Serial.println("Theremin Digital Iniciado. Rango por defecto: MEDIO");
}