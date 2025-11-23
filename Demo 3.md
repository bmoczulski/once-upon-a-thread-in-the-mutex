# Step 0 - build everything

```bash
make all
```

# Demo 3 - synchronized_value

1. show sv.cpp
2. `addAmount()` will always receive locked reference (how cool!)
2. remove `IF_BM_SV` (keep plain `const`) - GCC fails, BM still compiles

```bash
./sv-bm
```