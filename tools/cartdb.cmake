# Cartridge table generator, run as
#   cmake -DA7800=<a7800.xml> -DA2600=<a2600.xml> -DOUT=<cartdb.c> -P cartdb.cmake
# Fault prevented: a cartridge table that is not what the pinned CC0 lists
# say. src/cartdb.c is only ever this script's output over the two MAME
# hash files that pins.txt pins (policy.cartdb_generated compares them), and
# a slot value, feature, data area or chip layout this script does not know,
# or two 2600 SHA-1s sharing an 8-byte prefix, stops it with the XML byte
# offset instead of a guess. Slot values map to boards as
# .planning/preparation/A7800-CARTRIDGES-AND-PERIPHERALS.md section 2 says.
# Only SHA-1s and numbers leave the XML: no name, title or ROM byte.
cmake_minimum_required(VERSION 3.25)

foreach(var A7800 A2600 OUT)
  if("${${var}}" STREQUAL "")
    message(FATAL_ERROR "cartdb: -D${var} is required")
  endif()
endforeach()

# The MAME commit both pins.txt rows fetch the lists from.
set(commit dddf4cd1faeeca6a6dac26ecbc2da2da4242b740)

# Slot value: board, board options, sound, not a cartridge. Boards: linear 1,
# SuperGame 2, Activision 3, Absolute 4, Fractalus 5. Options bit 0: ROM at
# $4000 (SG9/EXROM). Sound bit 0: POKEY at $4000, bit 1: POKEY at $0450.
set(slot_a78_rom 1 0 0 0)
set(slot_a78_pokey 1 0 1 0)
set(slot_a78_sg 2 0 0 0)
set(slot_a78_sg_ram 2 0 0 0)
set(slot_a78_sg9 2 1 0 0)
set(slot_a78_sg_pokey 2 0 1 0)
set(slot_a78_p450_ta 2 1 2 0)
set(slot_a78_act 3 0 0 0)
set(slot_a78_abs 4 0 0 0)
set(slot_a78_mram 5 0 0 0)
set(slot_a78_hsc 0 0 0 1)
set(slot_a78_xboard 0 0 0 1)
set(slot_a78_xm 0 0 0 1)
set(slices_max 4)

macro(cartdb_fail at what)
  message(FATAL_ERROR "cartdb: ${A7800} offset ${at}: ${what}")
endmacro()

# Every argument a 40-digit lower-case SHA-1 in hex, or stop.
function(cartdb_check_sha1 file at sha1)
  string(LENGTH "${sha1}" len)
  if(NOT len EQUAL 40 OR NOT sha1 MATCHES "^[0-9a-f]+$")
    message(FATAL_ERROR "cartdb: ${file} offset ${at}: '${sha1}' is not a SHA-1")
  endif()
endfunction()

# The value of attribute NAME in TAG, or empty.
function(cartdb_attr tag name var)
  if(tag MATCHES " ${name}=\"([^\"]*)\"")
    set(${var} "${CMAKE_MATCH_1}" PARENT_SCOPE)
  else()
    set(${var} "" PARENT_SCOPE)
  endif()
endfunction()

# One list element per <software> block. The replaced characters are the
# ones that split or escape CMake list elements, one for one, so element
# lengths still add up to XML byte offsets.
file(READ "${A7800}" xml)
string(REPLACE "\\" "/" xml "${xml}")
string(REPLACE ";" "," xml "${xml}")
string(REPLACE "[" "(" xml "${xml}")
string(REPLACE "]" ")" xml "${xml}")
string(REPLACE "<software " ";<software " xml "${xml}")

