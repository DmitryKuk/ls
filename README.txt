This program is unix ls analog.

To build run:
	cd ls/ && make

To run run:
	./ls [-RGla] [DIR1 ...]

Options:
	-G  Enable colored output (enables automatically, if you use terminal (emulator))
	-R  Recursively process sub-directories
	-l  Long output format
	-a  Process all files, include ".*"

To clear run:
	make clear
