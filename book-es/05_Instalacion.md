# Capítulo 5: Instalación y Construcción

## 5.1 Requisitos Previos

Para construir T81 desde el código fuente (reconstrucción en sala limpia), necesitas:
*   **Compilador C++**: Clang 18+ o GCC 14+ (se requiere soporte C++23).
*   **Sistema de Construcción**: CMake 3.25+.
*   **Python**: Python 3.10+ (para scripts de validación).

## 5.2 Procedimiento de Construcción

1.  **Clonar el Repositorio**:
    ```bash
    git clone https://github.com/t81dev/t81-foundation.git
    cd t81-foundation
    ```

2.  **Configurar y Construir**:
    ```bash
    cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
    cmake --build build --parallel
    ```

3.  **Verificar la Construcción**:
    Ejecuta el script de puerta de determinismo para asegurar que tu cadena de herramientas produzca binarios exactos a nivel de bit en relación con los hashes de referencia.
    ```bash
    python3 scripts/ci/t81lang_repro_gate.py --t81-bin build/t81 --check
    ```

## 5.3 Puerta de Determinismo

El script `repro_gate.py` es el árbitro principal de la corrección de la construcción. Compila un conjunto de programas de referencia (`tests/fixtures/t81lang_determinism`) y compara los hashes de bytecode TISC resultantes contra un manifiesto `repro.json` conocido como bueno.

> **Fuente**: `scripts/ci/t81lang_repro_gate.py`.
