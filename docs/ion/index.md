---
title: Ion
---
# Ion

## Overview

Ion is a library of functions that abstract interacting with the hardware. For example, Ion exposes calls such as `serialNumber()` and `LED::setColor()`. Code in Epsilon always uses Ion functions to perform operations that deal directly with the hardware — be it reading a key, setting a pixel on the screen or setting the color of the LED.

By providing multiple implementations of the Ion functions, we therefore can get Epsilon to run on multiple platforms. For example, we have an implementation of `Display::pushRect()` that knows how to drive the LCD panel of an actual calculator, and another implementation of the same `Display::pushRect()` function that knows how do display content in a web browser. This way, the rest of the Epsilon code can run unmodified either on a calculator or a web browser.

## Device

This is the reference platform corresponding to the actual device. To really understand what the code is doing, you'll need to refer to our [Electrical Engineering](https://www.numworks.com/resources/engineering/hardware/electrical/) pages. Among other thing, Ion is responsible for handling the boot process and the memory layout of the code on the device:

### Boot

On boot, the Cortex core interprets the beginning of Flash memory as an array of addresses, each having a specific meaning. For example, the first value gives the address of the stack and the second one the address the processor jumps to right after reset. This list of addresses is called the [ISR table](https://github.com/numworks/epsilon/blob/master/ion/src/device/shared/boot/isr.c).

### Memory layout

Like we saw in the previous paragraph, the MCU has a specific memory layout (for example, Flash starts at address 0x08000000) and expects certain values at certain addresses. To ensure the firmware is laid out in memory exactly how the processor expects it, we use a custom [linker script](https://github.com/numworks/epsilon/blob/master/ion/src/device/n0110/flash.ld).

## Simulator

The simulator platform implements Ion calls using the [SDL](https://www.libsdl.org/) library. The result is a native GUI program that can run under a variety of operating systems such as Windows, macOS or most Linux distributions.

It's very practical to code using the simulator, but one has to pay attention to the differences from an actual device: it'll be significantly faster, and will have a lot more memory. So code written using the simulator should always be thoroughly tested on an actual device.

## Emscripten

The emscripten platform implements Ion calls for a Web browser. This lets us build a version of Epsilon that can run directly in a browser, as shown in our [online simulator](https://www.numworks.com/simulator/). The C++ code is transpiled to JavaScript using Emscripten, and then packaged in a webpage.

Building on Emscripten takes quite a lot of time so you will most likely not want to use it for development purposes. But obviously it's a very neat feature for end users who can give the calculator a spin straight from their browser.

## Tests

In order to ensure changes made to epsilon won't have unwanted behaviour, there are 3 types of tests, which can be done on all platform :
- [unitary tests](https://github.com/numworks/epsilon/tree/master/quiz) : some internal behaviour are described in test cases : the tester gives some parameters to a component of the firmware, and it compares the result to a predefined output. For instance, there is a [test](https://github.com/numworks/epsilon/blob/37e7686b60fc490c59ebd887f7d862a5af7aee23/poincare/test/simplification.cpp#L131) made to assert `-5π+3π` is parsed, simplified and output `-2×π`.
- [screenshot compare](https://github.com/numworks/epsilon/blob/master/build/compare/compare.sh) : there are some classical [scenarios](https://github.com/numworks/epsilon/tree/master/tests) which will be run on two different simulators. At each keypress, both of the simulators will save a screenshot of their state, and a script will compare if they are the same. This is very usefull to assert that other part of the user interface, unedited, won't change.
- [fuzzing](https://en.wikipedia.org/wiki/Fuzzing) : this test will try to find situation, in which a simulator will crash. It will start from a classical scenario (like the previous ones in screenshot compare), and will understand which change will make the simulator do something totally different. As soon as it manages to crash the simulator, it will save in a file the scenario, which can be repeted by a human to fix it. This solution is used either to assert that there are no crash which can happen in your software, or in a security goal, because a crash often is a breach in the software. Check out [this blog post](https://www.numworks.com/blog/fuzzing-a-calculator/) which explains more about it.
