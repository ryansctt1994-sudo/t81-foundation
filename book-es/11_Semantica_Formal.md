# Capítulo 11: Semántica Formal

## 11.1 Semántica Operacional

**Estado: Especificación**

La semántica operacional de T81 se define como un sistema de transición de estados.

$$ S' = \text{VM}(S, \text{Op}) $$

Donde $S$ es el estado de la máquina $(R, PC, SP, M_{seg}, \Phi)$.

### 11.1.1 La Función de Transición
La función de transición es determinista. Para cualquier estado $S$ y opcode $\text{Op}$, existe exactamente un siguiente estado válido $S'$ o una condición de fallo $\bot$.

Esta propiedad es verificada por la **Puerta de Determinismo** (`scripts/ci/t81lang_repro_gate.py`), que asegura que el intérprete produzca transiciones idénticas en todas las plataformas.

## 11.2 Semántica de Memoria

**Estado: Implementado**

La memoria en T81 no es un arreglo lineal de bytes sino un almacén estructurado de **Objetos Tipados**.
*   **Seguridad**: Acceder a un elemento de tensor fuera de los límites no es comportamiento indefinido; es un `TypeFault` o `StackFault` garantizado.
*   **Inmutabilidad**: Una vez que un tensor se compromete en CanonFS, es inmutable.

> **Verificación**: `tests/cpp/vm_bounds_test.cpp` asegura que todos los accesos fuera de límites se atrapen correctamente.
