Fundación T81

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

**Determinista Pila de tiempo de ejecución gobernada para computación auditable.**

T81 es un proyecto de refuerzo posterior a la versión 1.0 que ofrece una canalización de compilación y ejecución totalmente determinista (`T81Lang -> TISC -> HanoiVM`). Prioriza la auditabilidad, la aplicación de políticas (Axion) y la reproducibilidad sobre la velocidad bruta del hardware.

## ⚡ Evaluación de 30 segundos

Verifique las afirmaciones usted mismo en 4 pasos:

1. **Compilar y ejecutar Hello World**
```bash
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release && cmake --build build --parallel
./build/t81 compile examples/hello_world.t81 -o hello.tisc
./build/t81 run hello.tisc
```

2. **Ejecutar la puerta de determinismo**
```bash
# Verificar el hash de reproducibilidad entre arquitecturas
python3 scripts/ci/t81lang_repro_gate.py --t81-bin build/t81 --check
```

3. **Ejecutar una demostración de máquina virtual**
```bash
./build/t81_demo
```

4. **Inspeccionar un artefacto de seguimiento**
```bash
./build/t81 trace show trace.txt
```

---

## 🚫 Objetivos no definidos

Para ahorrar tiempo, aquí tiene Lo que T81 **NO** es:

* **NO es un acelerador de hardware:** No afirmamos mejoras de velocidad por hardware ternarias. Este entorno de ejecución de software busca la corrección determinista.
* **NO es un sustituto de propósito general:** Nos centramos en la lógica auditable de alto riesgo, sin sustituir a C++ o Python para tareas generales.
* **NO es una optimización flexible:** Si una optimización del rendimiento rompe el determinismo de trazas, la rechazamos.

--

## ❓ ¿Por qué existe?

Los entornos de ejecución modernos sacrifican la reproducibilidad por la velocidad. T81 lo invierte: **La auditabilidad es la principal restricción.**

Lo aplicamos mediante un estricto límite arquitectónico entre el lenguaje/compilador y el entorno de ejecución, regido por contratos explícitos.

[**Ver diagrama de límites arquitectónicos**](ARCHITECTURE.md#3-concurrent-workstream-view) | [**Ver Contrato de Tiempo de Ejecución**](contracts/runtime-contract.json)

---

## 📚 Mapa de Autoridades del Documento

| Documento | Propósito | Ámbito de Autoridad |
| :--- | :--- | :--- |
| **[STATUS.md](STATUS.md)** | Lo que es cierto *hoy* | Verdad Operacional |
| **[ROADMAP.md](ROADMAP.md)** | Plan a futuro | Estratégico |
| **[VERSIONING.md](VERSIONING.md)** | Reglas de compatibilidad | Normativo |
| **[spec/](spec/)** | Definición de comportamiento | Normativo |
| **[docs/EVIDENCE.md](docs/EVIDENCE.md)** | Prueba de reivindicaciones | Verificación |

---

## 🤝 Garantías de compatibilidad

* **Estable:** Sintaxis T81Lang, formato binario TISC, semántica de ejecución HanoiVM.
* **Experimental:** Compilación JIT, operaciones tensoriales distribuidas.
* **SemVer:** Seguimos el control de versiones semántico. Los cambios importantes en los componentes **Estables** incrementan la versión principal.

---

## 🖥️ Plataformas compatibles

| Plataforma | Compilador | Estado |
| :--- | :--- | :--- |
| **Linux (x86_64)** | Clang 18+, GCC 14+ | ✅ Puerta de determinismo |
| **Linux (ARM64)** | Clang 18+ | ✅ Puerta de determinismo |
| **macOS (ARM64)** | Apple Clang | ✅ Compatible |

---

## Inicio rápido (completo)

```bash
git clone https://github.com/t81dev/t81-foundation.git
cd t81-foundation
cmake -S . -B compilación -DCMAKE_BUILD_TYPE=Versión
cmake --build compilación --paralelo
ctest --test-dir compilación --salida-en-fallo
```

Modo seguro de un solo subproceso:
```bash
cmake --build compilación --paralelo 1
ctest --test-dir compilación --salida-en-fallo -j1
```

## Superficie CLI
Flujos de trabajo comunes:
```bash
# Compilar / ejecutar
t81 compilar ejemplos/hello_world.t81 -o compilar/hello.tisc
t81 ejecutar compilación/hello.tisc

# Inspeccionar / depurar
t81 desensamblar compilación/hello.tisc
t81 depurar compilación/hello.tisc

# Diagnóstico / reproducibilidad
t81 comprobar ejemplos/hello_world.t81
t81 repro-hash pruebas/fixtures/t81lang_determinism

# Flujos de trabajo de seguimiento
t81 trace show trace.txt
t81 trace diff trace_a.txt trace_b.txt
t81 trace replay build/hello.tisc trace.txt
```

Herramientas del modelo:
```bash
t81 weights import model.safetensors -o model.t81w
t81 weights info model.t81w
t81 weights quantize model.safetensors --to-gguf model.gguf
```

Consultar la ayuda completa del comando:
```bash
t81 help
```

## Mapa del repositorio
- [`include/t81/`](include/t81/): public API Headers
- [`src/`](src/): frontend, TISC, VM, Axion, CanonFS, implementación de CLI
- [`tests/`](tests/): conformidad, determinismo, VM/e2e, segmentos de propiedades
- [`docs/`](docs/): guías, estado, benchmarks, documentación sobre límites de tiempo de ejecución
- [`spec/`](spec/): semántica normativa y entradas de gobernanza
- [`examples/`](examples/): ejemplos y demostraciones ejecutables

## Límite de tiempo de ejecución
T81 utiliza un contrato explícito de límite de tiempo de ejecución:
- Marcador: [`contracts/runtime-contract.json`](contracts/runtime-contract.json)
- Política de límites: [`docs/runtime-semantics-boundary.md`](docs/runtime-semantics-boundary.md)

## Lecturas adicionales
- [`ARCHITECTURE.md`](ARCHITECTURE.md)
- [`docs/system-integration.md`](docs/system-integration.md)
- [`ANALYSIS.md`](ANALYSIS.md)
- [`CHANGELOG.md`](CHANGELOG.md)
- [`docs/research-guide.md`](docs/research-guide.md)
- [`docs/ai-quickstart.md`](docs/ai-quickstart.md)

## Licencia
Este repositorio está licenciado bajo el MIT (véase [`LICENSE`](LICENSE)).
