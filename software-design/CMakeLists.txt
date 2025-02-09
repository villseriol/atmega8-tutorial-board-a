cmake_minimum_required(VERSION 3.10)
project(atmega8a-evaluation-board)

set(DEVICE "atmega8")

set(F_CPU $ENV{F_CPU})
set(E_FUSE $ENV{E_FUSE})
set(L_FUSE $ENV{L_FUSE})
set(H_FUSE $ENV{H_FUSE})
set(PROGRAMMER $ENV{PROGRAMMER})

function(target_add_elf target)
   cmake_parse_arguments(PARSED_ARGS "UNOPTIMIZED" "" "" ${ARGN})

   message(STATUS "Adding .elf compilation target ${target}...")

   if(PARSED_ARGS_UNOPTIMIZED)
      set_target_properties(
         ${target}
         PROPERTIES
         COMPILE_OPTIONS -O0
         OUTPUT_NAME ${target}.elf
         COMPILE_FLAGS "-DF_CPU=${F_CPU} -mmcu=${DEVICE}"
      )
   else()
      set_target_properties(
         ${target}
         PROPERTIES
         COMPILE_OPTIONS -Os
         OUTPUT_NAME ${target}.elf
         COMPILE_FLAGS "-DF_CPU=${F_CPU} -mmcu=${DEVICE}"
      )
   endif()

   add_custom_command(TARGET ${target} POST_BUILD
      COMMAND ${AVR_OBJCOPY} -j .text -j .data -O ihex ${target}.elf ${target}.hex
      COMMAND ${AVR_SIZE_TOOL} -d ${target}.elf
      WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
      COMMENT "Generating hex file..."
   )

   add_custom_target(
      flash-${target}
      COMMAND ${AVRDUDE} ${AVRDUDE_ARGS} -u -U flash:w:${target}.hex:i
      WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
   )

   add_custom_target(
      disassemble-${target}
      COMMAND ${AVR_OBJDUMP} --architecture=avr -d -S ${target}.elf
      WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
   )
endfunction()

find_program(AVRDUDE avrdude)
set(AVRDUDE_ARGS -e -v -p ${DEVICE} -c ${PROGRAMMER})

set(CMAKE_ASM_FLAGS "-x assembler-with-cpp ${CFLAGS}")
set(CMAKE_EXE_LINKER_FLAGS -mmcu=${DEVICE})
set(CMAKE_C_STANDARD 99)

add_compile_options(-Wall -Os -pedantic)

add_library(common src/common.cpp)
target_include_directories(common PUBLIC include)

add_executable(main src/main.cpp)
target_link_libraries(main common)
target_add_elf(main)

add_executable(blink-sync src/blink_sync.cpp)
target_link_libraries(blink-sync common)
target_add_elf(blink-sync)

add_executable(blink-async src/blink_async.cpp)
target_link_libraries(blink-async common)
target_add_elf(blink-async)

add_executable(7-segment src/7_segment.cpp)
target_link_libraries(7-segment common)
target_add_elf(7-segment)

add_executable(timer src/timer.cpp)
target_link_libraries(timer common)
target_add_elf(timer)

add_executable(adc src/adc.cpp)
target_link_libraries(adc common)
target_add_elf(adc)

# #####################################################################
# #####################################################################
add_custom_target(
   fuse
   COMMAND ${AVRDUDE_ARGS} -U lfuse:w:${L_FUSE}:m -U hfuse:w:${H_FUSE}:m
   WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
)