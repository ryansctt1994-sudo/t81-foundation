# T81 Foundation (T81 基金会)

<p align="center">
<strong>确定性三进制原生计算栈，具有 base-81 数据类型、TISC 指令集、T81VM、T81Lang、Axion 安全/优化引擎以及递归认知层 —— 专为 AI、密码学和科学计算中的位精确（bit-exact）、可审计、可重现执行而构建。</strong>
</p>
<p align="center">
  <a href="https://github.com/t81dev/t81-foundation/stargazers"><img src="https://img.shields.io/github/stars/t81dev/t81-foundation?style=for-the-badge&logo=github&color=blue" alt="Stars"></a>
  <a href="https://github.com/t81dev/t81-foundation/network/members"><img src="https://img.shields.io/github/forks/t81dev/t81-foundation?style=for-the-badge&logo=github&color=blue" alt="Forks"></a>
  <a href="https://github.com/t81dev/t81-foundation/actions/workflows/ci.yml"><img src="https://img.shields.io/github/actions/workflow/status/t81dev/t81-foundation/ci.yml?branch=main&style=for-the-badge&logo=github" alt="CI Status"></a>
  <a href="https://github.com/t81dev/t81-foundation/commits/main"><img src="https://img.shields.io/github/commit-activity/m/t81dev/t81-foundation?style=for-the-badge&logo=github&color=green" alt="Commit Activity"></a>
  <a href="https://opensource.org/licenses/MIT"><img src="https://img.shields.io/badge/License-MIT-yellow.svg?style=for-the-badge" alt="License: MIT"></a>
  <a href="https://en.cppreference.com/w/cpp/23"><img src="https://img.shields.io/badge/Language-C%2B%2B23-00599C?style=for-the-badge&logo=c%2B%2B&logoColor=white" alt="Language: C++23"></a>
</p>

<p align="center">
  <a href="README.md"><img src="https://img.shields.io/badge/Lang-English-blue?style=flat-square" alt="English"></a>
  <a href="README.zh-CN.md"><img src="https://img.shields.io/badge/Lang-%E7%AE%80%E4%BD%93%E4%B8%AD%E6%96%87-red?style=flat-square" alt="简体中文"></a>
  <a href="README.es.md"><img src="https://img.shields.io/badge/Lang-Espa%C3%B1ol-green?style=flat-square" alt="Español"></a>
  <a href="README.ru.md"><img src="https://img.shields.io/badge/Lang-%D0%A0%D1%83%D1%81%D1%81%D0%BA%D0%B8%D0%B9-blueviolet?style=flat-square" alt="Русский"></a>
  <a href="README.pt-BR.md"><img src="https://img.shields.io/badge/Lang-Portugu%C3%AAs-orange?style=flat-square" alt="Português"></a>
</p>

---

T81 是一个主权计算栈，旨在消除浮点数的不确定性并实现完全可审计的执行。通过利用**平衡三进制逻辑**和 **base-81 数据类型**，T81 保证了在所有支持的架构（x86/ARM, macOS/Linux）上实现**位精确的可重现性**。它包含 **T81VM**、**Axion 安全引擎**以及用于从简单符号逻辑扩展到分布式无限形态的递归层级系统。

> 💡 **为什么这很重要：** 在 AI 安全、金融建模和密码学领域，“基本正确”是不够的。T81 提供了数学上的确定性，确保您的代码在任何地方、任何时间都以完全相同的方式执行。

## 特性

