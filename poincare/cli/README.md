# Command Line Interface for poincare

This is a dev tool to evaluate expressions using poincare with a simple,
scriptable and comfortable command line interface.

### Interactive use
```
$ make -f poincare/cli/Makefile poincare_cli.run
> simplify 2x-x
o0 = x
> log o0
<UserSymbol size="4" address="0x60000039c010" value="x"/>
> ^D
Exiting.
```

Tips:
Line edition is provided by libedit, the usual shell keybindings should be working.
The history is saved in `~/.poincare_history` between sessions.
Commands may be abbreviated if the prefix is unique e.g. `s` = `simplify`.
You can now experiment with poincare directly on bob or inside a container.

### Interactive debug
```lldb
$ lldb ./output/debug/macos/arm64/poincare_cli.bin
(lldb) target create "./output/debug/macos/arm64/poincare_cli.bin"
Current executable set to 'output/debug/macos/arm64/poincare_cli.bin' (arm64).
(lldb) b DeepExpand
Breakpoint 1: where = poincare_cli.bin`Poincare::Internal::AdvancedReduction::DeepExpand(Poincare::Internal::Tree*) + 16 at advanced_reduction.h:28:62, address = 0x000000010017f8d4
(lldb) r
Process 17314 launched: 'output/debug/macos/arm64/poincare_cli.bin' (arm64)
> expand (x+y)^3
Process 17314 stopped
* thread #1, stop reason = breakpoint 1.1
    frame #0: 0x000000010017f8d4 poincare_cli.bin`Poincare::Internal::AdvancedReduction::DeepExpand(e=0x00000001001c4960) at advanced_reduction.h:28:62
   27  	  // Top-Bottom deep expand
-> 28  	  static bool DeepExpand(Tree* e) { return PrivateDeepExpand(e); };
   29  	  // Top-Bottom deep expand using only algebraic operations
```

If you went too far and want to restart you can just press Up to get the previous expression back.
```lldb
(lldb) r
[Press Up]
> expand (x+y)^3
```


### Command line use
```
$ ./output/debug/macos/arm64/poincare_cli.bin approx 2pi
6.2831853071796
```

Tip:
Alias it as `pc` and use it as your daily calculator !
If you don't specify the operation, it defaults to `simplify`.
Mind the shell expansions with `(`,`)` and `*`. Wrap with `""` if needed.

### Batch use
```
$ cat in
s acos(-1/√(2))
s acos(1/√(2))
s acos(-(√(6)+√(2))/4)

$ ./output/debug/macos/arm64/poincare_cli.bin < in > out
$ cat out
(3×π)/4
π/4
(11×π)/12
```

Tip:
Use it to compare versions and platforms:
```
$ diff out_master out
1c1
< (3×π)/4
---
> (π×3)/4
3c3
< (11×π)/12
---
> (π×11)/12
```

### Bisect
Use the `bisect.sh` helper script to find when an expression result has changed.
For instance if 2+2 stopped being equal to 4, run:

```
$ git bisect start
$ git bisect bad HEAD
$ git bisect good master
$ git bisect run ./poincare/cli/bisect.sh simplify 2+2 4
running  './poincare/cli/bisect.sh' 's' '2+2' '4'
[...]
CXX     arm64/poincare/src/expression/number.o
AR      arm64/poincare.nostorage.a
LD      arm64/poincare_cli.bin
d8fee27c99b3bdcbe89bef3f63d0ed6577884f01 is the first bad commit
commit d8fee27c99b3bdcbe89bef3f63d0ed6577884f01
Author: MarcNumworks
Date:   Thu May 15 16:57:13 2025 +0200
   perf(poincare): optimize away additions

 poincare/src/expression/number.cpp | 1 +
 1 file changed, 1 insertion(+)
bisect found first bad commit

$ ./output/debug/macos/arm64/poincare_cli.bin s 2+2
5
```

### TODO
- add commands to get the dimension, the sign, dump the tree stack
- get timing and stack usage info (like ipython's timeit)
- set context, proposal `s cos(90) deg real`
- step through the advance reduction steps
- change poincare limits
- save and reload the history
