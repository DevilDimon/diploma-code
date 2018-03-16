cmake_minimum_required(VERSION 3.9)
project(gene)

set(CMAKE_CXX_STANDARD 14)

add_executable(gene
        gene_basics.cpp
        gene_basics.h
        gene_constraints.h
        gene_embedded_types.h
        main.cpp
        test.mojom.h
        test2.mojom.h
        test.mojom.server.h
        test2.mojom.server.h
        gene_runtime.h)
