all:
	cp ../../*.mojom.h .
	grep -v "gene_embedded_types" test.mojom.h > temp && mv temp test.mojom.h
	clang++ *.cpp --std=c++14 -g
