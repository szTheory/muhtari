# Writes docs/compatibility.md from the compatibility ledger, run as
#   cmake -DLEDGER=tests/compat/ledger.csv -DEXPECTED=tests/diag/expected.csv
#     -DINVENTORY=tests/inventory/ci.txt -DOUT=docs/compatibility.md
#     [-DCHECK=<committed page>] -P tools/compat_doc.cmake
# Fault prevented: a published compatibility page that disagrees with the
# ledger, or a ledger that claims more than it shows. With CHECK
# (policy.compat_doc_fresh, library.compat_doc) the page in OUT must equal
# CHECK byte for byte. The generator fails, naming the row, on a status outside
# pass/fail/unknown/unsupported/skipped, on unsupported or skipped without
# why=, on a pr row that is unknown, on a pinned-list row (an id without "/")
# that is pass or fail or carries evidence, on a board, peripheral or region
# token with no claim entry, on a claim whose backing test is not in the
# inventory, and on a claim whose sample-backed test (basic.<sample>,
# title.<sample>.fast or title.<sample>.bios) runs a sample whose ledger row
# lacks the claim's token.
cmake_minimum_required(VERSION 3.25)
foreach(required LEDGER OUT EXPECTED INVENTORY)
  if("${${required}}" STREQUAL "")
    message(FATAL_ERROR "compat_doc: -DLEDGER, -DEXPECTED, -DINVENTORY and -DOUT are required")
  endif()
endforeach()

# One claim per entry, kind:name:regex:tests. The regex selects the rows of
# the expected-value file (by ROM name) that back the claim, "-" for none; the
# tests are names in the inventory, "-" for none. Controller families are not
# ledger columns, so only this table names them.
set(claims
  "board:absolute:^absolute$:diag.retail"
  "board:activision:^activision$:diag.retail"
  "board:bank6_at_4000:^bank6$:diag.bank6"
  "board:bankset:^bankset:diag.bankset diag.bankset_tracer"
  "board:fractalus:^fractalus$:diag.retail"
  "board:linear:^linear16$:diag.linear16 title.simple.fast"
  "board:ram:-:basic.rmtramdemo basic.rmtramcompressiondemo"
  "board:rom_at_4000:^sg144$:diag.sg144"
  "board:supergame:^(sg144|bank6)$:diag.sg144 basic.rmtramdemo"
  "board:xboard:^expansion-x:diag.expansion"
  "board:xm:^expansion-m:diag.expansion"
  "controller:SNES2Atari and Mega7800:^motion[45]:diag.controllers"
  "controller:XG-1 light gun:^light:input.lightgun diag.controllers"
  "controller:driving:^motion0$:diag.controllers"
  "controller:paddles and keypad:^analog:diag.controllers"
  "controller:trak-ball and ST/Amiga motion:^motion[1-3]$:diag.controllers"
  "peripheral:hsc:^battery$:core.hsc.decode diag.battery"
  "peripheral:lightgun:^light0$:input.lightgun diag.controllers"
  "peripheral:pokey450:-:basic.pokey basic.rmtromdemo diag.pal"
  "peripheral:pokey4000:-:-"
  "peripheral:savekey:^battery$:input.savekey.protocol libretro.host.battery"
  "peripheral:xboard:^expansion-x:diag.expansion"
  "peripheral:xm:^expansion-m:diag.expansion"
  "region:NTSC:-:title.simple.fast"
  "region:PAL:^pal$:diag.pal runner.region.pal title.pal.fast"
  "region:any:-:-")

# Expected values per ROM: fit rows are fitted to a model, guide, hw and rtl
# rows come from a document, hardware or RTL. A claim is model-only when its
# fit rows outnumber the others, owned-oracle otherwise; with no row it takes
# the evidence kind of the ledger's pass rows that carry the token.
file(STRINGS "${EXPECTED}" expected_lines)
list(POP_FRONT expected_lines)
set(expected_roms "")
foreach(line IN LISTS expected_lines)
  string(REPLACE "," ";" f "${line}")
  list(GET f 0 3 g)
  list(POP_FRONT g rom source)
  list(FIND expected_roms "${rom}" known)
  if(known EQUAL -1)
    list(APPEND expected_roms "${rom}")
    set(fit_${rom} 0)
    set(other_${rom} 0)
  endif()
  if(source STREQUAL "fit")
    math(EXPR fit_${rom} "${fit_${rom}} + 1")
  else()
    math(EXPR other_${rom} "${other_${rom}} + 1")
  endif()
