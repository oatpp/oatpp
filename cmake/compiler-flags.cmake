#cmake_minimum_required(VERSION 3.1)

###################################################################################################
## INCLUDES #######################################################################################
###################################################################################################

include(CheckCXXCompilerFlag)


###################################################################################################
## FUNCTIONS ######################################################################################
###################################################################################################

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

function(add_compiler_flags)
  foreach(flag ${ARGN})
    add_cxx_compiler_flags(CMAKE_CXX_FLAGS ${flag})
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}" PARENT_SCOPE)
  endforeach()
endfunction()

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
add_compiler_flags("-Wall")
add_compiler_flags("-Wextra")
add_compiler_flags("-g3")

#
# C/C++ Warning Flags
# See https://github.com/vadz/gcc-warnings-tools for gcc flags and versions
#
add_compiler_flags("-Wcast-align")                                         # gcc 4.6.0
#add_compiler_flags("-Wconversion")                                         # gcc 4.6.0
#add_compiler_flags("-Wdouble-promotion")                                   # gcc 4.6.0
add_compiler_flags("-Wformat=2")                                           # gcc 4.6.0
add_compiler_flags("-Winvalid-pch")                                        # gcc 4.6.0
add_compiler_flags("-Wmissing-declarations")                               # gcc 4.6.0
add_compiler_flags("-Wmissing-format-attribute")                           # gcc 4.6.0
add_compiler_flags("-Wmissing-include-dirs")                               # gcc 4.6.0
add_compiler_flags("-Wpointer-arith")                                      # gcc 4.6.0
add_compiler_flags("-Wredundant-decls")                                    # gcc 4.6.0
#add_compiler_flags("-Wshadow")                                             # gcc 4.6.0
#add_compiler_flags("-Wsign-conversion")                                    # gcc 4.6.0
#add_compiler_flags("-Wsuggest-attribute=const")                            # gcc 4.6.0
#add_compiler_flags("-Wsuggest-attribute=noreturn")                         # gcc 4.6.0
#add_compiler_flags("-Wsuggest-attribute=pure")                             # gcc 4.6.0
add_compiler_flags("-Wswitch-default")                                     # gcc 4.6.0
add_compiler_flags("-Wswitch-enum")                                        # gcc 4.6.0
add_compiler_flags("-Wtype-limits")                                         # gcc 4.6.0
add_compiler_flags("-Wundef")                                              # gcc 4.6.0
add_compiler_flags("-Wuninitialized")                                       # gcc 4.6.0
add_compiler_flags("-Wunknown-pragmas")                                     # gcc 4.6.0
add_compiler_flags("-Wunsafe-loop-optimizations")                           # gcc 4.6.0
add_compiler_flags("-Wunused-but-set-parameter")                            # gcc 4.6.0
add_compiler_flags("-Wunused-but-set-variable")                             # gcc 4.6.0
add_compiler_flags("-Wunused-function")                                     # gcc 4.6.0
add_compiler_flags("-Wunused")                                              # gcc 4.6.0
add_compiler_flags("-Wunused-label")                                        # gcc 4.6.0
add_compiler_flags("-Wunused-macros")                                       # gcc 4.6.0
#add_compiler_flags("-Wunused-parameter")                                    # gcc 4.6.0
add_compiler_flags("-Wunused-result")                                       # gcc 4.6.0
add_compiler_flags("-Wunused-value")                                        # gcc 4.6.0
add_compiler_flags("-Wunused-variable")                                     # gcc 4.6.0

add_compiler_flags("-Wunused-local-typedefs")                               # gcc 4.7.0

#add_compiler_flags("-Wsuggest-attribute=format")                           # gcc 4.8.0

add_compiler_flags("-Wformat-signedness")                                  # gcc 5.1.0
#add_compiler_flags("-Wsuggest-final-methods")                              # gcc 5.1.0
#add_compiler_flags("-Wsuggest-final-types")                                # gcc 5.1.0

add_compiler_flags("-Wduplicated-cond")                                    # gcc 6.1.0
#add_compiler_flags("-Wlogical-op")                                         # gcc 6.1.0
add_compiler_flags("-Wnull-dereference")                                   # gcc 6.1.0

add_compiler_flags("-Wduplicated-branches")                                # gcc 7.1.0
add_compiler_flags("-Wformat-overflow=2")                                  # gcc 7.1.0
add_compiler_flags("-Wformat-truncation=2")                                # gcc 7.1.0

#add_compiler_flags("-Wcast-align=strict")                                  # gcc 8.1.0
#add_compiler_flags("-Wsuggest-attribute=cold")                             # gcc 8.1.0
add_compiler_flags("-Wsuggest-attribute=malloc")                           # gcc 8.1.0

#add_compiler_flags("-Warith-conversion")                                   # gcc 10.1.0

add_compiler_flags("-Wctad-maybe-unsupported")                             # gcc 11.1.0
add_compiler_flags("-Wdeprecated-enum-enum-conversion")                    # gcc 11.1.0
add_compiler_flags("-Wdeprecated-enum-float-conversion")                   # gcc 11.1.0
add_compiler_flags("-Winvalid-imported-macros")                            # gcc 11.1.0
add_compiler_flags("-Wrange-loop-construct")                               # gcc 11.1.0

