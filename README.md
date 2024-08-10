# wasm analyzer: static analyzer framework for wasm

## TLDR

```bash
cmake -B build .
cmake --build build
./build/src/wasm-analyzer --help
```

```supported pass
  --Printer
  --HighFrequencySubExpr
```

## feature roadmap

- [ ] control flow constructor
  - [x] Basic Block
  - [x] Extend Basic Block
  - [ ] Dominate
- [ ] Local Optimization
  - [ ] value numbering
  - [ ] tree-height balancing
