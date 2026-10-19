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

## Probing a cartridge image

`muhtari_probe(image, n, &info)` reads the `n` bytes at `image` and fills a `muhtari_cart_info` with what the
cartridge is: an A78 file, a headerless 7800 image or a 2600 image. It is pure: it allocates nothing, keeps no
state, and gives the same output for the same bytes, so concurrent calls on different outputs cannot interact.
It reads at most `MUHTARI_IMAGE_SIZE_MAX` bytes (1 MiB of ROM plus a 128-byte header) and hashes them a fixed
number of times. Running a probed image arrives in a later API version; until then `muhtari_run_frame` still
gives the test card.

```c
muhtari_cart_info info;
memset(&info, 0, sizeof info);
info.size = (uint32_t)sizeof info;
muhtari_status st = muhtari_probe(image, n, &info);
```

It checks, in this order:

1. `info` is NULL, or `image` is NULL with `n` above 0: `MUHTARI_STATUS_ERR_ARGUMENT`.
2. A bad size tag: `MUHTARI_STATUS_ERR_STRUCT_SIZE`.
3. `n` above `MUHTARI_IMAGE_SIZE_MAX`: `MUHTARI_STATUS_ERR_UNSUPPORTED_SIZE`, before any byte is hashed.
4. The image itself, as `docs/hardware/cartridge.md` describes: the A78 magic, the 7800 list, the 2600 list,
   the footer test and the size. A NULL `image` with `n` = 0 is `MUHTARI_STATUS_ERR_INVALID_IMAGE`.

| Status | Meaning | `info` |
|---|---|---|
| `MUHTARI_STATUS_OK` | A 7800 cartridge on a board this library runs | written |
| `MUHTARI_STATUS_ERR_UNSUPPORTED_BOARD` | A 7800 cartridge on a board it does not run yet | written |
| `MUHTARI_STATUS_ERR_UNSUPPORTED_2600` | A 2600 image | written: only `kind`, `source`, `rom_size` and `flags` are non-zero |
| `MUHTARI_STATUS_ERR_INVALID_IMAGE` | Not a cartridge image, or a size no board has | untouched |
| `MUHTARI_STATUS_ERR_TRUNCATED_IMAGE` | An A78 file that ends early | untouched |
| `MUHTARI_STATUS_ERR_UNSUPPORTED_SIZE` | Larger than `MUHTARI_IMAGE_SIZE_MAX` | untouched |

The two refusals that write `info` are the one exception to the rule that an error writes nothing: a host can
show what it refused.

| Field | Values |
|---|---|
| `kind` | `MUHTARI_KIND_7800`, `MUHTARI_KIND_2600` |
| `source` | What identified it: `MUHTARI_SOURCE_HEADER`, `MUHTARI_SOURCE_HASH`, `MUHTARI_SOURCE_FOOTER`, `MUHTARI_SOURCE_SIZE` |
| `flags` | `MUHTARI_CART_HEADER` (it has an A78 header), `MUHTARI_CART_HASH_MATCH` (a list entry matched) |
| `warnings` | Header defects it accepted, listed below |
| `rom_size` | Bytes the cartridge maps, header excluded |
| `board` | `MUHTARI_BOARD_LINEAR`, `MUHTARI_BOARD_SUPERGAME`, `MUHTARI_BOARD_ACTIVISION`, `MUHTARI_BOARD_ABSOLUTE`, `MUHTARI_BOARD_FRACTALUS`, `MUHTARI_BOARD_BANKSET`, `MUHTARI_BOARD_SOUPER` |
| `board_options` | Bits: `MUHTARI_BOARD_ROM_AT_4000`, `MUHTARI_BOARD_BANK6_AT_4000`, `MUHTARI_BOARD_BANKED_RAM`, `MUHTARI_BOARD_HALT_BANKED_RAM` |
| `ram_size` | Cartridge RAM in bytes |
| `sound` | Bits: `MUHTARI_SOUND_POKEY_4000`, `MUHTARI_SOUND_POKEY_450`, `MUHTARI_SOUND_POKEY_440`, `MUHTARI_SOUND_POKEY_800`, `MUHTARI_SOUND_YM2151`, `MUHTARI_SOUND_COVOX` |
| `devices` | Bits: `MUHTARI_DEVICE_HSC`, `MUHTARI_DEVICE_SAVEKEY`, `MUHTARI_DEVICE_XM` |
| `port1`, `port2` | `MUHTARI_PORT_NONE`, `MUHTARI_PORT_PROLINE`, `MUHTARI_PORT_LIGHTGUN`, `MUHTARI_PORT_PADDLE`, `MUHTARI_PORT_TRAKBALL`, `MUHTARI_PORT_JOYSTICK`, `MUHTARI_PORT_DRIVING`, `MUHTARI_PORT_KEYPAD`, `MUHTARI_PORT_ST_MOUSE`, `MUHTARI_PORT_AMIGA_MOUSE`, `MUHTARI_PORT_ATARIVOX`, `MUHTARI_PORT_SNES2ATARI`, `MUHTARI_PORT_MEGA7800`: the A78 codes 0 to 12 |
| `region` | `MUHTARI_REGION_NTSC`, `MUHTARI_REGION_PAL` |
| `a78_version`, `a78_rom_size`, `a78_cart_type`, `a78_port1`, `a78_port2`, `a78_tv`, `a78_save`, `a78_xm` | The A78 header's fields as written; zero without a header |
| `a78_mapper`, `a78_mapper_options`, `a78_audio`, `a78_irq` | The version 4 block as written; zero below version 4 |
| `title` | The header's title: at most 32 printable ASCII bytes, trailing spaces dropped, NUL-terminated and zero-padded |

Zero means absent in every field. A board moves from `MUHTARI_STATUS_ERR_UNSUPPORTED_BOARD` to
`MUHTARI_STATUS_OK` in the release that runs it. The warning bits:

| Bit | The header |
|---|---|
| `MUHTARI_CART_WARN_HEADER_V4_IGNORED` | says version 4, but its version 4 block is invalid, so the older fields were used |
| `MUHTARI_CART_WARN_HEADER_CORRECTED` | names a board, RAM or sound that a list entry for the same bytes corrects |
| `MUHTARI_CART_WARN_VERSION_ABOVE_4` | gives a version above 4, read as 4 |
| `MUHTARI_CART_WARN_SIZE_FIELD_MISMATCH` | gives a ROM size other than the bytes after it |
| `MUHTARI_CART_WARN_TITLE_NONPRINTABLE` | has a title byte outside printable ASCII, shown as `?` |
| `MUHTARI_CART_WARN_UNKNOWN_PORT` | names a controller code above 12, read as `MUHTARI_PORT_NONE` |
| `MUHTARI_CART_WARN_LEGACY_SAVE_BIT` | marks the High Score Cartridge with the old bit 2 |
| `MUHTARI_CART_WARN_XM_VALUE` | gives an XM byte other than 0 or 1 |
| `MUHTARI_CART_WARN_RESERVED_NONZERO` | has a reserved byte or bit set |
| `MUHTARI_CART_WARN_CART_TYPE_CONFLICT` | sets cart-type bits that cannot all hold, resolved as `docs/hardware/cartridge.md` says |

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