| 特性 | 状态 | 描述 |
| --- | --- | --- |
| **确定性执行** | ✨ 稳定 | 通过 `dmath` 和自定义浮点数实现 x86/ARM/Apple Silicon 的位精确结果。 |
| **三进制原生类型** | ✨ 稳定 | Base-81 平衡三进制整数和浮点数（无符号位，减少进位）。 |
| **T81VM 与 TISC** | ✨ 稳定 | 拥有 81 个寄存器的虚拟机，配备确定性解释器和 Trace-JIT。 |
| **Axion 引擎** | ✨ 稳定 | 运行时策略、安全、伦理和优化引擎，带有审计追踪功能。 |
| **模型工具链** | ✨ 稳定 | 支持 SafeTensors、GGUF、T81W 的导入/检查；支持量化。 |
| **重现性关卡** | ✨ 稳定 | CI 强制执行的 `t81lang_repro_gate.py` 确保 100% 的确定性。 |
| **认知层级** | 🚧 Beta | 递归执行层（符号层 → 分布式层 → 无限层）。 |
| **Trace-JIT** | 🚧 实验性 | 在保持严格确定性的前提下进行热点优化。 |
| **多语言文档** | 📚 已上线 | 包含中、英、西、葡、俄语的完整技术规范。 |

## 架构

*(此处为 Mermaid 图表，展示了从 T81Lang 到 TISC 字节码，再到 T81VM 解释器、Axion 引擎和模型工具的流程)*

## 快速开始

在 60 秒内从零开始实现可验证执行。

### 1. 构建

```bash
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build --parallel

```

### 2. 编译并运行 Hello World

```bash
# 将 T81 源码编译为 TISC 字节码
./build/t81 compile examples/hello_world.t81 -o hello.tisc

# 运行字节码
./build/t81 run hello.tisc

```

### 3. 验证确定性 (重现性关卡)

证明您的构建符合位精确标准：

```bash
python3 scripts/ci/t81lang_repro_gate.py --t81-bin build/t81 --check
# 输出: ✅ All determinism checks passed.

```

## 支持的平台

以下所有平台均通过了**确定性关卡**，具有相同的输出哈希值。

| 平台 | 架构 | 编译器 | 状态 |
| --- | --- | --- | --- |
| **Linux** | x86_64 | Clang 18+, GCC 14+ | ✅ 已验证 |
| **Linux** | ARM64 | Clang 18+ | ✅ 已验证 |
| **macOS** | Intel | Apple Clang / GCC | ✅ 已验证 |
| **macOS** | Apple Silicon | Apple Clang | ✅ 已验证 |

## CLI 示例

`t81` 命令行界面是您进行开发、调试和审计的主要工具。

```bash
# 🛠️ 开发
t81 compile src.t81 -o out.tisc      # 编译
t81 run out.tisc                     # 执行
t81 disasm out.tisc                  # 反汇编字节码

# 🐞 调试与审计
t81 debug out.tisc                   # 交互式调试器
t81 trace show trace.txt             # 查看执行追踪
t81 repro-hash tests/fixtures/       # 计算确定性哈希

# 🤖 AI / 张量
t81 weights import model.safetensors -o model.t81w
t81 weights quantize model.safetensors --to-gguf model.gguf

```

## 代码库地图

核心目录说明：

* **`src/`**: 核心 C++ 源码 (VM, Axion, TISC, CanonFS)。
* **`include/t81/`**: 公共头文件。
* **`book/book-en/`**: 权威技术专著（文档）。
* **`scripts/ci/`**: 持续集成与重现性关卡脚本。
* **`examples/`**: `.t81` 程序示例及 C++ 嵌入示例。
* **`tests/`**: 综合单元测试与集成测试套件。
* **`spec/`**: 标准规范 (TISC, 数据类型)。
* **`tools/`**: 工具脚本与 VSCode 扩展辅助程序。

## 兼容性与非目标

### 保证

* **TISC 字节码:** 在主版本内保持向上兼容。
* **确定性:** 绝对优先级。破坏确定性将被视为严重的安全性 Bug。

### 非目标

* **不惜一切代价追求原始速度:** 我们不会为了特定硬件的快速数学优化（fast-math）而牺牲位精确性。
* **通用替代方案:** T81 专注于可验证计算，并非旨在取代 C++ 或 Python 进行通用脚本编写。

## 贡献

我们欢迎各类贡献！请参阅 [CONTRIBUTING.md](CONTRIBUTING.md) 了解以下详情：

* 代码风格 (Clang-Format)。
* Pull Request 流程。
* 确定性验证要求。

## 许可证

本项目采用 **MIT 许可证**。详见 [LICENSE](LICENSE) 文件。
