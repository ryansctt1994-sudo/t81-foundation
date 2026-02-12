# Fundação T81

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

**Determinístico, Pilha de tempo de execução governada para computação auditável.**

T81 é um projeto de reforço de segurança pós-v1.0 que oferece um pipeline de compilação e execução totalmente determinístico (`T81Lang -> TISC -> HanoiVM`). Ele prioriza a auditabilidade, a aplicação de políticas (Axion) e a reprodutibilidade em detrimento da velocidade bruta do hardware.

## ⚡ Avaliação em 30 segundos

Verifique você mesmo as afirmações em 4 etapas:

1. **Compile e execute Hello World**

``bash

cmake -S . -B build -DCMAKE_BUILD_TYPE=Release && cmake --build build --parallel

./build/t81 compile examples/hello_world.t81 -o hello.tisc

./build/t81 run hello.tisc

``

2. **Executar o Determinismo**

``bash

# Verificar o hash de reprodutibilidade entre arquiteturas
python3 scripts/ci/t81lang_repro_gate.py --t81-bin build/t81 --check

``

3. **Executar uma Demonstração em VM**

``bash

./build/t81_demo

```

4. **Inspecionar um Artefato de Rastreamento**

``bash

./build/t81 trace show trace.txt

```

---

## 🚫 Objetivos Não Específicos

Para economizar seu tempo, aqui está o que o T81 **NÃO**:

* **NÃO é um acelerador de hardware:** Não reivindicamos ganhos de velocidade ternários por hardware. Este é um ambiente de execução de software para correção determinística.

* **NÃO é um substituto de propósito geral:** Nosso foco é em lógica auditável de alto risco, não em substituir C++ ou Python para tarefas gerais.

* **NÃO é "rápido e flexível":** Se uma otimização de desempenho quebrar o determinismo de rastreamento, nós a rejeitamos.

---

## ❓ Por que isso existe

Os ambientes de execução modernos trocam reprodutibilidade por velocidade. O T81 inverte isso: **Auditabilidade é a principal restrição.**

Impomos isso por meio de um limite arquitetônico estrito entre a Linguagem/Compilador e o Ambiente de Execução, regido por contratos explícitos.

[**Ver Diagrama de Limites Arquitetônicos**](ARCHITECTURE.md#3-concurrent-workstream-view) | [**Visualizar Contrato de Tempo de Execução**](contracts/runtime-contract.json)

---

## 📚 Mapa de Autoridade do Documento

| Documento | Propósito | Escopo da Autoridade |

| :--- | :--- | :--- |

| **[STATUS.md](STATUS.md)** | O que é verdade *hoje* | Verdade Operacional |

| **[ROADMAP.md](ROADMAP.md)** | Plano futuro | Estratégico |

| **[VERSIONING.md](VERSIONING.md)** | Regras de compatibilidade | Normativo |

| **[spec/](spec/)** | Definição comportamental | Normativo |

| **[docs/EVIDENCE.md](docs/EVIDENCE.md)** | Prova de alegações | Verificação |

---

## 🤝 Garantias de Compatibilidade

* **Estável:** Sintaxe T81Lang, Formato Binário TISC, Semântica de Execução da HanoiVM.

* **Experimental:** Compilação JIT, Operações de Tensor Distribuídas.

* **SemVer:** Seguimos o Versionamento Semântico. Alterações que quebram a compatibilidade com componentes **Estáveis** incrementam a versão principal.

---

## 🖥️ Plataformas Suportadas

| Plataforma | Compilador | Status |

| :--- | :--- | :--- |

| **Linux (x86_64)** | Clang 18+, GCC 14+ | ✅ Determinismo |

| **Linux (ARM64)** | Clang 18+ | ✅ Determinismo |

| **macOS (ARM64)** | Apple Clang | ✅ Suportado |

---

## Início Rápido (Completo)

```bash
git clone https://github.com/t81dev/t81-foundation.git
cd t81-foundation
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build --parallel
ctest --test-dir build --output-on-failure
```

Modo seguro de thread única:
```bash
cmake --build build --parallel 1
ctest --test-dir build --output-on-failure -j1
```

## Interface de linha de comando
Fluxos de trabalho comuns:
```bash
# Compilar / executar
t81 compile examples/hello_world.t81 -o build/hello.tisc
t81 run build/hello.tisc

# Inspecionar / depurar
t81 disasm build/hello.tisc
t81 debug build/hello.tisc

# Diagnóstico / reprodutibilidade
t81 check examples/hello_world.t81
t81 repro-hash testes/fixtures/t81lang_determinism

# Fluxos de trabalho de rastreamento
t81 trace show trace.txt
t81 trace diff trace_a.txt trace_b.txt
t81 trace replay build/hello.tisc trace.txt
```

Ferramentas de modelo:
```bash
t81 weights import model.safetensors -o model.t81w
t81 weights info model.t81w
t81 weights quantize model.safetensors --to-gguf model.gguf
```

Consulte a ajuda completa do comando:
```bash
t81 help
```

## Mapa do repositório
- [`include/t81/`](include/t81/): public A
