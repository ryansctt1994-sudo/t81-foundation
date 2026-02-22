# Capítulo 8: El Kernel de Seguridad Axion

## 8.1 Definición Formal

**Estado: Implementado y Probado**

El **Kernel Axion** es el supervisor basado en capacidades que gobierna la ejecución de la T81VM. Impone una estricta separación entre *mecanismo* (opcodes TISC) y *política* (restricciones de seguridad).

Formalmente, Axion es una función $\mathcal{A}: (S, I) \to \{ \text{Allow}, \text{Deny}, \text{Warn}, \text{Defer} \}$, donde $S$ es el estado actual de la VM e $I$ es la instrucción propuesta.

## 8.2 El Modelo de Políticas

**Estado: Implementado**

Las políticas de Axion son conjuntos de reglas declarativas que definen el entorno de ejecución permitido.

### 8.2.1 Gramática de Políticas
Un documento de política consta de:
1.  **Directivas**: Restricciones globales (por ejemplo, `max_stack_depth`, `max_cycles`).
2.  **Syscalls**: Concesiones de permisos para operaciones específicas (`io.net`, `fs.read`).
3.  **Límites de Nivel**: Nivel Cognitivo máximo permitido.
4.  **Ética**: Configuración para los Nueve Principios ($\Theta_1 \dots \Theta_9$).

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

## 8.3 Intercepción de Instrucciones

**Estado: Implementado**

La T81VM invoca a Axion antes de ejecutar instrucciones sensibles. Este mecanismo de intercepción es el punto de aplicación principal.

### 8.3.1 La Interfaz de Syscall
La VM llama a `eval_axion_call` (`src/vm/vm.cpp`) con un contexto que contiene:
*   `caller`: El módulo en ejecución.
*   `syscall`: El identificador de la operación (por ejemplo, `kAxRead`, `kMetaWrite`).
*   `payload`: Argumentos o direcciones de destino.
*   `pc`: Contador de programa actual.

### 8.3.2 Veredictos
Axion devuelve una estructura `Verdict`:
*   **Allow** (Permitir): La operación procede.
*   **Deny** (Denegar): La operación se bloquea y la VM se detiene con `SecurityFault`.
*   **Warn** (Advertir): La operación procede, pero se registra una advertencia en la traza.
*   **Defer** (Diferir): La decisión se difiere a una lógica de nivel superior.

## 8.4 El Registro de Auditoría (Traza)

**Estado: Implementado**

Cada decisión significativa de Axion se registra en la **Traza Axion**. Este registro es una secuencia de solo adición de registros `AxionEvent`.

> **Referencia**: Ver `include/t81/axion/api.hpp` para las definiciones de `AxionEvent` y `Verdict`.

## 8.5 Promoción Cognitiva

**Estado: Implementado**

Axion gestiona la escalada de privilegios a través de los **Niveles Cognitivos**. Cuando un programa intenta exceder los límites de su nivel actual (por ejemplo, profundidad de recursión > 81), la VM verifica la política. Si se permite, el nivel se promueve; de lo contrario, se detiene.

> **Verificación**: Ver el manejo de `Opcode::Call` en `src/vm/vm.cpp`.

## 8.6 Modelo de Capacidades

**Estado: Implementado**

Axion implementa un modelo de Capacidad de Objetos (OCap). Los recursos (archivos, sockets de red) se representan como manejadores ineludibles.
*   **Creación**: Solo las syscalls autorizadas pueden crear manejadores.
*   **Uso**: Los opcodes operan sobre manejadores, no sobre direcciones crudas.
*   **Revocación**: Los manejadores pueden ser revocados por la política en cualquier momento.

## 8.7 Lista de Verificación

*   [ ] **Intercepción**: ¿Todos los opcodes en `src/vm/vm.cpp` que tocan memoria/E/S llaman a `eval_axion_call`? (Verificado por inspección)
*   [ ] **Veredicto**: ¿`VerdictKind::Deny` siempre resulta en un `SecurityFault`? (Verificado por `tests/cpp/vm_fault_test.cpp`)
*   [ ] **Traza**: ¿Cada decisión de Axion se registra con una `etiqueta` y `valor` correctos? (Verificado por `tests/cpp/axion_log_determinism_test.cpp`)
