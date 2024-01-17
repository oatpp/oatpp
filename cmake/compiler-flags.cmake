###################################################################################################
## INCLUDES #######################################################################################
###################################################################################################

include(CheckCXXCompilerFlag)


###################################################################################################
## FUNCTIONS ######################################################################################
###################################################################################################

#
# Add compiler flags without test (assume they are supported)
#

function(add_cxx_compiler_flags_no_test var)
  foreach(flag ${ARGN})
    set(${var} "${${var}} ${flag}")
  endforeach()
  set(${var} "${${var}}" PARENT_SCOPE)
endfunction()


#
# Add compiler flags with test (do not assume they are supported)
#

function(add_cxx_compiler_flags var)
  foreach(flag ${ARGN})
    string(REGEX REPLACE "[^a-zA-Z0-9]+" "_" flag_var "CXXFLAG_${flag}")
    check_cxx_compiler_flag("${flag}" ${flag_var})
    if(${flag_var})
      set(${var} "${${var}} ${flag}")
    endif()
  endforeach()
  set(${var} "${${var}}" PARENT_SCOPE)
endfunction()


#
# Add compiler flags
#

function(add_compiler_flags version)
  if (CMAKE_CXX_COMPILER_VERSION VERSION_GREATER_EQUAL ${version})
    foreach(flag ${ARGN})
      add_cxx_compiler_flags_no_test(CMAKE_CXX_FLAGS ${flag})
      set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}" PARENT_SCOPE)
    endforeach()
  else (CMAKE_CXX_COMPILER_VERSION VERSION_GREATER_EQUAL ${version})
    foreach(flag ${ARGN})
      add_cxx_compiler_flags(CMAKE_CXX_FLAGS ${flag})
      set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}" PARENT_SCOPE)
    endforeach()
  endif (CMAKE_CXX_COMPILER_VERSION VERSION_GREATER_EQUAL ${version})
endfunction()


#
# Remove compiler flags
#

function(remove_any_compiler_flags var)
  foreach(flag ${ARGN})
    string(REPLACE       "${flag}" ""  ${var} "${${var}}")
    string(REGEX REPLACE "[ \t]+"  " " ${var} "${${var}}")
  endforeach()
  set(${var} "${${var}}" PARENT_SCOPE)
endfunction()

function(remove_compiler_flags)
  foreach(flag ${ARGN})
    remove_any_compiler_flags(CMAKE_CXX_FLAGS "${flag}")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}" PARENT_SCOPE)
  endforeach()
endfunction()


#
# Save and Restore compiler flags
#

function(save_compiler_flags)
  set(CMAKE_CXX_FLAGS_SAVED "${CMAKE_CXX_FLAGS}" CACHE STRING "Saved compiler C++ flags" FORCE)
endfunction()

function(restore_compiler_flags)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS_SAVED}"       )
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}" PARENT_SCOPE)
endfunction()


###################################################################################################
## COMPILER FLAGS #################################################################################
###################################################################################################

string(TOLOWER "${CMAKE_BUILD_TYPE}" CMAKE_BUILD_TYPE_LOWER)

#
# Generic flags
#
if (CMAKE_CXX_COMPILER_ID MATCHES GNU)
  add_compiler_flags(4.6 "-Wall")
  add_compiler_flags(4.6 "-Wextra")
endif (CMAKE_CXX_COMPILER_ID MATCHES GNU)

