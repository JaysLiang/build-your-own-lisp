CMAKE_MINIMUM_REQUIRED(VERSION 2.8)

project("build-your-own-lisp")

INCLUDE_DIRECTORIES(include)

ADD_EXECUTABLE(lispy src/main.c src/mpc.c)

TARGET_LINK_LIBRARIES(lispy edit)
