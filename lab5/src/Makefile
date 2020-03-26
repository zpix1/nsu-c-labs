all:
	gcc -O2 -Wall pq.c coder.c packer.c bitreader.c bitwriter.c cli.c -o huffman

do_tester:
	gcc -g -Wall pq.c coder.c packer.c bitreader.c bitwriter.c do_tester.c -o do_tester.out