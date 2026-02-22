# Capítulo 2: Princípios Centrais e Invariantes

## 2.1 O Invariante de Determinismo

**Status: Implementado e Testado**

O axioma central da arquitetura T81 é o **Determinismo Estrito**. Um programa T81 é uma função pura $f(S, I) \to S'$, onde $S$ é o estado inicial e $I$ é a entrada. Esta função deve produzir $S'$ idêntico em bits em qualquer plataforma de hardware compatível.

Alcançar isso requer a eliminação de todas as fontes de não-determinismo comuns na computação moderna:
*   **Ponto Flutuante de Hardware**: Substituído por `T81Float` (`dmath`) definido por software.
*   **Layout de Memória**: Endereços lógicos são desacoplados de ponteiros físicos.
*   **Concorrência**: O agendamento de threads é substituído por corrotinas determinísticas e ticks lógicos.
*   **Tempo do Sistema**: O tempo de relógio de parede é substituído por carimbos de data/hora Lamport (ticks lógicos).

### 2.1.1 Superfícies de Determinismo e Vetores de Ataque

A tabela a seguir mapeia as "superfícies" onde o não-determinismo pode vazar para o sistema e as mitigações específicas que o T81 emprega.

| Camada   | Risco de Determinismo        | Mitigação                 | Evidência               |
| -------- | ---------------------------- | ------------------------- | ----------------------- |
| Compiler | Ordenação de tokens          | Emissão de AST canônica   | `scripts/ci/t81lang_repro_gate.py` |
| VM       | Vazamento de endereço de memória | Sem observabilidade de endereço | `src/vm/vm.cpp` (Segmentos de Memória) |
| GC       | Coleta não-determinística    | Gatilhos de contagem de alocação | `src/vm/vm.cpp`: `run_gc_cycle_` |
| Float    | Deriva de FPU do hospedeiro (IEEE-754) | `dmath` float de software | `include/t81/core/T81Float.hpp` |
| JIT      | Divergência de otimização    | Equivalência baseada em trace | `src/vm/jit_compiler.cpp` |

> **Verificação**: O compilador JIT em `src/vm/jit_compiler.cpp` garante que traces otimizados saiam (`GuardDeopt`) mediante *qualquer* divergência de estado da linha de base interpretada.

## 2.2 Lógica Ternária (Base-3)

**Status: Implementado e Testado**

O T81 é um sistema **ternário balanceado**. A unidade fundamental é o **trit**, com valores $\{-1, 0, 1\}$ (frequentemente denotados como $-, 0, +$).

### 2.2.1 Por que Ternário?
1.  **Aritmética Simétrica**: O arredondamento é simplesmente o truncamento em direção ao inteiro mais próximo, já que $0.5$ não é uma fração representável na base-3 sem expansão infinita. Isso simplifica a biblioteca `dmath`.
2.  **Densidade de Informação**: O radix $3$ está mais próximo de $e \approx 2.718$ do que o $2$, oferecendo o ótimo teórico para economia de radix inteiro ($\text{radix} \times \text{largura}$).
3.  **Representação com Sinal**: Números negativos não requerem um bit de sinal ou Complemento de Dois. O trit principal indica o sinal naturalmente.

### 2.2.2 Implementação
Na base de código C++, trits são empacotados para eficiência, mas logicamente distintos.
*   **Armazenamento**: `T81Int` usa 2 bits por trit na forma empacotada (veja `include/t81/packing.hpp`).
*   **Aritmética**: Operações como `Add`, `Mul` são implementadas em `src/vm/vm.cpp` usando matemática inteira que simula cadeias de transporte (carry) ternárias balanceadas.

## 2.3 Auditabilidade e o Trace Axion

**Status: Implementado e Testado**

Cada transição de estado no T81 é auditável. O **Kernel Axion** produz um log criptográfico de execução chamado **Trace**.

### 2.3.1 A Estrutura do Trace
Um trace é uma sequência de registros `AxionEvent`, capturando opcodes, vereditos e dados associados.

> **Referência**: Veja `include/t81/axion/api.hpp` para as definições de evento.

Este trace serve como uma **Prova de Execução**. Ao reproduzir o trace contra o estado inicial, um auditor pode verificar que:
1.  A computação ocorreu conforme reivindicado.
2.  Nenhuma política de segurança foi violada.
3.  O resultado final está correto.

## 2.4 Os Nove Princípios (Aplicação de Ética)

**Status: Implementado e Testado**

O T81 incorpora uma camada de ética imutável (Os Nove Princípios $\Theta_1 \dots \Theta_9$) diretamente no motor de política da VM. Estas não são diretrizes, mas **restrições de tempo de execução**.

Por exemplo:
*   **$\Theta_7$ (Contenção de Entropia)**: Impede a expansão infinita de recursos sem permissão explícita `InfExpand`.
*   **$\Theta_4$ (Interpretabilidade)**: Exige que tensores opacos "caixa preta" não possam ser emitidos sem metadados acompanhantes ou grafos simbólicos.

> **Implementação**: Essas verificações são realizadas em `src/axion/ethics.cpp`. Uma violação resulta em um veredito `VerdictKind::Deny` e imediata `Trap::SecurityFault`.

## 2.5 Checklist de Verificação

*   [ ] **Consistência de Float**: O `T81Float` produz padrões de bits idênticos para funções transcendentais (`sin`, `exp`) em todas as plataformas? (Execute `tests/cpp/test_T81Float.cpp` e `tests/cpp/test_property_float.cpp`)
*   [ ] **Determinismo do GC**: O Coletor de Lixo roda em contagens exatas de instrução (alocações), não tempo de parede? (Verifique `kGcInterval` em `src/vm/vm.cpp`)
*   [ ] **Integridade do Trace**: O log Axion é imutável durante a execução? (Verificado por `tests/cpp/axion_log_determinism_test.cpp`)

## 2.6 Matriz de Auditoria Formal

| Princípio | Seção da Spec | Implementação | Cobertura de Teste |
| :--- | :--- | :--- | :--- |
| Determinismo Estrito | `spec/determinism-profile.md` | `src/vm/vm.cpp` | `tests/cpp/test_property_invariants.cpp` |
| Lógica Ternária | `spec/t81-data-types.md` | `include/t81/ternary.hpp` | `tests/cpp/ternary_arith_test.cpp` |
| Auditabilidade | `spec/axion-kernel.md` | `include/t81/axion/api.hpp` | `tests/cpp/test_ethics.cpp` |
