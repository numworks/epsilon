---
title: Software Engineering
breadcrumb: Software
---
# Epsilon

Epsilon is a high-performance graphing calculator operating system. It includes eight apps that cover the high school mathematics curriculum.

## Build your own version of Epsilon

First of all, you should learn [how to build and run](build/) your very own version of Epsilon. Note that you don't need an actual NumWorks calculator to do this. Indeed, Epsilon can be compiled as a standalone application that will run on your computer.

## Discover Epsilon's architecture

Epsilon's code is comprehensive, as it goes from a keyboard driver up to a math engine. Epsilon is made out of five main bricks: [Ion](<%= p "ion" %>), Kandinsky, [Poincaré](<%= p "poincare" %>), Escher, and Apps.

![Epsilon's architecture](<%= p "architecture.svg" %>)

### Ion — Hardware abstraction layer

Ion is the underlying library that [abstracts all the hardware operations](ion/). It performs tasks such as setting the backlight intensity, configuring the LED or setting pixel colors on the screen. It also answers to questions such as "tell me which keys are pressed" and "what is the battery voltage?".

### Kandinsky — Graphics engine

That library is in charge of doing all the drawing. It performs functions such as "draw that text at this location" or "fill that rectangle in blue".

### Escher — GUI toolkit

Escher is our GUI toolkit. It provides functionalities such as "draw a button" or "place three tabs named Foo, Bar and Baz". It asks Ion for events and uses Kandinsky to do draw the actual user interface.

### Poincare — Mathematics engine

Poincare is in charge of parsing, laying out and evaluating mathematical expressions. You feed it some text such as `sin(root(2/3,3))` and it will draw the expression as in a text book and tell you that this expression is approximatively equal to 0.01524.

### Apps — Applications

Last but not least, each app makes heavy use of both Escher and Poincare to display a nice user interface and to perform mathematical computation.

## Read our coding guidelines

We're listing here all the topics you should be familiar with before being able to efficiently contribute to the project. Those are not hard requirements, but we believe it would be more efficient if you got familiar with the following concepts.

### Using C++

The choice of a programming language is a controversial topic. Not all of them can be used to write an operating system, but quite a few can. We settled on C++ for several reasons:

- It is a [system](https://en.wikipedia.org/wiki/System_programming_language) programming language, which is something we need since we have to write some low-level code.
- It has excellent tooling: several extremely high-quality compilers
- It is used for several high-profile projects LLVM, WebKit, MySQL, Photoshop… This ensures a strong ecosystem of tools, code and documentation.
- It easily allows Object-Oriented Programming, which is a convenient abstraction.

Of course knowing a tool means knowing its limits. C++ isn't exempt of defaults:
- It *is* a complicated language. The C++ 11 specification is 1'300 pages long.
- It allows for a lot of abstractions, which is a double-edged sword. It can allow for some very tricky code, and it's very easy to have complex operations being run without noticing.

If you want to contribute to Epsilon, you'll need to learn some C++.

### Working with limited memory

Our device has 256 KB of RAM. That's very little memory by today's standards. That being said, by writing code carefully, a huge lot can be achieved in that space. After all, that's 64 times more memory than the computer of the Apollo mission!

#### Stack memory

The stack memory is possibly the most used area of memory. It contains all local variables, and keeps track of the context of code execution. It can be overflowed in case of nested function calls if the reserved space is too small. We booked 32KB for the stack.

#### Heap memory

Unfortunately, local variables can't answer all use cases, and sometimes one need to allocate memory that lives longer than a function call. This is traditionally done by using a pair of `malloc` / `free` functions.

This raises a lot of potential problems that can trigger unpredictable dynamic behaviors:

  <dl class="dl-horizontal">
    <dt>Memory leaks</dt>
    <dd class="text-justify">If one forgets to free memory that is no longer used, the system will eventually run out of available memory.</dd>
    <dt>Memory fragmentation</dt>
    <dd class="text-justify">Memory allocation has to be contiguous. So the allocation algorithm has to use a smart heuristic to ensure that it will not fragment its allocated space too much.</dd>
  </dl>

We decided to avoid `malloc` altogether and to use a mix of static allocation and a pool of relocatable garbage-collected nodes for manipulating mathematical expressions.

### Writing code that runs on the bare metal

Unlike code that runs inside of an operating system (pretty much everything these days), an embedded firmware doesn't make use of virtual memory.

In practice, this means that the firmware will need to know in advance how the memory space is laid out. In other words, it will need to answer those questions:

- Where will the stack be located in memory?
- What about the heap and global variables?
- Where will we store read-only variables?
- Where will the code live in memory?

The firmware will also need to take special care of the system initialization. There is no such thing as a "main" function on a firmware. Instead, on Cortex-M devices, after reset the CPU simply jumps to the address contained at address 0x00000000 (which happens to be the first bytes of flash memory). So if your firmware starts by 0x12345678, code execution will start at address 0x12345678.

Enforcing such a careful memory layout would be an impossible job without the proper tool. Fortunately, embedded linkers can be scripted and allow this kind of tailor-made configuration. You'll find Epsilon's linker script in [ion/src/device/n0110/flash.ld](https://github.com/numworks/epsilon/blob/master/ion/src/device/n0110/flash.ld) - it is heavily commented and should be self-explanatory.

That being said, there are additional things the OS usually takes care of which we need to do ourselves : for example, initialize global variables to zero. This is done in the [ion/src/device/shared/boot/rt0.cpp](https://github.com/numworks/epsilon/blob/master/ion/src/device/shared/boot/rt0.cpp) file, which is worth reading too.
