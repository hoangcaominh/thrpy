Thrpy
===

This is a tool for unpacking, parsing data and repacking Touhou Project replay files.
This tool currently runs on Unix-based operating system only.

Dependencies
---

### kaitai-struct-compiler

This project depends on [Kaitai Struct](https://kaitai.io/) for reading binary files in multiple programming languages.
[Download and install the compiler](https://kaitai.io/#download) in the system before building this project.

### Protobuf

This project depends on [protobuf](https://protobuf.dev/) for serializing and deserializing parse results.
Clone the [repository](https://github.com/protocolbuffers/protobuf) to the system, then build and install the library before building this project.
Follow the [CMake installation guide](https://github.com/protocolbuffers/protobuf/blob/main/cmake/README.md) for more details.

How to build and install
---

**NOTE**: If you have protobuf installed system-wide instead of using the installation prefix as instructed by the guide, drop the `-DCMAKE_PREFIX_PATH` flag in the command.

Clone this repository, make sure that kaitai-struct-compiler and protobuf are installed on the system, then run the following commands to build and install the executable.
```sh
cmake -S . -B build -DCMAKE_PREFIX_PATH=<absolute path to your protobuf location>

cmake --build build

cmake --install build
```

Integrate the program with your project
---

You can pass the parse results produced by the program in your project using protobuf and the compiled proto files in your preferred language.
Please refer to the guide or tutorial of protobuf for said language.

Proto files used in this project can be found [here](https://github.com/hoangcaominh/thrpy-proto).


Support status
---

| Game | Unpack | Repack | Parse |
|-|:-:|:-:|:-:|
| th06	| ✅ | ✅ | ✅ |
| th07	| ✅ | ✅ | ✅ |
| th075	| ❌ | ❌ | ❌ |
| th08	| ✅ | ✅ | ✅ |
| th09	| ❌ | ❌ | ❌ |
| th095	| ❌ | ❌ | ❌ |
| th10	| ✅ | ✅ | ✅ |
| th105	| ❌ | ❌ | ❌ |
| th11	| ✅ | ✅ | ✅ |
| th12	| ✅ | ✅ | ✅ |
| th123	| ❌ | ❌ | ❌ |
| th125	| ❌ | ❌ | ❌ |
| th128	| ✅ | ✅ | ✅ |
| th13	| ✅ | ✅ | ✅ |
| th135	| ❌ | ❌ | ❌ |
| th14	| ✅ | ✅ | ✅ |
| th143	| ❌ | ❌ | ❌ |
| th145	| ❌ | ❌ | ❌ |
| th15	| ✅ | ✅ | ✅ |
| th155	| ❌ | ❌ | ❌ |
| th16	| ✅ | ✅ | ✅ |
| th165	| ❌ | ❌ | ❌ |
| th17	| ✅ | ✅ | ✅ |
| th175	| ❌ | ❌ | ❌ |
| th18	| ✅ | ✅ | ✅ |
| th185	| ❌ | ❌ | ❌ |
| th20	| ✅ | ✅ | ✅ |
