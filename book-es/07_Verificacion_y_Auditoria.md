# Capítulo 7: Verificación y Auditoría

## 7.1 El Stack de Verificación

**Estado: Implementado**

T81 proporciona un conjunto integral de herramientas de verificación para asegurar la corrección en cada nivel del stack.

1.  **Pruebas Unitarias**: Pruebas de bajo nivel en C++ en `tests/cpp/`.
2.  **Pruebas de Integración**: Programas T81Lang de extremo a extremo en `tests/fixtures/`.
3.  **Pruebas de Propiedad**: Pruebas aleatorias basadas en propiedades para invariantes aritméticos (`tests/cpp/test_property_invariants.cpp`).
4.  **Puerta de Determinismo**: Script de CI `scripts/ci/t81lang_repro_gate.py` que impone la reproducibilidad exacta a nivel de bit.

## 7.2 Puerta de Determinismo

**Estado: Implementado y Activo**

El script `t81lang_repro_gate.py` es el árbitro principal de la corrección de la construcción. Este:
1.  Compila un conjunto de pruebas estándar (`tests/fixtures/t81lang_determinism`).
2.  Calcula el hash SHA-256 del bytecode TISC generado.
3.  Compara estos hashes contra un manifiesto canónico (`repro.json`).

Si algún hash difiere, la construcción falla. Esto asegura que los cambios en el compilador no alteren inadvertidamente la generación de código.

## 7.3 Verificación de Traza

**Estado: Implementado**

La Traza Axion permite la verificación *post-hoc* de la ejecución.
*   **Replay** (Reproducción): `t81 trace replay program.tisc trace.txt` re-ejecuta el programa y verifica que la traza grabada coincida con la ejecución en vivo.
*   **Diff**: `t81 trace diff trace_a.txt trace_b.txt` resalta las divergencias entre dos ejecuciones.

> **Verificación**: `tests/cpp/axion_log_determinism_test.cpp` asegura la integridad de la traza.
