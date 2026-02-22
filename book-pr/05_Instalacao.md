# Capítulo 5: Instalação e Build

## 5.1 Pré-requisitos

Para compilar o T81 a partir da fonte (reconstrução cleanroom), você precisa de:
*   **Compilador C++**: Clang 18+ ou GCC 14+ (suporte a C++23 necessário).
*   **Sistema de Build**: CMake 3.25+.
*   **Python**: Python 3.10+ (para scripts de validação).

## 5.2 Procedimento de Build

1.  **Clonar o Repositório**:
    ```bash
    git clone https://github.com/t81dev/t81-foundation.git
    cd t81-foundation
    ```

2.  **Configurar e Compilar**:
    ```bash
    cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
    cmake --build build --parallel
    ```

3.  **Verificar o Build**:
    Execute o script do portão de determinismo para garantir que sua toolchain produza binários bit-exact em relação aos hashes de referência.
    ```bash
    python3 scripts/ci/t81lang_repro_gate.py --t81-bin build/t81 --check
    ```

## 5.3 Portão de Determinismo (Determinism Gate)

O script `repro_gate.py` é o árbitro primário da correção do build. Ele compila uma suíte de programas de referência (`tests/fixtures/t81lang_determinism`) e compara os hashes de bytecode TISC resultantes com um manifesto `repro.json` validado.

> **Fonte**: `scripts/ci/t81lang_repro_gate.py`.
