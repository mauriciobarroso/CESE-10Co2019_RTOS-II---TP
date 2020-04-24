# Miembros del grupo
- Pablo Castillo
- Lorena Zacharewicz
- Mauricio Barroso

# Justificaciones
## Arquitecturas utilizadas
- Patrón de capas, utilizado para estructurar la aplicación y descomponerlo en tareas de menor complejidad.

![1](https://miro.medium.com/max/188/1*jMWk_JqqyyloVPhTs_Zd1A.png)

- Patrón de igual a igual, establece los componente individuales como pares, estos pueden funcionar tanto como cliente y como servidor.

![2](https://miro.medium.com/max/260/1*ROvkckSTw1UncrbQSmUJUQ.png)

## Elección del esquema de memoria dinámica utilizada
El proyecto utiliza la función ``pvPortMalloc()`` de FreeRTOS combinada con los algoritmos de memory pool de QuantumLeaps para garantinzar una correcta administración de la memoria dinámica reservada para los mensajes entrantes y salientes de la UART.
 
La implmentación de alocación de memoria proveída por FreeRTOS es heap_4, esto debido a sus características:
- Utiliza un algoritmo de primera opción que garantiza que pvPortMalloc() utilice el primer bloque de memoria que satisfaga las necesidades de tamaño requerido.
- Combina bloques de memoria adyacente para formar un único bloque mas grande, minimizando el riesgo de fragmentación de la memoria.
- Es adecuado para aplicaciones que asignan y liberan repetidamente bloques de memoria de RAM.

Además, la utilización de memory pool en el proyecto se debe a los siguientes beneficios que presenta sobre métodos como malloc:

-  Las agrupaciones de memoria permiten la asignación de memoria con tiempo de ejecución constante. La liberación de memoria para miles de objetos en un grupo es solo una operación, no una por una si se usa malloc para asignar memoria a cada objeto.
- Las agrupaciones de memoria se pueden agrupar en estructuras de árbol jerárquicas, lo que es adecuado para estructuras de programación especiales como bucles y recursiones.
- Las agrupaciones de memoria de bloque de tamaño fijo no necesitan almacenar metadatos de asignación para cada asignación, describiendo características como el tamaño del bloque asignado. Particularmente para asignaciones pequeñas, esto proporciona ahorros sustanciales de espacio.
- Permite un comportamiento determinista en sistemas en tiempo real evitando errores de falta de memoria.

# Requerimientos

Del TP:
- [x] R_TP-1
- [x] R_TP-2
- [x] R_TP-3
- [x] R_TP-4

Generales:
- [x] R_TP-1

Capa de separación de frames(C2):
- [x] R_C2-1
- [x] R_C2-2
- [x] R_C2-3
- [x] R_C2-4
- [x] R_C2-5
- [x] R_C2-6
- [x] R_C2-7
- [x] R_C2-8
- [x] R_C2-9
- [x] R_C2-10
- [x] R_C2-11
- [x] R_C2-12

Capa de aplicación(C3):
- [x] R_C3-1
- [x] R_C3-2
- [x] R_C3-3
- [x] R_C3-4
- [x] R_C3-5
- [x] R_C3-6
- [x] R_C3-7
- [x] R_C3-8

Opcionales:
- [ ] R_OP-1

# FUNCIONAMIENTO

![3](https://lh3.googleusercontent.com/SMNEJiUgooyBdZ9ayLZl48LWpvCFw0M3HAfhTAlNZ_5PEhT2vLnr0o5z3_hD3jTzWLb4fOOIyUA4oyyZRUGedw_3TI1cm80GELXqfU7VuUTDetQt3bJz6oygxZNp14e-XiCtdPqG0xftQtBy-vL2zTCEau9rTcb5-hD9aojc1cgzbDQrqEHdA6b_y1aVK49CIK_GLB30Ki2AmSz7QTMZ528ScETWmh9VPIfRckRJcAdRAQxvWqscJGK5_9Gcve-QQdukrHyooLU9l_7P0TNOajQcD6NrWuoOJkian8vSBuGlXG_RTAH5YVzW2wrBMA33F3o4pRHDdvyuMprLDBI3d0G6rsEaeuSKMI_Qkjp9FV5sS6K6aF-Ejx9WoSh5448viMhnhnldu0BXlbbpIV7d8GDrWnqyiNS1ZjAlfcKqrAg9tni4ojs2EnPwdSXaBU1R9Hp_M_fqfzO5sUXcCc36axB5dKOAeCmXy2JzBrI1PQVKvTUNp21OgRKc1yOCAL1cLsrQoHSKz7Y4hbTEyFLXgeK7Tq6_tK2bwKiyrzzli_SoeKSX4vMVTnsPYzOv7ruYbQyAIBRzkxv1nNNlUSe503l1D3uJi7XyLuEtDOO02KZb44vgn3W3D1zrq_PJsxe8UtGmomtlv0VA-r0HOg2sn7GfFu0Y9IfAQOSn-_2nHZ6E1d2svXvZqDSUmiM=w889-h333-no)

1. C1 recibe mediante interrupción los caracteres provenientes de la UART seleccionada y los almacena en un bloque de memoria dinámica, para comunicar a la siguiente capa de la existencia de un paquete se utiliza un timer que envía en los parámetros de la función de callback la dirección del bloque de memoria y el tamaño del paquete.

2. C2 ejecuta la función de callback del timer y obtiene el paquete proveniente de C1, entonces verifica que este contiene los caracteres de inicio y fin, despues verifica que el CRC es correcto. Una vez concluidas las verificaciones, el paquete se reduce al mensaje y su operación, inmediatamente es enviado a la cola de recepción de C3.

3. C3 recibe por la cola de recepción el paquete verificado y descompuesto por C2, entonces realiza una última verificación para detectar caracteres que no sean alfabéticos. Ejecutadas todas las verificaciones se construye el evento que debe ser enviado al Objeto Activo, que es el encargado de ejecutar la operación sobre el paquete.

4. El Objeto Activo es creado en función del evento que detectado y de todos los Objetos Activos registrados, si alguno de los Objetos Activos registrados contiene como evento el evento construído anteriormente entonces, se crea el thread y la cola de recepción de este.

5. Una vez que el Objeto Activo finaliza la operación para la cual existe, envía el paquete a la cola de recepción de respuesta del Objeto Activo de C3, luego se destruye eliminando su cola y su thread.

6. C3 recibe por la cola de recepción de respuesta del Objeto Activo el paquete procesado y los envía a su cola de transmisión para que este pueda ser enviado por la UART.

7. C2 recibe por la cola de transmisión de C3 el paquete que debe ser enviado, calcula el CRC del paquete y lo añade junto con los caracteres de inicio y fin correspondientes. Después habilita la interrupción por transmisor libre de la UART y envía todo el contenido del paquete.

8. C2 también ejecuta un timer cuya función de callback envías los caracteres de retorno de carro y fin de línea, además finaliza liberando el bloque de memoria utilizado para alojar el paquete que fue transmitido.   

# NOTAS
1. ~~La función `uartCallbackSet()` de la SAPI no funciona correctamente y no envía los parámetros de callback a la función de callback. Para probar que el sistema funcione correctamente los parámetros de la estructura `UartInstance_t`son declarados como globales en uartDriver.h y utilizados en las demás librerías con el prefijo `extern`.~~ Problema corregido
2. ~~Para la parte 2 del TP se escribieron 2 posibles implementaciones, una se ecuentra en la rama "master" y la otra en la rama "alternative".~~