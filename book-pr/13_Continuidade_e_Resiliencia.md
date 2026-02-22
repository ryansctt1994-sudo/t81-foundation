# Capítulo 13: Continuidade e Resiliência

## 13.1 O Protocolo Cleanroom

**Status: Documentado**

O **Protocolo de Reconstrução Cleanroom** define o conjunto mínimo de etapas necessárias para reconstruir o sistema T81 do zero, assumindo a perda total de infraestrutura.

### 13.1.1 Bootstrap Mínimo
1.  **Código Fonte**: Uma cópia dos diretórios `src/` e `include/`.
2.  **Compilador**: Qualquer compilador compatível com C++23 (Clang 18+, GCC 14+).
3.  **Sistema de Build**: CMake 3.25+.

**Comando**:
```bash
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build --parallel
```

### 13.1.2 Verificação
Após a reconstrução, o sistema deve verificar a si mesmo contra um conjunto conhecido de artefatos `CanonHash81`.
```bash
./build/t81 repro-hash tests/fixtures/t81lang_determinism
```

## 13.2 Arquivamento de Longo Prazo

**Status: Aspiracional**

A meta do T81 é ser legível e executável em 50+ anos.
*   **Estabilidade de Formato**: Os formatos de fonte `.t81` e bytecode `.tisc` estão congelados.
*   **Dependências**: A VM central tem zero dependências de tempo de execução externas além da biblioteca padrão C++.
