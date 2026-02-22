# Capítulo 4: Tipos de Datos y Serialización

## 4.1 Tipos Primitivos

**Estado: Implementado y Probado**

La arquitectura T81 está construida sobre una base de primitivas ternarias balanceadas. Estos tipos están diseñados para ser simulados eficientemente en hardware binario mientras mantienen las propiedades matemáticas de la lógica base-3.

### 4.1.1 Trits y Trytes
*   **Trit**: El átomo fundamental de información, tomando valores $\{-1, 0, 1\}$.
*   **Tryte**: Una secuencia de trits. El ancho estándar del tryte es de 4 trits ($3^4 = 81$ valores), a menudo empaquetado en un `uint8_t` para almacenamiento.

> **Implementación**: `include/t81/ternary.hpp` define el enum `Trit` y la lógica de conversión.

### 4.1.2 T81Int (Entero de Precisión Arbitraria)
`T81Int` es un tipo entero de ancho variable que utiliza una representación ternaria balanceada empaquetada.
*   **Almacenamiento**: 2 bits por trit.
*   **Rango**: Simétrico alrededor de cero ($-\frac{3^N-1}{2} \dots +\frac{3^N-1}{2}$).
*   **Normalización**: Los ceros a la izquierda están estrictamente prohibidos en la forma serializada canónica. Un valor cero se representa por un solo trit cero.

> **Verificación**: `tests/cpp/test_t81int.cpp` y `tests/cpp/test_property_invariants.cpp`.

## 4.2 T81Float y dmath

**Estado: Implementado (Core) / Parcial (Extendido)**

La aritmética de punto flotante es la fuente principal de no determinismo en la computación multiplataforma (debido a las variaciones de IEEE-754 en la fusión FMA, precisión trascendental, etc.). T81 aborda esto a través de `T81Float`.

### 4.2.1 Definición Canónica
Un `T81Float` es una tupla $(m, e)$, representando el valor $m \times 3^e$.
*   $m$: Mantisa (T81Int).
*   $e$: Exponente (T81Int).
*   **Invariante**: La mantisa $m$ debe estar normalizada de tal manera que su trit más significativo sea distinto de cero, a menos que el valor sea exactamente cero.

### 4.2.2 El Backend dmath
Para lograr el **Determinismo Estricto**, la VM emplea `dmath` (Matemática Determinista), una biblioteca aritmética definida por software.
*   **Operaciones Centrales**: `Add`, `Sub`, `Mul` son exactas y deterministas (implementadas en `T81Float.hpp`).
*   **Trascendentales**: `Sin`, `Cos`, `Tan`, `Exp`, `Log`, `Sqrt` se calculan utilizando `dmath` (series de Taylor con conteos de iteración fijos), garantizando resultados exactos a nivel de bit en cualquier arquitectura.
*   **Funciones Extendidas**: `Asin`, `Acos`, `Sinh`, `Pow` actualmente dependen de la precisión `double` del host (a menos que se defina `T81_DETERMINISTIC`, en cuyo caso pueden devolver `NaE` o usar emulación de software lenta).

> **Verificación**: `tests/cpp/test_T81Float.cpp` valida la corrección de valores especiales y trascendentales. `include/t81/core/detail/dmath.hpp` contiene la implementación.

## 4.3 Tensores y Diseños Canónicos

**Estado: Implementado y Probado**

Los tensores (`T729Tensor`, `T81Tensor`) son los caballos de batalla de los niveles cognitivos.

### 4.3.1 Diseño de Memoria
Los tensores se almacenan en orden **Row-Major** (Fila Principal).
*   **Forma**: Un vector de dimensiones $(d_0, d_1, \dots, d_n)$.
*   **Stride (Paso)**: Calculado como $s_i = \prod_{j=i+1}^n d_j$.
*   **Alineación**: Los datos del tensor están alineados a límites de 64 bytes en el segmento de memoria `Tensor`.

### 4.3.2 Serialización (.t81w)
El formato `.t81w` (Pesos T81) es el contenedor estándar para persistir modelos de tensores. La Versión 2 (`T81W2`) soporta cuantización y hashing canónico.

**Estructura Binaria**:
1.  **Encabezado Mágico**: `0x54383157` ("T81W").
2.  **Versión**: `0x02`.
3.  **Tabla de Contenidos**: Lista de tuplas `(Hash, Offset, Longitud)`.
4.  **Datos Blob**: Datos contiguos del tensor.

**Formatos de Cuantización**:
*   **F32**: Float estándar IEEE-754 (canonicalizado).
*   **T3_K**: Empaquetado de 2 bits por trit con escalado por bloque.

> **Fuente**: `include/t81/weights.hpp` y `include/t81/tensor.hpp`.

## 4.4 Reglas de Serialización Canónica

**Estado: Implementado**

Para asegurar un hashing consistente (`CanonRef`), todos los datos deben normalizarse antes de la serialización.

1.  **BigInt**: Eliminar ceros a la izquierda. Cero es `[0]`.
2.  **Fraction**:
    *   Reducir a términos mínimos: $\gcd(num, den) = 1$.
    *   El denominador debe ser positivo.
    *   Cero es $0/1$.
3.  **Float**:
    *   Estandarizar mantisa/exponente.
    *   Las cargas útiles NaN se ponen a cero.
    *   El cero negativo se normaliza a cero positivo.
4.  **Map/Dictionary**:
    *   Las claves deben ordenarse lexicográficamente por su representación binaria canónica.
5.  **Graph**:
    *   Los nodos se re-indexan por orden de clasificación topológica para asegurar que el isomorfismo de grafos produzca flujos de bytes idénticos.

> **Verificación**: `tests/cpp/test_property_invariants.cpp` verifica estas propiedades de normalización a través de pruebas basadas en propiedades.
