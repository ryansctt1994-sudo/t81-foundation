# Capítulo 12: Modelagem Adversária

## 12.1 Modelo de Ameaça

**Status: Ativo**

O T81 assume um ambiente adversário onde o hardware hospedeiro, sistema operacional e pares de rede podem ser maliciosos ou defeituosos.

### 12.1.1 O Vetor "Libm Gap"
Um vetor de ataque sutil existe onde um nó malicioso explora diferenças na biblioteca matemática padrão do hospedeiro (`libm`).
*   **Ataque**: O Nó A (x86) computa `sin(x)` ligeiramente diferente do Nó B (ARM).
*   **Consequência**: Divergência de estado leva a um fork de consenso.
*   **Mitigação**: O T81 impõe o uso de `dmath` para todas as funções transcendentais críticas, garantindo resultados bit-exact independentemente da implementação `libm` subjacente.

> **Verificação**: `tests/cpp/test_property_float.cpp` verifica a consistência multiplataforma das operações `T81Float`.

### 12.1.2 Ataques de Viagem no Tempo
Em um sistema distribuído, um par malicioso pode injetar mensagens com carimbos de data/hora futuros para manipular o relógio lógico.
*   **Mitigação**: O protocolo `TickSync` impõe atualizações de relógio monotônicas. Uma mensagem com um carimbo de data/hora muito no futuro pode ser rejeitada ou limitada por política.
*   **Verificação**: `tests/cpp/tier4_vm_test.cpp` testa a lógica de sincronização de relógio.

## 12.2 Resiliência a Canal Lateral

**Status: Aspiracional / Parcial**

Embora o T81 garanta determinismo lógico, ele atualmente não garante execução em tempo constante para todas as operações. Canais laterais de tempo podem existir na implementação atual de multiplicação `BigInt` e funções `dmath`.
