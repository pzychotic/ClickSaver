/* Poly
   Useful
   Lib */

#include "include/pul.h"

void puHorGradientRect( PUU32 _RastPort, pusRect* _pRect, PUU8 _R1, PUU8 _G1, PUU8 _B1, PUU8 _R2, PUU8 _G2, PUU8 _B2 )
{
	PUS32 Red, Green, Blue;
	PUS32 dRed, dGreen, dBlue;
	PUS32 Count;
	pusRect Rect;

	Rect.X = _pRect->X;
	Rect.Y = _pRect->Y;
	Rect.Height = _pRect->Height;
	Rect.Width = 1;

	Red = _R1 << 16;
	Green = _G1 << 16;
	Blue = _B1 << 16;

	Count = _pRect->Width;

	if( Count == 0 )
		return;

	dRed = ( ( _R2 << 16 ) - Red ) / Count;
	dGreen = ( ( _G2 << 16 ) - Green ) / Count;
	dBlue = ( ( _B2 << 16 ) - Blue ) / Count;

	for( ; Count > 0; Count-- )
	{
		puSetFillPen( _RastPort, ( PUU8 )( Red >> 16 ), ( PUU8 )( Green >> 16 ), ( PUU8 )( Blue >> 16 ) );
		puRect( _RastPort, &Rect );

		Red += dRed;
		Green += dGreen;
		Blue += dBlue;

		Rect.X++;
	}
}

void puVerGradientRect( PUU32 _RastPort, pusRect* _pRect, PUU8 _R1, PUU8 _G1, PUU8 _B1, PUU8 _R2, PUU8 _G2, PUU8 _B2 )
{
	PUS32 Red, Green, Blue;
	PUS32 dRed, dGreen, dBlue;
	PUS32 Count;
	pusRect Rect;

	Rect.X = _pRect->X;
	Rect.Y = _pRect->Y;
	Rect.Width = _pRect->Width;
	Rect.Height = 1;

	Red = _R1 << 16;
	Green = _G1 << 16;
	Blue = _B1 << 16;

	Count = _pRect->Height;

	if( Count == 0 )
		return;

	dRed = ( ( _R2 << 16 ) - Red ) / Count;
	dGreen = ( ( _G2 << 16 ) - Green ) / Count;
	dBlue = ( ( _B2 << 16 ) - Blue ) / Count;

	for( ; Count > 0; Count-- )
	{
		puSetFillPen( _RastPort, ( PUU8 )( Red >> 16 ), ( PUU8 )( Green >> 16 ), ( PUU8 )( Blue >> 16 ) );
		puRect( _RastPort, &Rect );

		Red += dRed;
		Green += dGreen;
		Blue += dBlue;

		Rect.Y++;
	}
}