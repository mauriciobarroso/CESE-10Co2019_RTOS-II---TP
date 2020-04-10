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
- [ ] R_C3-5
- [ ] R_C3-6
- [ ] R_C3-7
- [ ] R_C3-8

Opcionales:
- [ ] R_OP-1

# NOTAS
1. ~~La función `uartCallbackSet()` de la SAPI no funciona correctamente y no envía los parámetros de callback a la función de callback. Para probar que el sistema funcione correctamente los parámetros de la estructura `UartInstance_t`son declarados como globales en uartDriver.h y utilizados en las demás librerías con el prefijo `extern`.~~ Problema corregido
