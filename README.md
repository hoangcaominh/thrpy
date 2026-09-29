Thrpy
===

This is a tool for unpacking, parsing data and repacking Touhou Project replay files.

Dependencies
---

This project depends on [protobuf](https://github.com/protocolbuffers/protobuf) for serializing and deserializing parse results.
Build `protobuf` with flag `-Dprotobuf_BUILD_SHARED_LIBS=ON` and install the library system-wide.
Follow the [CMake installation guide](https://github.com/protocolbuffers/protobuf/blob/main/cmake/README.md) for more details.

How to build
---

Clone all submodules by running `git submodule update --init`, or drop `--init` to update existing submodules.
Verify that `protobuf` is installed on the system, then run the following commands to install the executable.
```sh
cmake -S . -B build
cmake --build build
sudo cmake --install build
```

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
