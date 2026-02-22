# Capítulo 6: Uso e CLI

## 6.1 A CLI Unificada

**Status: Implementado e Testado**

O executável `t81` fornece uma interface unificada para todas as operações.

### 6.1.1 Comandos Básicos

*   **`compile`**: Compila código-fonte T81Lang (`.t81`) para bytecode TISC (`.tisc`).
    ```bash
    t81 compile examples/hello_world.t81 -o hello.tisc
    ```

*   **`run`**: Executa bytecode TISC na VM.
    ```bash
    t81 run hello.tisc
    ```

*   **`check`**: Realiza análise sintática e semântica sem gerar código.
    ```bash
    t81 check examples/hello_world.t81
    ```

### 6.1.2 Depuração e Inspeção

*   **`disasm`**: Desmonta bytecode TISC em mnemônicos legíveis.
    ```bash
    t81 disasm hello.tisc
    ```

*   **`debug`**: Lança o depurador interativo (passo a passo, inspecionar registradores).
    ```bash
    t81 debug hello.tisc
    ```

*   **`trace`**: Gerencia traces de auditoria Axion.
    ```bash
    t81 trace show trace.txt
    t81 trace diff trace_a.txt trace_b.txt
    t81 trace replay hello.tisc trace.txt
    ```

### 6.1.3 Gerenciamento de Modelo

*   **`weights`**: Ferramentas para importar e quantizar pesos de redes neurais.
    ```bash
    t81 weights import model.safetensors -o model.t81w
    t81 weights info model.t81w
    t81 weights quantize model.safetensors --to-gguf model.gguf
    ```

*   **`canonize-tensor`**: Verifica e normaliza um arquivo de tensor.
    ```bash
    t81 canonize-tensor model.t81w
    ```

*   **`repro-hash`**: Computa o hash canônico de um diretório para verificação.
    ```bash
    t81 repro-hash tests/fixtures/t81lang_determinism
    ```

> **Verificação**: Execute `build/t81 --help` para ver o uso exato atual.