set(keys "")
set(entries "")
set(layouts "")
set(entry 0)
set(base 0)
foreach(block IN LISTS xml)
  string(LENGTH "${block}" block_len)
  set(at ${base})
  math(EXPR base "${base} + ${block_len}")
  if(NOT block MATCHES "^<software ")
    continue()
  endif()
  if(entry GREATER 255)
    cartdb_fail(${at} "more than 256 entries do not fit the key's entry byte")
  endif()

  # Features: one slot, and optionally the light gun.
  string(REGEX MATCHALL "<feature [^>]*>" features "${block}")
  set(slot "")
  set(flags 0)
  foreach(tag IN LISTS features)
    string(FIND "${block}" "${tag}" pos)
    math(EXPR pos "${at} + ${pos}")
    cartdb_attr("${tag}" name name)
    cartdb_attr("${tag}" value value)
    if(name STREQUAL "slot" AND slot STREQUAL "")
      if(NOT DEFINED slot_${value})
        cartdb_fail(${pos} "unknown slot value '${value}'")
      endif()
      set(slot ${value})
    elseif(name STREQUAL "peripheral" AND value STREQUAL "lightgun")
      math(EXPR flags "${flags} | 4")
    else()
      cartdb_fail(${pos} "unknown feature ${name}='${value}'")
    endif()
  endforeach()
  if(slot STREQUAL "")
    cartdb_fail(${at} "entry has no slot feature")
  endif()
  list(GET slot_${slot} 0 board)
  list(GET slot_${slot} 1 options)
  list(GET slot_${slot} 2 sound)
  list(GET slot_${slot} 3 not_cart)
  if(not_cart)
    math(EXPR flags "${flags} | 8")
  endif()

  # TV systems.
  string(REGEX MATCHALL "<sharedfeat [^>]*>" shared "${block}")
  foreach(tag IN LISTS shared)
    string(FIND "${block}" "${tag}" pos)
    math(EXPR pos "${at} + ${pos}")
    cartdb_attr("${tag}" name name)
    cartdb_attr("${tag}" value value)
    if(NOT name STREQUAL "compatibility")
      cartdb_fail(${pos} "unknown shared feature '${name}'")
    endif()
    string(REPLACE "," ";" systems "${value}")
    foreach(tv IN LISTS systems)
      if(tv STREQUAL "NTSC")
        math(EXPR flags "${flags} | 1")
      elseif(tv STREQUAL "PAL")
        math(EXPR flags "${flags} | 2")
      else()
        cartdb_fail(${pos} "unknown compatibility '${tv}'")
      endif()
    endforeach()
  endforeach()

  # Data areas: one rom, at most one ram; nvram is the HSC's own.
  string(REGEX MATCHALL "<dataarea [^>]*>" areas "${block}")
  set(rom_size "")
  set(ram_kib 0)
  foreach(tag IN LISTS areas)
    string(FIND "${block}" "${tag}" pos)
    math(EXPR pos "${at} + ${pos}")
    cartdb_attr("${tag}" name name)
    cartdb_attr("${tag}" size size)
    math(EXPR size "${size}")
    if(name STREQUAL "rom" AND rom_size STREQUAL "")
      set(rom_size ${size})
    elseif(name STREQUAL "ram" AND ram_kib EQUAL 0)
      math(EXPR ram_kib "${size} / 1024")
      math(EXPR rest "${size} % 1024")
      if(NOT rest EQUAL 0 OR ram_kib EQUAL 0)
        cartdb_fail(${pos} "RAM of ${size} bytes is not a whole KiB")
      endif()
    elseif(NOT name STREQUAL "nvram")
      cartdb_fail(${pos} "unknown or repeated data area '${name}'")
    endif()
  endforeach()
  if(rom_size STREQUAL "")
    cartdb_fail(${at} "entry has no rom data area")
  endif()

  # Chips. A chip with a SHA-1 starts a slice; "continue" extends the last
  # slice; "reload" adds a slice that repeats the last chip.
  string(REGEX MATCHALL "<rom [^>]*>" roms "${block}")
  set(sizes "")
  set(parts "")
  set(offsets "")
  set(continued 0)
  set(keyed 0)
  foreach(tag IN LISTS roms)
    string(FIND "${block}" "${tag}" pos)
    math(EXPR pos "${at} + ${pos}")
    cartdb_attr("${tag}" size size)
    cartdb_attr("${tag}" offset offset)
    cartdb_attr("${tag}" sha1 sha1)
    cartdb_attr("${tag}" loadflag loadflag)
    if(size STREQUAL "")
      cartdb_fail(${pos} "chip has no size")
    endif()
    math(EXPR size "${size}")
    if(offset STREQUAL "")
      set(offset -1)
    else()
      math(EXPR offset "${offset}")
    endif()
    list(LENGTH sizes n)
    if(loadflag STREQUAL "continue" AND n GREATER 0 AND sha1 STREQUAL "")
      math(EXPR last "${n} - 1")
      list(GET sizes ${last} s)
      math(EXPR s "${s} + ${size}")
      list(REMOVE_AT sizes ${last})
      list(APPEND sizes ${s})
      set(continued 1)
    elseif(loadflag STREQUAL "reload" AND n GREATER 0 AND sha1 STREQUAL "")
      math(EXPR last "${n} - 1")
      list(GET parts ${last} p)
      list(APPEND sizes ${size})
      list(APPEND parts ${p})
      list(APPEND offsets ${offset})
    elseif(loadflag STREQUAL "" AND NOT sha1 STREQUAL "")
      cartdb_check_sha1("${A7800}" ${pos} "${sha1}")
      math(EXPR padded "1000 + ${entry}")
      string(SUBSTRING "${padded}" 1 3 padded)
      list(APPEND keys "${sha1}|${padded}|${keyed}")
      list(APPEND sizes ${size})
      list(APPEND parts ${keyed})
      list(APPEND offsets ${offset})
      math(EXPR keyed "${keyed} + 1")
    else()
      cartdb_fail(${pos} "chip with loadflag '${loadflag}' and SHA-1 '${sha1}' is not handled")
    endif()
  endforeach()

  # A multi-chip entry's slices cover its rom area in address order.
  set(layout 0)
  if(keyed GREATER 1)
    if(continued)
      cartdb_fail(${at} "a multi-chip entry continues a chip")
    endif()
    list(LENGTH sizes n)
    if(n GREATER slices_max)
      cartdb_fail(${at} "${n} slices, more than ${slices_max}")
    endif()
    set(sum 0)
    foreach(i RANGE 1 ${n})
      math(EXPR i "${i} - 1")
      list(GET sizes ${i} s)
      list(GET offsets ${i} o)
      if(NOT o EQUAL sum)
        cartdb_fail(${at} "slice ${i} starts at ${o}, not at ${sum}")
      endif()
      math(EXPR sum "${sum} + ${s}")
    endforeach()
    if(NOT sum EQUAL rom_size)
      cartdb_fail(${at} "slices cover ${sum} bytes of a ${rom_size}-byte rom area")
    endif()
    while(n LESS slices_max)
      list(APPEND sizes 0)
      list(APPEND parts 0)
      math(EXPR n "${n} + 1")
    endwhile()
    list(JOIN sizes ", " s)
    list(JOIN parts ", " p)
    set(row "{{${s}}, {${p}}},")
    list(FIND layouts "${row}" i)
    if(i EQUAL -1)
      list(LENGTH layouts i)
      list(APPEND layouts "${row}")
    endif()
    math(EXPR layout "${i} + 1")
  endif()

  list(APPEND entries "{${ram_kib}, ${board}, ${options}, ${sound}, ${flags}, ${keyed}, ${layout}},")
  math(EXPR entry "${entry} + 1")
