set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_SYSTEM_PROCESSOR arm)

set(TOOLCHAIN_PREFIX arm-none-eabi-)

set(ARM_GNU_TOOLCHAIN_BIN "" CACHE PATH "Path to the Arm GNU Toolchain bin directory")

if(NOT ARM_GNU_TOOLCHAIN_BIN)
    foreach(_candidate
        "C:/Program Files (x86)/Arm GNU Toolchain arm-none-eabi/14.2 rel1/bin"
        "C:/Program Files (x86)/Arm GNU Toolchain arm-none-eabi/12.2 mpacbti-rel1/bin")
        if(EXISTS "${_candidate}/${TOOLCHAIN_PREFIX}gcc.exe")
            set(ARM_GNU_TOOLCHAIN_BIN "${_candidate}" CACHE PATH "Path to the Arm GNU Toolchain bin directory" FORCE)
            break()
        endif()
    endforeach()
endif()

set(_arm_tool_hints)
if(ARM_GNU_TOOLCHAIN_BIN)
    list(APPEND _arm_tool_hints "${ARM_GNU_TOOLCHAIN_BIN}")
endif()

find_program(CMAKE_C_COMPILER NAMES ${TOOLCHAIN_PREFIX}gcc HINTS ${_arm_tool_hints} REQUIRED)
find_program(CMAKE_ASM_COMPILER NAMES ${TOOLCHAIN_PREFIX}gcc HINTS ${_arm_tool_hints} REQUIRED)
find_program(CMAKE_OBJCOPY NAMES ${TOOLCHAIN_PREFIX}objcopy HINTS ${_arm_tool_hints} REQUIRED)
find_program(CMAKE_OBJDUMP NAMES ${TOOLCHAIN_PREFIX}objdump HINTS ${_arm_tool_hints} REQUIRED)
find_program(CMAKE_SIZE NAMES ${TOOLCHAIN_PREFIX}size HINTS ${_arm_tool_hints} REQUIRED)

set(CMAKE_TRY_COMPILE_TARGET_TYPE STATIC_LIBRARY)
