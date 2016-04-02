/* Poly
   Useful
   Lib */

#include <pul/pul.h>
#include <windows.h>
#include "win32.h"

void puSetFont( PUU32 _RastPort, PUU8 _Type );

void puWin32InitRastPort( pusWin32RastPort* _pRastPort )
{
	HGDIOBJ OldPen;
	HGDIOBJ OldBrush;

	_pRastPort->PenX = 0;
	_pRastPort->PenY = 0;

	_pRastPort->hpen = CreatePen( PS_SOLID, 0, RGB( 0, 0, 0 ) );
	_pRastPort->hbrush = CreateSolidBrush( RGB( 0, 0, 0 ) );
	_pRastPort->invispen = CreatePen( PS_NULL, 0, RGB( 0, 0, 0 ) );

	OldPen = SelectObject( _pRastPort->hdc, _pRastPort->hpen );
	DeleteObject( OldPen );

	OldBrush = SelectObject( _pRastPort->hdc, _pRastPort->hbrush );
	DeleteObject( OldBrush );

	puSetFont( ( PUU32 )_pRastPort, PUFONT_NORMAL );
}

void puWin32DeleteRastPort( pusWin32RastPort* _pRastPort )
{
	DeleteObject( _pRastPort->invispen );
	DeleteObject( _pRastPort->hbrush );
	DeleteObject( _pRastPort->hpen );
	_pRastPort->hpen = 0;
	_pRastPort->invispen = 0;
}

PUU32 puGetRastPort( PUU32 _Window )
{
	pusWin32RastPort* pRastPort;

	if( !( pRastPort = malloc( sizeof( pusWin32RastPort ) ) ) )
		return 0;

	pRastPort->hdc = GetDC( ( HWND )_Window );
	puWin32InitRastPort( pRastPort );

	return ( PUU32 )pRastPort;
}

void puReleaseRastPort( PUU32 _Window, PUU32 _RastPort )
{
	pusWin32RastPort* pRastPort;
	pRastPort = ( pusWin32RastPort* )_RastPort;
	
	ReleaseDC( ( HWND )_Window, pRastPort->hdc );
	puWin32DeleteRastPort( ( pusWin32RastPort* )_RastPort );

	free( ( void* )_RastPort );
}

void puSetFont( PUU32 _RastPort, PUU8 _Type )
{
	HFONT hfnt;
	int Type;
	pusWin32RastPort* pRastPort;
	pRastPort = ( pusWin32RastPort* )_RastPort;

	switch( _Type )
	{
		case PUFONT_NORMAL:
			Type  = ANSI_VAR_FONT;
			break;

		case PUFONT_FIXED:
			Type = ANSI_FIXED_FONT;
	}

	if( hfnt = GetStockObject( Type ) )
	{
		SelectObject( pRastPort->hdc, hfnt );
		DeleteObject( hfnt );
	}
}

void puGetPenPos( PUU32 _RastPort, PUU32* _pXPos, PUU32* _pYPos )
{
	pusWin32RastPort* pRastPort;
	pRastPort = ( pusWin32RastPort* )_RastPort;

	*_pXPos = pRastPort->PenX;
	*_pYPos = pRastPort->PenY;
}

void puSetDrawPen( PUU32 _RastPort, PUU8 _Red, PUU8 _Green, PUU8 _Blue )
{
	HGDIOBJ OldPen;
	pusWin32RastPort* pRastPort;
	pRastPort = ( pusWin32RastPort* )_RastPort;

	//OldPen = pRastPort->hpen;
	pRastPort->hpen = CreatePen( PS_SOLID, 0, RGB( _Red, _Green, _Blue ) );
	OldPen = SelectObject( pRastPort->hdc, pRastPort->hpen );
	DeleteObject( OldPen );

	SetTextColor( pRastPort->hdc, RGB( _Red, _Green, _Blue ) );
}

void puSetFillPen( PUU32 _RastPort, PUU8 _Red, PUU8 _Green, PUU8 _Blue )
{
	HGDIOBJ OldBrush;
	pusWin32RastPort* pRastPort;
	pRastPort = ( pusWin32RastPort* )_RastPort;

	//OldBrush = pRastPort->hbrush;
	pRastPort->hbrush = CreateSolidBrush( RGB( _Red, _Green, _Blue ) );
	OldBrush = SelectObject( pRastPort->hdc, pRastPort->hbrush );
	DeleteObject( OldBrush );
}

void puMove( PUU32 _RastPort, PUU32 _XPos, PUU32 _YPos )
{
	pusWin32RastPort* pRastPort;
	pRastPort = ( pusWin32RastPort* )_RastPort;

	pRastPort->PenX = _XPos;
	pRastPort->PenY = _YPos;
}

void puLine( PUU32 _RastPort, PUU32 _XPos, PUU32 _YPos )
{
	pusWin32RastPort* pRastPort;
	pRastPort = ( pusWin32RastPort* )_RastPort;

	MoveToEx( pRastPort->hdc, pRastPort->PenX, pRastPort->PenY, NULL );
	LineTo( pRastPort->hdc, _XPos, _YPos );

	pRastPort->PenX = _XPos;
	pRastPort->PenY = _YPos;
}