endforeach()

# Each backing test must be an inventory name, as info_claims.cmake checks.
# A sample-backed test must also load a sample whose ledger row carries the
# claim, so a linear sample cannot back a supergame claim. That check runs
# after the ledger loop, which records each sample's columns.
file(STRINGS "${INVENTORY}" inventory)
foreach(claim IN LISTS claims)
  string(REPLACE ":" ";" f "${claim}")
  list(GET f 0 1 3 g)
  list(POP_FRONT g kind name tests)
  if(NOT tests STREQUAL "-")
    string(REPLACE " " ";" tests "${tests}")
    foreach(test IN LISTS tests)
      string(REPLACE "." "\\." pattern "${test}")
      set(found ${inventory})
      list(FILTER found INCLUDE REGEX "^${pattern}\t")
      if(NOT found)
        message(FATAL_ERROR "compat_doc: ${name} names ${test}, which is not in ${INVENTORY}")
      endif()
    endforeach()
  endif()
endforeach()

# No field holds a comma; a predicate's ";why=" is kept as a placeholder.
file(READ "${LEDGER}" text)
string(REPLACE ";" "<semicolon>" text "${text}")
string(REGEX REPLACE "\n$" "" text "${text}")
string(REPLACE "\n" ";" lines "${text}")
list(POP_FRONT lines)

macro(compat_inc name)
  if(NOT DEFINED ${name})
    set(${name} 0)
  endif()
  math(EXPR ${name} "${${name}} + 1")
endmacro()

set(statuses pass fail unknown unsupported skipped)
set(owned_table "")
set(pinned_table "")
set(pinned_total 0)
set(pinned_accounted 0)
foreach(line IN LISTS lines)
  string(REPLACE "," ";" f "${line}")
  list(GET f 0 1 6 7 8 9 10 11 12 13 14 15 g)
  list(POP_FRONT g id title board region peripherals predicate status tier commit evidence oracle checked)
  if(NOT status IN_LIST statuses)
    message(FATAL_ERROR "compat_doc: ${id}: status ${status} is outside pass, fail, unknown, unsupported, skipped")
  endif()
  set(why "")
  if(predicate MATCHES "<semicolon>why=(.+)$")
    set(why " (${CMAKE_MATCH_1})")
  endif()
  set(reason "${why}")
  if(NOT status STREQUAL "unsupported" AND NOT status STREQUAL "skipped")
    set(reason "")
  endif()
  if((status STREQUAL "unsupported" OR status STREQUAL "skipped") AND why STREQUAL "")
    message(FATAL_ERROR "compat_doc: ${id}: ${status} needs why= in its predicate")
  endif()
  if(tier STREQUAL "pr" AND status STREQUAL "unknown")
    message(FATAL_ERROR "compat_doc: ${id}: a pr row cannot be unknown")
  endif()
  set(pop owned)
  if(NOT id MATCHES "/")
    set(pop pinned)
    if(status STREQUAL "pass" OR status STREQUAL "fail")
      message(FATAL_ERROR "compat_doc: ${id}: a pinned-list row cannot be ${status}")
    endif()
    foreach(field commit evidence oracle checked)
      if(NOT "${${field}}" STREQUAL "-")
        message(FATAL_ERROR "compat_doc: ${id}: a pinned-list row cannot carry ${field}")
      endif()
    endforeach()
    compat_inc(pinned_accounted)
  endif()
  compat_inc(${pop}_total)
  if(id MATCHES "^7800basic-0[.]41/(.+)$")
    set(sample_board_${CMAKE_MATCH_1} "${board}")
    set(sample_peripheral_${CMAKE_MATCH_1} "${peripherals}")
    set(sample_region_${CMAKE_MATCH_1} "${region}")
  endif()

  # Evidence kind by rule: an owned diagnostic is model conformance; a
  # 7800basic title@ pass is a public title screen, the frame the sample shows
  # by the predicate's frame (docs/emulation-tests.md, title), not gameplay; a
  # boots@ pass is boot only.
  set(label "${status}")
  if(status STREQUAL "pass")
    if(id MATCHES "^diag/")
      set(label model)
    elseif(predicate MATCHES "^boots@")
      set(label boot)
    elseif(id MATCHES "^7800basic-0[.]41/" AND predicate MATCHES "^title@")
      set(label title)
    else()
      set(label model)
    endif()
  endif()
  compat_inc(kind_${pop}_${label})
  compat_inc(tier_${pop}_${status}_${tier})
  compat_inc(tier_${pop}_${status}_total)

  # Tokens of the board, peripheral and region columns need a claim entry.
  foreach(column board peripherals region)
    set(kind ${column})
    if(column STREQUAL "peripherals")
      set(kind peripheral)
    endif()
    string(REPLACE "+" ";" tokens "${${column}}")
    foreach(token IN LISTS tokens)
      if(token STREQUAL "-")
        continue()
      endif()
      if(NOT DEFINED seen_${kind}_${token})
        set(seen_${kind}_${token} ON)
        set(mapped OFF)
        foreach(claim IN LISTS claims)
          if(claim MATCHES "^${kind}:${token}:")
            set(mapped ON)
          endif()
        endforeach()
        if(NOT mapped)
          message(FATAL_ERROR "compat_doc: ${token} has no claim entry")
        endif()
      endif()
      if(status STREQUAL "pass")
        set(carried_${kind}_${token}_${label} ON)
      endif()
    endforeach()
  endforeach()

  string(REGEX MATCH "^[a-z]+" shape "${predicate}")
  set(shown "${status}")
  if(status STREQUAL "unknown")
    set(shown "not evaluated")
  endif()
  set(row "| `${id}` | ${title} | ${board} | ${region} | ${peripherals} | ${shape} | ${shown}${reason} |\n")
  string(APPEND ${pop}_table "${row}")
