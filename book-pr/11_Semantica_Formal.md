# Capítulo 11: Semântica Formal

## 11.1 Semântica Operacional

**Status: Especificação**

A semântica operacional do T81 é definida como um sistema de transição de estado.

$$ S' = \text{VM}(S, \text{Op}) $$

Onde $S$ é o estado da máquina $(R, PC, SP, M_{seg}, \Phi)$.

### 11.1.1 A Função de Transição
A função de transição é determinística. Para qualquer estado $S$ e opcode $\text{Op}$, existe exatamente um próximo estado válido $S'$ ou uma condição de falha $\bot$.

Esta propriedade é verificada pelo **Portão de Determinismo** (`scripts/ci/t81lang_repro_gate.py`), que garante que o intérprete produza transições idênticas entre plataformas.

## 11.2 Semântica de Memória

**Status: Implementado**

A memória no T81 não é um array linear de bytes, mas um armazenamento estruturado de **Objetos Tipados**.
*   **Segurança**: Acessar um elemento de tensor fora dos limites não é comportamento indefinido; é uma `TypeFault` ou `StackFault` garantida.
*   **Imutabilidade**: Uma vez que um tensor é gravado no CanonFS, ele é imutável.

> **Verificação**: `tests/cpp/vm_bounds_test.cpp` garante que todos os acessos fora dos limites sejam interceptados corretamente.
