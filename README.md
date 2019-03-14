# BigInt
[![Codacy Badge](https://api.codacy.com/project/badge/Grade/52406353f564468e9e301645f02127a3)](https://www.codacy.com/app/AlexanderJDupree/File_Reader?utm_source=github.com&amp;utm_medium=referral&amp;utm_content=AlexanderJDupree/File_Reader&amp;utm_campaign=Badge_Grade)\
[![Build Status](https://travis-ci.com/AlexanderJDupree/BigInt.svg?branch=master)](https://travis-ci.com/AlexanderJDupree/newsfeed)
[![GitHub license](https://img.shields.io/badge/license-MIT-blue.svg)](https://github.com/AlexanderJDupree/BigInt/blob/master/LICENSE)

## Introduction

**BigInt** is a numeric type that can represent integers with arbitrary precision. This allows the user to safely store and perform arithmetic on numbers greater than the CPU architecture limit without risk of overflow. At least, this will be the case when the project is fully implemented. 

Project Goals
1. First and foremost, accuracy. 
2. Optimize until BigInt is blazing fast.
3. Support for x86 and x64 architechtures. 
4. Port to C++ to make use of operator overloads.

Feature Roadmap
1. ~~Implement reserve, value, and string parsing constructor~~ PR #1
2. ~~Implement addition operations~~ PR #4
3. ~~Implement subtraction operations~~ PR #5
4. Implement multiplication operations
5. Implement division operations 
6. Implement exponention operations
7. Release version 1.0.0!

## Getting Started

All releases are header only, meaning all you need to do is download the latest [release here](https://github.com/AlexanderJDupree/BigInt/releases), place the header into a visible include path, then include the file in your project. *Note*: Currently there are no releases

### Usage 

Here's a sample function that can display the Nth fibonacci number. It's important to note that this function is only limited by the amount of memory on your machine. This is the idea of the BigInt library. 

```c++

void run_BigFibonacci(unsigned n)
{
    BigInt* a = val_BigInt(0);
    BigInt* b = val_BigInt(1);

    printf("Fibonacci Number %d is:\n", n);
    for (unsigned i = 1; i < n; ++i)
    {
        BigInt* c = add(a, b);
        free_BigInt(a);
        a = b;
        b = c;
    }
    display(b);
    free_BigInt(a);
    free_BigInt(b);
    return;
}

```

Running this function with n=1000 will print:

```
0x21d8cb07b572c25732bb116f2c33bab0e83d0c699bad1a727a736a7e42ca93b697ad224d55398373062f18ff62b99c28068131a3fab0c12e3510283c1d60b00930b7e8803c312b4c8e6d5286805fc70b594dc75cc0604b
```

Currently, BigInt only support hexadecimal for input/output. However, I am open for contributors to pitch in and build support for input/output with arbitrary bases.

## What's in this Repo?

This repo utilizes [Premake5](https://github.com/premake/premake-core) to build the necessary project files. Once premake5 is installed just run:

```
premake5 gmake
```

This will generate the GNU makefiles for this project. If you need specific project files run 'premake5 --help' for a list of supported actions. See the [Premake5](https://github.com/premake/premake-core) documentaion for further usage instructions.

If the GNU makefiles suffice, then you can generate the File_Reader static library and unit tests by running:

```
cd gmake/ && make
```

By default make will build the debug_x64 build. To get a list of configurations run 'make help'. Then to build, for example, release_x64 run:
```
make config=release_x64
```

I've included a postbuild command in premake5.lua that will run the unit tests automatically, however if you want to rerun the tests they are located in bin/tests/

## Built With

* [Catch2](https://github.com/catchorg/Catch2) - Unit Testing framework used
* [Premake5](https://github.com/premake/premake-core) - Build system

## License

This project is licensed under the MIT License - see the [LICENSE.md](https://raw.githubusercontent.com/AlexanderJDupree/BigInt/master/LICENSE) file for details

