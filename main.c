
/* a test of the functions of doublebox.h */

#include "doublebox.h"

/* printf a line describing the contents of a doublebox */
void print_doublebox(const double d)
{
	uint64_t i = *(uint64_t*)&d;
	
	printf("0x");
	for (int x = 0; x < 32; x += 4)
		printf("%c", "0123456789ABCDEF"[(char)(i >> (60-x)) & 15]);
	printf(" 0x");
	for (int x = 32; x < 64; x += 4)
		printf("%c", "0123456789ABCDEF"[(char)(i >> (60-x)) & 15]);

	if (is_doublebox_double(d))
	{
		printf(" = %1.16lf\n", d);
	}
	else
	{
		const char* desc = "";
		char buffer[50], sixchars[6];
		switch (i >> 48)
		{
			case 0xFFF9: desc = "null"; break;
			case 0xFFFA: desc = "undefined"; break;
			case 0xFFFB: if (i & DOUBLEBOX_DATAMASK) desc = "boolean true"; else desc = "boolean false"; break;
			case 0xFFFC: sprintf(buffer, "integer %llu", get_doublebox_integer(d)); desc = buffer; break;
			case 0xFFFD: get_doublebox_string(d, sixchars); sprintf(buffer, "string \"%s\"", sixchars); desc = buffer; break;
			case 0xFFFE: sprintf(buffer, "custom %llu", get_doublebox_integer(d)); desc = buffer; break;
			case 0xFFFF: desc = "empty"; break;
		}
		printf(" = %s\n", desc);
	}
}

/* let's test */
int main(int argc, char *argv[]) 
{
	print_doublebox(0);
	print_doublebox(10);
	print_doublebox(-10);
	print_doublebox(3.14159265358979323846264);
	print_doublebox(0.0000000000000001);
	print_doublebox(INFINITY);
	print_doublebox(-INFINITY);
	print_doublebox(NAN);
	print_doublebox(doublebox_null());
	print_doublebox(doublebox_undefined());
	print_doublebox(doublebox_bool(false));
	print_doublebox(doublebox_bool(true));
	print_doublebox(doublebox_integer(123456789));
	print_doublebox(doublebox_string("ABCDEF"));
	print_doublebox(doublebox_custom(123));
	print_doublebox(doublebox_bytes(0xFFFFFFFFFFFFFFFF));
}

/* desired output:

0x00000000 0x00000000 = 0.0000000000000000
0x40240000 0x00000000 = 10.0000000000000000
0xC0240000 0x00000000 = -10.0000000000000000
0x400921FB 0x54442D18 = 3.1415926535897931
0x3C9CD2B2 0x97D889BC = 0.0000000000000001
0x7FF00000 0x00000000 = inf
0xFFF00000 0x00000000 = -inf
0x7FF80000 0x00000000 = nan
0xFFF90000 0x00000000 = null
0xFFFA0000 0x00000000 = undefined
0xFFFB0000 0x00000000 = boolean false
0xFFFB0000 0x00000001 = boolean true
0xFFFC0000 0x075BCD15 = integer 123456789
0xFFFD0045 0x44434241 = string "ABCDE"
0xFFFE0000 0x0000007B = custom 123
0xFFFFFFFF 0xFFFFFFFF = empty

*/