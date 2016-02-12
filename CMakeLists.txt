########################################################################
# Project setup
########################################################################
cmake_minimum_required(VERSION 2.8.9)
project(BTLE_Blocks CXX)

find_package(Pothos CONFIG REQUIRED)

########################################################################
## BTLE blocks
########################################################################
POTHOS_MODULE_UTIL(
    TARGET BTLE_Blocks
    SOURCES
        DecodeBTLE.cpp
    DESTINATION btle
    ENABLE_DOCS
)
