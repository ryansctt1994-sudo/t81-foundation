# Capítulo 2: Principios Centrales e Invariantes

## 2.1 El Invariante de Determinismo

**Estado: Implementado y Probado**

El axioma central de la arquitectura T81 es el **Determinismo Estricto**. Un programa T81 es una función pura $f(S, I) \to S'$, donde $S$ es el estado inicial e $I$ es la entrada. Esta función debe producir $S'$ idéntico bit a bit en cualquier plataforma de hardware compatible.

Lograr esto requiere eliminar todas las fuentes de no determinismo comunes en la computación moderna:
*   **Punto Flotante de Hardware**: Reemplazado por `T81Float` (`dmath`) definido por software.
*   **Diseño de Memoria**: Las direcciones lógicas están desacopladas de los punteros físicos.
*   **Concurrencia**: La programación de hilos se reemplaza por corrutinas deterministas y tics lógicos.
*   **Tiempo del Sistema**: El tiempo del reloj de pared se reemplaza por marcas de tiempo de Lamport (tics lógicos).

### 2.1.1 Superficies de Determinismo y Vectores de Ataque

La siguiente tabla mapea las "superficies" donde el no determinismo puede filtrarse en el sistema y las mitigaciones específicas que emplea T81.

| Capa     | Riesgo de Determinismo       | Mitigación                | Evidencia               |
| -------- | ---------------------------- | ------------------------- | ----------------------- |
| Compilador | Orden de tokens              | Emisión canónica de AST   | `scripts/ci/t81lang_repro_gate.py` |
| VM       | Fuga de direcciones de memoria | Sin observabilidad de direcciones | `src/vm/vm.cpp` (Segmentos de Memoria) |
| GC       | Recolección no determinista  | Disparadores por conteo de asignaciones | `src/vm/vm.cpp`: `run_gc_cycle_` |
| Float    | Deriva de FPU del host (IEEE-754) | `dmath` float por software | `include/t81/core/T81Float.hpp` |
| JIT      | Divergencia de optimización  | Equivalencia basada en trazas | `src/vm/jit_compiler.cpp` |

> **Verificación**: El compilador JIT en `src/vm/jit_compiler.cpp` asegura que las trazas optimizadas salgan (`GuardDeopt`) ante *cualquier* divergencia de estado con respecto a la línea base interpretada.

## 2.2 Lógica Ternaria (Base-3)

**Estado: Implementado y Probado**

T81 es un sistema **ternario balanceado**. La unidad fundamental es el **trit**, con valores $\{-1, 0, 1\}$ (a menudo denotados como $-, 0, +$).

### 2.2.1 ¿Por qué Ternario?
1.  **Aritmética Simétrica**: El redondeo es simplemente un truncamiento hacia el entero más cercano, ya que $0.5$ no es una fracción representable en base-3 sin expansión infinita. Esto simplifica la biblioteca `dmath`.
2.  **Densidad de Información**: La base $3$ está más cerca de $e \approx 2.718$ que $2$, ofreciendo el óptimo teórico para la economía de base entera ($\text{base} \times \text{ancho}$).
3.  **Representación con Signo**: Los números negativos no requieren un bit de signo o Complemento a Dos. El trit principal indica el signo naturalmente.

### 2.2.2 Implementación
En el código base de C++, los trits están empaquetados para eficiencia pero son lógicamente distintos.
*   **Almacenamiento**: `T81Int` usa 2 bits por trit en forma empaquetada (ver `include/t81/packing.hpp`).
*   **Aritmética**: Operaciones como `Add`, `Mul` se implementan en `src/vm/vm.cpp` usando matemáticas enteras que simulan cadenas de acarreo ternario balanceado.

## 2.3 Auditabilidad y la Traza Axion

**Estado: Implementado y Probado**

Cada transición de estado en T81 es auditable. El **Kernel Axion** produce un registro criptográfico de ejecución llamado **Traza**.

### 2.3.1 La Estructura de la Traza
Una traza es una secuencia de registros `AxionEvent`, capturando opcodes, veredictos y datos asociados.

> **Referencia**: Ver `include/t81/axion/api.hpp` para las definiciones de eventos.

Esta traza sirve como una **Prueba de Ejecución**. Al reproducir la traza contra el estado inicial, un auditor puede verificar que:
1.  La computación ocurrió como se afirmó.
2.  No se violaron políticas de seguridad.
3.  El resultado final es correcto.

## 2.4 Los Nueve Principios (Cumplimiento Ético)

**Estado: Implementado y Probado**

T81 integra una capa ética inmutable (Los Nueve Principios $\Theta_1 \dots \Theta_9$) directamente en el motor de políticas de la VM. Estas no son pautas sino **restricciones en tiempo de ejecución**.

Por ejemplo:
*   **$\Theta_7$ (Contención de Entropía)**: Previene la expansión infinita de recursos sin permiso explícito `InfExpand`.
*   **$\Theta_4$ (Interpretabilidad)**: Ordena que los tensores opacos de "caja negra" no pueden emitirse sin metadatos adjuntos o gráficos simbólicos.

> **Implementación**: Estas comprobaciones se realizan en `src/axion/ethics.cpp`. Una violación resulta en un `VerdictKind::Deny` y un `Trap::SecurityFault` inmediato.

## 2.5 Lista de Verificación

*   [ ] **Consistencia de Float**: ¿`T81Float` produce patrones de bits idénticos para funciones trascendentales (`sin`, `exp`) en todas las plataformas? (Ejecutar `tests/cpp/test_T81Float.cpp` y `tests/cpp/test_property_float.cpp`)
*   [ ] **Determinismo del GC**: ¿El Recolector de Basura se ejecuta en recuentos exactos de instrucciones (asignaciones), no en tiempo de pared? (Verificar `kGcInterval` en `src/vm/vm.cpp`)
*   [ ] **Integridad de la Traza**: ¿Es inmutable el registro de Axion durante la ejecución? (Verificado por `tests/cpp/axion_log_determinism_test.cpp`)

## 2.6 Matriz de Auditoría Formal

| Principio | Sección de Especificación | Implementación | Cobertura de Pruebas |
| :--- | :--- | :--- | :--- |
| Determinismo Estricto | `spec/determinism-profile.md` | `src/vm/vm.cpp` | `tests/cpp/test_property_invariants.cpp` |
| Lógica Ternaria | `spec/t81-data-types.md` | `include/t81/ternary.hpp` | `tests/cpp/ternary_arith_test.cpp` |
| Auditabilidad | `spec/axion-kernel.md` | `include/t81/axion/api.hpp` | `tests/cpp/test_ethics.cpp` |
