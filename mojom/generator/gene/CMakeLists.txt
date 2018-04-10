cmake_minimum_required(VERSION 3.9)
project(gene)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_COMPILER g++)

add_executable(gene
        gene_basics.cpp
        gene_basics.h
        gene_constraints.h
        gene_embedded_types.h
        gene_includes.h
        main.cpp
        gene_basics_um.cpp
        test.mojom.h
        test2.mojom.h
        test.mojom.server.h
        test2.mojom.server.h
        test.mojom.client.h
        test2.mojom.client.h
        gene_runtime.h)
