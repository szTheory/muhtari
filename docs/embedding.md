# Embedding the library

The library is one static archive and one header, `muhtari.h`, written in C17. It allocates memory only through
the allocator you pass, keeps all state in the instance, starts no threads, opens no files and uses no floating
point. It needs nothing from the C library beyond `memcpy`, `memmove`, `memset` and `memcmp`.

## Getting it

**Release archive.** `muhtari-library-<os>-<arch>.zip` from the
[latest release](https://github.com/szTheory/muhtari/releases/latest) holds:

```text
include/muhtari.h
lib/libmuhtari.a                 (lib/muhtari.lib on Windows)
lib/cmake/muhtari/               CMake package, target muhtari::muhtari
lib/pkgconfig/muhtari.pc
LICENSE
THIRD-PARTY-NOTICES.md
```

Unzip it anywhere; both package files find the other files relative to themselves. On Windows the library is
built against the static C runtime (`/MT`) and records no default runtime library, so it links into programs on
either runtime.

**CMake package.**

```cmake
find_package(muhtari CONFIG REQUIRED)
target_link_libraries(app PRIVATE muhtari::muhtari)
```

Configure with `-DCMAKE_PREFIX_PATH=<unzipped directory>`. Until version 1.0 a minor release may change the API,
so the package accepts a request for the same major and minor version only.

**Source tree.** `add_subdirectory` or `FetchContent` gives the same `muhtari::muhtari` target. Only the C
compiler is needed: an embedding build gets the library alone, without the runner, the libretro core, the tests
or this project's warning flags.

```cmake
add_subdirectory(third_party/muhtari)
target_link_libraries(app PRIVATE muhtari::muhtari)
```

**pkg-config.**

```sh
PKG_CONFIG_PATH=<unzipped directory>/lib/pkgconfig cc app.c $(pkg-config --cflags --libs muhtari)
```

## Structs and sizes

Every struct you pass starts with `uint32_t size`. Zero the whole struct, then set `size`:

```c
muhtari_config cfg;
memset(&cfg, 0, sizeof cfg);
cfg.size = (uint32_t)sizeof cfg;
```

There is no init macro. The size tag lets a program built against one header run with a library from another:

- A size below the struct's first released size, or above 4096 bytes, returns `MUHTARI_STATUS_ERR_STRUCT_SIZE`.
- A smaller size from an older header is accepted. Input fields it lacks take their zero default, and an output
  struct receives only its first `size` bytes.
- A larger size from a newer header is accepted only if every byte the library does not know is zero; otherwise
  the call returns `MUHTARI_STATUS_ERR_STRUCT_SIZE`.
- A field added later always keeps the old behaviour when it is zero.

## The allocator

`muhtari_config.allocator`, a `muhtari_allocator`, needs both `alloc` and `free`; either one missing returns
`MUHTARI_STATUS_ERR_ARGUMENT`. There is no default. `alloc` must return memory aligned for `max_align_t`, as
`malloc` does. A host with a C library can pass this wrapper:

```c
static void *host_alloc(void *user, size_t size) {
    (void)user; return malloc(size);
}
static void host_free(void *user, void *ptr, size_t size) {
    (void)user; (void)size; free(ptr);
}
```

`free` receives the size that was allocated, so a fixed arena or pool needs no header of its own. `user` is
passed back unchanged. All allocation happens in `muhtari_create`, which returns `MUHTARI_STATUS_ERR_NO_MEMORY`
when `alloc` returns NULL; running frames allocates nothing.

## A frame

```c
muhtari *m = NULL;
cfg.allocator.alloc = host_alloc;
cfg.allocator.free = host_free;
if (muhtari_create(&cfg, &m) != MUHTARI_STATUS_OK) { /* handle the error */ }

muhtari_frame io;
memset(&io, 0, sizeof io);
io.size = (uint32_t)sizeof io;
io.video = video;               /* 320 x 242 bytes, one colour number per pixel */
io.video_pitch = 320;           /* bytes per row, 320..65536 */
io.audio = audio;               /* room for 801 samples */
io.audio_capacity = 801;
muhtari_status st = muhtari_run_frame(m, &io);   /* io.audio_count: 800 or 801 */
muhtari_destroy(m);
```

`muhtari_get_info` fills a `muhtari_info` with the picture size, the frame rate, the pixel aspect ratio, the
sample rate and the largest audio count per frame, so a host need not hard-code them. A NULL `video` or `audio`
skips that output. With `video` set, a `video_pitch` outside 320..65536 returns `MUHTARI_STATUS_ERR_ARGUMENT`;
with `audio` set, an `audio_capacity` below the frame's sample count, 800 or 801, returns
`MUHTARI_STATUS_ERR_BUFFER_TOO_SMALL`, so size the buffer from `max_audio_samples`. `muhtari_run_frame` checks its
arguments before it changes anything. Once `m` and `io` are not NULL and the size tag is accepted, it writes
`audio_count` on every return, zero on an error.

## Statuses

`muhtari_status` is a `uint32_t`; the `MUHTARI_STATUS_*` names are constants of an unnamed enum, so the type has the
same size on every target. In C++, a conditional expression that mixes the two, such as
`ok ? st : MUHTARI_STATUS_ERR_ARGUMENT`, draws a warning under `g++ -Wextra` (an error with `-Werror`). Compare
instead (`st == MUHTARI_STATUS_OK`), or cast the constant: `(muhtari_status)MUHTARI_STATUS_ERR_ARGUMENT`.

## Version numbers

`MUHTARI_API_VERSION` in the header is the API the program was compiled against. At run time,
`muhtari_get_versions` fills a `muhtari_versions`:

| Field | Meaning |
|---|---|
| `library_major`, `library_minor`, `library_patch` | The release. `library_major` is 0 before version 1.0. |
| `api` | The API the library implements. |
| `abi` | The binary interface: struct layouts and calling conventions. |
| `behaviour_revision` | Changes when the same inputs give a different frame, audio or state hash. |

`api`, `abi` and `behaviour_revision` start at 1, so zero means absent: a field that a library older than your
header does not know stays at the zero you wrote. The library version numbers carry no such meaning.
