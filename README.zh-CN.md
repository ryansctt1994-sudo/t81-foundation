# T81 Foundation 🔥

<div align="center">

</div>

T81 是一个具备**确定性**的**原生三进制**计算栈 🌐。其特性包括 Base-81 数据类型、TISC 指令集、T81 虚拟机 (T81VM)、T81Lang 语言、Axion 安全与优化引擎以及递归认知层。它为算力密集型领域提供位精确 (bit-exact) 且可审计的执行环境 ⚡，是可验证 AI、密码学和科学计算的理想选择。

> **关于浮点数确定性的说明** ⚠️
> 初等超越函数（`sin`, `cos`, `tan`, `log`, `exp`, `sqrt`）使用确定性的 `dmath` 后端，确保跨平台位精确一致。
> 在非严格模式下，除法、反函数及双曲函数可能会回退到宿主机的行为。
> `T81Int`、`T81BigInt`、`T81Fraction` 以及核心 `T81Float` 算术保证具备**全严格确定性**。 ✅

---

## 目录 📑

* [快速入门 🚀](https://www.google.com/search?q=%23%E5%BF%AB%E9%80%9F%E5%85%A5%E9%97%A8)
* [核心特性 🌟](https://www.google.com/search?q=%23%E6%A0%B8%E5%BF%83%E7%89%B9%E6%80%A7)
* [为何选择三进制？ 🧠](https://www.google.com/search?q=%23%E4%B8%BA%E4%BD%95%E9%80%89%E6%8B%A9%E4%B8%89%E8%BF%9B%E5%88%B6)
* [系统架构 🏗️](https://www.google.com/search?q=%23%E7%B3%BB%E7%BB%9F%E6%9E%B6%E6%9E%84)
* [支持平台 🌍](https://www.google.com/search?q=%23%E6%94%AF%E6%8C%81%E5%B9%B3%E5%8F%B0)
* [CLI 示例 🔧](https://www.google.com/search?q=%23cli-%E7%A4%BA%E4%BE%8B)
* [代码库地图 📂](https://www.google.com/search?q=%23%E4%BB%A3%E7%A0%81%E5%BA%93%E5%9C%B0%E5%9B%BE)
* [文档权威地图 📜](https://www.google.com/search?q=%23%E6%96%87%E6%A1%A3%E6%9D%83%E5%A8%81%E5%9C%B0%E5%9B%BE)
* [兼容性保证 🔄](https://www.google.com/search?q=%23%E5%85%BC%E5%AE%B9%E6%80%A7%E4%BF%9D%E8%AF%81)
* [非目标 🚫](https://www.google.com/search?q=%23%E9%9D%9E%E7%9B%AE%E6%A0%87)
* [运行时边界 🔐](https://www.google.com/search?q=%23%E8%BF%90%E8%A1%8C%E6%97%B6%E8%BE%B9%E7%95%8C)
* [延伸阅读 📖](https://www.google.com/search?q=%23%E5%BB%B6%E4%BC%B8%E9%98%85%E8%AF%BB)
* [技术专题论文 📘](https://www.google.com/search?q=%23%E6%8A%80%E6%9C%AF%E4%B8%93%E9%A2%98%E8%AE%BA%E6%96%87)
* [许可证 📜](https://www.google.com/search?q=%23%E8%AE%B8%E5%8F%AF%E8%AF%81)

---

## 快速入门 🚀⚡

在 30 秒内验证核心主张：

1. **构建并运行 Hello World** 🏃‍♂️
```bash
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release && cmake --build build --parallel
./build/t81 compile examples/hello_world.t81 -o hello.tisc
./build/t81 run hello.tisc

```


2. **运行确定性网关测试** 🔄✅
```bash
python3 scripts/ci/t81lang_repro_gate.py --t81-bin build/t81 --check

```


3. **运行虚拟机演示** ▶️🔥
```bash
./build/t81_demo

```


4. **检查追踪日志** 🔍📜
```bash
./build/t81 trace show trace.txt

```



---

## 核心特性 🌟

| 特性 | 状态 | 描述 |
| --- | --- | --- |
| ✅ **确定性执行** | 稳定 🔥 | 跨平台的位精确可重现性 |
| ✅ **原生三进制数据类型** | 稳定 🌐 | 采用平衡三进制算法的 Base-81 系统 |
| ✅ **Axion 策略引擎** | 稳定 🔐 | 运行时安全、优化及伦理强制执行 |
| ✅ **T81VM** | 稳定 ⚙️ | 81 寄存器虚拟机 + 确定性解释器与 Trace-JIT |
| ✅ **TISC IR** | 稳定 📡 | 三进制精简指令集计算机中间表示 |
| ✅ **软件定义数学库** | 稳定 🧮 | 跨平台一致的浮点运算 (`dmath`) |
| 🚧 **Trace-JIT 编译** | 实验性 ⚡ | 热点追踪与确定性即时编译 |
| 🚧 **分布式张量** | 实验性 🌍 | 大规模分布式张量支持 |
| ✅ **模型工具链** | 稳定 🤖 | 支持 SafeTensors / GGUF / T81W 的导入、量化与检查 |

---

## 为何选择三进制？ 🧠🧮

平衡三进制（-1, 0, +1）和 Base-81 消除了符号位，简化了加减法（减少进位），并在理论上具有更高的密度和能效优势——这在数值负载（AI 推理、密码学、信号处理）中尤为重要。

T81 将这些优势引入软件，同时将**确定性**和**可审计性**置于纯粹的速度之上。关于硬件实验，请参阅 [ternary-memory-research](https://github.com/t81dev/ternary-memory-research) 中关于 SKY130 PDK 的指标。 🔬

---

## 系统架构 🏗️

```mermaid
graph TD
    subgraph "语言工具链"
        A["T81Lang (.t81)"] --> B[词法分析]
        B --> C[语法分析]
        C --> D[AST]
        D --> E["语义分析器"]
        E --> F["IR 生成器"]
        F --> G["TISC IR"]
        G --> H["二进制发射器"]
        H --> I["TISC 字节码"]
    end
    subgraph "运行时"
        I --> J["T81VM 解释器"]
        J --> K["热点追踪检测"]
        K --> L["确定性 Trace JIT"]
        L --> M["编译执行"]
    end
    subgraph "安全与审计"
        J --> N["Axion 策略引擎"]
        M --> N
        N --> O["事件 / 判定 / 追踪"]
    end
    subgraph "模型工具链"
        P["SafeTensors / GGUF / T81W"] --> Q["权重导入"]
        Q --> R["张量句柄"]
        R --> J
    end

```

---

## 支持平台 🌍

| 平台 | 编译器 | 状态 | 确定性网关 | 备注 |
| --- | --- | --- | --- | --- |
| Linux x86_64 | Clang 18+, GCC 14+ | ✅ 通过 🔥 | ✅ | 全网关通过 |
| Linux ARM64 | Clang 18+ | ✅ 通过 🔥 | ✅ | 全网关通过 |
| macOS Intel | Apple Clang / GCC | ✅ 通过 | ✅ | 原生支持 |
| macOS Apple Silicon | Apple Clang | ✅ 通过 | ✅ | 活跃研究中 (CMake/标志优化) |

---

## CLI 示例 🔧🔍

```bash
# 编译并运行 🚀
t81 compile examples/hello_world.t81 -o hello.tisc
t81 run hello.tisc

# 调试与检查 🕵️
t81 disasm hello.tisc
t81 debug hello.tisc
t81 trace show trace.txt
t81 repro-hash tests/fixtures/t81lang_determinism

# 模型工具 🤖
t81 weights import model.safetensors -o model.t81w
t81 weights quantize model.safetensors --to-gguf model.gguf

```

---

## 文档权威地图 📜

| 文档 | 用途 | 权威性 |
| --- | --- | --- |
| `spec/constitution.md` | 基础原则 | 规范性 (Normative) 🔒 |
| `spec/determinism-profile.md` | 确定性保证 | 规范性 (Normative) ✅ |
| `spec/t81-data-types.md` | 数据类型与序列化规范 | 规范性 (Normative) 🧮 |
| `spec/tisc-spec.md` | TISC 指令集 | 规范性 (Normative) 📡 |
| `docs/index.md` | 文档入口 | 信息性 (Informational) 📖 |

---

## 非目标 🚫

T81 **不是**：

* 硬件三进制加速器 🖥️
* C++/Python/Rust 的通用替代品 🛑
* 以牺牲确定性为代价追求最大吞吐量的系统 ⚡❌

---

## 📘 技术专题论文 (Definitive Technical Monograph)

如需了解架构的完整、规范级描述（包括形式化语义、确定性不变性、对抗建模和长期持续性设计），请参阅：

➡️ **[T81 基金会 — 技术专题论文](https://www.google.com/search?q=book/README.md)**

**阅读指南：**

* **初学者？** → 从第一部分开始。
* **开发者/实现者？** → 侧重第二和第三部分。
* **审计员？** → 仔细阅读第三和第四部分。
* **研究员？** → 重点阅读第四和第五部分。

<details>
<summary><strong>第一部分 — 基础</strong></summary>

1. 引言（范围、定义、愿景）
2. 核心原则与不变性（确定性、三进制逻辑、Axion 追踪、伦理强制）

</details>

<details>
<summary><strong>第二部分 — 确定性机器</strong></summary>

3. T81VM 架构（形式化状态机、内存布局、TISC ISA、故障语义）
4. 数据类型与规范化序列化（T81Float、dmath、张量布局）
5. 安装与构建验证
6. CLI 与 API 使用（C++/Python 嵌入）

</details>

<details>
<summary><strong>第三部分 — 治理与验证</strong></summary>

7. 验证与审计（形式化验证、属性测试、确定性网关）
8. Axion 安全内核（策略模型、指令拦截、审计日志）
9. 认知层与分布式计算（Trace-JIT、无限形式）

</details>

---

## 许可证

MIT 许可证 — 详见 [LICENSE](https://www.google.com/search?q=LICENSE)。

---

我还能为您做什么？例如**解释三进制算法的原理**，或者**协助您配置 T81 的开发环境**？
