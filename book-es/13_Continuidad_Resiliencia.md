# Capítulo 13: Continuidad y Resiliencia

## 13.1 El Protocolo de Sala Limpia

**Estado: Documentado**

El **Protocolo de Reconstrucción en Sala Limpia** define el conjunto mínimo de pasos requeridos para reconstruir el sistema T81 desde cero, asumiendo una pérdida total de infraestructura.

### 13.1.1 Arranque Mínimo
1.  **Código Fuente**: Una copia de los directorios `src/` e `include/`.
2.  **Compilador**: Cualquier compilador compatible con C++23 (Clang 18+, GCC 14+).
3.  **Sistema de Construcción**: CMake 3.25+.

**Comando**:
```bash
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build --parallel
```

### 13.1.2 Verificación
Después de reconstruir, el sistema debe verificarse contra un conjunto conocido de artefactos `CanonHash81`.
```bash
./build/t81 repro-hash tests/fixtures/t81lang_determinism
```

## 13.2 Archivo a Largo Plazo

**Estado: Aspiracional**

El objetivo de T81 es ser legible y ejecutable dentro de 50+ años.
*   **Estabilidad de Formato**: Los formatos de código fuente `.t81` y bytecode `.tisc` están congelados.
*   **Dependencias**: La VM central tiene cero dependencias externas en tiempo de ejecución más allá de la biblioteca estándar de C++.