void puRect( PUU32 _RastPort, pusRect* _pRect )
{
	pusWin32RastPort* pRastPort;
	pRastPort = ( pusWin32RastPort* )_RastPort;

	SelectObject( pRastPort->hdc, pRastPort->invispen );
	Rectangle( pRastPort->hdc, _pRect->X, _pRect->Y, _pRect->X + _pRect->Width + 1, _pRect->Y + _pRect->Height + 1 );
	SelectObject( pRastPort->hdc, pRastPort->hpen );
}

void puInvRect( PUU32 _RastPort, pusRect* _pRect )
{
	pusWin32RastPort* pRastPort;
	pRastPort = ( pusWin32RastPort* )_RastPort;

	PatBlt( pRastPort->hdc, _pRect->X, _pRect->Y, _pRect->Width, _pRect->Height, DSTINVERT );
}

void puTriangle( PUU32 _RastPort, PUU32 _X1, PUU32 _Y1, PUU32 _X2, PUU32 _Y2, PUU32 _X3, PUU32 _Y3 )
{
	POINT Vertices[3];
	pusWin32RastPort* pRastPort;
	pRastPort = ( pusWin32RastPort* )_RastPort;

	Vertices[0].x = _X1;
	Vertices[0].y = _Y1;
	Vertices[1].x = _X2;
	Vertices[1].y = _Y2;
	Vertices[2].x = _X3;
	Vertices[2].y = _Y3;

	Polygon( pRastPort->hdc, Vertices, 3 );
}

void puTextSize( PUU32 _RastPort, PUU8* _pText, PUU32 _Length, pusRect* _pRect )
{
	pusWin32RastPort* pRastPort;
	SIZE Size;

	pRastPort = ( pusWin32RastPort* )_RastPort;

	GetTextExtentPoint32( pRastPort->hdc, _pText, ( _Length ? _Length : strlen( _pText ) ), &Size );
	_pRect->Width = Size.cx;
	_pRect->Height = Size.cy;
}

PUU32 puTextFit( PUU32 _RastPort, PUU8* _pText, PUU32 _Length, PUU32 _Size )
{
	pusWin32RastPort* pRastPort;
	SIZE Size;
	PUU32 NumChars;

	pRastPort = ( pusWin32RastPort* )_RastPort;

	GetTextExtentExPoint( pRastPort->hdc, _pText, ( _Length ? _Length : strlen( _pText ) ), _Size, &NumChars, NULL, &Size );

	return NumChars;
}

void puDrawText( PUU32 _RastPort, PUU8* _pText, PUU32 _Length, PUU32 _XPos, PUU32 _YPos )
{
	pusWin32RastPort* pRastPort;
	PUU32 Length;

	if( _Length )
		Length = _Length;
	else
		Length = strlen( _pText );

	pRastPort = ( pusWin32RastPort* )_RastPort;
	SetBkMode( pRastPort->hdc, TRANSPARENT );

	TextOut( pRastPort->hdc, _XPos, _YPos, _pText, Length );
}

void puSetClipping( PUU32 _RastPort, pusRect* pRect )
{
	pusWin32RastPort* pRastPort;
	pRastPort = ( pusWin32RastPort* )_RastPort;

	if( pRect )
		IntersectClipRect( pRastPort->hdc, pRect->X, pRect->Y, pRect->X + pRect->Width, pRect->Y + pRect->Height ); 
	else
		SelectClipRgn( pRastPort->hdc, NULL );
}

PUU32 puAllocBitmap( PUU32 _RastPort, PUU32 _Width, PUU32 _Height, PUU32 _PixFormat, void* _pData )
{
	HBITMAP hBitmap;
	BITMAPINFOHEADER* pbmHeader;
	BITMAPINFO bmInfo;
	pusWin32RastPort* pRastPort;
	pRastPort = ( pusWin32RastPort* )_RastPort;

	pbmHeader = &bmInfo.bmiHeader;

	pbmHeader->biSize = sizeof( BITMAPINFOHEADER );
	pbmHeader->biWidth = _Width;
	pbmHeader->biHeight = -( ( PUS32 )_Height );
	pbmHeader->biPlanes = 1;
	pbmHeader->biBitCount = ( WORD )_PixFormat;
	pbmHeader->biCompression = BI_RGB;
	pbmHeader->biSizeImage = 0;
	pbmHeader->biXPelsPerMeter = 1000;
	pbmHeader->biYPelsPerMeter = 1000;
	pbmHeader->biClrUsed = 0;
	pbmHeader->biClrImportant = 0;

	hBitmap = CreateDIBitmap( pRastPort->hdc, pbmHeader, CBM_INIT, _pData, &bmInfo, DIB_RGB_COLORS );

	return ( PUU32 )hBitmap;
}

void puFreeBitmap( PUU32 _Bitmap )
{
	DeleteObject( ( HGDIOBJ )_Bitmap );
}

void puDrawBitmap( PUU32 _RastPort, PUU32 _Bitmap, pusRect* _pRect )
{
	HDC hdc;
	pusWin32RastPort* pRastPort;
	pRastPort = ( pusWin32RastPort* )_RastPort;


	if( !( hdc = CreateCompatibleDC( pRastPort->hdc ) ) )
		return;

	if( !SelectObject( hdc, ( HGDIOBJ )_Bitmap ) )
	{
		DeleteDC( hdc );
		return;
	}

	BitBlt( pRastPort->hdc, _pRect->X, _pRect->Y, _pRect->Width, _pRect->Height, hdc, 0, 0, SRCCOPY );
	DeleteDC( hdc );
}