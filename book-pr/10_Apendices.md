# Capítulo 10: Apêndices

## 10.1 O Que Ainda Não Foi Implementado

Embora a arquitetura central T81 seja estável, vários recursos permanecem em estados experimentais ou aspiracionais a partir de Fevereiro de 2026.

1.  **Transcendentais Totalmente Determinísticas (Fase 2)**:
    *   Atualmente, funções trigonométricas inversas (`asin`, `acos`, `atan`) e funções hiperbólicas (`sinh`, `cosh`, `tanh`) dependem da `libm` do hospedeiro, a menos que `T81_DETERMINISTIC` esteja definido (o que as desabilita ou retorna erros).
    *   **Meta**: Implementar suporte `dmath` para todas as funções transcendentais.

2.  **Recursos Avançados do CanonFS**:
    *   Atualmente, o CanonFS suporta carregamento básico endereçável por conteúdo.
    *   **Faltando**: Fixação distribuída (pinning), replicação peer-to-peer e coleta de lixo de artefatos não referenciados.

3.  **Maturidade do Trace-JIT**:
    *   O Trace-JIT (`src/vm/jit_compiler.cpp`) é funcional, mas considerado **Experimental**. Ele ainda não cobre todos os opcodes e pode recorrer ao intérprete frequentemente.

4.  **Álgebra Simbólica de Nível 1 Completa**:
    *   O suporte básico a grafos simbólicos existe (`src/cog/tier1/symbolic.cpp`), mas a reescrita e simplificação algébrica completa (capacidades CAS) ainda não estão expostas via opcodes padrão.

5.  **Tipos Holotensor**:
    *   Mencionados nas primeiras especificações como um formato de tensor esparso de alta dimensão. Atualmente, apenas `T729Tensor` denso e `T81Tensor` estão implementados.

## 10.2 Códigos de Erro

| Código | Nome | Descrição |
| :--- | :--- | :--- |
| `0x00` | `Ok` | Sucesso. |
| `0x01` | `SecurityFault` | Violação de política Axion. |
| `0x02` | `TypeFault` | Tipo de operando inválido. |
| `0x03` | `StackFault` | Estouro de pilha (overflow/underflow). |
| `0x04` | `MathFault` | Divisão por zero ou erro de domínio. |

## 10.3 Links Úteis

*   **Repositório**: [github.com/t81dev/t81-foundation](https://github.com/t81dev/t81-foundation)
*   **Especificação**: diretório `spec/` no repositório.
*   **Issues**: Rastreador de Issues do GitHub.
