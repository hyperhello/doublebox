
/* a test of the functions of doublebox.h */

#include "doublebox.h"

/* printf a line describing the contents of a doublebox */
void print_doublebox(const double d)
{
	uint64_t i = get_doublebox_bytes(d);
	
	printf("0x");
	for (int x = 0; x < 32; x += 4)
		printf("%c", "0123456789ABCDEF"[(char)(i >> (60-x)) & 15]);
	printf(" 0x");
	for (int x = 32; x < 64; x += 4)
		printf("%c", "0123456789ABCDEF"[(char)(i >> (60-x)) & 15]);

	char buffer[50], sixchars[6];
	const char* desc = buffer;
	switch (get_doublebox_type(d))
	{
		case 0: 
			sprintf(buffer, "%1.16lf", d); 
			break;
		case DOUBLEBOX_NULL: 
			desc = "null"; 
			break;
		case DOUBLEBOX_UNDEFINED: 
			desc = "undefined"; 
			break;
		case DOUBLEBOX_BOOL: 
			if (i & DOUBLEBOX_DATAMASK) desc = "bool true"; else desc = "bool false"; 
			break;
		case DOUBLEBOX_INTEGER: 
			sprintf(buffer, "integer %llu", get_doublebox_integer(d)); 
			break;
		case DOUBLEBOX_STRING: 
			get_doublebox_string(d, sixchars); sprintf(buffer, "string \"%s\"", sixchars); 
			break;
		case DOUBLEBOX_CUSTOM: 
			sprintf(buffer, "custom %llu", get_doublebox_integer(d)); 
			break;
		case DOUBLEBOX_EMPTY: 
			desc = "empty"; 
			break;
		default: 
			sprintf(buffer, "unknown %llu", get_doublebox_type(d)); 
			break;
	}
	
	printf(" = %s\n", desc);
}

/* let's test */
int main(int argc, char *argv[]) 
{
	print_doublebox(0.0);
	print_doublebox(-0.0);
	print_doublebox(10);
	print_doublebox(-10);
	print_doublebox(3.14159265358979323846264);
	print_doublebox(0.0000000000000001);
	print_doublebox(doublebox_double(1));
	print_doublebox(doublebox_double(-1));
	print_doublebox(-doublebox_double(1));
	print_doublebox(INFINITY);
	print_doublebox(-INFINITY);
	print_doublebox(NAN);
	
	print_doublebox(doublebox_bytes(DOUBLEBOX_MAX_DOUBLE));
	print_doublebox(doublebox_null());
	print_doublebox(doublebox_undefined());
	print_doublebox(doublebox_bool(false));
	print_doublebox(doublebox_bool(true));
	print_doublebox(doublebox_integer(123456789));
	print_doublebox(doublebox_string("ABCDE..."));		/* only the first five chars are stored */
	print_doublebox(doublebox_custom(123));
	print_doublebox(doublebox_bytes(0xFFFFFFFFFFFFFFFF));
	print_doublebox(doublebox_double(doublebox_bytes(0xFFFFFFFFFFFFFFFF)));
}

/* desired output:

0x00000000 0x00000000 = 0.0000000000000000
0x80000000 0x00000000 = -0.0000000000000000
0x40240000 0x00000000 = 10.0000000000000000
0xC0240000 0x00000000 = -10.0000000000000000
0x400921FB 0x54442D18 = 3.1415926535897931
0x3C9CD2B2 0x97D889BC = 0.0000000000000001
0x3FF00000 0x00000000 = 1.0000000000000000
0xBFF00000 0x00000000 = -1.0000000000000000
0xBFF00000 0x00000000 = -1.0000000000000000
0x7FF00000 0x00000000 = inf
0xFFF00000 0x00000000 = -inf
0x7FF80000 0x00000000 = nan
0xFFF8FFFF 0xFFFFFFFF = nan
0xFFF90000 0x00000000 = null
0xFFFA0000 0x00000000 = undefined
0xFFFB0000 0x00000000 = bool false
0xFFFB0000 0x00000001 = bool true
0xFFFC0000 0x075BCD15 = integer 123456789
0xFFFD0045 0x44434241 = string "ABCDE"
0xFFFE0000 0x0000007B = custom 123
0xFFFFFFFF 0xFFFFFFFF = empty
0x7FF80000 0x00000000 = nan

*/