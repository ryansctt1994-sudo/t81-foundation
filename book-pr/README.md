# A Fundação T81 — Monografia Técnica Definitiva

## Prefácio

Existem duas maneiras de construir sistemas.

Uma é otimizar pela conveniência — mover-se rapidamente, aproximar, aceitar que o bit final pode variar, que a deriva de ponto flutuante é tolerável, que compiladores podem reordenar, que o hardware decidirá o que significa "próximo o suficiente".

A outra é insistir que a computação não é sugestão, mas declaração.

O T81 pertence ao segundo caminho.

Em seu núcleo, este projeto não é sobre aritmética ternária, máquinas virtuais ou motores de política — embora contenha todos esses. É sobre **integridade de execução**. É sobre desenhar um limite ao redor de um processo computacional e dizer: dentro deste limite, o comportamento não é incidental.

O determinismo é frequentemente tratado como uma troca de desempenho ou uma conveniência de depuração. Aqui é tratado como uma restrição civilizacional. Se duas máquinas não podem concordar sobre o resultado do mesmo programa, então a computação nunca foi verdadeiramente definida — ela foi apenas executada.

Ternário balanceado, serialização canônica, matemática definida por software, registro de rastreamento, aplicação de políticas — estas não são escolhas estéticas. Elas são instrumentos em um único argumento:

> Uma computação deve ser reprodutível, auditável e estruturalmente honesta.

Sistemas modernos são construídos com camadas de abstração que escondem transições de estado atrás de otimizadores, execução especulativa, peculiaridades de ponto flutuante e efeitos colaterais implícitos. O T81 tenta algo diferente: tornar cada transição explícita, cada representação canônica, cada execução rastreável.

É um experimento arquitetural em restrição.

O sistema não assume hardware benevolente.
Ele não assume bibliotecas de ponto flutuante idênticas.
Ele não assume que compiladores se comportam da mesma forma entre arquiteturas.
Ele não assume que a execução sem registro é aceitável.

Em vez disso, ele codifica regras:

* Transições de estado devem ser definíveis.
* Dados devem ter uma única forma canônica.
* O consumo de recursos deve ser contabilizável.
* Políticas devem ser aplicáveis.
* O comportamento deve ser reproduzível.

O resultado não é a máquina mais rápida.
Não é o ambiente mais flexível.
Não é projetado para substituir ecossistemas de script de propósito geral.

É projetado para responder a uma pergunta mais estreita, mas mais exigente:

**Pode um sistema de software ser construído de tal forma que seu comportamento seja comprovadamente invariante através do espaço e do tempo?**

Este livro existe para documentar essa tentativa.

Não como mitologia.
Não como marketing.
Mas como um livro-razão.

Cada subsistema descrito aqui — T81Lang, TISC, a T81VM, Axion, CanonFS, os portões de determinismo, os níveis cognitivos — é parte de uma estrutura em camadas construída em torno de um invariante:

> Entradas idênticas devem produzir saídas idênticas, sob regras explicitamente definidas.

Se esta arquitetura se tornará amplamente adotada é secundário. O que importa é que ela foi tornada concreta, implementada, testada e descrita com precisão suficiente para que possa ser compreendida, verificada ou desafiada por outros.

Este volume é, portanto, tanto técnico quanto filosófico.

É técnico porque descreve um sistema em funcionamento.
É filosófico porque afirma que a reprodutibilidade não é opcional em certos domínios.

Se o repositório evoluir, este livro deve evoluir com ele.
Se o projeto cessar, este documento deve permanecer suficiente para reconstruir o que foi tentado e por quê.

No final, o T81 não é uma reivindicação de perfeição.

É um compromisso com a restrição.

E a restrição, quando aplicada deliberadamente, é uma forma de clareza.

---

## Como Ler Este Livro

* **Novo no T81?** → Comece pela Parte I, depois Parte II.
* **Implementador?** → Foque nas Partes II e III.
* **Auditor?** → Leia as Partes III e IV cuidadosamente.
* **Pesquisador?** → Dê ênfase às Partes IV e V.
* **Mantenedor de longo prazo?** → As Partes IV e V são críticas.

---

## Navegação

<details open>
<summary><strong>Parte I — Fundamentos</strong></summary>