add_compiler_flags("-Warray-compare")                                      # gcc 12.1.0
add_compiler_flags("-Wbidi-chars=unpaired,ucn")                            # gcc 12.1.0
add_compiler_flags("-Winfinite-recursion")                                 # gcc 12.1.0
add_compiler_flags("-Wopenacc-parallelism")                                # gcc 12.1.0
add_compiler_flags("-ftrivial-auto-var-init=zero")                         # gcc 12.1.0
add_compiler_flags("-Wtrivial-auto-var-init")                              # gcc 12.1.0

#
# C++ Warning Flags
# See https://github.com/vadz/gcc-warnings-tools for gcc flags and versions
#
#add_cxx_compiler_flags(CMAKE_CXX_FLAGS "-Wctor-dtor-privacy")              # gcc 4.6.0
add_cxx_compiler_flags(CMAKE_CXX_FLAGS "-Wnoexcept")                       # gcc 4.6.0
#add_cxx_compiler_flags(CMAKE_CXX_FLAGS "-Wold-style-cast")                 # gcc 4.6.0
add_cxx_compiler_flags(CMAKE_CXX_FLAGS "-Woverloaded-virtual")             # gcc 4.6.0
add_cxx_compiler_flags(CMAKE_CXX_FLAGS "-Wsign-promo")                     # gcc 4.6.0
add_cxx_compiler_flags(CMAKE_CXX_FLAGS "-Wstrict-null-sentinel")           # gcc 4.6.0

add_cxx_compiler_flags(CMAKE_CXX_FLAGS "-Wzero-as-null-pointer-constant")  # gcc 4.7.0

#add_cxx_compiler_flags(CMAKE_CXX_FLAGS "-Wuseless-cast")                   # gcc 4.8.0

add_cxx_compiler_flags(CMAKE_CXX_FLAGS "-Wconditionally-supported")        # gcc 4.9.0

add_cxx_compiler_flags(CMAKE_CXX_FLAGS "-Wsuggest-override")               # gcc 5.1.0

add_cxx_compiler_flags(CMAKE_CXX_FLAGS "-Wnoexcept-type")                  # gcc 7.1.0

add_cxx_compiler_flags(CMAKE_CXX_FLAGS "-Wextra-semi")                     # gcc 8.1.0

add_cxx_compiler_flags(CMAKE_CXX_FLAGS "-Wcomma-subscript")                # gcc 10.1.0
add_cxx_compiler_flags(CMAKE_CXX_FLAGS "-Wmismatched-tags")                # gcc 10.1.0
add_cxx_compiler_flags(CMAKE_CXX_FLAGS "-Wredundant-tags")                 # gcc 10.1.0
add_cxx_compiler_flags(CMAKE_CXX_FLAGS "-Wvolatile")                       # gcc 10.1.0

#
# Allow the linker to remove unused data and functions
#
add_compiler_flags("-fdata-sections")
add_compiler_flags("-ffunction-sections")
add_compiler_flags("-fno-common")
if (CMAKE_CXX_COMPILER_ID MATCHES GNU)
  add_compiler_flags("-Wl,--gc-sections")
endif (CMAKE_CXX_COMPILER_ID MATCHES GNU)

#
# Hardening flags (see https://developers.redhat.com/blog/2018/03/21/compiler-and-linker-flags-gcc)
#
IF (NOT CMAKE_BUILD_TYPE_LOWER STREQUAL "debug")
  add_compiler_flags("-D_FORTIFY_SOURCE=2")
ENDIF (NOT CMAKE_BUILD_TYPE_LOWER STREQUAL "debug")
add_compiler_flags("-D_GLIBCXX_ASSERTIONS")
add_compiler_flags("-fasynchronous-unwind-tables")
add_compiler_flags("-fexceptions")
add_compiler_flags("-fstack-clash-protection")
add_compiler_flags("-fstack-protector-strong")
add_compiler_flags("-grecord-gcc-switches")
add_compiler_flags("-fcf-protection")
add_compiler_flags("-pipe")
add_compiler_flags("-Wformat=2 -Werror=format-security -Wno-format-nonliteral")
add_compiler_flags("-fPIE")
if (CMAKE_CXX_COMPILER_ID MATCHES GNU)
  add_compiler_flags("-Wl,-z,defs")
  add_compiler_flags("-Wl,-z,now")
  add_compiler_flags("-Wl,-z,relro")
endif (CMAKE_CXX_COMPILER_ID MATCHES GNU)

#
# Profiling and Debugging (see https://fedoraproject.org/wiki/Changes/fno-omit-frame-pointer)
#
#add_compiler_flags("-fno-omit-frame-pointer")

#
# Disable some warnings
#
add_compiler_flags("-Wno-dangling-reference")
add_compiler_flags("-Wno-missing-field-initializers")
add_compiler_flags("-Wno-pessimizing-move")
add_compiler_flags("-Wno-sign-compare")
add_compiler_flags("-Wno-suggest-attribute=format")
add_compiler_flags("-Wno-unused-parameter")

#
# Sanitize flags
#
#add_cxx_compiler_flags(CMAKE_CXX_FLAGS "-fsanitize=address")
#add_cxx_compiler_flags(CMAKE_CXX_FLAGS "-fsanitize=thread")
