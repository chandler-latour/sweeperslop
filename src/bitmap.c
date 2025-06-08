//A file for containing helper data structures and functions for accessing bitmaps
//NOT BITMAP IMAGES, just a literal bitmap
//That being, an array of integers in which the individual bits are poked and prodded at.

#include "bitmap.h"


/*
This function takes the index of the individual bit you want in bits
and returns a bit address for it
so say you want the address of the 37th bit, you put 37 into this function
and it'll return a bit address for that
on a standard x86_64 executable (32 bit standard ints), that'll be
an int_index of 1 (second integer) and a bit_mask of 0x00000010
*/
t_bitaddr BitIndexToBitAddr(int index) {
	int int_bitsize = sizeof(int) * 8; //size of an int in bits, not bytes
	t_bitaddr ba;
	ba.int_index = index/int_bitsize;
	int interbyte_index = index%int_bitsize; //bit index within the bytes
	//the bit mask is one, elevated to the very last bit, then brought down by the interbyte index.
	ba.bit_mask = 0b1 << (int_bitsize - 1 - interbyte_index);
	return ba;
}

