Installation
First you have to compile the kaleidoscope compiler itself. Create a directory outside of the git repo and cd into it.

```c++
cmake ~/path/to/kaleidoscope/gitrepo && make
Usage
The kaleidoscope binary is located in the src folder. Currently kaleidoscope only parses the input and shows which expression was parsed.
```

$ ./kaleidoscope
ready> def foo(x y) x+foo(y, 4.0);
Parsed a function definition.
ready> def foo(x y) x+y y;
Parsed a function definition.
Parsed a top-level expr
ready> def foo(x y) x+y );
Parsed a function definition.
Error: unknown token when expecting an expression
ready> extern sin(a);
Parsed an extern
ready> ^D
$