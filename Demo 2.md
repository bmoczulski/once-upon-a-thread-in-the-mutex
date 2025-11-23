# Step 0 - build everything

```bash
make all
```

# Demo 2 - TSA

1. show account-TSA.cpp
2. show tsa.h (attributes)
3. run vanilla

```bash
./account-TSA
```

4. remove `REQUIRES(m)`, rebuild - TSA fails with error (others succeed)
5. revetr, then comment locks - TSA fails with error (others succeed)
6. revetr, then uncomment ABBA - TSA fails with error (others succeed)

```bash
./account-NO-TSA                             # no error, more dough
./account-tsan                               # runtime error (but continues!)
valgrind --tool=helgrind ./account-NO-TSA    # same as TSan
```

7. revert, changes lock_guard to unique_lock - TSA fails with error
