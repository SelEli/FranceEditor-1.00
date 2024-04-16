# CMakeList.txt : projet CMake pour FranceEditor-1.00, incluez la source et définissez
# la logique spécifique au projet ici.
#
cmake_minimum_required (VERSION 3.8)

# Activez Rechargement à chaud pour les compilateurs MSVC si cela est pris en charge.
if (POLICY CMP0141)
  cmake_policy(SET CMP0141 NEW)
  set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT "$<IF:$<AND:$<C_COMPILER_ID:MSVC>,$<CXX_COMPILER_ID:MSVC>>,$<$<CONFIG:Debug,RelWithDebInfo>:EditAndContinue>,$<$<CONFIG:Debug,RelWithDebInfo>:ProgramDatabase>>")
endif()

project ("FranceEditor-1.00")

# Ajoutez une source à l'exécutable de ce projet.
add_executable (FranceEditor-1.00 "FranceEditor-1.00.cpp" "FranceEditor-1.00.h")

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET FranceEditor-1.00 PROPERTY CXX_STANDARD 20)
endif()

# TODO: Ajoutez des tests et installez des cibles si nécessaire.