endforeach()

# 7800 keys sorted by SHA-1, then entry: the padded entry sorts as text.
list(SORT keys)
set(key_rows "")
foreach(key IN LISTS keys)
  string(REPLACE "|" ";" key "${key}")
  list(GET key 0 sha1)
  list(GET key 1 e)
  list(GET key 2 p)
  math(EXPR e "${e}")
  string(REGEX REPLACE "([0-9a-f][0-9a-f])" "0x\\1," bytes "${sha1}")
  string(REGEX REPLACE ",$" "" bytes "${bytes}")
  string(APPEND key_rows "{{${bytes}},${e},${p}},\n")
endforeach()

# 2600 prefixes: every distinct SHA-1, sorted, cut to its first 8 bytes.
file(READ "${A2600}" xml2600)
string(REGEX MATCHALL "sha1=\"[^\"]*\"" sha1s "${xml2600}")
string(REGEX REPLACE "sha1=\"([^\"]*)\"" "\\1" sha1s "${sha1s}")
list(REMOVE_DUPLICATES sha1s)
list(SORT sha1s)
set(prefix_rows "")
set(previous "")
set(previous_sha1 "")
foreach(sha1 IN LISTS sha1s)
  string(FIND "${xml2600}" "sha1=\"${sha1}\"" pos)
  cartdb_check_sha1("${A2600}" ${pos} "${sha1}")
  string(SUBSTRING "${sha1}" 0 16 prefix)
  if(prefix STREQUAL previous)
    message(FATAL_ERROR "cartdb: ${A2600} offset ${pos}: ${sha1} and ${previous_sha1} "
      "share the 8-byte prefix ${prefix}")
  endif()
  set(previous ${prefix})
  set(previous_sha1 ${sha1})
  string(REGEX REPLACE "([0-9a-f][0-9a-f])" "0x\\1," bytes "${prefix}")
  string(REGEX REPLACE ",$" "" bytes "${bytes}")
  string(APPEND prefix_rows "{${bytes}},\n")
endforeach()

list(LENGTH keys key_count)
list(LENGTH entries entry_count)
list(LENGTH layouts layout_count)
list(LENGTH sha1s prefix_count)
list(JOIN entries "\n" entry_rows)
list(JOIN layouts "\n" layout_rows)

set(text "/* SPDX-License-Identifier: MIT AND CC0-1.0 */
/* Generated by tools/cartdb.cmake from MAME hash/a7800.xml and hash/a2600.xml at
   ${commit}; do not edit. */
#include \"cartdb.h\"

/* clang-format off */
const struct muhtari__cartdb_key muhtari__cartdb_7800_keys[] = {
${key_rows}};
const uint32_t muhtari__cartdb_7800_key_count = ${key_count};

const struct muhtari__cartdb_entry muhtari__cartdb_7800_entries[] = {
${entry_rows}
};
const uint32_t muhtari__cartdb_7800_entry_count = ${entry_count};

const struct muhtari__cartdb_layout muhtari__cartdb_7800_layouts[] = {
${layout_rows}
};
const uint32_t muhtari__cartdb_7800_layout_count = ${layout_count};

const uint8_t muhtari__cartdb_2600_prefix[][8] = {
${prefix_rows}};
const uint32_t muhtari__cartdb_2600_prefix_count = ${prefix_count};
/* clang-format on */
")
file(WRITE "${OUT}" "${text}")
message(STATUS "cartdb: ${key_count} keys, ${entry_count} entries, ${layout_count} layouts, "
  "${prefix_count} 2600 prefixes")
