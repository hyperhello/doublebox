	Doublebox is a simplified implementation of encoding data in floating point numbers, or "nanboxing".

	Theory: In IEEE 754 64-bit floating point, a NaN can be represented by the high 16 bits equalling 0xFFF8 through 0xFFFF. In practice, hardware only produces NaN with 0xFFF8. We can use 0xFFF9—0xFFFF to easily encode at least seven things in the other 48 bits:

	0xFFF9 (1001) null
	0xFFFA (1010) undefined
	0xFFFB (1011)	bool, with zero (false) or nonzero (true) in data area
	0xFFFC (1100) integer or other data, 48 bits must be passed as uint64_t.
	0xFFFD (1101) string, up to five chars that will be padded with trailing zeros.
	0xFFFE (1101) custom, available for your adapted use 
	0xFFFF (1111) empty, it's easy to memset an array with 0xFF.
	
	There is no doublebox type. The code all just uses doubles, but you will probably want to typedef in your own project. Double numbers (from -inf to inf inclusive) just work as normal doubles. In some cases, such as the string type, you can reliably compare two doubleboxes with == and !=. 

	ToDo: Need to check big endian compatibility.
