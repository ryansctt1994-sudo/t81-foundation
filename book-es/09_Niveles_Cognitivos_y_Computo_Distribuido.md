# Capítulo 9: Niveles Cognitivos y Cómputo Distribuido

## 9.1 El Modelo de Niveles Cognitivos

**Estado: Implementado**

T81 organiza la complejidad computacional en una jerarquía de **Niveles Cognitivos**. Esto permite al Kernel Axion razonar sobre la *intención* y las *capacidades* de un programa antes de la ejecución.

| Nivel | Nombre | Capacidades | Límite de Recursión |
| :--- | :--- | :--- | :--- |
| **0** | **Base** | Aritmética básica (`Add`, `Sub`), flujo lineal. | 0 |
| **1** | **Simbólico** | Ops de tensores, bucles básicos. | 81 |
| **2** | **Reflexivo** | `MetaRead`, `MetaReflect`. | 243 |
| **3** | **Recursivo** | Automodificación, generación de pruebas. | 1024 (Política) |
| **4** | **Distribuido** | Gossip, Fusión de Estados. | N/A |
| **5** | **Infinito** | Series Geométricas, formas no terminantes. | N/A |

> **Implementación**: La lógica de niveles está modularizada en `src/cog/tier[1-5]/`.

## 9.2 Cómputo Distribuido (Nivel 4)

**Estado: Implementado y Probado**

El **Nivel Distribuido** permite que múltiples instancias de T81VM operen como un enjambre coherente.

### 9.2.1 Protocolo Gossip
Los nodos intercambian actualizaciones de estado a través de un protocolo determinista de chismes (gossip) utilizando los opcodes `Gossip` y `Merge`.
*   **Formato de Mensaje**: `(Etiqueta, Carga Útil, LamportTick, NodeID)`.
*   **Estrategia de Fusión**: Fusión tipo CRDT basada en marcas de tiempo `TickSync`.
*   **Determinismo**: Dada la misma secuencia de llegada de mensajes, el estado fusionado final es idéntico en todos los nodos.

> **Referencia**: `src/cog/tier4/distributed.cpp` y `tests/cpp/test_tier4_distributed.cpp`.

### 9.2.2 Relojes Lógicos (TickSync)
La VM mantiene un reloj lógico de Lamport (`R75`).
*   **Tic Interno**: Incrementa en cada instrucción.
*   **Sincronización**: Actualiza al recibir el mensaje: `Tick = max(LocalTick, RemoteTick) + 1`.
*   **Coherencia**: El opcode `Coherence` devuelve la deriva entre los tics locales y globales.

## 9.3 Formas Infinitas (Nivel 5)

**Estado: Implementado y Probado**

El Nivel 5 introduce estructuras de datos "Infinitas", como las series geométricas.
*   **Representación**: Un generador finito `(a, r)` representa la serie $a + ar + ar^2 + \dots$.
*   **Operaciones**: `InfExpand` calcula el $N$-ésimo término. `InfConverge` verifica si $|r| < 1$.
*   **Firma**: `InfSignature` genera un hash único de la *función generadora*, no de los datos infinitos.

> **Referencia**: `src/cog/tier5/infinite.cpp` y `tests/cpp/test_infinite_opcodes.cpp`.

## 9.4 Lista de Verificación

*   [ ] **Promoción**: ¿El intento de recursión > 81 sin permiso falla? (Verificado por `tests/cpp/axion_recursion_guardrails_test.cpp`)
*   [ ] **TickSync**: ¿El reloj lógico incrementa exactamente una vez por instrucción? (Verificado por `tests/cpp/axion_instruction_counter_test.cpp`)
*   [ ] **Nivel 5**: ¿`InfExpand` e `InfConverge` producen resultados correctos? (Verificado por `tests/cpp/tier5_test.cpp`)
