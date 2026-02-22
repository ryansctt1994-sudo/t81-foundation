# Capítulo 9: Níveis Cognitivos e Computação Distribuída

## 9.1 O Modelo de Nível Cognitivo

**Status: Implementado**

O T81 organiza a complexidade computacional em uma hierarquia de **Níveis Cognitivos**. Isso permite que o Kernel Axion raciocine sobre a *intenção* e *capacidades* de um programa antes da execução.

| Nível | Nome | Capacidades | Limite de Recursão |
| :--- | :--- | :--- | :--- |
| **0** | **Base** | Aritmética básica (`Add`, `Sub`), fluxo linear. | 0 |
| **1** | **Simbólico** | Ops de tensor, loops básicos. | 81 |
| **2** | **Reflexivo** | `MetaRead`, `MetaReflect`. | 243 |
| **3** | **Recursivo** | Auto-modificação, geração de provas. | 1024 (Política) |
| **4** | **Distribuído** | Gossip, Fusão de Estado. | N/A |
| **5** | **Infinito** | Séries Geométricas, formas não terminais. | N/A |

> **Implementação**: A lógica de nível é modularizada em `src/cog/tier[1-5]/`.

## 9.2 Computação Distribuída (Nível 4)

**Status: Implementado e Testado**

O **Nível Distribuído** permite que múltiplas instâncias da T81VM operem como um enxame coerente.

### 9.2.1 Protocolo de Gossip
Nós trocam atualizações de estado via um protocolo de gossip determinístico usando opcodes `Gossip` e `Merge`.
*   **Formato de Mensagem**: `(Tag, Payload, LamportTick, NodeID)`.
*   **Estratégia de Fusão**: Fusão tipo CRDT baseada em carimbos de data/hora `TickSync`.
*   **Determinismo**: Dada a mesma sequência de chegadas de mensagens, o estado fundido final é idêntico em todos os nós.

> **Referência**: `src/cog/tier4/distributed.cpp` e `tests/cpp/test_tier4_distributed.cpp`.

### 9.2.2 Relógios Lógicos (TickSync)
A VM mantém um relógio lógico de Lamport (`R75`).
*   **Tick Interno**: Incrementa a cada instrução.
*   **Sync**: Atualiza no recebimento de mensagem: `Tick = max(LocalTick, RemoteTick) + 1`.
*   **Coerência**: O opcode `Coherence` retorna a deriva entre ticks locais e globais.

## 9.3 Formas Infinitas (Nível 5)

**Status: Implementado e Testado**

O Nível 5 introduz estruturas de dados "Infinitas", como séries geométricas.
*   **Representação**: Um gerador finito `(a, r)` representa a série $a + ar + ar^2 + \dots$.
*   **Operações**: `InfExpand` computa o $N$-ésimo termo. `InfConverge` verifica se $|r| < 1$.
*   **Assinatura**: `InfSignature` gera um hash único da *função geradora*, não dos dados infinitos.

> **Referência**: `src/cog/tier5/infinite.cpp` e `tests/cpp/test_infinite_opcodes.cpp`.

## 9.4 Checklist de Verificação

*   [ ] **Promoção**: Tentar recursão > 81 sem permissão falha? (Verificado por `tests/cpp/axion_recursion_guardrails_test.cpp`)
*   [ ] **TickSync**: O relógio lógico incrementa exatamente uma vez por instrução? (Verificado por `tests/cpp/axion_instruction_counter_test.cpp`)
*   [ ] **Nível 5**: `InfExpand` e `InfConverge` produzem resultados corretos? (Verificado por `tests/cpp/tier5_test.cpp`)
