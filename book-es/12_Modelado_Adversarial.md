# Capítulo 12: Modelado Adversarial

## 12.1 Modelo de Amenazas

**Estado: Activo**

T81 asume un entorno adversarial donde el hardware del host, el sistema operativo y los pares de red pueden ser maliciosos o defectuosos.

### 12.1.1 El Vector "Libm Gap"
Existe un vector de ataque sutil donde un nodo malicioso explota las diferencias en la biblioteca matemática estándar (`libm`) del host.
*   **Ataque**: El Nodo A (x86) calcula `sin(x)` ligeramente diferente al Nodo B (ARM).
*   **Consecuencia**: La divergencia de estado conduce a una bifurcación de consenso.
*   **Mitigación**: T81 impone el uso de `dmath` para todas las funciones trascendentales críticas, asegurando resultados exactos a nivel de bit independientemente de la implementación subyacente de `libm`.

> **Verificación**: `tests/cpp/test_property_float.cpp` verifica la consistencia multiplataforma de las operaciones `T81Float`.

### 12.1.2 Ataques de Viaje en el Tiempo
En un sistema distribuido, un par malicioso podría inyectar mensajes con marcas de tiempo futuras para manipular el reloj lógico.
*   **Mitigación**: El protocolo `TickSync` impone actualizaciones de reloj monotónicas. Un mensaje con una marca de tiempo muy en el futuro puede ser rechazado o limitado por política.
*   **Verificación**: `tests/cpp/tier4_vm_test.cpp` prueba la lógica de sincronización del reloj.

## 12.2 Resiliencia a Canales Laterales

**Estado: Aspiracional / Parcial**

Si bien T81 garantiza el determinismo lógico, actualmente no garantiza la ejecución en tiempo constante para todas las operaciones. Pueden existir canales laterales de temporización en la implementación actual de la multiplicación `BigInt` y las funciones `dmath`.
