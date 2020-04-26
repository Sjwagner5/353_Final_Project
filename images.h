#ifndef __IMAGES_H__
#define __IMAGES_H__

#include "main.h"

// ==========================================================================
// structure definition
// ==========================================================================

// This structure describes a single character's display information
typedef struct
{
	const uint8_t widthBits;					// width, in bits (or pixels), of the character
	const uint16_t offset;					// offset of the character's bitmap, in bytes, into the the FONT_INFO's data array
	
} FONT_CHAR_INFO;	

// Describes a single font
typedef struct
{
	const uint8_t 			heightPages;	// height, in pages (8 pixels), of the font's characters
	const uint8_t 			startChar;		// the first character in the font (e.g. in charInfo and data)
	const uint8_t 			endChar;		// the last character in the font
	const uint8_t			spacePixels;	// number of pixels that a space character takes up
	const FONT_CHAR_INFO*	charInfo;		// pointer to array of char information
	const uint8_t*			data;			// pointer to generated array of character visual representation
		
} FONT_INFO;	

// Bitmap info for banana
extern const uint8_t bananaBitmaps[];
extern const uint8_t bananaWidthPixels;
extern const uint8_t bananaHeightPixels;

// Bitmap info for pineapple
extern const uint8_t pineappleBitmaps[];
extern const uint8_t pineappleWidthPixels;
extern const uint8_t pineappleHeightPixels;

// Bitmap info for apple
extern const uint8_t appleBitmaps[];
extern const uint8_t appleWidthPixels;
extern const uint8_t appleHeightPixels;

/* Bitmap info for orange */
extern const uint8_t orangeBitmaps[];
extern const uint8_t orangeWidthPixels;
extern const uint8_t orangeHeightPixels;

/* Font data for Microsoft Sans Serif 8pt */
extern const uint8_t microsoftSansSerif_8ptBitmaps[];
extern const FONT_INFO microsoftSansSerif_8ptFontInfo;
extern const FONT_CHAR_INFO microsoftSansSerif_8ptDescriptors[];

// Bitmap info for explosion
extern const uint8_t explosionBitmaps [];
extern const uint8_t explosionWidthPixels;
extern const uint8_t explosionHeightPixels;




#endif
