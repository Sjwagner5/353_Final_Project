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

// Font data for Viner Hand ITC 14pt
// Used for "WELCOME TO FRUIT ASSASSIN"
extern const uint8_t vinerHandITC_14ptBitmaps[];
extern const FONT_INFO vinerHandITC_14ptFontInfo;
extern const FONT_CHAR_INFO vinerHandITC_14ptDescriptors[];


// Bitmap info for explosion
extern const uint8_t explosionBitmaps [];
extern const uint8_t explosionWidthPixels;
extern const uint8_t explosionHeightPixels;

// Font data for Courier New 10pt
// Used for "USE JOYSTICK TO SELECT LEVEL"
extern const uint8_t courierNew_10ptBitmaps[];
extern const FONT_INFO courierNew_10ptFontInfo;
extern const FONT_CHAR_INFO courierNew_10ptDescriptors[];

// Font data for Courier New 12pt
// Used for "EASY", "MEDIUM, and "HARD"
extern const uint8_t courierNew_12ptBitmaps[];
extern const FONT_INFO courierNew_12ptFontInfo;
extern const FONT_CHAR_INFO courierNew_12ptDescriptors[];

// Font data for Courier New 10pt
// Used for "PUSH ANY BUTTON TO START"
extern const uint8_t courierNew_10ptBitmaps2[];
extern const FONT_INFO courierNew_10ptFontInfo2;
extern const FONT_CHAR_INFO courierNew_10ptDescriptors2[];

// Font data for Viner Hand ITC 14pt
// Used for "GAME OVER"
extern const uint8_t vinerHandITC_14ptBitmaps2[];
extern const FONT_INFO vinerHandITC_14ptFontInfo2;
extern const FONT_CHAR_INFO vinerHandITC_14ptDescriptors2[];

// Font data for Courier New 12pt
// Used for "SCORE:", "HIGHSCORE:", and "NEW HIGHSCORE!"
extern const uint8_t courierNew_12ptBitmaps2[];
extern const FONT_INFO courierNew_12ptFontInfo2;
extern const FONT_CHAR_INFO courierNew_12ptDescriptors2[];

// Font data for Courier New 12pt
// Used for numbers printed on screen
extern const uint8_t courierNew_12ptBitmaps3[];
extern const FONT_INFO courierNew_12ptFontInfo3;
extern const FONT_CHAR_INFO courierNew_12ptDescriptors3[];


#endif
