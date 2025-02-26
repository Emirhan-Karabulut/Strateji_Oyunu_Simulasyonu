cmake_minimum_required(VERSION 3.10)

# Proje adı
project(project C)

# C standardını ayarla
set(CMAKE_C_STANDARD 99)

# zlib yolunu belirt
set(ZLIB_INCLUDE_DIR "C:/Users/monster/vcpkg/installed/x64-windows/include")
set(ZLIB_LIBRARY "C:/Users/monster/vcpkg/installed/x64-windows/lib/zlib.lib")

# libcurl yolunu belirt
set(CURL_INCLUDE_DIR "C:/Users/monster/vcpkg/installed/x64-windows/include")
set(CURL_LIBRARY "C:/Users/monster/vcpkg/installed/x64-windows/lib/libcurl.lib")

# raylib yolunu manuel olarak belirt
set(RAYLIB_INCLUDE_DIR "C:/Users/monster/vcpkg/installed/x64-windows/include")
set(RAYLIB_LIBRARY "C:/Users/monster/vcpkg/installed/x64-windows/lib/raylib.lib")

# Zlib, libcurl ve raylib kütüphanelerini bul ve projeye ekle
find_package(ZLIB REQUIRED)
find_package(CURL REQUIRED)

# Hedef dosya oluştur
add_executable(project pro_lab_proje.c)

# Kütüphane bağlantılarını ayarla
target_include_directories(project PRIVATE ${CURL_INCLUDE_DIR} ${ZLIB_INCLUDE_DIR} ${RAYLIB_INCLUDE_DIR})
target_link_libraries(project ${CURL_LIBRARY} ${ZLIB_LIBRARY} ${RAYLIB_LIBRARY})
