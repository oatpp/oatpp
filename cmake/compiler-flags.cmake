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
if (CMAKE_CXX_COMPILER_ID MATCHES GNU)
  add_cxx_compiler_flags(CMAKE_CXX_FLAGS "-Wall")
  add_cxx_compiler_flags(CMAKE_CXX_FLAGS "-Wextra")
endif (CMAKE_CXX_COMPILER_ID MATCHES GNU)


#
# Preserve debug information flags
#
if (CMAKE_CXX_COMPILER_ID MATCHES GNU)
  add_cxx_compiler_flags(CMAKE_CXX_FLAGS "-g3")
endif (CMAKE_CXX_COMPILER_ID MATCHES GNU)


#
# C/C++ Warning Flags
# See https://github.com/vadz/gcc-warnings-tools for gcc flags and versions
#
if (CMAKE_CXX_COMPILER_ID MATCHES GNU)
  add_cxx_compiler_flags(CMAKE_CXX_FLAGS "-Wcast-align")                                         # gcc 4.6.0
  #add_cxx_compiler_flags(CMAKE_CXX_FLAGS "-Wconversion")                                         # gcc 4.6.0
  #add_cxx_compiler_flags(CMAKE_CXX_FLAGS "-Wdouble-promotion")                                   # gcc 4.6.0
  add_cxx_compiler_flags(CMAKE_CXX_FLAGS "-Wformat=2")                                           # gcc 4.6.0
  add_cxx_compiler_flags(CMAKE_CXX_FLAGS "-Winvalid-pch")                                        # gcc 4.6.0
  add_cxx_compiler_flags(CMAKE_CXX_FLAGS "-Wmissing-declarations")                               # gcc 4.6.0
  add_cxx_compiler_flags(CMAKE_CXX_FLAGS "-Wmissing-format-attribute")                           # gcc 4.6.0
  add_cxx_compiler_flags(CMAKE_CXX_FLAGS "-Wmissing-include-dirs")                               # gcc 4.6.0
  add_cxx_compiler_flags(CMAKE_CXX_FLAGS "-Wpointer-arith")                                      # gcc 4.6.0
  add_cxx_compiler_flags(CMAKE_CXX_FLAGS "-Wredundant-decls")                                    # gcc 4.6.0
  #add_cxx_compiler_flags(CMAKE_CXX_FLAGS "-Wshadow")                                             # gcc 4.6.0
  #add_cxx_compiler_flags(CMAKE_CXX_FLAGS "-Wsign-conversion")                                    # gcc 4.6.0
  #add_cxx_compiler_flags(CMAKE_CXX_FLAGS "-Wsuggest-attribute=const")                            # gcc 4.6.0
  #add_cxx_compiler_flags(CMAKE_CXX_FLAGS "-Wsuggest-attribute=noreturn")                         # gcc 4.6.0
  #add_cxx_compiler_flags(CMAKE_CXX_FLAGS "-Wsuggest-attribute=pure")                             # gcc 4.6.0
  add_cxx_compiler_flags(CMAKE_CXX_FLAGS "-Wswitch-default")                                     # gcc 4.6.0
  add_cxx_compiler_flags(CMAKE_CXX_FLAGS "-Wswitch-enum")                                        # gcc 4.6.0
  add_cxx_compiler_flags(CMAKE_CXX_FLAGS "-Wtype-limits")                                         # gcc 4.6.0
  add_cxx_compiler_flags(CMAKE_CXX_FLAGS "-Wundef")                                              # gcc 4.6.0
  add_cxx_compiler_flags(CMAKE_CXX_FLAGS "-Wuninitialized")                                       # gcc 4.6.0
  add_cxx_compiler_flags(CMAKE_CXX_FLAGS "-Wunknown-pragmas")                                     # gcc 4.6.0
  add_cxx_compiler_flags(CMAKE_CXX_FLAGS "-Wunsafe-loop-optimizations")                           # gcc 4.6.0
  add_cxx_compiler_flags(CMAKE_CXX_FLAGS "-Wunused-but-set-parameter")                            # gcc 4.6.0
  add_cxx_compiler_flags(CMAKE_CXX_FLAGS "-Wunused-but-set-variable")                             # gcc 4.6.0
  add_cxx_compiler_flags(CMAKE_CXX_FLAGS "-Wunused-function")                                     # gcc 4.6.0
  add_cxx_compiler_flags(CMAKE_CXX_FLAGS "-Wunused")                                              # gcc 4.6.0
  add_cxx_compiler_flags(CMAKE_CXX_FLAGS "-Wunused-label")                                        # gcc 4.6.0
  add_cxx_compiler_flags(CMAKE_CXX_FLAGS "-Wunused-macros")                                       # gcc 4.6.0
  #add_cxx_compiler_flags(CMAKE_CXX_FLAGS "-Wunused-parameter")                                    # gcc 4.6.0
  add_cxx_compiler_flags(CMAKE_CXX_FLAGS "-Wunused-result")                                       # gcc 4.6.0
  add_cxx_compiler_flags(CMAKE_CXX_FLAGS "-Wunused-value")                                        # gcc 4.6.0
  add_cxx_compiler_flags(CMAKE_CXX_FLAGS "-Wunused-variable")                                     # gcc 4.6.0

  add_cxx_compiler_flags(CMAKE_CXX_FLAGS "-Wunused-local-typedefs")                               # gcc 4.7.0

  #add_cxx_compiler_flags(CMAKE_CXX_FLAGS "-Wsuggest-attribute=format")                           # gcc 4.8.0

  add_cxx_compiler_flags(CMAKE_CXX_FLAGS "-Wformat-signedness")                                  # gcc 5.1.0
  #add_cxx_compiler_flags(CMAKE_CXX_FLAGS "-Wsuggest-final-methods")                              # gcc 5.1.0
  #add_cxx_compiler_flags(CMAKE_CXX_FLAGS "-Wsuggest-final-types")                                # gcc 5.1.0

  add_cxx_compiler_flags(CMAKE_CXX_FLAGS "-Wduplicated-cond")                                    # gcc 6.1.0
  #add_cxx_compiler_flags(CMAKE_CXX_FLAGS "-Wlogical-op")                                         # gcc 6.1.0
  add_cxx_compiler_flags(CMAKE_CXX_FLAGS "-Wnull-dereference")                                   # gcc 6.1.0

  add_cxx_compiler_flags(CMAKE_CXX_FLAGS "-Wduplicated-branches")                                # gcc 7.1.0
  add_cxx_compiler_flags(CMAKE_CXX_FLAGS "-Wformat-overflow=2")                                  # gcc 7.1.0
  add_cxx_compiler_flags(CMAKE_CXX_FLAGS "-Wformat-truncation=2")                                # gcc 7.1.0

  #add_cxx_compiler_flags(CMAKE_CXX_FLAGS "-Wcast-align=strict")                                  # gcc 8.1.0
  #add_cxx_compiler_flags(CMAKE_CXX_FLAGS "-Wsuggest-attribute=cold")                             # gcc 8.1.0
  add_cxx_compiler_flags(CMAKE_CXX_FLAGS "-Wsuggest-attribute=malloc")                           # gcc 8.1.0

  #add_cxx_compiler_flags(CMAKE_CXX_FLAGS "-Warith-conversion")                                   # gcc 10.1.0

  add_cxx_compiler_flags(CMAKE_CXX_FLAGS "-Wctad-maybe-unsupported")                             # gcc 11.1.0
  add_cxx_compiler_flags(CMAKE_CXX_FLAGS "-Wdeprecated-enum-enum-conversion")                    # gcc 11.1.0
  add_cxx_compiler_flags(CMAKE_CXX_FLAGS "-Wdeprecated-enum-float-conversion")                   # gcc 11.1.0
  add_cxx_compiler_flags(CMAKE_CXX_FLAGS "-Winvalid-imported-macros")                            # gcc 11.1.0
  add_cxx_compiler_flags(CMAKE_CXX_FLAGS "-Wrange-loop-construct")                               # gcc 11.1.0

  add_cxx_compiler_flags(CMAKE_CXX_FLAGS "-Warray-compare")                                      # gcc 12.1.0
  add_cxx_compiler_flags(CMAKE_CXX_FLAGS "-Wbidi-chars=unpaired,ucn")                            # gcc 12.1.0
  add_cxx_compiler_flags(CMAKE_CXX_FLAGS "-Winfinite-recursion")                                 # gcc 12.1.0
  add_cxx_compiler_flags(CMAKE_CXX_FLAGS "-Wopenacc-parallelism")                                # gcc 12.1.0
  add_cxx_compiler_flags(CMAKE_CXX_FLAGS "-ftrivial-auto-var-init=zero")                         # gcc 12.1.0
  add_cxx_compiler_flags(CMAKE_CXX_FLAGS "-Wtrivial-auto-var-init")                              # gcc 12.1.0
