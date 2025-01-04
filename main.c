
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