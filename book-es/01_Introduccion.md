# Capítulo 1: Introducción

## 1.1 Alcance y Definición

**Estado: Implementado y Probado**

El proyecto **Fundación T81** implementa una arquitectura de máquina virtual determinista y nativa en ternario, diseñada para la computación verificable. A diferencia de los entornos de ejecución de propósito general que priorizan el rendimiento o la abstracción del hardware, T81 prioriza la **reproducibilidad exacta a nivel de bit** y la **auditabilidad**.

El sistema se define por los siguientes invariantes centrales:
1.  **Determinismo Estricto**: La ejecución de un programa válido TISC (Computadora con Conjunto de Instrucciones Ternarias) $P$ con entrada $I$ produce una secuencia de transición de estados $S_0 \to S_1 \to \dots \to S_n$ que es idéntica en todas las arquitecturas host compatibles (x86_64, ARM64).
2.  **Nativo Ternario**: La arquitectura opera sobre lógica ternaria balanceada (trits $\in \{-1, 0, 1\}$), utilizando una pila aritmética personalizada (`dmath`) para evitar el no determinismo del punto flotante binario.
3.  **Cumplimiento de Políticas**: Toda la ejecución está gobernada por el **Kernel Axion**, un supervisor basado en capacidades que impone políticas de seguridad (límites de recursión, límites de memoria, restricciones éticas) antes del retiro de instrucciones.

> **Ancla de Verificación**: El bucle de ejecución determinista se implementa en `src/vm/vm.cpp` (ver `Interpreter::step()`). Las primitivas aritméticas ternarias se definen en `include/t81/ternary.hpp` y `include/t81/core/T81Float.hpp`.

## 1.2 Arquitectura del Sistema

El stack T81 consta de cuatro capas principales, cada una con responsabilidades y límites de verificación distintos.

### 1.2.1 La Máquina Virtual TISC (T81VM)

**Estado: Implementado y Probado**

La T81VM es un intérprete basado en pila para la ISA **TISC (Computadora con Conjunto de Instrucciones Ternarias)**. Gestiona un modelo de memoria segmentado que comprende:
*   **Código**: Segmento de instrucciones de solo lectura.
*   **Pila**: Almacenamiento LIFO para variables locales y direcciones de retorno.
*   **Montón (Heap)**: Asignación dinámica para objetos complejos (Tensores, Gráficos).
*   **Tensor**: Almacenamiento especializado para datos numéricos de alta dimensión.
*   **Meta**: Capacidades de reflexión e introspección.

El estado de la VM se define formalmente como una tupla $S = (R, PC, SP, M_{seg}, \Phi)$, donde $R$ representa el archivo de registros (81 registros), $PC$ el contador de programa, $SP$ el puntero de pila, $M_{seg}$ los segmentos de memoria, y $\Phi$ los indicadores de estado.

> **Referencia**: Ver `src/vm/vm.cpp`, struct `State`.

### 1.2.2 El Kernel de Seguridad Axion

**Estado: Implementado y Probado**

Axion actúa como un hipervisor para la T81VM. Intercepta cada despacho de instrucción para verificar el cumplimiento con la **Política** activa. Las políticas son conjuntos de reglas declarativas que restringen:
*   **Uso de Recursos**: Límites de asignación de memoria, conteo de ciclos.
*   **Flujo de Control**: Profundidad de recursión, complejidad de ramificación.
*   **Capacidades**: Acceso a llamadas del sistema de E/S, red o sistema de archivos.

Si una instrucción viola una política, Axion emite un veredicto de `Deny` (Denegar), causando que la VM se detenga con un `SecurityFault` (Fallo de Seguridad).

> **Referencia**: La lógica de políticas se implementa en `src/axion/policy_engine.cpp` y `include/t81/axion/api.hpp`.

### 1.2.3 Sistema de Archivos Canónico (CanonFS)

**Estado: Implementación Parcial**

CanonFS es una capa de almacenamiento direccionable por contenido que garantiza la **inmutabilidad estructural**. Los objetos (pesos, código, datos) se identifican por su hash SHA3-256 (`CanonHash81`). Cargar un objeto desde CanonFS asegura que los datos en memoria sean idénticos bit a bit al artefacto que fue firmado y publicado, eliminando ataques de "deriva de dependencias".

> **Referencia**: Implementado en `src/canonfs/` y definido en `spec/canonfs-spec.md`. Actualmente soporta verificación básica de hash y carga.

### 1.2.4 Los Niveles Cognitivos

**Estado: Implementado (Niveles 1-5)**

T81 organiza la complejidad computacional en **Niveles Cognitivos**, que van desde la aritmética pura (Nivel 1) hasta formas recursivas infinitas (Nivel 5).
*   **Nivel 1 (Simbólico)**: Aritmética y lógica básica.
*   **Nivel 2 (Reflexivo)**: Auto-inspección y captura de trazas.
*   **Nivel 3 (Recursivo)**: Recursión acotada y generación de pruebas.
*   **Nivel 4 (Distribuido)**: Protocolos de chismes (gossip) y fusión de estados.
*   **Nivel 5 (Infinito)**: Series geométricas y formas no terminantes.

> **Referencia**: La lógica de niveles se encuentra en `src/cog/`. Ver `src/cog/tier3/recursive.cpp` y `src/cog/tier5/infinite.cpp`.

## 1.3 Misión de Cómputo Verificable

La aplicación principal de T81 es el **Cómputo Soberano**: la capacidad de ejecutar código y verificar el resultado sin confiar en el operador del hardware. Al combinar aritmética estricta definida por software (`dmath`) con un registro de auditoría criptográfico (Traza Axion), T81 permite:
*   **Inferencia de IA Sin Confianza**: Verificar que un modelo específico produjo una salida específica.
*   **Contratos Inteligentes**: Ejecutar lógica donde el consenso depende de transiciones de estado exactas a nivel de bit.
*   **Reproducibilidad Científica**: garantizar que las simulaciones ejecutadas en 2025 produzcan los mismos resultados en 2050.

## 1.4 Terminología

| Término | Definición |
| :--- | :--- |
| **Trit** | Un dígito en base-3: $\{-1, 0, 1\}$. |
| **Tryte** | Una secuencia de trits, típicamente 3 o 9. |
| **TISC** | Computadora con Conjunto de Instrucciones Ternarias (la ISA). |
| **Axion** | El kernel de seguridad y cumplimiento de políticas. |
| **CanonRef** | Una referencia canónica (hash) a un objeto inmutable. |
| **promoción** | El acto de escalar privilegios o capacidades de nivel. |

## 1.5 Lista de Verificación

*   [ ] **determinismo**: ¿La VM produce trazas idénticas en x86 y ARM? (Verificado por `scripts/ci/t81lang_repro_gate.py`)
*   [ ] **aislamiento**: ¿Axion intercepta correctamente las instrucciones prohibidas? (Verificado por `tests/cpp/test_ethics.cpp`)
*   [ ] **persistencia**: ¿CanonFS recupera objetos por hash correctamente? (Verificado por `tests/cpp/canonfs_driver_test.cpp`)
