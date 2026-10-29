# User-facing wording check, run as
#   cmake -DROOT=<source dir> -P claims_wording.cmake
# Fault prevented: README.md or the libretro .info drops its disclaimer that
# the evidence covers only the rows listed, or a user-facing file overclaims
# compatibility. The phrase list below is the whole deny list; it is matched
# without regard to case in README.md, docs/*.md and libretro/*.info.in.
if(NOT IS_DIRECTORY "${ROOT}")
  message(FATAL_ERROR "claims_wording: -DROOT=<source dir> is required")
endif()

set(readme_sentence
  "Evidence covers the rows listed; there is no claim of universal title compatibility.")
set(info_sentence "No universal title compatibility.")
set(deny "whole[ -]library" "every game" "all games" "fully compatible" "full compatibility")

set(problems "")
file(GLOB info_files "${ROOT}/libretro/*.info.in")
file(GLOB doc_files "${ROOT}/docs/*.md")

# The disclaimer is required where users read it.
if(EXISTS "${ROOT}/README.md")
  file(READ "${ROOT}/README.md" text)
  string(FIND "${text}" "${readme_sentence}" at)
  if(at EQUAL -1)
    string(APPEND problems "\n  README.md: missing \"${readme_sentence}\"")
  endif()
else()
  string(APPEND problems "\n  README.md: file is missing")
endif()
if(EXISTS "${ROOT}/libretro/muhtari_libretro.info.in")
  file(READ "${ROOT}/libretro/muhtari_libretro.info.in" text)
  string(FIND "${text}" "${info_sentence}" at)
  if(at EQUAL -1)
    string(APPEND problems "\n  libretro/muhtari_libretro.info.in: missing \"${info_sentence}\"")
  endif()
else()
  string(APPEND problems "\n  libretro/muhtari_libretro.info.in: file is missing")
endif()

# No user-facing file overclaims.
set(scanned "")
foreach(file IN ITEMS "${ROOT}/README.md" ${doc_files} ${info_files})
  if(NOT EXISTS "${file}")
    continue()
  endif()
  file(RELATIVE_PATH rel "${ROOT}" "${file}")
  list(APPEND scanned "${rel}")
  file(READ "${file}" text)
  string(TOLOWER "${text}" text)
  foreach(phrase IN LISTS deny)
    if(text MATCHES "${phrase}")
      string(APPEND problems "\n  ${rel}: overclaims with \"${phrase}\"")
    endif()
  endforeach()
endforeach()

if(problems)
  message(FATAL_ERROR "claims_wording:${problems}")
endif()
list(LENGTH scanned count)
message(STATUS "claims_wording: ${count} files checked")
