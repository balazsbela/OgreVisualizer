cmake_minimum_required(version 2.6)
 
project(ogrevisualizer)
 
if(win32)
	set(cmake_module_path "$env{ogre_home}/cmake/;${cmake_module_path}")
	set(ogre_samples_includepath
		$env{ogre_home}/samples/include
	)
endif(win32)
 
if(unix)
	if(exists "/usr/local/lib/ogre/cmake")

	  set(cmake_module_path "/usr/local/lib/ogre/cmake/;${cmake_module_path}")
	  set(ogre_samples_includepath "/usr/local/share/ogre/samples/common/include/") # we could just *assume* that developers uses this basepath : /usr/local

	elseif(exists "/usr/lib/ogre/cmake")

	  set(cmake_module_path "/usr/lib/ogre/cmake/;${cmake_module_path}")
	  set(ogre_samples_includepath "/usr/share/ogre/samples/common/include/") # otherwise, this one

	else ()
	  message(send_error "failed to find module path.")
	endif(exists "/usr/local/lib/ogre")
endif(unix)
 
if (cmake_build_type strequal "")
  # cmake defaults to leaving cmake_build_type empty. this screws up
  # differentiation between debug and release builds.
  set(cmake_build_type "relwithdebinfo" cache string "choose the type of build, options are: none (cmake_cxx_flags or cmake_c_flags used) debug release relwithdebinfo minsizerel." force)
endif ()
 
set(cmake_debug_postfix "_d")
 
set(cmake_install_prefix "${cmake_current_binary_dir}/dist")
 
find_package(ogre required)
 
#if(not "${ogre_version_name}" strequal "cthugha")
#  message(send_error "you need ogre 1.7 cthugha to build this.")
#endif()
 
find_package(ois required)
 
if(not ois_found)
	message(send_error "failed to find ois.")
endif()
 
# find boost
if (not ogre_build_platform_iphone)
	if (win32 or apple)
		set(boost_use_static_libs true)
	else ()
		# statically linking boost to a dynamic ogre build doesn't work on linux 64bit
		set(boost_use_static_libs ${ogre_static})
	endif ()
	if (mingw)
		# this is probably a bug in cmake: the boost find module tries to look for
		# boost libraries with name libboost_*, but cmake already prefixes library
		# search names with "lib". this is the workaround.
		set(cmake_find_library_prefixes ${cmake_find_library_prefixes} "")
	endif ()
	set(boost_additional_versions "1.51" "1.51.0" "1.50" "1.50.0" "1.49" "1.49.0" "1.48" "1.48.0" "1.47" "1.47.0" "1.46" "1.46.0" "1.45" "1.45.0" "1.44" "1.44.0" "1.42" "1.42.0" "1.41.0" "1.41" "1.40.0" "1.40")
	# components that need linking (nb does not include header-only components like bind)
	set(ogre_boost_components system thread date_time)
	find_package(boost components ${ogre_boost_components} quiet)
	if (not boost_found)
		# try again with the other type of libs
		set(boost_use_static_libs not ${boost_use_static_libs})
		find_package(boost components ${ogre_boost_components} quiet)
	endif()
	find_package(boost quiet)
 
	#find_sdl
	find_package(sdl required)
	if(sdl_found)	  
	  message( "sdl found" )
	else(sdl_found)
	  message( fatal_error "error: sdl library not found, it is required to build")
	  message( "make sure the path is correctly defined or set the envirnomental variable sdldir to the correct location" )
	endif(sdl_found)
	
	# find sdl_mixer
	find_package(sdl_mixer required)
	if(sdlmixer_found)
	  message( "sdl mixer found" )
	else(sdlmixer_found)
	    message( "error: sdl_mixer library not found, it was selected to be included")
	endif(sdlmixer_found)
 
        #fftw3        
	find_path(fftw3_include_dir
		names
			fftw3.h
		paths
			/usr/include
			/usr/local/include
			/opt/local/include
			/sw/include
	)

	find_library(fftw3_library
		names
			fftw3
		paths
			/usr/lib
			/usr/local/lib
			/opt/local/lib
			/sw/lib
	)
	
	set(fftw3_include_dirs
		${fftw3_include_dir}
	)
	set(fftw3_libraries
		${fftw3_library}
	)

	if (fftw3_include_dirs and fftw3_libraries)
		set(fftw3_found true)
	endif (fftw3_include_dirs and fftw3_libraries)

	if (fftw3_found)
		message("fftw found")
	else (fftw3_found)		
		message(fatal_error "could not find fftw3")
	endif (fftw3_found)
 
	# set up referencing of boost
	include_directories(${boost_include_dir})
	add_definitions(-dboost_all_no_lib)
	set(ogre_libraries ${ogre_libraries} ${boost_libraries})
