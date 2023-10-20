cmake_minimum_required(VERSION 3.20)

#[[
Set default source groups.

set_target_source_groups(<target>
                         [STRIP_PREFIX <strip-prefix>]
                         [STRIP_SOURCE_PREFIX <strip-source-prefix>]
                         [STRIP_BINARY_PREFIX <strip-binary-prefix>]
                         [EXTRA_BINARY_DIRECTORY <extra-binary-dir>])

Create source groups "Header Files", "Header Templates", "Source Files", "Source Templates",
"Generated Files" for the source files of the target <target>. Only files inside the directory
trees rooted at SOURCE_DIR and BINARY_DIR of <target> are grouped.

If specified, the common path prefix <strip-source-prefix> of the files inside SOURCE_DIR and
the common path prefix <strip-binary-prefix> of the files inside BINARY_DIR gets removed,
it is an error if not all paths start with that prefix. Use <strip-prefix> if the same prefix
should be used for SOURCE_DIR and BINARY_DIR, the other two parameters must not be specified in that case.

If <extra-binary-dir> is specified, source files of <target> inside that directory will also get added
to the group "Generated Files", no prefix stripping will be applied to these files. If <extra-binary-dir>
is not absolute it is interpreted relative to ${CMAKE_CURRENT_BINARY_DIR}. The <extra-binary-dir> may
contain BINARY_DIR of <target>, the contents of BINARY_DIR will get excluded when processing <extra-binary-dir>.
]]
function(set_target_source_groups arg_TARGET)
  set(options "")
  set(singleValues STRIP_PREFIX STRIP_SOURCE_PREFIX STRIP_BINARY_PREFIX EXTRA_BINARY_DIRECTORY)
  set(multiValues "")
  cmake_parse_arguments(arg "${options}" "${singleValues}" "${multiValues}" ${ARGN})

  get_target_property(sourceDir ${arg_TARGET} SOURCE_DIR)
  get_target_property(binaryDir ${arg_TARGET} BINARY_DIR)
  get_target_property(sources ${arg_TARGET} SOURCES)

  if(DEFINED arg_STRIP_PREFIX)
    if(DEFINED arg_STRIP_SOURCE_PREFIX OR DEFINED arg_STRIP_BINARY_PREFIX)
      message(FATAL_ERROR "STRIP_PREFIX must not be used together with STRIP_SOURCE_PREFIX or STRIP_BINARY_PREFIX")
    endif()
    set(arg_STRIP_SOURCE_PREFIX "${arg_STRIP_PREFIX}")
    set(arg_STRIP_BINARY_PREFIX "${arg_STRIP_PREFIX}")
  endif()
  if(DEFINED arg_STRIP_SOURCE_PREFIX)
    cmake_path(SET sourceTreeDir NORMALIZE "${sourceDir}/${arg_STRIP_SOURCE_PREFIX}")
  else()
    set(sourceTreeDir "${sourceDir}")
  endif()
  if(DEFINED arg_STRIP_BINARY_PREFIX)
    cmake_path(SET binaryTreeDir NORMALIZE "${binaryDir}/${arg_STRIP_BINARY_PREFIX}")
  else()
    set(binaryTreeDir "${binaryDir}")
  endif()
  if(DEFINED arg_EXTRA_BINARY_DIRECTORY)
    cmake_path(ABSOLUTE_PATH arg_EXTRA_BINARY_DIRECTORY BASE_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}" NORMALIZE)
  endif()

  set(fileSources "")
  foreach(source IN LISTS sources)
    cmake_path(ABSOLUTE_PATH source BASE_DIRECTORY "${sourceDir}" NORMALIZE OUTPUT_VARIABLE file)
    list(APPEND fileSources "${file}")
  endforeach()
  # Normally, the build tree is a subdirectory of the source tree. For the root directory the prefix match
  # will also match the binary tree, so create separate file lists by filtering the binary prefix explicit.
  # This will fail when doing an in-tree build, in that case everything will be classified as binary.
  set(sourceFiles "")
  set(binaryFiles "")
  foreach(file IN LISTS fileSources)
    cmake_path(IS_PREFIX sourceDir "${file}" isSourceFile)
    cmake_path(IS_PREFIX binaryDir "${file}" isBinaryFile)
    if(isBinaryFile)
      list(APPEND binaryFiles "${file}")
    elseif(isSourceFile)
      list(APPEND sourceFiles "${file}")
    endif()
  endforeach()

  set(filterSources "${sourceFiles}")
  list(FILTER filterSources INCLUDE REGEX "/.+\\.h(h|pp)?$")
  source_group(
    TREE "${sourceTreeDir}"
    PREFIX "Header Files"
    FILES ${filterSources}
  )
  set(filterSources "${sourceFiles}")
  list(FILTER filterSources INCLUDE REGEX "/.+\\.h(h|pp)?\\.in$")
  source_group(
    TREE "${sourceTreeDir}"
    PREFIX "Header Templates"
    FILES ${filterSources}
  )
  set(filterSources "${sourceFiles}")
  list(FILTER filterSources INCLUDE REGEX "/.+\\.c(c|xx|pp)?$")
  source_group(
    TREE "${sourceTreeDir}"
    PREFIX "Source Files"
    FILES ${filterSources}
  )
  set(filterSources "${sourceFiles}")
  list(FILTER filterSources INCLUDE REGEX "/.+\\.c(c|xx|pp)?\\.in$")
  source_group(
    TREE "${sourceTreeDir}"
    PREFIX "Source Templates"
    FILES ${filterSources}
  )

  set(filterSources "${binaryFiles}")
  source_group(
    TREE "${binaryTreeDir}"
    PREFIX "Generated Files"
    FILES ${filterSources}
  )
  if(DEFINED arg_EXTRA_BINARY_DIRECTORY)
    # If the specified directory contains the binary dir of the target, the files will get added
    # twice with a different path, so exclude that directory.
    set(filterSources "")
    foreach(file IN LISTS fileSources)
      cmake_path(IS_PREFIX arg_EXTRA_BINARY_DIRECTORY "${file}" isExtraBinaryFile)
      if(isExtraBinaryFile)
        cmake_path(IS_PREFIX binaryDir "${file}" isBinaryFile)
        if(NOT isBinaryFile)
          list(APPEND filterSources "${file}")
        endif()
      endif()
    endforeach()
    source_group(
      TREE "${arg_EXTRA_BINARY_DIRECTORY}"
      PREFIX "Generated Files"
      FILES ${filterSources}
    )
  endif()
endfunction()
