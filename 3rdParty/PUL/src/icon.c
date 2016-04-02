/* Poly
   Useful
   Lib */

#include "include/pul.h"

void puDrawIcon( PUU32 _RastPort, PUU32 _Type, PUU32 _State, PUU32 _Left, PUU32 _Top )
{
	pusRect Rect;
	Rect.X = _Left;
	Rect.Y = _Top;

	switch( _Type )
	{
		case PUICON_CHECKBOX:
			Rect.Width = 16;
			Rect.Height = 16;
			puDrawFrame( _RastPort, PUFRAME_BUTTON, _State, &Rect, NULL );
			break;
	}
}

void puGetIconSize( PUU32 _Type, PUU32 _State, pusRect* _pRect )
{
	switch( _Type )
	{
		case PUICON_CHECKBOX:
			_pRect->Width = 16;
			_pRect->Height = 16;
			break;
	}
}