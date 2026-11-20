# C host quickstart

This ROM-free example proves that a C host can discover the installed package, link the library, create an
instance, size its buffers, and run one NTSC frame. It does not test a game or establish game compatibility.
The first frame produces 800 signed 16-bit audio samples at the library's 48 kHz rate; the example checks that
count before printing its success line. The samples are output data for the host to consume, not audio played
by the library.

Use the [exact C source](../examples/embedding/quickstart.c) and its
[standalone CMake project](../examples/embedding/CMakeLists.txt) from the source revision matching the library
archive. The complete [embedding API guide](embedding.md) describes the rest of the public contract.

## Choose and extract the package

Download `muhtari-library-<os>-<arch>.zip` from the
[latest release](https://github.com/szTheory/muhtari/releases/latest). Choose the archive for the
compiler's output architecture: for example, `muhtari-library-macos-arm64.zip` for Apple Silicon,
`muhtari-library-linux-x64.zip` for Linux x64, or `muhtari-library-windows-x64.zip` for Windows x64. Extract
the archive into a separate `library` directory and place the two linked example files in a separate `consumer`
directory beside it:

```text
quickstart-work/
  library/       # extracted release ZIP; this is the package prefix
  consumer/
    CMakeLists.txt
    quickstart.c
```

The CMake route needs CMake 3.25 or newer and a C17 compiler. On POSIX, install `unzip` as well. Use a fresh
consumer build directory so CMake tests the extracted package and not another copy of Muhtari.

## CMake on Linux or macOS

From `quickstart-work`, after saving the linked files into `consumer/`:

```sh
cmake -S consumer -B consumer/build -DCMAKE_PREFIX_PATH="$PWD/library" -DCMAKE_BUILD_TYPE=Release
cmake --build consumer/build --config Release
# Single-config generators, such as Unix Makefiles or Ninja:
./consumer/build/quickstart
# Multi-config generators, such as Xcode:
./consumer/build/Release/quickstart
```

The CMake package resolves the imported `muhtari::muhtari` target from the extracted prefix. The same commands
work with the Linux or macOS archive that matches the compiler. Run only the executable path for the generator
you selected.

## pkg-config on Linux or macOS

Install `pkg-config` and run from a fresh working directory with a path that contains no spaces (for example
`/tmp/muhtari-quickstart`). Keep the extraction prefix as the literal relative directory `library`; its
`muhtari.pc` supplies the include and link flags. After saving the linked files into `consumer/`:

```sh
set -eu
if ! pkg_flags=$(PKG_CONFIG_LIBDIR=library/lib/pkgconfig PKG_CONFIG_PATH= pkg-config --cflags --libs muhtari); then
    echo "could not read muhtari flags from the extracted package" >&2
    exit 1
fi
printf '%s\n' "$pkg_flags" | xargs cc -std=c17 consumer/quickstart.c -o consumer/quickstart
./consumer/quickstart
```

The environment is applied to the `pkg-config` lookup itself, isolating it from system packages. The compiler
then receives those flags after the source file so the static library links. Both routes must print:

```text
quickstart: one frame, 800 samples
```

## CMake on Windows

Run these commands in a Visual Studio Developer PowerShell or another shell where the C17 MSVC compiler is
available. For a Visual Studio multi-config generator:

```powershell
cmake -S consumer -B consumer\build -DCMAKE_PREFIX_PATH="$PWD\library"
cmake --build consumer\build --config Release
.\consumer\build\Release\quickstart.exe
```

With a single-config generator such as Ninja, set `-DCMAKE_BUILD_TYPE=Release` when configuring and run
`consumer\build\quickstart.exe` instead. Windows uses the CMake route; the pkg-config recipe below is for
POSIX toolchains.

## What the example sets up

The library has no default allocator, so the example supplies both `malloc` and `free` callbacks. It zeroes
each public struct and sets its `size` field, queries `muhtari_info`, allocates video and audio buffers from the
reported dimensions and sample capacity, then passes their pitch and capacity to `muhtari_run_frame`. It
reports setup, allocation, frame, and sample-count failures to standard error, cleans up acquired resources,
and prints success only after the frame returns `MUHTARI_STATUS_OK` with exactly 800 samples.
