//x is the x coordinate, y is y topdown and z is scanline
//x and y both start from 1
#define pixelOffset(x, y, z) ( ( ( y ) - 1 ) * z ) + ( (x - 1) * 4 )

//Just like pixelOffset but x and y start from 0 and the math is faster
#define pixelOffset0(x, y, z) ( ( ( y ) ) * z ) + ( ( x ) * 4 )

#define rgb(r, g, b) (u32)(b + (g * 256) + (r * 256 * 256))

#ifndef _USEFUL_H_
#define _USEFUL_H_
//x is the x coordinate for the certical line to be printed
//c is color code is hex
void PRINTERROR(u32 x, u32 c) {
	for (u32 seggs = 1; seggs <= bootboot.fb_height; seggs++) {*(u32*)(&fb + pixelOffset(x, seggs, bootboot.fb_scanline)) = c;}
}
#endif