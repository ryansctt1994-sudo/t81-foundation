# Фонд T81

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

**Детерминированный, Управляемый стек среды выполнения для проверяемых вычислений.**

T81 — это проект, направленный на повышение безопасности после версии 1.0, предоставляющий полностью детерминированный конвейер компиляции и выполнения (`T81Lang -> TISC -> HanoiVM`). Он отдает приоритет проверяемости, обеспечению соблюдения политик (Axion) и воспроизводимости, а не скорости работы оборудования.

## ⚡ Оценка за 30 секунд

Проверьте утверждения самостоятельно в 4 шага:

1. **Соберите и запустите Hello World**

``bash

cmake -S . -B build -DCMAKE_BUILD_TYPE=Release && cmake --build build --parallel

2. **Запуск Determinism Gate**

```bash

# Проверка воспроизводимости на разных архитектурах

python3 scripts/ci/t81lang_repro_gate.py --t81-bin build/t81 --check

```

3. **Запуск демонстрации виртуальной машины**

```bash

./build/t81_demo

```

4. **Проверка артефакта трассировки**

```bash

./build/t81 trace show trace.txt

```

---

## 🚫 Нецелевые задачи

Чтобы сэкономить ваше время, вот что T81 НЕ является:

* **НЕ является аппаратным ускорителем:** Мы не заявляем о тройном аппаратном ускорении. Это программная среда выполнения для обеспечения детерминированной корректности.

* **НЕ является заменой для общего назначения:** Мы фокусируемся на ответственной, проверяемой логике, а не заменяем C++ или Python для общих задач.

* **НЕ является «свободным подходом»:** Если оптимизация производительности нарушает детерминизм трассировки, мы её отклоняем.

--

## ❓ Почему это существует

Современные среды выполнения жертвуют воспроизводимостью ради скорости. T81 переворачивает это с ног на голову: **Проверяемость является основным ограничением.**

Мы обеспечиваем это посредством строгой архитектурной границы между языком/компилятором и средой выполнения, регулируемой явными контрактами.


[**Просмотр диаграммы архитектурных границ**](ARCHITECTURE.md#3-concurrent-workstream-view) | [**Просмотр контракта среды выполнения**](contracts/runtime-contract.json)

---

## 📚 Карта полномочий документа

| Документ | Назначение | Область полномочий |

| :--- | :--- | :--- |

| **[STATUS.md](STATUS.md)** | Что верно *сегодня* | Операционная истина |

| **[ROADMAP.md](ROADMAP.md)** | Перспективный план | Стратегический |

| **[VERSIONING.md](VERSIONING.md)** | Правила совместимости | Нормативные |

| **[spec/](spec/)** | Определение поведения | Нормативные |

| **[docs/EVIDENCE.md](docs/EVIDENCE.md)** | Подтверждение утверждений | Верификация |

---

## 🤝 Гарантии совместимости

* **Стабильная версия:** Синтаксис T81Lang, бинарный формат TISC, семантика выполнения HanoiVM.

* **Экспериментальная версия:** JIT-компиляция, распределенные тензорные операции.

* **Семантическая версия:** Мы используем семантическое версионирование. Изменения, нарушающие совместимость **стабильных** компонентов, увеличивают основную версию.

---

## 🖥️ Поддерживаемые платформы

| Платформа | Компилятор | Статус |

| :--- | :--- | :--- |

| **Linux (x86_64)** | Clang 18+, GCC 14+ | ✅ Детерминированный вентиль |

| **Linux (ARM64)** | Clang 18+ | ✅ Детерминированный шлюз |

| **macOS (ARM64)** | Apple Clang | ✅ Поддерживается |

---

## Быстрый старт (полная версия)

```bash
git clone https://github.com/t81dev/t81-foundation.git
cd t81-foundation
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build --parallel
ctest --test-dir build --output-on-failure
```

Безопасный однопоточный режим:
```bash
cmake --build build --parallel 1
ctest --test-dir build --output-on-failure -j1
```

## Интерфейс командной строки
Распространенные рабочие процессы:
```bash
# Компиляция / запуск
t81 compile examples/hello_world.t81 -o build/hello.tisc
t81 run build/hello.tisc

# Проверка / отладка
t81 disasm build/hello.tisc

t81 debug build/hello.tisc

# Диагностика / воспроизводимость
t81 check examples/hello_world.t81
t81 repro-hash tests/fixtures/t81lang_determinism

# Трассировка рабочих процессов
t81 trace show trace.txt
t81 trace diff trace_a.txt trace_b.txt
t81 trace replay build/hello.tisc trace.txt
```

Инструменты моделирования:
```bash
t81 weights import model.safetensors -o model.t81w
t81 weights info model.t81w
t81 weights quantize model.safetensors --to-gguf model.gguf
```

Полная справка по команде:
```bash
t81 help
```

## Карта репозитория
- [`include/t81/`](include/t81/): public A
