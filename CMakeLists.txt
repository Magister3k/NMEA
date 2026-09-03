cmake_minimum_required(VERSION 3.15)
project(HighSpeedNmeaSystem C CXX)

# ====================================================================
# 1. ГЛОБАЛЬНЫЕ НАСТРОЙКИ СТАНДАРТОВ
# ====================================================================
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)

# Определение исходных файлов проекта
set(PROJECT_SOURCES
    main.cpp
    nmea_processor.cpp
    nmea_service.cpp
    nmea450_decoder.cpp
    nmea_decoder.cpp
    ais_decoder.cpp
    high_speed_geo_parser.cpp
)

# Создание исполняемого файла
add_executable(nmea_high_speed ${PROJECT_SOURCES})

# ====================================================================
# 2. НАСТРОЙКА АГРЕССИВНОЙ КОМПИЛЯЦИИ И FAST-MATH
# ====================================================================
if(MSVC)
    target_compile_options(nmea_high_speed PRIVATE 
        /O2          # Максимальная оптимизация по скорости
        /Oi          # Генерация встроенных (intrinsic) функций
        /Ot          # Преимущество оптимизации скорости перед размером
        /fp:fast     # Эквивалент -ffast-math для компилятора MSVC
        /arch:AVX2   # Включение инструкций AVX2 (включая аппаратный FMA)
        /nologo      # Подавление вывода заголовка компилятора
        /W3          # Третий уровень предупреждений
    )
    message(STATUS "[MSVC OPTIMIZATION] Включен высокоскоростной математический режим /fp:fast и инструкции AVX2.")
else()
    # Настройки для компиляторов GCC и Clang (Linux / MinGW)
    target_compile_options(nmea_high_speed PRIVATE 
        -O3                 # Максимальный уровень оптимизации компилятора
        -ffast-math         # Включение высокоскоростной математики (IEEE 754 bypass)
        -march=native       # Оптимизация под текущую архитектуру процессора хоста
        -ftree-vectorize    # Агрессивная авто-векторизация циклов процессором
        -Wall               # Включение всех базовых предупреждений
        -Wextra             # Дополнительные предупреждения для чистоты кода
    )
    # Линковка системной библиотеки POSIX потоков для работы фонового GC
    target_link_libraries(nmea_high_speed PRIVATE pthread)
    message(STATUS "[GCC/CLANG OPTIMIZATION] Включены флаги -O3, -ffast-math и авто-векторизация под архитектуру -march=native.")
endif()

# Добавление отдельного исполняемого файла для юнит-теста математики
add_executable(test_math test_vectorized_math.cpp)

if(MSVC)
    target_compile_options(test_math PRIVATE /O2 /fp:fast /arch:AVX2)
else()
    target_compile_options(test_math PRIVATE -O3 -ffast-math -march=native -ftree-vectorize)
endif()

# Регистрация автоматического запуска (Опционально, для CTest)
enable_testing()
add_test(NAME GeodeticMathVerification COMMAND test_math)
