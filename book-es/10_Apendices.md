# Capítulo 10: Apéndices

## 10.1 Lo Que Aún No Está Implementado

Si bien la arquitectura central de T81 es estable, varias características permanecen en estados experimentales o aspiracionales a partir de febrero de 2026.

1.  **Trascendentales Totalmente Deterministas (Fase 2)**:
    *   Actualmente, las funciones trigonométricas inversas (`asin`, `acos`, `atan`) y las funciones hiperbólicas (`sinh`, `cosh`, `tanh`) dependen de la `libm` del host a menos que se establezca `T81_DETERMINISTIC` (lo que las deshabilita o devuelve errores).
    *   **Meta**: Implementar soporte `dmath` para todas las funciones trascendentales.

2.  **Características Avanzadas de CanonFS**:
    *   Actualmente, CanonFS soporta carga básica direccionable por contenido.
    *   **Faltante**: Fijación (pinning) distribuida, replicación peer-to-peer y recolección de basura de artefactos no referenciados.

3.  **Madurez de Trace-JIT**:
    *   El Trace-JIT (`src/vm/jit_compiler.cpp`) es funcional pero se considera **Experimental**. Aún no cubre todos los opcodes y puede recurrir al intérprete con frecuencia.

4.  **Álgebra Simbólica de Nivel 1 Completa**:
    *   Existe soporte básico de grafos simbólicos (`src/cog/tier1/symbolic.cpp`), pero la reescritura algebraica completa y la simplificación (capacidades CAS) aún no están expuestas a través de opcodes estándar.

5.  **Tipos de Holotensor**:
    *   Mencionados en especificaciones tempranas como un formato de tensor disperso de alta dimensión. Actualmente, solo están implementados los densos `T729Tensor` y `T81Tensor`.

## 10.2 Códigos de Error

| Código | Nombre | Descripción |
| :--- | :--- | :--- |
| `0x00` | `Ok` | Éxito. |
| `0x01` | `SecurityFault` | Violación de política Axion. |
| `0x02` | `TypeFault` | Tipo de operando inválido. |
| `0x03` | `StackFault` | Desbordamiento/subdesbordamiento de pila. |
| `0x04` | `MathFault` | División por cero o error de dominio. |

## 10.3 Enlaces Útiles

*   **Repositorio**: [github.com/t81dev/t81-foundation](https://github.com/t81dev/t81-foundation)
*   **Especificación**: Directorio `spec/` en el repositorio.
*   **Problemas (Issues)**: Rastreador de Issues de GitHub.