1. **[Introdução](./01_Introducao.md)**

   * [1.1 Escopo e Definição](./01_Introducao.md#11-escopo-e-definicao)
   * [1.2 Arquitetura do Sistema](./01_Introducao.md#12-arquitetura-do-sistema)
   * [1.3 Missão de Computação Verificável](./01_Introducao.md#13-missao-de-computacao-verificavel)

2. **[Princípios Centrais e Invariantes](./02_Principios.md)**

   * [2.1 O Invariante de Determinismo](./02_Principios.md#21-o-invariante-de-determinismo)
   * [2.1.1 Superfícies de Determinismo e Vetores de Ataque](./02_Principios.md#211-superficies-de-determinismo-e-vetores-de-ataque)
   * [2.2 Lógica Ternária (Base-3)](./02_Principios.md#22-logica-ternaria-base-3)
   * [2.3 Auditabilidade e o Trace Axion](./02_Principios.md#23-auditabilidade-e-o-trace-axion)
   * [2.4 Os Nove Princípios (Aplicação de Ética)](./02_Principios.md#24-os-nove-principios-aplicacao-de-etica)

</details>

---

<details>
<summary><strong>Parte II — A Máquina Determinística</strong></summary>

3. **[Arquitetura T81VM](./03_Arquitetura.md)**

   * [3.1 Máquina de Estados Formal](./03_Arquitetura.md#31-maquina-de-estados-formal)
   * [3.1.1 Definição de Estado](./03_Arquitetura.md#311-definicao-de-estado)
   * [3.2 Layout de Memória](./03_Arquitetura.md#32-layout-de-memoria)
   * [3.3 Arquivo de Registradores](./03_Arquitetura.md#33-arquivo-de-registradores)
   * [3.4 Arquitetura do Conjunto de Instruções TISC (ISA)](./03_Arquitetura.md#34-arquitetura-do-conjunto-de-instrucoes-tisc-isa)
   * [3.5 Semântica de Falhas](./03_Arquitetura.md#35-semantica-de-falhas)
   * [3.6 Coleta de Lixo (Garbage Collection)](./03_Arquitetura.md#36-coleta-de-lixo-garbage-collection)

4. **[Tipos de Dados e Serialização Canônica](./04_Tipos_de_Dados_e_Serializacao.md)**

   * [4.1 Tipos Primitivos](./04_Tipos_de_Dados_e_Serializacao.md#41-tipos-primitivos)
   * [4.2 T81Float e dmath](./04_Tipos_de_Dados_e_Serializacao.md#42-t81float-e-dmath)
   * [4.3 Tensores e Layouts Canônicos](./04_Tipos_de_Dados_e_Serializacao.md#43-tensores-e-layouts-canonicos)
   * [4.4 Regras de Serialização Canônica](./04_Tipos_de_Dados_e_Serializacao.md#44-regras-de-serializacao-canonica)

5. **[Instalação e Verificação de Build](./05_Instalacao.md)**

   * [5.1 Pré-requisitos](./05_Instalacao.md#51-pre-requisitos)
   * [5.2 Compilando a partir da Fonte](./05_Instalacao.md#52-compilando-a-partir-da-fonte)
   * [5.3 Verificando o Build](./05_Instalacao.md#53-verificando-o-build)

6. **[Uso de CLI e API](./06_Uso.md)**

   * [6.1 Interface de Linha de Comando](./06_Uso.md#61-interface-de-linha-de-comando)
   * [6.2 Embarcando T81 (API C++)](./06_Uso.md#62-embarcando-t81-api-c)
   * [6.3 Embarcando T81 (API Python)](./06_Uso.md#63-embarcando-t81-api-python)
   * [6.4 Depuração](./06_Uso.md#64-depuracao)

</details>

---

<details>
<summary><strong>Parte III — Governança e Verificação</strong></summary>

7. **[Verificação e Auditoria](./07_Verificacao_e_Auditoria.md)**

   * [7.1 Metodologia de Verificação Formal](./07_Verificacao_e_Auditoria.md#71-metodologia-de-verificacao-formal)
   * [7.2 A Matriz de Auditoria Formal](./07_Verificacao_e_Auditoria.md#72-a-matriz-de-auditoria-formal)
   * [7.3 Testes Baseados em Propriedades](./07_Verificacao_e_Auditoria.md#73-testes-baseados-em-propriedades)
   * [7.4 O Portão de Determinismo (Determinism Gate)](./07_Verificacao_e_Auditoria.md#74-o-portao-de-determinismo-determinism-gate)

8. **[O Kernel de Segurança Axion](./08_O_Kernel_Axion.md)**

   * [8.1 Definição Formal](./08_O_Kernel_Axion.md#81-definicao-formal)
   * [8.2 O Modelo de Política](./08_O_Kernel_Axion.md#82-o-modelo-de-politica)
   * [8.3 Interceptação de Instruções](./08_O_Kernel_Axion.md#83-interceptacao-de-instrucoes)
   * [8.4 O Log de Auditoria (Trace)](./08_O_Kernel_Axion.md#84-o-log-de-auditoria-trace)
   * [8.5 Promoção Cognitiva](./08_O_Kernel_Axion.md#85-promocao-cognitiva)

9. **[Níveis Cognitivos e Computação Distribuída](./09_Niveis_Cognitivos_e_Computacao_Distribuida.md)**

   * [9.1 O Modelo de Nível Cognitivo](./09_Niveis_Cognitivos_e_Computacao_Distribuida.md#91-o-modelo-de-nivel-cognitivo)
   * [9.2 Computação Distribuída (Nível 4)](./09_Niveis_Cognitivos_e_Computacao_Distribuida.md#92-computacao-distribuida-nivel-4)
   * [9.3 Compilação JIT Baseada em Trace](./09_Niveis_Cognitivos_e_Computacao_Distribuida.md#93-compilacao-jit-baseada-em-trace)
   * [9.4 Formas Infinitas (Nível 5)](./09_Niveis_Cognitivos_e_Computacao_Distribuida.md#94-formas-infinitas-nivel-5)

10. **[Apêndices](./10_Apendices.md)**

* [10.1 O Que Ainda Não Foi Implementado](./10_Apendices.md#101-o-que-ainda-nao-foi-implementado)
* [10.2 Modelo de Ameaça e Superfície de Ataque ao Determinismo](./10_Apendices.md#102-modelo-de-ameaca-e-superficie-de-ataque-ao-determinismo)
* [10.3 Glossário](./10_Apendices.md#103-glossario)

</details>

---

<details>
<summary><strong>Parte IV — Formalização e Endurecimento Estrutural</strong></summary>

11. **[Semântica Formal de TISC e T81VM](./11_Semantica_Formal.md)**

* [Semântica Denotacional de TISC](./11_Semantica_Formal.md#semantica-denotacional-de-tisc)
* [Função de Transição Algébrica δ](./11_Semantica_Formal.md#funcao-de-transicao-algebrica-δ)
* [Sistema de Reescrita de Canonicalização](./11_Semantica_Formal.md#sistema-de-reescrita-de-canonicalizacao)
* [Esboços de Prova de Determinismo](./11_Semantica_Formal.md#esbocos-de-prova-de-determinismo)
* [Equivalência entre Intérprete e Trace-JIT](./11_Semantica_Formal.md#equivalencia-entre-interprete-e-trace-jit)

12. **[Modelagem Adversária e Ataques ao Determinismo](./12_Modelagem_Adversaria.md)**

* [Ataques ao Nível do Compilador](./12_Modelagem_Adversaria.md#ataques-ao-nivel-do-compilador)
* [Vetores de Ataque em VM e GC](./12_Modelagem_Adversaria.md#vetores-de-ataque-em-vm-e-gc)
* [Ataques a CanonFS e Hash](./12_Modelagem_Adversaria.md#ataques-a-canonfs-e-hash)
* [Ataque de Viagem no Tempo em Nível Distribuído](./12_Modelagem_Adversaria.md#ataque-de-viagem-no-tempo-em-nivel-distribuido)
* [Template de Post-mortem de Violação de Determinismo](./12_Modelagem_Adversaria.md#template-de-post-mortem-de-violacao-de-determinismo)

</details>

---

<details>
<summary><strong>Parte V — Continuidade e Horizonte de Pesquisa</strong></summary>

13. **[Continuidade e Resiliência](./13_Continuidade_e_Resiliencia.md)**

* [Protocolo de Reconstrução em Ambiente Limpo (Cleanroom)](./13_Continuidade_e_Resiliencia.md#protocolo-de-reconstrucao-em-ambiente-limpo-cleanroom)
* [Pontos Únicos de Falha](./13_Continuidade_e_Resiliencia.md#pontos-unicos-de-falha)
* [Manifesto de Continuidade](./13_Continuidade_e_Resiliencia.md#manifesto-de-continuidade)
* [Invariantes Formais Imutáveis](./13_Continuidade_e_Resiliencia.md#invariantes-formais-imutaveis)

14. **[Fronteira de Pesquisa](./14_Fronteira_de_Pesquisa.md)**

* [Aceleração de Hardware Ternário](./14_Fronteira_de_Pesquisa.md#aceleracao-de-hardware-ternario)
* [Caminhos de Verificação Formal](./14_Fronteira_de_Pesquisa.md#caminhos-de-verificacao-formal)
* [CanonFS como Substrato Merkle](./14_Fronteira_de_Pesquisa.md#canonfs-como-substrato-merkle)
* [Inferência de IA Determinística em Escala](./14_Fronteira_de_Pesquisa.md#inferencia-de-ia-deterministica-em-escala)

</details>
