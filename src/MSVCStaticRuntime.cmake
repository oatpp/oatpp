macro(configure_msvc_static_runtime)
	if(MSVC)
		# Set compiler options.
		set(variables
			CMAKE_C_FLAGS
			CMAKE_C_FLAGS_DEBUG
			CMAKE_C_FLAGS_MINSIZEREL
			CMAKE_C_FLAGS_RELEASE
			CMAKE_C_FLAGS_RELWITHDEBINFO
			CMAKE_CXX_FLAGS
			CMAKE_CXX_FLAGS_DEBUG
			CMAKE_CXX_FLAGS_MINSIZEREL
			CMAKE_CXX_FLAGS_RELEASE
			CMAKE_CXX_FLAGS_RELWITHDEBINFO)

      message(STATUS "MSVC: using statically-linked runtime (/MT and /MTd).")
      foreach(variable ${variables})
         if(${variable} MATCHES "/MD")
            string(REGEX REPLACE "/MD" "/MT" ${variable} "${${variable}}")
         endif()
      endforeach()

		foreach(variable ${variables})
			set(${variable} "${${variable}}" CACHE STRING "MSVC_${variable}" FORCE)
		endforeach()
	endif()
endmacro(configure_msvc_static_runtime)