endforeach()

# A status by tier table of one population; every status prints, zeros included.
function(compat_tiers pop result)
  set(out "| Status | pr | nightly | local | total |\n|---|---|---|---|---|\n")
  foreach(status IN LISTS statuses)
    set(shown "${status}")
    if(status STREQUAL "unknown")
      set(shown "not evaluated")
    endif()
    string(APPEND out "| ${shown} |")
    foreach(tier pr nightly local total)
      set(n 0)
      if(DEFINED tier_${pop}_${status}_${tier})
        set(n ${tier_${pop}_${status}_${tier}})
      endif()
      string(APPEND out " ${n} |")
    endforeach()
    string(APPEND out "\n")
  endforeach()
  set(${result} "${out}" PARENT_SCOPE)
endfunction()

# An evidence kind table: pairs of the counter's suffix and its label.
function(compat_kinds pop result)
  set(out "| Evidence kind | Rows |\n|---|---|\n")
  foreach(pair IN LISTS ARGN)
    string(REPLACE "=" ";" pair "${pair}")
    list(GET pair 0 suffix)
    list(GET pair 1 shown)
    set(n 0)
    if(DEFINED kind_${pop}_${suffix})
      set(n ${kind_${pop}_${suffix}})
    endif()
    string(APPEND out "| ${shown} | ${n} |\n")
  endforeach()
  set(${result} "${out}" PARENT_SCOPE)
endfunction()

foreach(pop owned pinned)
  if(NOT DEFINED ${pop}_total)
    set(${pop}_total 0)
  endif()
endforeach()
compat_tiers(pinned pinned_tiers)
compat_tiers(owned owned_tiers)
compat_kinds(pinned pinned_kinds "pass=pass" "fail=fail" "unknown=not evaluated"
  "skipped=skipped (reason)" "unsupported=unsupported (reason)")
compat_kinds(owned owned_kinds "title=public title screen" "boot=public boot only"
  "model=model conformance" "fail=fail" "unknown=not evaluated"
  "skipped=skipped (reason)" "unsupported=unsupported (reason)")

foreach(status IN LISTS statuses)
  set(n 0)
  if(DEFINED tier_pinned_${status}_total)
    set(n ${tier_pinned_${status}_total})
  endif()
  set(count_${status} ${n})
endforeach()
set(headline "${pinned_accounted} of ${pinned_total} pinned-list entries accounted for; ${count_pass} pass, ${count_fail} fail, ${count_unsupported} unsupported, ${count_unknown} not evaluated, ${count_skipped} skipped. Not evaluated means untested, not failing.")

