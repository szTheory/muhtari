# The readable-file rule shared by the local pass (local.cmake) and the library
# preset's configure-time scans (library.cmake). Include it, then call
#   compat_files(<prefix> <files_var> <unread_var> <root>...)
# which sets <files_var> to the sorted regular files under the roots that a
# scan may read (size above 0, at most the image maximum) and <unread_var> to
# the count of links and unreadable entries it skipped; a root whose own name
# holds '[', ']' or ';' stops it first (compat_name_check does that alone). A link, dangling or not,
# is never followed, and nothing skipped is named.
# Faults this prevents: a scan that hands a link, an unreadable entry or a name
# a CMake list cannot carry to file(SIZE) or file(SHA1), whose own error text
# prints the full private path, or that miscounts files. IS_READABLE needs
# CMake 3.29; the presets already need more.

# A bracket or a semicolon in a root is also a glob metacharacter or a list
# separator, so a path holding one would be split or matched wrongly.
function(compat_name_check prefix path)
  if("${path}" MATCHES "[][;]")
    message(FATAL_ERROR "${prefix}: the directory name holds a bracket or a semicolon")
  endif()
endfunction()

# The roots are read from ARGV, not ARGN, so a ';' inside one root stays in it.
function(compat_files prefix files_var unread_var)
  if(ARGC LESS 4)
    message(FATAL_ERROR "files: compat_files needs a root")
  endif()
  math(EXPR last "${ARGC} - 1")
  set(found "")
  foreach(at RANGE 3 ${last})
    set(root "${ARGV${at}}")
    compat_name_check("${prefix}" "${root}")
    file(GLOB_RECURSE under LIST_DIRECTORIES false "${root}/*")
    list(APPEND found "${under}")
  endforeach()
  list(REMOVE_DUPLICATES found)
  list(SORT found)
  set(kept "")
  set(unread 0)
  foreach(file IN LISTS found)
    if(IS_SYMLINK "${file}")
      math(EXPR unread "${unread} + 1")
      continue()
    endif()
    # A ';' in a name splits it and an unmatched '[' merges the elements that
    # follow. A file name holds no '/', so the piece after a split is never
    # absolute and a merged element holds a ';': either is a name the list could
    # not carry. (EXISTS is no test here: CMake reports a write-only file absent.)
    if("${file}" MATCHES ";" OR NOT IS_ABSOLUTE "${file}")
      message(FATAL_ERROR "${prefix}: a file name holds an unmatched bracket or a semicolon")
    endif()
    if(NOT IS_READABLE "${file}" OR IS_DIRECTORY "${file}")
      math(EXPR unread "${unread} + 1")
      continue()
    endif()
    file(SIZE "${file}" size)
    if(size GREATER 0 AND size LESS_EQUAL 1048704)
      list(APPEND kept "${file}")
    endif()
  endforeach()
  set(${files_var} "${kept}" PARENT_SCOPE)
  set(${unread_var} "${unread}" PARENT_SCOPE)
endfunction()
