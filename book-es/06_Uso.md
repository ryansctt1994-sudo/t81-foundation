# Capítulo 6: Uso y CLI

## 6.1 La CLI Unificada

**Estado: Implementado y Probado**

El ejecutable `t81` proporciona una interfaz unificada para todas las operaciones.

### 6.1.1 Comandos Básicos

*   **`compile`**: Compila código fuente T81Lang (`.t81`) a bytecode TISC (`.tisc`).
    ```bash
    t81 compile examples/hello_world.t81 -o hello.tisc
    ```

*   **`run`**: Ejecuta bytecode TISC en la VM.
    ```bash
    t81 run hello.tisc
    ```

*   **`check`**: Realiza análisis sintáctico y semántico sin generar código.
    ```bash
    t81 check examples/hello_world.t81
    ```

### 6.1.2 Depuración e Inspección

*   **`disasm`**: Desensambla bytecode TISC en mnemónicos legibles.
    ```bash
    t81 disasm hello.tisc
    ```

*   **`debug`**: Lanza el depurador interactivo (paso a paso, inspeccionar registros).
    ```bash
    t81 debug hello.tisc
    ```

*   **`trace`**: Gestiona las trazas de auditoría Axion.
    ```bash
    t81 trace show trace.txt
    t81 trace diff trace_a.txt trace_b.txt
    t81 trace replay hello.tisc trace.txt
    ```

### 6.1.3 Gestión de Modelos

*   **`weights`**: Herramientas para importar y cuantizar pesos de redes neuronales.
    ```bash
    t81 weights import model.safetensors -o model.t81w
    t81 weights info model.t81w
    t81 weights quantize model.safetensors --to-gguf model.gguf
    ```

*   **`canonize-tensor`**: Verifica y normaliza un archivo de tensor.
    ```bash
    t81 canonize-tensor model.t81w
    ```

*   **`repro-hash`**: Calcula el hash canónico de un directorio para verificación.
    ```bash
    t81 repro-hash tests/fixtures/t81lang_determinism
    ```

> **Verificación**: Ejecuta `build/t81 --help` para ver el uso actual exacto.
