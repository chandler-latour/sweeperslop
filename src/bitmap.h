//A file for containing helper data structures and functions for accessing bitmaps
//NOT BITMAP IMAGES, just a literal bitmap
//That being, an array of integers in which the individual bits are poked and prodded at.

#ifndef BITMAP
#define BITMAP
typedef struct s_bitaddr {
	unsigned int int_index; //the index of the integer of the bit we want
	unsigned int bit_mask; //the bit mask which targets the bit we want within the int
	/*
	basically, it's an integer for which every bit is 0 except the one we are targetting
	if you wanted to target this bit of the int and set it to one, you could do
	array[int_index] = array[int_index] | bit_mask
	the above is a bitwise "or"/union operation, where it'll produce a number 
	that has its bits equal to one for both those individual numbers

	here's how you use the bit mask to make a targetted bit 0:
	array[int_index] = array[int_index] & (~bit_mask)
	that's an and operation of the original bitmap and the inverse of the mask
	basically, it flips all the bits in the mask to their opposite value, then
	ANDs it with the original int
	the result is that all of the bits in the original that were not the target
	will stay the same because they will be ANDed to a 1
	so if they're already 0 they'll fail the AND and stay 0
	and if they're already 1 they'll pass
	and then the targetted bit will guaranteed fail the AND because it's
	being ANDed to a 0 regardless of what it is, so it's effectively set to 0.

	you can do much more with this bit mask using all sorts of binary operators!
	*/
} t_bitaddr;


/*
This function takes the index of the individual bit you want in bits
and returns a bit address for it
so say you want the address of the 37th bit, you put 37 into this function
and it'll return a bit address for that
on a standard x86_64 executable (32 bit standard ints), that'll be
an int_index of 1 (second integer) and a bit_mask of 0x00000010
*/
t_bitaddr BitIndexToBitAddr(int);
#endif
