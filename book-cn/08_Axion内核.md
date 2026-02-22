# 第 8 章：Axion 安全内核

## 8.1 形式化定义

**状态：已实现并测试**

**Axion 内核** 是管理 T81VM 执行的基于能力的监督器。它强制执行 *机制*（TISC 操作码）与 *策略*（安全约束）之间的严格分离。

形式上，Axion 是一个函数 $\mathcal{A}: (S, I) \to \{ \text{Allow}, \text{Deny}, \text{Warn}, \text{Defer} \}$，其中 $S$ 是当前 VM 状态，$I$ 是拟执行的指令。

## 8.2 策略模型

**状态：已实现**

Axion 策略是定义允许执行范围的声明性规则集。

### 8.2.1 策略语法
一个策略文档包括：
1.  **指令**：全局约束（例如，`max_stack_depth`, `max_cycles`）。
2.  **系统调用**：特定操作的许可授予（`io.net`, `fs.read`）。
3.  **层级限制**：最大允许的认知层级。
4.  **伦理**：九项原则的配置 ($\Theta_1 \dots \Theta_9$)。

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

## 8.3 指令拦截

**状态：已实现**

T81VM 在执行敏感指令之前调用 Axion。这种拦截机制是主要的强制执行点。

### 8.3.1 系统调用接口
VM 调用 `eval_axion_call` (`src/vm/vm.cpp`)，上下文包含：
*   `caller`：执行模块。
*   `syscall`：操作标识符（例如，`kAxRead`, `kMetaWrite`）。
*   `payload`：参数或目标地址。
*   `pc`：当前程序计数器。

### 8.3.2 裁决
Axion 返回一个 `Verdict` 结构体：
*   **Allow**：操作继续进行。
*   **Deny**：操作被阻止，VM 陷入 `SecurityFault`。
*   **Warn**：操作继续进行，但警告被记录在追踪中。
*   **Defer**：决定被推迟到更高层级的逻辑。

## 8.4 审计日志 (追踪)

**状态：已实现**

每个重要的 Axion 决定都记录在 **Axion 追踪** 中。该日志是 `AxionEvent` 记录的仅追加序列。

> **参考**：参见 `include/t81/axion/api.hpp` 获取 `AxionEvent` 和 `Verdict` 的定义。

## 8.5 认知提升

**状态：已实现**

Axion 通过 **认知层 (Cognitive Tiers)** 管理特权的升级。当程序试图超出其当前层级的限制（例如，递归深度 > 81）时，VM 检查策略。如果允许，层级将被提升；否则，它将陷入陷阱。

> **验证**：参见 `src/vm/vm.cpp` 中的 `Opcode::Call` 处理。

## 8.6 能力模型

**状态：已实现**

Axion 实现了对象能力 (OCap) 模型。资源（文件、网络套接字）被表示为不可伪造的句柄。
*   **创建**：只有授权的系统调用可以创建句柄。
*   **使用**：操作码对句柄进行操作，而不是原始地址。
*   **撤销**：策略可以随时撤销句柄。

## 8.7 验证清单

*   [ ] **拦截**：`src/vm/vm.cpp` 中所有触及内存/IO 的操作码是否都调用了 `eval_axion_call`？（通过检查验证）
*   [ ] **裁决**：`VerdictKind::Deny` 是否总是导致 `SecurityFault`？（由 `tests/cpp/vm_fault_test.cpp` 验证）
*   [ ] **追踪**：每个 Axion 决定是否都记录了正确的 `tag` 和 `value`？（由 `tests/cpp/axion_log_determinism_test.cpp` 验证）