endif()
 
set(hdrs
	./baseapplication.h
	./cubegenerator.h
	./visualizerapplication.h
)
 
set(srcs
	./baseapplication.cpp
	./cubegenerator.cpp
	./visualizerapplication.cpp
	./main.cpp
)


include_directories(${fftw3_include_dirs} ${sdl_include_dir} ${sdlmixer_include_dir} ${ois_include_dirs}
	${ogre_include_dirs}
	${ogre_samples_includepath}
)
 
add_executable(ogreapp win32 ${hdrs} ${srcs})
 
set_target_properties(ogreapp properties debug_postfix _d)
	 
target_link_libraries(ogreapp ${boost_system_library} ${fftw3_library} ${sdl_library} ${sdlmixer_library} ${ogre_libraries} ${ois_libraries})
 
file(make_directory ${cmake_current_binary_dir}/dist/bin)
file(make_directory ${cmake_current_binary_dir}/dist/media)
 
# post-build copy for win32
if(win32 and not mingw)
	add_custom_command( target ogreapp pre_build
		command if not exist .\\dist\\bin mkdir .\\dist\\bin )
	add_custom_command( target ogreapp post_build
		command copy \"$(targetpath)\" .\\dist\\bin )
endif(win32 and not mingw)

if(mingw or unix)
	set(executable_output_path ${project_binary_dir}/dist/bin)
endif(mingw or unix)
 
if(win32)
 
	install(targets ogreapp
		runtime destination bin
		configurations all)
 
	install(directory ${cmake_source_dir}/dist/media
		destination ./
		configurations release relwithdebinfo debug
	)
 
	install(files ${cmake_source_dir}/dist/bin/plugins.cfg
		${cmake_source_dir}/dist/bin/resources.cfg
		destination bin
		configurations release relwithdebinfo
	)
 
	install(files ${cmake_source_dir}/dist/bin/plugins_d.cfg
		${cmake_source_dir}/dist/bin/resources_d.cfg
		destination bin
		configurations debug
	)
 
        # note: for the 1.7.1 sdk the ois dll is called ois.dll instead of libois.dll
        # so you'll have to change that to make it work with 1.7.1
	install(files ${ogre_plugin_dir_rel}/ogremain.dll
		${ogre_plugin_dir_rel}/rendersystem_direct3d9.dll
		${ogre_plugin_dir_rel}/rendersystem_gl.dll
		${ogre_plugin_dir_rel}/libois.dll
		destination bin
		configurations release relwithdebinfo
	)
 
	install(files ${ogre_plugin_dir_dbg}/ogremain_d.dll
		${ogre_plugin_dir_dbg}/rendersystem_direct3d9_d.dll
		${ogre_plugin_dir_dbg}/rendersystem_gl_d.dll
		${ogre_plugin_dir_dbg}/libois_d.dll
		destination bin
		configurations debug
	)
 
   # as of sdk 1.7.2 we need to copy the boost dll's as well
   # because they're not linked statically (it worked with 1.7.1 though)
   install(files ${boost_date_time_library_release}
      ${boost_thread_library_release}
      destination bin
      configurations release relwithdebinfo
   )
 
   install(files ${boost_date_time_library_debug}
      ${boost_thread_library_debug}
      destination bin
      configurations debug
   )
endif(win32)

if(unix)
 
	install(targets ogreapp
		runtime destination bin
		configurations all)
 
	install(directory ${cmake_source_dir}/dist/media
		destination ./
		configurations release relwithdebinfo debug
	)
 
	install(files ${cmake_source_dir}/dist/bin/plugins.cfg
		${cmake_source_dir}/dist/bin/resources.cfg
		destination bin
		configurations release relwithdebinfo debug
	)
 
endif(unix)
 
