# Contributing

![Contributions](https://img.shields.io/badge/contributions-open-green)

Contributions are encouraged, however, discussing the proposed changes beforehand via [twitter or discord](https://github.com/VertexEngine/VertexEngine#support) is appreciated.

## Code style

It is important for a healthy code base to have a consistent style through all internal code. This extends to design patterns as well as just the formatting of the code itself.

### Code formatting
Vertex formatting is based on a custom style, that can be described as most similar to [the WebKit coding style](https://webkit.org/code-style-guidelines/). A `clang-format` file is provided to automatically format the project files for you (clang-format should be installed in order to work).

### Patterns
* Header files must have the `.h` extension.

* Use `#pragma once` instead of include guards.

* File names must be in `PascalCase`.

* Include only necessary headers.

* Include common header files in `Vertex/src/Core/vx_pch.h`.

## Code of conduct
See [CODE_OF_CONDUCT](CODE_OF_CONDUCT.md).
