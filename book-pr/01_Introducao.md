# Capítulo 1: Introdução

## 1.1 Escopo e Definição

**Status: Implementado e Testado**

O projeto **Fundação T81** implementa uma arquitetura de máquina virtual nativa ternária e determinística, projetada para computação verificável. Diferente de ambientes de execução de propósito geral que priorizam throughput ou abstração de hardware, o T81 prioriza **reprodutibilidade bit-exact** e **auditabilidade**.

O sistema é definido pelos seguintes invariantes centrais:
1.  **Determinismo Estrito**: A execução de um programa válido TISC (Computador de Conjunto de Instruções Ternárias) $P$ na entrada $I$ produz uma sequência de transição de estado $S_0 \to S_1 \to \dots \to S_n$ que é idêntica em todas as arquiteturas hospedeiras compatíveis (x86_64, ARM64).
2.  **Nativo Ternário**: A arquitetura opera em lógica ternária balanceada (trits $\in \{-1, 0, 1\}$), utilizando uma pilha aritmética personalizada (`dmath`) para evitar o não-determinismo de ponto flutuante binário.
3.  **Política Aplicada**: Toda execução é governada pelo **Kernel Axion**, um supervisor baseado em capacidades que impõe políticas de segurança (limites de recursão, limites de memória, restrições éticas) antes da retirada da instrução.

> **Âncora de Verificação**: O loop de execução determinística é implementado em `src/vm/vm.cpp` (veja `Interpreter::step()`). As primitivas aritméticas ternárias são definidas em `include/t81/ternary.hpp` e `include/t81/core/T81Float.hpp`.

## 1.2 Arquitetura do Sistema

A stack T81 consiste em quatro camadas primárias, cada uma com responsabilidades distintas e limites de verificação.

### 1.2.1 A Máquina Virtual TISC (T81VM)

**Status: Implementado e Testado**

A T81VM é um intérprete baseado em pilha para a ISA **Computador de Conjunto de Instruções Ternárias (TISC)**. Ela gerencia um modelo de memória segmentado compreendendo:
*   **Código**: Segmento de instrução somente leitura.
*   **Pilha (Stack)**: Armazenamento LIFO para variáveis locais e endereços de retorno.
*   **Heap**: Alocação dinâmica para objetos complexos (Tensores, Grafos).
*   **Tensor**: Armazenamento especializado para dados numéricos de alta dimensão.
*   **Meta**: Capacidades de reflexão e introspecção.

O estado da VM é formalmente definido como uma tupla $S = (R, PC, SP, M_{seg}, \Phi)$, onde $R$ representa o arquivo de registradores (81 registradores), $PC$ o contador de programa, $SP$ o ponteiro de pilha, $M_{seg}$ os segmentos de memória, e $\Phi$ as flags de status.

> **Referência**: Veja `src/vm/vm.cpp`, struct `State`.

### 1.2.2 O Kernel de Segurança Axion

**Status: Implementado e Testado**

O Axion atua como um hipervisor para a T81VM. Ele intercepta cada despacho de instrução para verificar a conformidade com a **Política** ativa. Políticas são conjuntos de regras declarativas que restringem:
*   **Uso de Recursos**: Limites de alocação de memória, contagens de ciclos.
*   **Fluxo de Controle**: Profundidade de recursão, complexidade de ramificação.
*   **Capacidades**: Acesso a chamadas de sistema de E/S, rede ou sistema de arquivos.

Se uma instrução viola uma política, o Axion emite um veredito `Deny`, fazendo com que a VM intercepte com uma `SecurityFault`.

> **Referência**: A lógica de política é implementada em `src/axion/policy_engine.cpp` e `include/t81/axion/api.hpp`.

### 1.2.3 Sistema de Arquivos Canônico (CanonFS)

**Status: Implementação Parcial**

O CanonFS é uma camada de armazenamento endereçável por conteúdo que garante **imutabilidade estrutural**. Objetos (pesos, código, dados) são identificados por seu hash SHA3-256 (`CanonHash81`). Carregar um objeto do CanonFS garante que os dados na memória sejam idênticos bit-a-bit ao artefato que foi assinado e publicado, eliminando ataques de "deriva de dependência".

> **Referência**: Implementado em `src/canonfs/` e definido em `spec/canonfs-spec.md`. Atualmente suporta verificação básica de hash e carregamento.

### 1.2.4 Os Níveis Cognitivos

**Status: Implementado (Níveis 1-5)**

O T81 organiza a complexidade computacional em **Níveis Cognitivos**, variando de aritmética pura (Nível 1) a formas recursivas infinitas (Nível 5).
*   **Nível 1 (Simbólico)**: Aritmética básica e lógica.
*   **Nível 2 (Reflexivo)**: Auto-inspeção e captura de rastreamento.
*   **Nível 3 (Recursivo)**: Recursão limitada e geração de provas.
*   **Nível 4 (Distribuído)**: Protocolos de fofoca (gossip) e fusão de estado.
*   **Nível 5 (Infinito)**: Séries geométricas e formas não terminais.

> **Referência**: A lógica dos níveis está localizada em `src/cog/`. Veja `src/cog/tier3/recursive.cpp` e `src/cog/tier5/infinite.cpp`.

## 1.3 Missão de Computação Verificável

A aplicação primária do T81 é **Computação Soberana**: a capacidade de executar código e verificar o resultado sem confiar no operador do hardware. Ao combinar aritmética estrita definida por software (`dmath`) com um log de auditoria criptográfico (Trace Axion), o T81 permite:
*   **Inferência de IA Trustless**: Verificar que um modelo específico produziu uma saída específica.
*   **Contratos Inteligentes**: Executar lógica onde o consenso depende de transições de estado bit-exact.
*   **Reprodutibilidade Científica**: garantir que simulações executadas em 2025 produzam os mesmos resultados em 2050.

## 1.4 Terminologia

| Termo | Definição |
| :--- | :--- |
| **Trit** | Um dígito de base-3: $\{-1, 0, 1\}$. |
| **Tryte** | Uma sequência de trits, tipicamente 3 ou 9. |
| **TISC** | Computador de Conjunto de Instruções Ternárias (a ISA). |
| **Axion** | O kernel de segurança e aplicação de políticas. |
| **CanonRef** | Uma referência canônica (hash) para um objeto imutável. |
| **promoção** | O ato de escalar privilégios ou capacidades de nível. |

## 1.5 Checklist de Verificação

*   [ ] **determinismo**: A VM produz traços idênticos em x86 e ARM? (Verificado por `scripts/ci/t81lang_repro_gate.py`)
*   [ ] **isolamento**: O Axion intercepta corretamente instruções proibidas? (Verificado por `tests/cpp/test_ethics.cpp`)
*   [ ] **persistência**: O CanonFS recupera objetos por hash corretamente? (Verificado por `tests/cpp/canonfs_driver_test.cpp`)
