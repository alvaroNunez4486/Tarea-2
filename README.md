Theremin Digital sobre Arduino UNO

Este proyecto consiste en el diseño, construcción y validación de un Theremin Digital
desarrollado sobre la plataforma Arduino UNO. El instrumento mide la distancia entre la
mano del usuario y un sensor ultrasónico para traducir esa distancia en el tono emitido por
un zumbador. Asimismo, permite seleccionar tres registros de frecuencia (Grave, Medio y
Agudo) mediante pulsadores con filtrado antirrebote por software no bloqueante.


Explicación Detallada de la Lógica del Programa

El sistema opera bajo una arquitectura no bloqueante basada en un ciclo de supervisión continuo, dividiéndose en cuatro módulos funcionales principales:

1. Temporización No Bloqueante con millis()
En lugar de congelar la ejecución del microcontrolador con la función delay(), el programa
utiliza millis() para registrar el tiempo transcurrido desde que se encendió el Arduino.
Esto permite ejecutar las mediciones del sensor ultrasónico cada 50 ms y verificar los
pulsadores de forma ininterrumpida.

2. Lectura de Pulsadores con Antirrebote
Los tres botones de selección de rango (Grave, Medio y Agudo) están configurados con la
resistencia interna INPUT_PULLUP. Dado que las láminas metálicas internas de los pulsadores
generan rebotes mecánicos durante unos microsegundos al presionarse, la función
leerBotonConDebounce() verifica la marca de tiempo ultimoTiempoRebote. Solo cuando el
estado bajo se mantiene durante más de 50 ms, se valida el cambio de rango tonal y se
actualiza la variable rangoActual.

3. Medición Ultrasónica y Filtro de Promedio Móvil
La distancia se calcula activando el pin TRIG del sensor HC-SR04 con un pulso de 10 us. El
pin ECHO mide el tiempo de retorno del ultrasonido en microsegundos.
Para mitigar el ruido y la inestabilidad de la señal, la medición bruta pasa por un filtro
de promedio móvil de 5 muestras. Se mantiene un arreglo circular con los últimos 5 valores
obtenidos y se calcula su media aritmética antes de generar el sonido.

4. Mapeo Tonal e Inversión de Frecuencia
La distancia filtrada se restringe dentro de la zona útil (5 a 30 cm).
Si la mano se encuentra a menos de 5 cm o a más de 30 cm, la función noTone() apaga inmediatamente el zumbador.
Mediante las funciones map() y constrain(), el rango de distancia se convierte a frecuencia en Hertz de forma inversamente proporcional:
5 cm (mano muy cerca) frecuencia máxima del rango activo (nota más agudo).
30 cm (mano alejada) frecuencia mínima del rango activo (nota más grave).
