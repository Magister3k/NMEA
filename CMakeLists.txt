cmake_minimum_required(VERSION 3.15)
project(NmeaDecoder C CXX)

# CMake-generated project files are kept in mingw/ or msvc/.
# Runtime artifacts are kept separately in build/<toolchain>/.
if(MSVC)
    set(NMEA_OUTPUT_ROOT "${CMAKE_SOURCE_DIR}/build/msvc")
elseif(MINGW)
    set(NMEA_OUTPUT_ROOT "${CMAKE_SOURCE_DIR}/build/mingw")
else()
    set(NMEA_OUTPUT_ROOT "${CMAKE_BINARY_DIR}")
endif()

set(NMEA_RELEASE_DIR "${NMEA_OUTPUT_ROOT}/release")
set(NMEA_ARCHIVE_DIR "${NMEA_OUTPUT_ROOT}/archive")
set(NMEA_LIB_DIR "${NMEA_OUTPUT_ROOT}/lib")

foreach(CONFIG Debug Release RelWithDebInfo MinSizeRel)
    string(TOUPPER "${CONFIG}" CONFIG_UPPER)
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_${CONFIG_UPPER} "${NMEA_RELEASE_DIR}")
    set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_${CONFIG_UPPER} "${NMEA_LIB_DIR}")
    set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_${CONFIG_UPPER} "${NMEA_ARCHIVE_DIR}")
endforeach()

function(nmea_set_target_output_directories TARGET_NAME)
    set_target_properties(${TARGET_NAME} PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY "${NMEA_RELEASE_DIR}"
        LIBRARY_OUTPUT_DIRECTORY "${NMEA_LIB_DIR}"
        ARCHIVE_OUTPUT_DIRECTORY "${NMEA_ARCHIVE_DIR}"
    )
endfunction()

# ====================================================================
# 1. ���������� ��������� ����������
# ====================================================================
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)

# ����������� �������� ������ �������
set(PROJECT_SOURCES
    NMEA/main.cpp
    NMEA/nmea_processor.cpp
    NMEA/nmea_service.cpp
    NMEA/nmea450_decoder.cpp
    NMEA/nmea_decoder.cpp
    NMEA/ais_decoder.cpp
    NMEA/geo_parser.cpp
)

# �������� ������������ �����
add_executable(nmea_decoder ${PROJECT_SOURCES})
nmea_set_target_output_directories(nmea_decoder)
target_include_directories(nmea_decoder PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/NMEA
)

# ====================================================================
# 2. ��������� ����������� ���������� � FAST-MATH
# ====================================================================
if(MSVC)
    target_compile_options(nmea_decoder PRIVATE 
        /O2          # ������������ ����������� �� ��������
        /Oi          # ��������� ���������� (intrinsic) �������
        /Ot          # ������������ ����������� �������� ����� ��������
        /fp:fast     # ���������� -ffast-math ��� ����������� MSVC
        /arch:AVX2   # ��������� ���������� AVX2 (������� ���������� FMA)
        /nologo      # ���������� ������ ��������� �����������
        /W3          # ������ ������� ��������������
    )
    message(STATUS "[MSVC OPTIMIZATION] ������� ���������������� �������������� ����� /fp:fast � ���������� AVX2.")
else()
    # ��������� ��� ������������ GCC � Clang (Linux / MinGW)
    target_compile_options(nmea_decoder PRIVATE 
        -O3                 # ������������ ������� ����������� �����������
        -ffast-math         # ��������� ���������������� ���������� (IEEE 754 bypass)
        -march=native       # ����������� ��� ������� ����������� ���������� �����
        -ftree-vectorize    # ����������� ����-������������ ������ �����������
        -Wall               # ��������� ���� ������� ��������������
        -Wextra             # �������������� �������������� ��� ������� ����
    )
    # �������� ��������� ���������� POSIX ������� ��� ������ �������� GC
    target_link_libraries(nmea_decoder PRIVATE pthread)
    message(STATUS "[GCC/CLANG OPTIMIZATION] �������� ����� -O3, -ffast-math � ����-������������ ��� ����������� -march=native.")
endif()

# ���������� ���������� ������������ ����� ��� ����-����� ����������
add_executable(test_math NMEA/test_vectorized_math.cpp)
nmea_set_target_output_directories(test_math)

target_include_directories(test_math PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/NMEA
)

if(MSVC)
    target_compile_options(test_math PRIVATE /O2 /fp:fast /arch:AVX2)
else()
    target_compile_options(test_math PRIVATE -O3 -ffast-math -march=native -ftree-vectorize)
endif()

# ����������� ��������������� ������� (�����������, ��� CTest)
enable_testing()
add_test(NAME GeodeticMathVerification COMMAND test_math)

if(WIN32)
    add_library(nmea_decoder_ext SHARED
        main.cpp
        nmea_decoder.cpp
        NMEA/nmea_decoder.cpp
        NMEA/nmea450_decoder.cpp
    )

    target_include_directories(nmea_decoder_ext PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${CMAKE_CURRENT_SOURCE_DIR}/NMEA
        ${CMAKE_CURRENT_SOURCE_DIR}/DHF/_example/include
        ${CMAKE_CURRENT_SOURCE_DIR}/DHF/_example
    )
    target_compile_features(nmea_decoder_ext PRIVATE cxx_std_17)
    target_compile_definitions(nmea_decoder_ext PRIVATE NMEA_DECODER_EXT_BUILD)
    set_target_properties(nmea_decoder_ext PROPERTIES
        OUTPUT_NAME "NmeaDecoderExt"
        RUNTIME_OUTPUT_DIRECTORY "${NMEA_RELEASE_DIR}"
        LIBRARY_OUTPUT_DIRECTORY "${NMEA_LIB_DIR}"
        ARCHIVE_OUTPUT_DIRECTORY "${NMEA_ARCHIVE_DIR}"
    )

    foreach(CONFIG Debug Release RelWithDebInfo MinSizeRel)
        string(TOUPPER "${CONFIG}" CONFIG_UPPER)
        set_target_properties(nmea_decoder_ext PROPERTIES
            RUNTIME_OUTPUT_DIRECTORY_${CONFIG_UPPER} "${NMEA_RELEASE_DIR}"
            LIBRARY_OUTPUT_DIRECTORY_${CONFIG_UPPER} "${NMEA_LIB_DIR}"
            ARCHIVE_OUTPUT_DIRECTORY_${CONFIG_UPPER} "${NMEA_ARCHIVE_DIR}"
        )
    endforeach()

    if(MINGW)
        set_target_properties(nmea_decoder_ext PROPERTIES PREFIX "")
    endif()
endif()
