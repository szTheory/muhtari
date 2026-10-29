# The readable-file rule shared by the local pass (local.cmake) and the library
# preset's configure-time scans (library.cmake). Include it, then call
#   compat_files(<prefix> <files_var> <unread_var> <root>...)
# which sets <files_var> to the sorted regular files under the roots that a
# scan may read (size above 0, at most the image maximum) and <unread_var> to
# the count of links and unreadable entries it skipped. A link, dangling or not,
# is never followed, and nothing skipped is named.
# Faults this prevents: a scan that hands a link, an unreadable entry or a name
# a CMake list cannot carry to file(SIZE) or file(SHA1), whose own error text
# prints the full private path, or that miscounts files. IS_READABLE needs
# CMake 3.29; the presets already need more.

function(compat_files prefix files_var unread_var)
  set(found "")
  foreach(root IN LISTS ARGN)
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