endif (CMAKE_CXX_COMPILER_ID MATCHES GNU)


#
# C++ Warning Flags
# See https://github.com/vadz/gcc-warnings-tools for gcc flags and versions
#
if (CMAKE_CXX_COMPILER_ID MATCHES GNU)
  add_cxx_compiler_flags(CMAKE_CXX_FLAGS "-Wctor-dtor-privacy")              # gcc 4.6.0
  add_cxx_compiler_flags(CMAKE_CXX_FLAGS "-Wnoexcept")                       # gcc 4.6.0
  add_cxx_compiler_flags(CMAKE_CXX_FLAGS "-Wold-style-cast")                 # gcc 4.6.0
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
endif (CMAKE_CXX_COMPILER_ID MATCHES GNU)


#
# Allow the linker to remove unused data and functions
#
if (CMAKE_CXX_COMPILER_ID MATCHES GNU)
  add_cxx_compiler_flags(CMAKE_CXX_FLAGS "-fdata-sections")
  add_cxx_compiler_flags(CMAKE_CXX_FLAGS "-ffunction-sections")
  add_cxx_compiler_flags(CMAKE_CXX_FLAGS "-fno-common")
  add_cxx_compiler_flags(CMAKE_CXX_FLAGS "-Wl,--gc-sections")
