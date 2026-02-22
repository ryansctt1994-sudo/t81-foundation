# Capítulo 8: O Kernel de Segurança Axion

## 8.1 Definição Formal

**Status: Implementado e Testado**

O **Kernel Axion** é o supervisor baseado em capacidades que governa a execução da T81VM. Ele impõe uma separação estrita entre *mecanismo* (opcodes TISC) e *política* (restrições de segurança).

Formalmente, Axion é uma função $\mathcal{A}: (S, I) \to \{ \text{Allow}, \text{Deny}, \text{Warn}, \text{Defer} \}$, onde $S$ é o estado atual da VM e $I$ é a instrução proposta.

## 8.2 O Modelo de Política

**Status: Implementado**

Políticas Axion são conjuntos de regras declarativas que definem o envelope permissível de execução.

### 8.2.1 Gramática de Política
Um documento de política consiste em:
1.  **Diretivas**: Restrições globais (ex: `max_stack_depth`, `max_cycles`).
2.  **Syscalls**: Concessões de permissão para operações específicas (`io.net`, `fs.read`).
3.  **Limites de Nível**: Nível Cognitivo máximo permitido.
4.  **Ética**: Configuração para os Nove Princípios ($\Theta_1 \dots \Theta_9$).

```yaml
policy:
  version: "1.0"
  directives:
    max_stack_depth: 1024
    max_cycles: 1000000
    allow_recursion: true
  syscalls:
    - allow: "io.print"
    - deny: "fs.write"
  tiers:
    max_tier: 3
```

## 8.3 Interceptação de Instrução

**Status: Implementado**

A T81VM invoca o Axion antes de executar instruções sensíveis. Esse mecanismo de interceptação é o ponto primário de aplicação.

### 8.3.1 A Interface Syscall
A VM chama `eval_axion_call` (`src/vm/vm.cpp`) com um contexto contendo:
*   `caller`: O módulo em execução.
*   `syscall`: O identificador da operação (ex: `kAxRead`, `kMetaWrite`).
*   `payload`: Argumentos ou endereços de destino.
*   `pc`: Contador de programa atual.

### 8.3.2 Vereditos
O Axion retorna uma estrutura `Verdict`:
*   **Allow**: A operação prossegue.
*   **Deny**: A operação é bloqueada, e a VM intercepta com `SecurityFault`.
*   **Warn**: A operação prossegue, mas um aviso é registrado no trace.
*   **Defer**: A decisão é adiada para uma lógica de nível superior.

## 8.4 O Log de Auditoria (Trace)

**Status: Implementado**

Cada decisão significativa do Axion é registrada no **Trace Axion**. Este log é uma sequência append-only de registros `AxionEvent`.

> **Referência**: Veja `include/t81/axion/api.hpp` para as definições de `AxionEvent` e `Verdict`.

## 8.5 Promoção Cognitiva

**Status: Implementado**

O Axion gerencia o escalonamento de privilégios através dos **Níveis Cognitivos**. Quando um programa tenta exceder os limites de seu nível atual (ex: profundidade de recursão > 81), a VM verifica a política. Se permitido, o nível é promovido; caso contrário, ocorre um trap.

> **Verificação**: Veja o tratamento de `Opcode::Call` em `src/vm/vm.cpp`.

## 8.6 Modelo de Capacidade

**Status: Implementado**

O Axion implementa um modelo de Capacidade de Objeto (OCap). Recursos (arquivos, sockets de rede) são representados como handles infalsificáveis.
*   **Criação**: Apenas syscalls autorizadas podem criar handles.
*   **Uso**: Opcodes operam em handles, não em endereços brutos.
*   **Revogação**: Handles podem ser revogados pela política a qualquer momento.

## 8.7 Checklist de Verificação

*   [ ] **Interceptação**: Todos os opcodes em `src/vm/vm.cpp` que tocam memória/IO chamam `eval_axion_call`? (Verificado por inspeção)
*   [ ] **Veredito**: O `VerdictKind::Deny` sempre resulta em uma `SecurityFault`? (Verificado por `tests/cpp/vm_fault_test.cpp`)
*   [ ] **Trace**: Cada decisão do Axion é registrada com um `tag` e `value` corretos? (Verificado por `tests/cpp/axion_log_determinism_test.cpp`)
