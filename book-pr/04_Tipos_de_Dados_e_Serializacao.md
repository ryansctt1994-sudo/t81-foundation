# Capítulo 4: Tipos de Dados e Serialização

## 4.1 Tipos Primitivos

**Status: Implementado e Testado**

A arquitetura T81 é construída sobre uma fundação de primitivas ternárias balanceadas. Esses tipos são projetados para serem eficientemente simulados em hardware binário enquanto mantêm as propriedades matemáticas da lógica de base-3.

### 4.1.1 Trits e Trytes
*   **Trit**: O átomo fundamental de informação, assumindo valores $\{-1, 0, 1\}$.
*   **Tryte**: Uma sequência de trits. A largura padrão do tryte é 4 trits ($3^4 = 81$ valores), frequentemente empacotado em um `uint8_t` para armazenamento.

> **Implementação**: `include/t81/ternary.hpp` define o enum `Trit` e a lógica de conversão.

### 4.1.2 T81Int (Inteiro de Precisão Arbitrária)
`T81Int` é um tipo inteiro de largura variável usando uma representação ternária balanceada empacotada.
*   **Armazenamento**: 2 bits por trit.
*   **Intervalo**: Simétrico em torno de zero ($-\frac{3^N-1}{2} \dots +\frac{3^N-1}{2}$).
*   **Normalização**: Zeros à esquerda são estritamente proibidos na forma serializada canônica. Um valor zero é representado por um único trit zero.

> **Verificação**: `tests/cpp/test_t81int.cpp` e `tests/cpp/test_property_invariants.cpp`.

## 4.2 T81Float e dmath

**Status: Implementado (Core) / Parcial (Estendido)**

A aritmética de ponto flutuante é a principal fonte de não-determinismo na computação multiplataforma (devido a variações IEEE-754 na fusão FMA, precisão transcendental, etc.). O T81 aborda isso via `T81Float`.

### 4.2.1 Definição Canônica
Um `T81Float` é uma tupla $(m, e)$, representando o valor $m \times 3^e$.
*   $m$: Mantissa (T81Int).
*   $e$: Expoente (T81Int).
*   **Invariante**: A mantissa $m$ deve ser normalizada de tal forma que seu trit mais significativo seja não-zero, a menos que o valor seja exatamente zero.

### 4.2.2 O Backend dmath
Para alcançar **Determinismo Estrito**, a VM emprega `dmath` (Matemática Determinística), uma biblioteca aritmética definida por software.
*   **Operações Core**: `Add`, `Sub`, `Mul` são exatas e determinísticas (implementadas em `T81Float.hpp`).
*   **Transcendentais**: `Sin`, `Cos`, `Tan`, `Exp`, `Log`, `Sqrt` são computados usando `dmath` (série de Taylor com contagens fixas de iteração), garantindo resultados bit-exact em qualquer arquitetura.
*   **Funções Estendidas**: `Asin`, `Acos`, `Sinh`, `Pow` atualmente dependem da precisão `double` do hospedeiro (a menos que `T81_DETERMINISTIC` seja definido, caso em que podem retornar `NaE` ou usar emulação de software lenta).

> **Verificação**: `tests/cpp/test_T81Float.cpp` valida a correção de valores especiais e transcendentais. `include/t81/core/detail/dmath.hpp` contém a implementação.

## 4.3 Tensores e Layouts Canônicos

**Status: Implementado e Testado**

Tensores (`T729Tensor`, `T81Tensor`) são os cavalos de batalha dos níveis cognitivos.

### 4.3.1 Layout de Memória
Tensores são armazenados em ordem **Row-Major** (linha principal).
*   **Forma (Shape)**: Um vetor de dimensões $(d_0, d_1, \dots, d_n)$.
*   **Stride**: Calculado como $s_i = \prod_{j=i+1}^n d_j$.
*   **Alinhamento**: Dados de tensor são alinhados a limites de 64 bytes no segmento de memória `Tensor`.

### 4.3.2 Serialização (.t81w)
O formato `.t81w` (Pesos T81) é o container padrão para persistir modelos de tensor. A Versão 2 (`T81W2`) suporta quantização e hash canônico.

**Estrutura Binária**:
1.  **Cabeçalho Mágico**: `0x54383157` ("T81W").
2.  **Versão**: `0x02`.
3.  **Tabela de Conteúdos**: Lista de tuplas `(Hash, Offset, Length)`.
4.  **Dados de Blob**: Dados de tensor contíguos.

**Formatos de Quantização**:
*   **F32**: Float IEEE-754 padrão (canonicalizado).
*   **T3_K**: Empacotamento de 2 bits por trit com escala por bloco.

> **Fonte**: `include/t81/weights.hpp` e `include/t81/tensor.hpp`.

## 4.4 Regras de Serialização Canônica

**Status: Implementado**

Para garantir hash consistente (`CanonRef`), todos os dados devem ser normalizados antes da serialização.

1.  **BigInt**: Remover zeros à esquerda. Zero é `[0]`.
2.  **Fração**:
    *   Reduzir aos termos mais baixos: $\gcd(num, den) = 1$.
    *   Denominador deve ser positivo.
    *   Zero é $0/1$.
3.  **Float**:
    *   Padronizar mantissa/expoente.
    *   Payloads NaN são zerados.
    *   Zero negativo é normalizado para zero positivo.
4.  **Mapa/Dicionário**:
    *   Chaves devem ser ordenadas lexicograficamente por sua representação binária canônica.
5.  **Grafo**:
    *   Nós são reindexados por ordem de classificação topológica para garantir que isomorfismo de grafo produza fluxos de bytes idênticos.

> **Verificação**: `tests/cpp/test_property_invariants.cpp` verifica essas propriedades de normalização via testes baseados em propriedades.
