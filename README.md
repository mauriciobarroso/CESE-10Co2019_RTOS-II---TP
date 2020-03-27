# Miembros del grupo
- Pablo Castillo
- Lorena Zacharewicz
- Mauricio Barroso
------------------------------------------
# Justificaciones
## Arquitecturas utilizadas
- Patrón de capas, utilizado para estructurar la aplicación y descomponerlo en tareas de menor complejidad.
<center>![1](https://miro.medium.com/max/188/1*jMWk_JqqyyloVPhTs_Zd1A.png)</center>
-
- Patrón de igual a igual, establece los componente individuales como pares, estos pueden funcionar tanto como cliente y como servidor.
<center>![2](https://miro.medium.com/max/260/1*ROvkckSTw1UncrbQSmUJUQ.png)</center>
-

## Elección del esquema de memoria dinámica utilizada
El proyecto utiliza un esquema de memoria dinámica basado en los métodos propuestos por FreeRTOS, ya que en todo momento en la aplicación los bloques de memoria solicitados serán del mismo tamaño y no serán solicitados con gran frecuencia. También mediante ```#define USE_QM``` puede hacerse uso de la librería de asignación dinámica de memoria RAM de QuantumLeaps.


Además, los mecanismos de alocación y liberación de memoria proveídos por FreeRTOS(pvPortMalloc() y vPortFree()) resultan suficientes para la aplicación desarrollada.

El método elegido es Heap_4 tomando en cuenta sus características y beneficios, estos son:
- Utiliza un algoritmo de primera opción que garantiza que pvPortMalloc() utilice el primer bloque de memoria que satisfaga las necesidades de tamaño requerido.
- Combina bloques de memoria adyacente para formar un único bloque mas grande, minimizando el riesgo de fragmentación de la memoria.
- Es adecuado para aplicaciones que asignan y liberan repetidamente bloques de memoria de RAM.