if (CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  add_compiler_flags(10 "-Weverything")
endif (CMAKE_CXX_COMPILER_ID MATCHES "Clang")


#
# Preserve debug information flags
#
if (CMAKE_CXX_COMPILER_ID MATCHES GNU)
  add_compiler_flags(4.6 "-g3")
endif (CMAKE_CXX_COMPILER_ID MATCHES GNU)

if (CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  add_compiler_flags(10 "-g3")
endif (CMAKE_CXX_COMPILER_ID MATCHES "Clang")


#
# C/C++ Warning Flags
# See https://github.com/vadz/gcc-warnings-tools for gcc flags and versions
#
if (CMAKE_CXX_COMPILER_ID MATCHES GNU)
  add_compiler_flags(4.6 "-Wcast-align")
  add_compiler_flags(4.6 "-Wconversion")
  add_compiler_flags(4.6 "-Wdouble-promotion")
  add_compiler_flags(4.6 "-Winvalid-pch")
  add_compiler_flags(4.6 "-Wmissing-declarations")
  add_compiler_flags(4.6 "-Wmissing-format-attribute")
  add_compiler_flags(4.6 "-Wmissing-include-dirs")
  add_compiler_flags(4.6 "-Wpointer-arith")
  add_compiler_flags(4.6 "-Wredundant-decls")
  add_compiler_flags(4.6 "-Wshadow")
  add_compiler_flags(4.6 "-Wsign-conversion")
  #add_compiler_flags(4.6 "-Wsuggest-attribute=const")
  add_compiler_flags(4.6 "-Wsuggest-attribute=noreturn")
  #add_compiler_flags(4.6 "-Wsuggest-attribute=pure")
  add_compiler_flags(4.6 "-Wswitch-default")
  add_compiler_flags(4.6 "-Wswitch-enum")
  add_compiler_flags(4.6 "-Wtype-limits")
  add_compiler_flags(4.6 "-Wundef")
  add_compiler_flags(4.6 "-Wuninitialized")
  add_compiler_flags(4.6 "-Wunknown-pragmas")
  add_compiler_flags(4.6 "-Wunsafe-loop-optimizations")
  add_compiler_flags(4.6 "-Wunused-but-set-parameter")
  add_compiler_flags(4.6 "-Wunused-but-set-variable")
  add_compiler_flags(4.6 "-Wunused-function")
  add_compiler_flags(4.6 "-Wunused")
  add_compiler_flags(4.6 "-Wunused-label")
  add_compiler_flags(4.6 "-Wunused-macros")
  #add_compiler_flags(4.6 "-Wunused-parameter")
  add_compiler_flags(4.6 "-Wunused-result")
  add_compiler_flags(4.6 "-Wunused-value")
  add_compiler_flags(4.6 "-Wunused-variable")

  add_compiler_flags(4.7 "-Wunused-local-typedefs")

  add_compiler_flags(4.8 "-Wformat=2")
  add_compiler_flags(4.8 "-Wsuggest-attribute=format")

  add_compiler_flags(5.1 "-Wformat-signedness")
  #add_compiler_flags(5.1 "-Wsuggest-final-methods")
  #add_compiler_flags(5.1 "-Wsuggest-final-types")

  add_compiler_flags(6.1 "-Wduplicated-cond")
  add_compiler_flags(6.1 "-Wlogical-op")
  add_compiler_flags(6.1 "-Wnull-dereference")

  add_compiler_flags(7.1 "-Wduplicated-branches")
  add_compiler_flags(7.1 "-Wformat-overflow=2")
  add_compiler_flags(7.1 "-Wformat-truncation=2")

  #add_compiler_flags(8.1 "-Wcast-align=strict")
  add_compiler_flags(8.1 "-Wsuggest-attribute=cold")
  add_compiler_flags(8.1 "-Wsuggest-attribute=malloc")

  add_compiler_flags(10.1 "-Warith-conversion")

  add_compiler_flags(12.1 "-ftrivial-auto-var-init=zero")
  add_compiler_flags(12.1 "-Warray-compare")
  add_compiler_flags(12.1 "-Wbidi-chars=unpaired,ucn")
  add_compiler_flags(12.1 "-Winfinite-recursion")
  add_compiler_flags(12.1 "-Wopenacc-parallelism")
  add_compiler_flags(12.1 "-Wtrivial-auto-var-init")
endif (CMAKE_CXX_COMPILER_ID MATCHES GNU)


#
# C++ Warning Flags
# See https://github.com/vadz/gcc-warnings-tools for gcc flags and versions
#
if (CMAKE_CXX_COMPILER_ID MATCHES GNU)
  add_compiler_flags(4.6 "-Wctor-dtor-privacy")
  add_compiler_flags(4.6 "-Wnoexcept")
  add_compiler_flags(4.6 "-Wold-style-cast")
  add_compiler_flags(4.6 "-Woverloaded-virtual")
  add_compiler_flags(4.6 "-Wsign-promo")
  add_compiler_flags(4.6 "-Wstrict-null-sentinel")

  add_compiler_flags(4.7 "-Wzero-as-null-pointer-constant")

  add_compiler_flags(4.8 "-Wuseless-cast")

  add_compiler_flags(4.9 "-Wconditionally-supported")

  add_compiler_flags(5.1 "-Wsuggest-override")

  add_compiler_flags(7.1 "-Wnoexcept-type")

  add_compiler_flags(8.1 "-Wextra-semi")

  add_compiler_flags(10.1 "-Wcomma-subscript")
  add_compiler_flags(10.1 "-Wmismatched-tags")
  add_compiler_flags(10.1 "-Wredundant-tags")
  add_compiler_flags(10.1 "-Wvolatile")

  add_compiler_flags(11.1 "-Wctad-maybe-unsupported")
  add_compiler_flags(11.1 "-Wdeprecated-enum-enum-conversion")
  add_compiler_flags(11.1 "-Wdeprecated-enum-float-conversion")
  add_compiler_flags(11.1 "-Winvalid-imported-macros")
  add_compiler_flags(11.1 "-Wrange-loop-construct")
endif (CMAKE_CXX_COMPILER_ID MATCHES GNU)


#
# Allow the linker to remove unused data and functions
#
if (CMAKE_CXX_COMPILER_ID MATCHES GNU)
  add_compiler_flags(4.6 "-fdata-sections")
  add_compiler_flags(4.6 "-ffunction-sections")
  add_compiler_flags(4.6 "-fno-common")
  add_compiler_flags(4.6 "-Wl,--gc-sections")
endif (CMAKE_CXX_COMPILER_ID MATCHES GNU)


#
# Hardening flags
# See https://developers.redhat.com/blog/2018/03/21/compiler-and-linker-flags-gcc
#
if (CMAKE_CXX_COMPILER_ID MATCHES GNU)
  IF (NOT CMAKE_BUILD_TYPE_LOWER STREQUAL "debug")
    add_cxx_compiler_flags_no_test(CMAKE_CXX_FLAGS "-D_FORTIFY_SOURCE=2")
  ENDIF (NOT CMAKE_BUILD_TYPE_LOWER STREQUAL "debug")
  add_cxx_compiler_flags_no_test(CMAKE_CXX_FLAGS "-D_GLIBCXX_ASSERTIONS")
  add_compiler_flags(4.6 "-fasynchronous-unwind-tables")
  add_compiler_flags(4.6 "-fexceptions")
  add_compiler_flags(8.1 "-fstack-clash-protection")
  add_compiler_flags(4.6 "-fstack-protector-strong")
  add_compiler_flags(4.6 "-grecord-gcc-switches")
  # Issue 872: https://github.com/oatpp/oatpp/issues/872
  # -fcf-protection is supported only on x86 GNU/Linux per this gcc doc:
  # https://gcc.gnu.org/onlinedocs/gcc/Instrumentation-Options.html#index-fcf-protection
  if ((CMAKE_SYSTEM_NAME STREQUAL "Linux") AND (CMAKE_SYSTEM_PROCESSOR MATCHES "(x86)|(X86)"))
    add_compiler_flags(8.1 "-fcf-protection")
  endif ()
  add_compiler_flags(4.6 "-pipe")
  add_compiler_flags(4.6 "-Werror=format-security")
  add_compiler_flags(4.6 "-Wno-format-nonliteral")
  add_compiler_flags(4.6 "-fPIE")
  add_compiler_flags(4.6 "-Wl,-z,defs")
  add_compiler_flags(4.6 "-Wl,-z,now")
  add_compiler_flags(4.6 "-Wl,-z,relro")
endif (CMAKE_CXX_COMPILER_ID MATCHES GNU)


#
# Profiling and Debugging
# See https://fedoraproject.org/wiki/Changes/fno-omit-frame-pointer
#
#if (CMAKE_CXX_COMPILER_ID MATCHES GNU)
#  add_compiler_flags(4.6 "-fno-omit-frame-pointer")
#endif (CMAKE_CXX_COMPILER_ID MATCHES GNU)


#
# Disable some warnings
#
if (CMAKE_CXX_COMPILER_ID MATCHES GNU)
  add_compiler_flags(4.6 "-Wno-unused-parameter")

  add_compiler_flags(9.1 "-Wno-pessimizing-move")
endif (CMAKE_CXX_COMPILER_ID MATCHES GNU)

if (CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  add_compiler_flags(14 "-Wno-c++14-attribute-extensions")
  add_compiler_flags(10 "-Wno-c++14-extensions")
  add_compiler_flags(10 "-Wno-c++98-compat")
  add_compiler_flags(10 "-Wno-c++98-compat-pedantic")
  add_compiler_flags(10 "-Wno-covered-switch-default")
  add_compiler_flags(10 "-Wno-delete-non-abstract-non-virtual-dtor")
  add_compiler_flags(10 "-Wno-deprecated-copy-dtor")
  add_compiler_flags(14 "-Wno-deprecated-copy-with-user-provided-dtor")
  add_compiler_flags(10 "-Wno-documentation")
  add_compiler_flags(10 "-Wno-documentation-unknown-command")
  add_compiler_flags(10 "-Wno-exit-time-destructors")
  add_compiler_flags(10 "-Wno-format-nonliteral")
  add_compiler_flags(10 "-Wno-global-constructors")
  add_compiler_flags(10 "-Wno-gnu-zero-variadic-macro-arguments")
  add_compiler_flags(10 "-Wno-implicit-int-conversion")
  add_compiler_flags(10 "-Wno-non-virtual-dtor")
  add_compiler_flags(10 "-Wno-padded")
  add_compiler_flags(10 "-Wno-pessimizing-move")
  add_compiler_flags(99 "-Wno-return-std-move-in-c++11")
  add_compiler_flags(10 "-Wno-reserved-id-macro")
  add_compiler_flags(14 "-Wno-reserved-identifier")
  add_compiler_flags(10 "-Wno-string-conversion")
  add_compiler_flags(14 "-Wno-suggest-destructor-override")
  add_compiler_flags(10 "-Wno-unknown-warning-option")
  add_compiler_flags(10 "-Wno-unneeded-member-function")
  add_compiler_flags(10 "-Wno-unreachable-code-return")
  add_compiler_flags(10 "-Wno-unsafe-buffer-usage")
  add_compiler_flags(10 "-Wno-unused-member-function")
  add_compiler_flags(10 "-Wno-unused-parameter")
  add_compiler_flags(10 "-Wno-unused-template")
  add_compiler_flags(10 "-Wno-weak-vtables")
endif (CMAKE_CXX_COMPILER_ID MATCHES "Clang")


#
# Sanitize flags
#
#if (CMAKE_CXX_COMPILER_ID MATCHES GNU)
#  add_compiler_flags(4.6 "-fsanitize=address")
#  add_compiler_flags(4.6 "-fsanitize=thread")
#endif (CMAKE_CXX_COMPILER_ID MATCHES GNU)
