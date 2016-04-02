/* Poly
   Useful
   Lib */

#ifndef __PUL_SRC_TEXTAREACLASS_H__
#define __PUL_SRC_TEXTAREACLASS_H__

#include <pul/pul.h>

typedef struct _pusTextAreaLine pusTextAreaLine;

struct _pusTextAreaLine
{
	pusNode	Node;

	// For wrapped lines: pointer to the unwrapped line
	// it belongs to
	// For unwrapped lines: pointer to the first wrapped line belonging
	// to it
	pusTextAreaLine*	pSibling;
	PUU8*	pText;
	PUU16	Length;

	union
	{
		// Used by unwrapped lines only: number of wrapped lines referencing it
		PUU16		RefCount;
		PUU16		LineNum;
	};
};

typedef struct
{
	pusTextAreaLine* pLine;
	PUS16 CharPos;
	PUU16 PixelPos;
} pusTextAreaPos;

typedef struct
{
	pusList		UnwrappedLines;
	pusList		WrappedLines;

	pusList*	pDisplayLines;
	pusTextAreaLine*	pFirstDisplayLine;

	PUU32			WrapMode;
	PUU8			bForceHeight;
	PUU8			bReadOnly;
	PUU8			bNoCutPaste;

	PUU8			CenterMode;
	PUU8			Font;

	// Scroll area
	PUU32			NumDisplayLines;
	PUU32			NumBufferLines;
	PUU32			NumVisibleLines;
	PUU32			ScrollWidth;

	// Scroll position
	PUU32			ScrollX;
	PUU32			ScrollY;

	// Cursor/selection position
	pusTextAreaPos	SelBegin;
	pusTextAreaPos	SelEnd;
} pusTextAreaClassData;

PUU32 puTextAreaClassHandler( PULID _Object, PULID _Class, void* _pData, PUU32 _MethodID, PUU32 _Param1, PUU32 _Param2 );

/* "TextArea" */
/*#define PU_TEXTAREACLASS_ATT	0x130000
#define PU_TEXTAREACLASS_MTD	0x131000

#define PUA_TEXTENTRY_BUFFER		PU_TEXTAREACLASS_ATT + 1
#define PUA_TEXTENTRY_WRAPMODE		PU_TEXTAREACLASS_ATT + 2
#define PUA_TEXTENTRY_FORCEHEIGHT	PU_TEXTAREACLASS_ATT + 3
#define PUA_TEXTENTRY_READONLY		PU_TEXTAREACLASS_ATT + 4
#define PUA_TEXTENTRY_NOCUTPASTE	PU_TEXTAREACLASS_ATT + 5

enum
{
	PUWRAPMODE_NONE,
	PUWRAPMODE_CHARACTER,
	PUWRAPMODE_WORD
};
*/

#endif