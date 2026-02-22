# Capítulo 7: Verificação e Auditoria

## 7.1 A Pilha de Verificação

**Status: Implementado**

O T81 fornece uma suíte abrangente de ferramentas de verificação para garantir a correção em cada nível da stack.

1.  **Testes Unitários**: Testes C++ de baixo nível em `tests/cpp/`.
2.  **Testes de Integração**: Programas T81Lang end-to-end em `tests/fixtures/`.
3.  **Testes de Propriedade**: Testes baseados em propriedades randomizados para invariantes aritméticos (`tests/cpp/test_property_invariants.cpp`).
4.  **Portão de Determinismo**: Script de CI `scripts/ci/t81lang_repro_gate.py` que impõe reprodutibilidade bit-exact.

## 7.2 Portão de Determinismo (Determinism Gate)

**Status: Implementado e Ativo**

O script `t81lang_repro_gate.py` é o árbitro primário da correção do build. Ele:
1.  Compila uma suíte de testes padrão (`tests/fixtures/t81lang_determinism`).
2.  Computa o hash SHA-256 do bytecode TISC gerado.
3.  Compara esses hashes contra um manifesto canônico (`repro.json`).

Se qualquer hash diferir, o build falha. Isso garante que mudanças no compilador não alterem inadvertidamente a geração de código.

## 7.3 Verificação de Trace

**Status: Implementado**

O Trace Axion permite a verificação post-hoc da execução.
*   **Replay**: `t81 trace replay program.tisc trace.txt` re-executa o programa e verifica se o trace gravado corresponde à execução ao vivo.
*   **Diff**: `t81 trace diff trace_a.txt trace_b.txt` destaca divergências entre duas execuções.

> **Verificação**: `tests/cpp/axion_log_determinism_test.cpp` garante a integridade do trace.
