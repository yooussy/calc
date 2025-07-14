function(add_shad_executable NAME)
  add_executable(${NAME} ${ARGN})
endfunction()

function(add_shad_library NAME)
  add_library(${NAME} ${ARGN})
endfunction()

function(add_shad_shared_library NAME)
  add_library(${NAME} SHARED ${ARGN})
endfunction()

function(add_catch TARGET)
  add_shad_executable(${TARGET} ${ARGN})
  target_link_libraries(${TARGET} PRIVATE Catch2::Catch2WithMain)
endfunction()
