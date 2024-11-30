cmake_minimum_required(VERSION 3.5)

idf_component_register(
        SRCS "aht20.c"
        PRIV_REQUIRES driver
        INCLUDE_DIRS "include"
)
