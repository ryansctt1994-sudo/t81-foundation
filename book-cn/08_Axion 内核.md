# 第 8 章：Axion 安全内核

## 8.1 正式定义

**状态：已实现并已测试**

**Axion 内核**是一个基于能力（capability-based）的监督器，负责管辖 T81VM 的执行。它强制执行“机制”（TISC 操作码）与“策略”（安全约束）的严格分离。

从形式化角度看，Axion 是一个函数 ，其中  是当前虚拟机状态， 是拟执行的指令。

## 8.2 策略模型

**状态：已实现**

Axion 策略是声明性的规则集，定义了执行允许的“包络面”（permissible envelope）。

### 8.2.1 策略语法

一份策略文档包含：

1. **指令 (Directives)**：全局约束（如 `max_stack_depth`、`max_cycles`）。
2. **系统调用 (Syscalls)**：特定操作的权限授予（`io.net`、`fs.read`）。
3. **层级限制 (Tier Limits)**：允许的最高认知层级。
4. **伦理 (Ethics)**：九大原则 () 的配置。

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

T81VM 在执行敏感指令前会调用 Axion。这种拦截机制是主要的策略执行点。

### 8.3.1 系统调用接口

虚拟机会调用 `eval_axion_call` (`src/vm/vm.cpp`)，并传入包含以下内容的上下文：

* `caller`：执行模块。
* `syscall`：操作标识符（如 `kAxRead`、`kMetaWrite`）。
* `payload`：参数或目标地址。
* `pc`：当前程序计数器。

### 8.3.2 裁定结果 (Verdicts)

Axion 返回一个 `Verdict` 结构体：

* **Allow (允许)**：操作继续执行。
* **Deny (拒绝)**：操作被阻止，虚拟机触发 `SecurityFault` 中断。
* **Warn (警告)**：操作继续执行，但在追踪日志中记录一条警告。
* **Defer (推迟)**：将决策推迟至更高层的逻辑处理。

## 8.4 审计日志 (追踪)

**状态：已实现**

每一个重大的 Axion 决策都会记录在 **Axion 追踪 (Axion Trace)** 中。该日志是一个只增的 `AxionEvent` 记录序列。

> **参考**：`AxionEvent` 和 `Verdict` 的定义请参见 `include/t81/axion/api.hpp`。

## 8.5 认知提权 (Cognitive Promotion)

**状态：已实现**

Axion 通过**认知层级**管理特权的升级。当程序尝试超过其当前层级的限制（例如：递归深度 > 81）时，虚拟机会检查策略。如果允许，则层级得到提升（Promotion）；否则，触发中断。

> **验证**：参见 `src/vm/vm.cpp` 中对 `Opcode::Call` 的处理。

## 8.6 能力模型 (Capability Model)

**状态：已实现**

Axion 实现了对象能力（Object-Capability, OCap）模型。资源（文件、网络套接字）表现为不可伪造的句柄（handles）。

* **创建**：仅有授权的系统调用可以创建句柄。
* **使用**：操作码作用于句柄，而非原始地址。
* **撤销**：策略可以随时撤销句柄。

## 8.7 验证清单

* [ ] **拦截性**：`src/vm/vm.cpp` 中所有涉及内存/IO 的操作码是否都调用了 `eval_axion_call`？（已通过代码审查验证）
* [ ] **裁定一致性**：`VerdictKind::Deny` 是否总是导致 `SecurityFault`？（已通过 `tests/cpp/vm_fault_test.cpp` 验证）
* [ ] **追踪完整性**：每一个 Axion 决策是否都记录了正确的 `tag` 和 `value`？（已通过 `tests/cpp/axion_log_determinism_test.cpp` 验证）