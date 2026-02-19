# T81 基金会

[![CI](https://github.com/t81dev/t81-foundation/actions/workflows/ci.yml/badge.svg)](https://github.com/t81dev/t81-foundation/actions/workflows/ci.yml)
[![Determinism Gate](https://img.shields.io/badge/Determinism%20Gate-Passing-success)](https://github.com/t81dev/t81-foundation/actions/workflows/ci.yml)
[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](https://opensource.org/licenses/MIT)
[![C++ Standard](https://img.shields.io/badge/C%2B%2B-23-blue.svg)](https://en.cppreference.com/w/cpp/23)

[![English](https://img.shields.io/badge/Language-English-blue?style=flat-square)](/README.md)
[![简体中文](https://img.shields.io/badge/Language-%E7%AE%80%E4%BD%93%E4%B8%AD%E6%96%87-red?style=flat-square)](/README.zh-CN.md)
[![Español](https://img.shields.io/badge/Language-Español-green?style=flat-square)](/README.es.md)
[![Русский](https://img.shields.io/badge/Language-Русский-brightgreen?style=flat-square)](/README.ru.md)
[![Português](https://img.shields.io/badge/Language-Português%20(Brasil)-blueviolet?style=flat-square)](/README.pt-BR.md)

---

**用于可审计计算的确定性、受控运行时堆栈。**

T81 是一个在 v1.0 版本之后进行的强化项目，它提供了一个完全确定性的编译和执行流水线（`T81Lang -> TISC -> HanoiVM`）。它优先考虑可审计性、策略执行（Axion）和可复现性，而非硬件速度。

## ⚡ 30 秒评估

只需 4 个步骤即可自行验证：

1. **构建并运行 Hello World 程序**

```bash

cmake -S . -B build -DCMAKE_BUILD_TYPE=Release && cmake --build build --parallel

./build/t81 compile examples/hello_world.t81 -o hello.tisc

./build/t81 run hello.tisc

```

2. **运行确定性测试**

```bash

# 验证跨架构可复现性哈希值

python3 scripts/ci/t81lang_repro_gate.py --t81-bin build/t81 --check

```

3. **运行虚拟机演示**

```bash

./build/t81_demo

```

4. **检查跟踪工件**

```bash

./build/t81 trace show trace.txt

```

---

## 🚫 非目标

为了节省您的时间，以下是 T81 的说明**并非**：

* **并非硬件加速器：**我们不声称能实现三元硬件加速。这是一个用于确保确定性正确性的软件运行时。

* **并非通用替代方案：**我们专注于高风险、可审计的逻辑，而非取代 C++ 或 Python 来处理通用任务。

* **并非“快速且随意”：**如果性能优化破坏了跟踪的确定性，我们将拒绝它。

---

## ❓ 其存在的意义

现代运行时为了速度而牺牲可复现性。T81 则反其道而行之：**可审计性是首要约束。**

我们通过在语言/编译器和执行运行时之间建立严格的架构边界来强制执行这一点，该边界由明确的契约约束。

[**查看架构边界图**](docs/explanation/ARCHITECTURE.md#3-concurrent-workstream-view) | [**查看运行时合约**](contracts/runtime-contract.json)

---

## 📚 文档权限图

| 文档 | 目的 | 权限范围 |

| :--- | :--- | :--- |

| **[STATUS.md](docs/reference/STATUS.md)** | *当前* 的真实情况 | 运行状态 |

| **[ROADMAP.md](docs/roadmaps-plans/ROADMAP.md)** | 未来规划 | 战略 |

| **[VERSIONING.md](docs/reference/VERSIONING.md)** | 兼容性规则 | 规范 |

| **[spec/](spec/)** | 行为定义 | 规范 |

| **[docs/EVIDENCE.md](docs/policies/EVIDENCE.md)** | 声明证明 | 验证 |

---

## 🤝 兼容性保证

* **稳定版：** T81Lang 语法、TISC 二进制格式、HanoiVM 执行语义。

* **实验版：** JIT 编译、分布式张量操作。

* **语义化版本控制：** 我们遵循语义化版本控制。对**稳定版**组件的重大更改会增加主版本号。

---

## 🖥️ 支持的平台

| 平台 | 编译器 | 状态 |

| :--- | :--- | :--- |

| **Linux (x86_64)** | Clang 18+、GCC 14+ | ✅ 确定性验证 |

| **Linux (ARM64)** | Clang 18+ | ✅ 确定性验证 |

| **macOS (ARM64)** | Apple Clang | ✅ 已支持 |

---

## 快速入门（完整版）

```bash

git clone https://github.com/t81dev/t81-foundation.git

cd t81-foundation

cmake -S . -B build -DCMAKE_BUILD_TYPE=Release

cmake --build build --parallel

ctest --test-dir build --output-on-failure

```

单线程安全模式：

```bash

cmake --build build --parallel 1

ctest --test-dir build --output-on-failure -j1

```

## CLI 界面

常用工作流程：

```bash

# 编译/运行

t81 compile examples/hello_world.t81 -o build/hello.tisc

t81 run build/hello.tisc

# 检查/调试

t81 disasm build/hello.tisc

t81 debug build/hello.tisc

# 诊断/复现性

t81 check examples/hello_world.t81

t81 repro-hash tests/fixtures/t81lang_determinism

# 跟踪工作流

t81 trace show trace.txt

t81 trace diff trace_a.txt trace_b.txt

t81 trace replay build/hello.tisc trace.txt

```

模型工具：

```bash

t81 weights import model.safetensors -o model.t81w

t81 weights info model.t81w

t81 weights quantize model.safetensors --to-gguf model.gguf

```

查看完整命令帮助：

```bash

t81 help

```

## 仓库映射

- [`include/t81/`](include/t81/): public API Headers

- [`src/`](src/): 前端、TISC、VM、Axion、CanonFS、CLI 实现

- [`tests/`](tests/): 一致性、确定性、VM/e2e、属性切片

- [`docs/`](docs/): 指南、状态、基准测试、运行时边界文档

- [`spec/`](spec/): 规范语义和治理输入

- [`examples/`](examples/): 可运行示例和演示

## 运行时边界

T81 使用显式运行时边界契约：

- 标记：[`contracts/runtime-contract.json`](contracts/runtime-contract.json)

- 边界策略：[`docs/explanation/runtime-semantics-boundary.md`](docs/explanation/runtime-semantics-boundary.md)

## 延伸阅读

- [`ARCHITECTURE.md`](docs/explanation/ARCHITECTURE.md)

- [`docs/system-integration.md`](docs/how-to/system-integration.md)

- [`ANALYSIS.md`](docs/explanation/ANALYSIS.md)

- [`CHANGELOG.md`](docs/reference/CHANGELOG.md)

- [`docs/research-guide.md`](docs/how-to/research-guide.md)

- [`docs/ai-quickstart.md`](docs/tutorials/ai-quickstart.md)

## 许可证

本仓库采用 MIT 许可证（参见 [`LICENSE`](L​​ICENSE)）。
