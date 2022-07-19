all: build_part_1 build_part_2

build_part_1:
	gcc -ggdb -O0 -o part_1 ArtX_part_1.c -lev

build_part_2:
	gcc -ggdb -O0 -o part_2 ArtX_part_2.c -lev -pthread