# The claims table, sorted by kind then name.
set(claim_table "| Kind | Claim | Evidence class | Backing tests |\n|---|---|---|---|\n")
set(sorted ${claims})
list(SORT sorted)
foreach(claim IN LISTS sorted)
  string(REPLACE ":" ";" f "${claim}")
  list(GET f 0 1 2 3 g)
  list(POP_FRONT g kind name regex tests)
  set(fit 0)
  set(other 0)
  if(NOT regex STREQUAL "-")
    foreach(rom IN LISTS expected_roms)
      if(rom MATCHES "${regex}")
        math(EXPR fit "${fit} + ${fit_${rom}}")
        math(EXPR other "${other} + ${other_${rom}}")
      endif()
    endforeach()
  endif()
  set(class none)
  if(fit GREATER other)
    set(class model-only)
  elseif(fit GREATER 0 OR other GREATER 0)
    set(class owned-oracle)
  elseif(NOT kind STREQUAL "controller")
    if(carried_${kind}_${name}_model)
      set(class model-only)
    endif()
    if(carried_${kind}_${name}_boot)
      set(class "public boot only")
    endif()
    if(carried_${kind}_${name}_title)
      set(class "public title screen")
    endif()
  endif()
  string(REPLACE " " ", " tests "${tests}")
  if(tests STREQUAL "-")
    set(tests none)
  endif()
  string(APPEND claim_table "| ${kind} | ${name} | ${class} | ${tests} |\n")
endforeach()

foreach(claim IN LISTS claims)
  string(REPLACE ":" ";" f "${claim}")
  list(GET f 0 1 3 g)
  list(POP_FRONT g kind name tests)
  if(kind STREQUAL "controller" OR tests STREQUAL "-")
    continue()
  endif()
  string(REPLACE " " ";" tests "${tests}")
  foreach(test IN LISTS tests)
    # Match one pattern at a time: a failed match clears CMAKE_MATCH_1, so an
    # OR of the two would lose the sample of a basic.<sample> test.
    set(sample "")
    if(test MATCHES "^basic[.](.+)$")
      set(sample "${CMAKE_MATCH_1}")
    elseif(test MATCHES "^title[.](.+)[.](fast|bios)$")
      set(sample "${CMAKE_MATCH_1}")
    endif()
    if(NOT sample STREQUAL "")
      if(DEFINED sample_${kind}_${sample})
        string(REPLACE "+" ";" tokens "${sample_${kind}_${sample}}")
        if(NOT name IN_LIST tokens)
          message(FATAL_ERROR "compat_doc: ${name} names ${test}, whose sample ${sample} does not carry ${name}")
        endif()
      endif()
    endif()
  endforeach()
endforeach()

set(columns "| Id | Title | Board | Region | Peripherals | Predicate kind | Status |
|---|---|---|---|---|---|---|")
file(WRITE "${OUT}" "# Compatibility

Generated from `tests/compat/ledger.csv` by `tools/compat_doc.cmake`; do not edit it by hand.
[`docs/emulation-tests.md`](emulation-tests.md#ledger) says what each column and status means and how a
row is judged.

Evidence covers the rows listed; there is no claim of universal title compatibility. The pinned list's
board, region and peripheral columns repeat its declarations: `src/cartdb.c` recognising an image is
metadata, not playability. Owned diagnostics show model conformance against documented sequences, never
gameplay. Detail behind the claims: [cartridge behavior](hardware/cartridge.md),
[PAL evidence and limits](pal-tests.md), [BankSet](bankset-tests.md) and [expansion](expansion-tests.md)
tests.

${headline}

## Status by tier

Pinned CC0 list: ${pinned_total} entries

${pinned_tiers}
Owned samples and diagnostics: ${owned_total} rows

${owned_tiers}
## Evidence kinds

Pinned list rows:

${pinned_kinds}
Owned rows (a diagnostic is model conformance, never a title screen):

${owned_kinds}
## Claims

Each claim names its evidence class and the tests that exercise it. owned-oracle: expected values taken
from documents, hardware or RTL. model-only: mostly fitted values, so the model follows its documented
sequences without hardware calibration. public title screen: a ledger pass row whose frame hash matches
the screen a public sample shows by that frame, which is not gameplay. public boot only: a ledger pass row
that shows only that the sample boots. none: no evidence. No real HSC ROM is exercised and PAL is not retail-verified. A class
describes the evidence, not a promise about any title.

${claim_table}
## Owned rows

${columns}
${owned_table}
## Pinned list rows

${columns}
${pinned_table}")
if(NOT "${CHECK}" STREQUAL "")
  file(READ "${OUT}" fresh)
  file(READ "${CHECK}" committed)
  if(NOT fresh STREQUAL committed)
    message(FATAL_ERROR "compat_doc: ${CHECK} is stale; regenerate it with\n"
      "  cmake -DLEDGER=tests/compat/ledger.csv -DEXPECTED=tests/diag/expected.csv"
      " -DINVENTORY=tests/inventory/ci.txt -DOUT=docs/compatibility.md -P tools/compat_doc.cmake")
  endif()
endif()
