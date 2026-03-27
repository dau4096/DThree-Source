/* drawing.h */
#ifndef DRAWING_H
#define DRAWING_H
#define STB_IMAGE_WRITE_IMPLEMENTATION


#include <glm/glm.hpp>
#include <stb_image_write.h>
#include <cstdio>

#include "types.h"
#include "font.h"


namespace screen {

	//8x8 chars.
	#define WIDTH_GLYPH 8u
	#define HEIGHT_GLYPH 8u

	//64x48 canvas.
	#define WIDTH_CHARS 64u
	#define HEIGHT_CHARS 48u

	//Define as the above.
	#define WIDTH_PX (WIDTH_CHARS * WIDTH_GLYPH)
	#define HEIGHT_PX (HEIGHT_CHARS * HEIGHT_GLYPH)


	//Colour enums;
	//8 Colours = 3 bits, use for binary channels bitmask.
	enum Colour {
		C_BLACK   = 0b000,

		C_RED     = 0b100,
		C_GREEN   = 0b010,
		C_BLUE    = 0b001,

		C_YELLOW  = 0b110,
		C_CYAN    = 0b011,
		C_MAGENTA = 0b101,

		C_WHITE   = 0b111
	};


	//Store the char codes, and the actual buffer seperately.
	//Allows for just changing diff.
	inline types::Glyph data[WIDTH_CHARS * HEIGHT_CHARS];
	inline uint8_t buffer[WIDTH_PX * HEIGHT_PX]; //Indexed colour.

	const uint8_t ColourRGB[8][3] = {
		{0x00u, 0x00u, 0x00u}, //C_BLACK
		{0x00u, 0x00u, 0xFFu}, //C_BLUE
		{0x00u, 0xFFu, 0x00u}, //C_GREEN
		{0x00u, 0xFFu, 0xFFu}, //C_CYAN
		{0xFFu, 0x00u, 0x00u}, //C_RED
		{0xFFu, 0x00u, 0xFFu}, //C_MAGENTA
		{0xFFu, 0xFFu, 0x00u}, //C_YELLOW
		{0xFFu, 0xFFu, 0xFFu}, //C_WHITE
	};
}


namespace draw {

	inline bool pixel(screen::Colour c, glm::ivec2 position) {
		//Position in pixel coordinates.
		//Check bounds;
		if (
			((position.x < 0) || (position.x >= WIDTH_PX)) ||
			((position.y < 0) || (position.y >= HEIGHT_PX))
		) {
			return false; //OOB.
		}

		screen::buffer[(position.y * WIDTH_PX) + position.x] = c;
		return true; //Success
	}


	inline void displayGlyph(const types::Glyph& glf) {
		for (unsigned int y=0u; y<HEIGHT_GLYPH; y++) {
			for (unsigned int x=0u; x<WIDTH_GLYPH; x++) {
				//Mask out a single bit.
				unsigned int bitShift = 63 - ((y*WIDTH_GLYPH)+x);
				assert(bitShift < 64); //Should never try shift too far.
				bool isForeground = (
					(glf >> bitShift) & 0x1u
				) != 0u;

				if (isForeground) {
					std::cout << "▓▓";
				} else {
					std::cout << "░░";
				}
			}
			std::cout << "\n";
		}
		std::cout << std::flush;
	}


	inline bool glyph(
		GlyphID ID, glm::ivec2 position,
		screen::Colour fg=screen::C_BLACK,
		screen::Colour bg=screen::C_WHITE
	) {
		//Position in text coordinates.
		//Check bounds;
		if (
			((position.x < 0) || (position.x >= (int)(WIDTH_CHARS))) ||
			((position.y < 0) || (position.y >= (int)(HEIGHT_CHARS)))
		) {
			return false; //OOB.
		}

		//Fetch and read px.
		const types::Glyph& glf = font::glyphs[ID]; //Single uint64_t (typenamed Glyph)
		displayGlyph(glf);
		
		uint8_t* ptr = screen::buffer + (position.y * HEIGHT_GLYPH * WIDTH_PX) + (position.x * WIDTH_GLYPH); //Start ptr.
		//printf("\n\n%p", ptr);
		for (unsigned int y=0u; y<HEIGHT_GLYPH; y++) {
			for (unsigned int x=0u; x<WIDTH_GLYPH; x++) {
				//Mask out a single bit.
				unsigned int bitShift = 63 - ((y*WIDTH_GLYPH)+x);
				assert(bitShift < 64); //Should never try shift too far.
				bool isForeground = (
					(glf >> bitShift) & 0x1u
				) != 0u;
				//printf("\n%p : (%u, %u), << %u", ptr, x, y, bitShift);
				*ptr = (isForeground) ? fg : bg;

				//Increment ptr.
				//New column.
				ptr++;
			}
			//New row.
			ptr += WIDTH_PX - WIDTH_GLYPH;
		}
		return true; //Success
	}


	inline void text(
		const std::string& text, glm::ivec2 position,
		screen::Colour fg=screen::C_BLACK,
		screen::Colour bg=screen::C_WHITE
	) {
		//Draws some string. First char at position.
		int x = position.x;
		std::cout << "Title: " << text << std::endl;
		for (const char& c : text) {
			if (x >= (int)(WIDTH_CHARS)) {break; /* If it starts to go offscreen, exit early. Can never re-enter screen. */}
			std::cout << std::format(
				"Char: \'{}\', glyphID: {}", c, std::to_string(font::asciiToGEnum(c))
			) << std::endl;

			glyph(
				font::asciiToGEnum(c), //GlyphID
				glm::ivec2(x++, position.y), //Position, increment X.
				fg, bg //Colours
			);
		}
	}


	inline void saveBuffer(const std::string filePath) {
		std::vector<uint8_t> RGBbuf(WIDTH_PX * HEIGHT_PX * 3u);
		uint8_t* ptr = RGBbuf.data();
		for (uint8_t& col : screen::buffer) {
			//Add to buffer.
			ptr[0] = (col & 0b100) * 0xFFu; //R
			ptr[1] = (col & 0b010) * 0xFFu; //G
			ptr[2] = (col & 0b001) * 0xFFu; //B
			ptr += 3u;
		}

		//Write data.
		stbi_write_png(
			filePath.c_str(), //Where
			WIDTH_PX, HEIGHT_PX, 3u, //Image data format
			RGBbuf.data(), //What
			WIDTH_PX * 3u //Stride
		);
	}

}


#endif