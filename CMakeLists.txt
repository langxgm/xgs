# Minimum CMake required
cmake_minimum_required(VERSION 2.8)

# Project
project(server)

add_subdirectory(xbase)
add_subdirectory(xgame)
add_subdirectory(xshare)
add_subdirectory(xdb)
add_subdirectory(link)
add_subdirectory(login)
add_subdirectory(gate)
add_subdirectory(game)
add_subdirectory(world)
add_subdirectory(sdk)

add_dependencies(xgame_static xbase_static)
add_dependencies(xshare_static xbase_static)
add_dependencies(xdb_static xbase_static)
add_dependencies(link xshare_static xdb_static)
add_dependencies(login xgame_static xshare_static xdb_static)
add_dependencies(gate xshare_static xdb_static)
add_dependencies(game xgame_static xshare_static xdb_static)
add_dependencies(world xgame_static xshare_static xdb_static)
add_dependencies(sdk xshare_static xdb_static)
