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


// FUNCIÓN: LECTURA DE BOTONES CON ANTIRREBOTE
void leerBotonConDebounce() {
  unsigned long tiempoActual = millis();

  // Si ha pasado el tiempo mínimo de estabilidad
  if (tiempoActual - ultimoTiempoRebote > DELAY_REBOTE) {
    if (digitalRead(PIN_BOTON_GRAVE) == LOW) {
      rangoActual = GRAVE;
      ultimoTiempoRebote = tiempoActual;
      Serial.println("Rango cambiado a: GRAVE");
    } 
    else if (digitalRead(PIN_BOTON_MEDIO) == LOW) {
      rangoActual = MEDIO;
      ultimoTiempoRebote = tiempoActual;
      Serial.println("Rango cambiado a: MEDIO");
    } 
    else if (digitalRead(PIN_BOTON_AGUDO) == LOW) {
      rangoActual = AGUDO;
      ultimoTiempoRebote = tiempoActual;
      Serial.println("Rango cambiado a: AGUDO");
    }
  }
}


// FUNCIÓN: MEDIR DISTANCIA (HC-SR04)
int medirDistancia() {
  digitalWrite(PIN_TRIG, LOW);
  delayMicroseconds(2);

  // Pulso de disparo de 10 us
  digitalWrite(PIN_TRIG, HIGH);
  delayMicroseconds(10);
  digitalWrite(PIN_TRIG, LOW);

  // Tiempo del eco en microsegundos (timeout de 30 ms)
  long duracion = pulseIn(PIN_ECHO, HIGH, 30000);

  if (duracion == 0) return 0; // Lectura inválida

  // Convertir tiempo a centímetros
  return duracion / 58.2;  // o bien, su equivalente mas extenso: return (duracion * 0.0343) / 2;
}


// FUNCIÓN: FILTRO DE PROMEDIO MÓVIL
int aplicarFiltro(int nuevaMedida) {
  lecturas[indiceMuestra] = nuevaMedida;
  indiceMuestra = (indiceMuestra + 1) % MUESTRAS;

  long suma = 0;
  for (int i = 0; i < MUESTRAS; i++) {
    suma += lecturas[i];
  }

  return suma / MUESTRAS;
}


// FUNCIÓN: MAPEO Y GENERACIÓN DE SONIDO
void mapearYEmitirSonido(int distanciaFiltrada) {
  // Si la mano no está en la zona útil, apagar sonido
  if (distanciaFiltrada < DIST_MIN_CM || distanciaFiltrada > DIST_MAX_CM) {
    noTone(PIN_BUZZER);
    return;
  }

  int frecMin = FREC_MEDIO_MIN;
  int frecMax = FREC_MEDIO_MAX;

  if (rangoActual == GRAVE) {
    frecMin = FREC_GRAVE_MIN;
    frecMax = FREC_GRAVE_MAX;
  } else if (rangoActual == AGUDO) {
    frecMin = FREC_AGUDO_MIN;
    frecMax = FREC_AGUDO_MAX;
  }

  // Mapeo inversamente proporcional (menor distancia -> tono más agudo)
  int frecuencia = map(distanciaFiltrada, DIST_MIN_CM, DIST_MAX_CM, frecMax, frecMin);
  frecuencia = constrain(frecuencia, frecMin, frecMax);

  tone(PIN_BUZZER, frecuencia);
}