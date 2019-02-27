# BigInt
[![Codacy Badge](https://api.codacy.com/project/badge/Grade/52406353f564468e9e301645f02127a3)](https://www.codacy.com/app/AlexanderJDupree/File_Reader?utm_source=github.com&amp;utm_medium=referral&amp;utm_content=AlexanderJDupree/File_Reader&amp;utm_campaign=Badge_Grade)

## Introduction

**BigInt** is a numeric type that can represent integers with arbitrary precision. This allows the user to safely store and perform arithmetic on numbers greater than the current limit without risk of overflow. At least, this will be the case when the project is actually implemented. 

Project Goals:
    1. First and foremost, accuracy. 
    2. Optimize until BigInt is blazing fast.
    3. Support for x86 and x64 architechtures. 
    4. Port to C++ to make use of operator overloads.

## Getting Started

All releases are header only, meaning all you need to do is download the latest [release here](https://github.com/AlexanderJDupree/BigInt/releases), place the header into a visible include path, then include the file in your project. *Note*: Currently there are no releases

### Usage 

```c++

int main()
{
    // TODO Create example code
    return 0;
}

```

## What's in this Repo?

BigInt utilizes premake5 to generate the requisite project files. The generated GNU makefiles are provided in this repo, however if you need to build visual studio or xcode project files just run:

```
premake5 <action>
```

where <action> is the type of project files needed. See the [Premake5](https://github.com/premake/premake-core) repo for further usage instructions.

If the GNU makefiles suffice then you can generate the BigInt static library and unit tests by running:

```
cd gmake && make
```
The executable and static lib will be located in the *bin/* directory.
*Note*: By default make will build debug_x86 configuration. Run 'make help' for a list of possible configurations.

## Built With

* [Catch2](https://github.com/catchorg/Catch2) - Unit Testing framework used
* [Premake5](https://github.com/premake/premake-core) - Build system

## License

This project is licensed under the MIT License - see the [LICENSE.md](https://raw.githubusercontent.com/AlexanderJDupree/BigInt/master/LICENSE) file for details