endif (CMAKE_CXX_COMPILER_ID MATCHES GNU)


#
# Hardening flags
# See https://developers.redhat.com/blog/2018/03/21/compiler-and-linker-flags-gcc
#
if (CMAKE_CXX_COMPILER_ID MATCHES GNU)
  IF (NOT CMAKE_BUILD_TYPE_LOWER STREQUAL "debug")
    add_cxx_compiler_flags(CMAKE_CXX_FLAGS "-D_FORTIFY_SOURCE=2")
  ENDIF (NOT CMAKE_BUILD_TYPE_LOWER STREQUAL "debug")
  add_cxx_compiler_flags(CMAKE_CXX_FLAGS "-D_GLIBCXX_ASSERTIONS")
  add_cxx_compiler_flags(CMAKE_CXX_FLAGS "-fasynchronous-unwind-tables")
  add_cxx_compiler_flags(CMAKE_CXX_FLAGS "-fexceptions")
  add_cxx_compiler_flags(CMAKE_CXX_FLAGS "-fstack-clash-protection")
  add_cxx_compiler_flags(CMAKE_CXX_FLAGS "-fstack-protector-strong")
  add_cxx_compiler_flags(CMAKE_CXX_FLAGS "-grecord-gcc-switches")
  add_cxx_compiler_flags(CMAKE_CXX_FLAGS "-fcf-protection")
  add_cxx_compiler_flags(CMAKE_CXX_FLAGS "-pipe")
  add_cxx_compiler_flags(CMAKE_CXX_FLAGS "-Wformat=2 -Werror=format-security -Wno-format-nonliteral")
  add_cxx_compiler_flags(CMAKE_CXX_FLAGS "-fPIE")
  add_cxx_compiler_flags(CMAKE_CXX_FLAGS "-Wl,-z,defs")
  add_cxx_compiler_flags(CMAKE_CXX_FLAGS "-Wl,-z,now")
  add_cxx_compiler_flags(CMAKE_CXX_FLAGS "-Wl,-z,relro")
endif (CMAKE_CXX_COMPILER_ID MATCHES GNU)


#
# Profiling and Debugging
# See https://fedoraproject.org/wiki/Changes/fno-omit-frame-pointer
#
#if (CMAKE_CXX_COMPILER_ID MATCHES GNU)
#  add_cxx_compiler_flags(CMAKE_CXX_FLAGS "-fno-omit-frame-pointer")
#endif (CMAKE_CXX_COMPILER_ID MATCHES GNU)


#
# Disable some warnings
#
if (CMAKE_CXX_COMPILER_ID MATCHES GNU)
  add_cxx_compiler_flags(CMAKE_CXX_FLAGS "-Wno-dangling-reference")
  add_cxx_compiler_flags(CMAKE_CXX_FLAGS "-Wno-pessimizing-move")
  add_cxx_compiler_flags(CMAKE_CXX_FLAGS "-Wno-sign-compare")
  add_cxx_compiler_flags(CMAKE_CXX_FLAGS "-Wno-suggest-attribute=format")
  add_cxx_compiler_flags(CMAKE_CXX_FLAGS "-Wno-unused-parameter")
endif (CMAKE_CXX_COMPILER_ID MATCHES GNU)


#
# Sanitize flags
#
#if (CMAKE_CXX_COMPILER_ID MATCHES GNU)
#  add_cxx_compiler_flags(CMAKE_CXX_FLAGS "-fsanitize=address")
#  add_cxx_compiler_flags(CMAKE_CXX_FLAGS "-fsanitize=thread")
#endif (CMAKE_CXX_COMPILER_ID MATCHES GNU)
