/* Poly
   Useful
   Lib */

#include "include/pul.h"

void puDrawButtonFrame( PUU32 _RastPort, PUU32 _State, pusRect* _pRect );
void puGetButtonFrameInnerRect( PUU32 _State, pusRect* _pRect );
void puGetButtonFrameOuterRect( PUU32 _State, pusRect* _pRect );

void puDrawNormalButtonFrame( PUU32 _RastPort, pusRect* _pRect );
void puDrawActiveButtonFrame( PUU32 _RastPort, pusRect* _pRect );
void puDrawBoxFrame( PUU32 _RastPort, pusRect* _pRect );
void puDrawHSliderFrame( PUU32 _RastPort, PUU32 _State, pusRect* _pRect );
void puDrawVSliderFrame( PUU32 _RastPort, PUU32 _State, pusRect* _pRect );
void puDrawActiveHSliderFrame( PUU32 _RastPort, pusRect* _pRect );
void puDrawNormalVSliderFrame( PUU32 _RastPort, pusRect* _pRect );
void puDrawActiveVSliderFrame( PUU32 _RastPort, pusRect* _pRect );

void puDrawTextEntryFrame( PUU32 _RastPort, PUU32 _State, pusRect* _pRect );
void puDrawReadOnlyTextEntryFrame( PUU32 _RastPort, PUU32 _State, pusRect* _pRect );
void puGetTextEntryFrameInnerRect( PUU32 _State, pusRect* _pRect );
void puGetTextEntryFrameOuterRect( PUU32 _State, pusRect* _pRect );

void puDrawGroupFrame( PUU32 _RastPort, PUU32 _State, pusRect* _pRect, PUU8* _pLabel );
void puGetGroupFrameInnerRect( PUU32 _RastPort, PUU32 _State, pusRect* _pRect, PUU8* _pLabel );
void puGetGroupFrameOuterRect( PUU32 _RastPort, PUU32 _State, pusRect* _pRect, PUU8* _pLabel );

void puDrawTitleFrame( PUU32 _RastPort, PUU32 _State, pusRect* _pRect, PUU8* _pLabel );
void puGetTitleFrameInnerRect( PUU32 _RastPort, PUU32 _State, pusRect* _pRect, PUU8* _pLabel );
void puGetTitleFrameOuterRect( PUU32 _RastPort, PUU32 _State, pusRect* _pRect, PUU8* _pLabel );

void puDrawInfoBoxFrame( PUU32 _RastPort, PUU32 _State, pusRect* _pRect );
void puGetInfoBoxFrameInnerRect( PUU32 _State, pusRect* _pRect );
void puGetInfoBoxFrameOuterRect( PUU32 _State, pusRect* _pRect );

void puDrawBoxFrameBG( PUU32 _RastPort, pusRect* _pRect );
void puDrawTextEntryFrameBG( PUU32 _RastPort, pusRect* _pRect );
void puDrawReadOnlyTextEntryFrameBG( PUU32 _RastPort, pusRect* _pRect );


void puDrawFrame( PUU32 _RastPort, PUU32 _Type, PUU32 _State, pusRect* _pRect, PUU8* _pLabel )
{
	switch( _Type )
	{
		case PUFRAME_BUTTON:
		case PUFRAME_SMALLBUTTON:
			puDrawButtonFrame( _RastPort, _State, _pRect );
			break;

		case PUFRAME_BOX:
			puDrawBoxFrame( _RastPort, _pRect );
			break;

		case PUFRAME_HSLIDER:
			puDrawHSliderFrame( _RastPort, _State, _pRect );
			break;

		case PUFRAME_VSLIDER:
			puDrawVSliderFrame( _RastPort, _State, _pRect );
			break;

		case PUFRAME_TEXTENTRY:
			puDrawTextEntryFrame( _RastPort, _State, _pRect );
			break;

		case PUFRAME_READONLYTEXTENTRY:
			puDrawReadOnlyTextEntryFrame( _RastPort, _State, _pRect );
			break;

		case PUFRAME_GROUP:
			puDrawGroupFrame( _RastPort, _State, _pRect, _pLabel );
			break;

		case PUFRAME_TITLE:
			puDrawTitleFrame( _RastPort, _State, _pRect, _pLabel );
			break;

		case PUFRAME_INFOBOX:
			puDrawInfoBoxFrame( _RastPort, _State, _pRect );
			break;
	}
}

void puDrawFrameBG( PUU32 _RastPort, PUU32 _Type, PUU32 _State, pusRect* _pRect )
{
	switch( _Type )
	{
		case PUFRAME_NONE:
		case PUFRAME_BOX:
		case PUFRAME_INFOBOX:
		case PUFRAME_GROUP:
		case PUFRAME_TITLE:
			puDrawBoxFrameBG( _RastPort, _pRect );
			break;

		case PUFRAME_TEXTENTRY:
			puDrawTextEntryFrameBG( _RastPort, _pRect );
			break;

		case PUFRAME_READONLYTEXTENTRY:
			puDrawReadOnlyTextEntryFrameBG( _RastPort, _pRect );
			break;

		//	puDrawGroupFrame( _RastPort, _State, _pRect, _pLabel );
			break;

		//	puDrawTitleFrame( _RastPort, _State, _pRect, _pLabel );
			break;
	}
}

void puGetFrameInnerRect( PUU32 _RastPort, PUU32 _Type, PUU32 _State, pusRect* _pRect, PUU8* _pLabel )
{
	switch( _Type )
	{
		case PUFRAME_BUTTON:
		case PUFRAME_BOX:
		case PUFRAME_HSLIDER:
		case PUFRAME_VSLIDER:
			puGetButtonFrameInnerRect( _State, _pRect );
			break;

		case PUFRAME_TEXTENTRY:
		case PUFRAME_READONLYTEXTENTRY:
		case PUFRAME_SMALLBUTTON:
			puGetTextEntryFrameInnerRect( _State, _pRect );
			break;

		case PUFRAME_GROUP:
			puGetGroupFrameInnerRect( _RastPort, _State, _pRect, _pLabel );
			break;

		case PUFRAME_TITLE:
			puGetTitleFrameInnerRect( _RastPort, _State, _pRect, _pLabel );
			break;

		case PUFRAME_INFOBOX:
			puGetInfoBoxFrameInnerRect( _State, _pRect );
			break;
	}
}

void puGetFrameOuterRect( PUU32 _RastPort, PUU32 _Type, PUU32 _State, pusRect* _pRect, PUU8* _pLabel )
{
	switch( _Type )
	{
		case PUFRAME_BUTTON:
		case PUFRAME_BOX:
		case PUFRAME_HSLIDER:
		case PUFRAME_VSLIDER:
			puGetButtonFrameOuterRect( _State, _pRect );
			break;

		case PUFRAME_TEXTENTRY:
		case PUFRAME_READONLYTEXTENTRY:
		case PUFRAME_SMALLBUTTON:
			puGetTextEntryFrameOuterRect( _State, _pRect );
			break;

		case PUFRAME_GROUP:
			puGetGroupFrameOuterRect( _RastPort, _State, _pRect, _pLabel );
			break;

		case PUFRAME_TITLE:
			puGetTitleFrameOuterRect( _RastPort, _State, _pRect, _pLabel );
			break;

		case PUFRAME_INFOBOX:
			puGetInfoBoxFrameOuterRect( _State, _pRect );
			break;	}
}


void puDrawBoxFrameBG( PUU32 _RastPort, pusRect* _pRect )
{
	puSetFillPen( _RastPort, 170, 170, 170 );
	puRect( _RastPort, _pRect );
}

void puDrawTextEntryFrameBG( PUU32 _RastPort, pusRect* _pRect )
{
	puSetFillPen( _RastPort, 255, 255, 255 );
	puRect( _RastPort, _pRect );
}

void puDrawReadOnlyTextEntryFrameBG( PUU32 _RastPort, pusRect* _pRect )
{
	puSetFillPen( _RastPort, 200, 200, 200 );
	puRect( _RastPort, _pRect );
}


void puDrawButtonFrame( PUU32 _RastPort, PUU32 _State, pusRect* _pRect )
{
	switch( _State )
	{
		case PUSTATE_NORMAL:
			puDrawNormalButtonFrame( _RastPort, _pRect );
			break;

		case PUSTATE_ACTIVE:
			puDrawActiveButtonFrame( _RastPort, _pRect );
			break;
	}
}

void puDrawHSliderFrame( PUU32 _RastPort, PUU32 _State, pusRect* _pRect )
{
	switch( _State )
	{
		case PUSTATE_NORMAL:
			puDrawNormalButtonFrame( _RastPort, _pRect );
			break;

		case PUSTATE_ACTIVE:
			puDrawActiveHSliderFrame( _RastPort, _pRect );
			break;
	}
}

void puDrawVSliderFrame( PUU32 _RastPort, PUU32 _State, pusRect* _pRect )
{
	switch( _State )
	{
		case PUSTATE_NORMAL:
			puDrawNormalVSliderFrame( _RastPort, _pRect );
			break;

		case PUSTATE_ACTIVE:
			puDrawActiveVSliderFrame( _RastPort, _pRect );
			break;
	}
}


/* Rendering states */
/*enum
{
	PUSTATE_NORMAL,
	PUSTATE_DISABLED,
	PUSTATE_HIGHLIGHTED,
	PUSTATE_ACTIVE
};*/

void puDrawNormalButtonFrame( PUU32 _RastPort, pusRect* _pRect )
{
	pusRect Rect;

	/* Border, white part */
	puSetFillPen( _RastPort, 255, 255, 255 );
	Rect.X = _pRect->X;
	Rect.Y = _pRect->Y;
	Rect.Width = _pRect->Width;
	Rect.Height = 2;
	puRect( _RastPort, &Rect );

	Rect.Height = _pRect->Height;
	Rect.Width = 2;
	puRect( _RastPort, &Rect );

	/* Border, black part */
	puSetFillPen( _RastPort, 0, 0, 0 );
	Rect.X += 1;
	Rect.Y += _pRect->Height - 2;
	Rect.Width = _pRect->Width - 1;
	Rect.Height = 2;
	puRect( _RastPort, &Rect );

	Rect.X += _pRect->Width - 3;
	Rect.Y = _pRect->Y + 1;
	Rect.Height = _pRect->Height - 1;
	Rect.Width = 2;
	puRect( _RastPort, &Rect );

	/* Fill with gradient */
	Rect.X = _pRect->X + 2;
	Rect.Y = _pRect->Y + 2;
	Rect.Width = _pRect->Width - 4;
	Rect.Height = _pRect->Height - 4;
	puVerGradientRect( _RastPort, &Rect, 220, 220, 225, 60 + 20, 100 + 20, 128 + 20 );
	//puSetFillPen( _RastPort, 170, 170, 170 );
	//puRect( _RastPort, &Rect );
}

void puDrawActiveButtonFrame( PUU32 _RastPort, pusRect* _pRect )
{
	pusRect Rect;

	/* Border, white part */
	puSetFillPen( _RastPort, 0, 0, 0 );
//	puSetFillPen( _RastPort, 255, 255, 255 );
	Rect.X = _pRect->X;
	Rect.Y = _pRect->Y;
	Rect.Width = _pRect->Width;
	Rect.Height = 2;
	puRect( _RastPort, &Rect );

	Rect.Height = _pRect->Height;
	Rect.Width = 2;
	puRect( _RastPort, &Rect );

	/* Border, black part */
	puSetFillPen( _RastPort, 255, 255, 255 );
//	puSetFillPen( _RastPort, 0, 0, 0 );
	Rect.X += 1;
	Rect.Y += _pRect->Height - 2;
	Rect.Width = _pRect->Width - 1;
	Rect.Height = 2;
	puRect( _RastPort, &Rect );

	Rect.X += _pRect->Width - 3;
	Rect.Y = _pRect->Y + 1;
	Rect.Height = _pRect->Height - 1;
	Rect.Width = 2;
	puRect( _RastPort, &Rect );

	/* Fill with gradient */
	Rect.X = _pRect->X + 2;
	Rect.Y = _pRect->Y + 2;
	Rect.Width = _pRect->Width - 4;
	Rect.Height = _pRect->Height - 4;
//	puVerGradientRect( _RastPort, &Rect, 60, 100, 200, 250, 250, 255 );
	puVerGradientRect( _RastPort, &Rect, 150, 190, 228, 255, 255, 255 );
//	puVerGradientRect( _RastPort, &Rect, 255, 255, 255, 150, 190, 228 );
//	puVerGradientRect( _RastPort, &Rect, 60 + 20, 100 + 20, 128 + 20, 220, 220, 225 );

}

void puDrawBoxFrame( PUU32 _RastPort, pusRect* _pRect )
{
	pusRect Rect;

	/* Border, white part */
	puSetFillPen( _RastPort, 255, 255, 255 );
	Rect.X = _pRect->X;
	Rect.Y = _pRect->Y;
	Rect.Width = _pRect->Width;
	Rect.Height = 2;
	puRect( _RastPort, &Rect );

	Rect.Height = _pRect->Height;
	Rect.Width = 2;
	puRect( _RastPort, &Rect );

	/* Border, black part */
	puSetFillPen( _RastPort, 0, 0, 0 );
	Rect.X += 1;
	Rect.Y += _pRect->Height - 2;
	Rect.Width = _pRect->Width - 1;
	Rect.Height = 2;
	puRect( _RastPort, &Rect );

	Rect.X += _pRect->Width - 3;
	Rect.Y = _pRect->Y + 1;
	Rect.Height = _pRect->Height - 1;
	Rect.Width = 2;
	puRect( _RastPort, &Rect );

	/* Fill */
/*	Rect.X = _pRect->X + 2;
	Rect.Y = _pRect->Y + 2;
	Rect.Width = _pRect->Width - 4;
	Rect.Height = _pRect->Height - 4;
	puSetFillPen( _RastPort, 170, 170, 170 );
	puRect( _RastPort, &Rect );*/
	//puVerGradientRect( _RastPort, &Rect, 220, 220, 225, 60 + 20, 100 + 20, 128 + 20 );

	Rect.X = _pRect->X + 2;
	Rect.Y = _pRect->Y + 2;
	Rect.Width = _pRect->Width - 4;
	Rect.Height = 2;
	puSetFillPen( _RastPort, 170, 170, 170 );
	puRect( _RastPort, &Rect );

	Rect.Y += _pRect->Height - 6;
	puRect( _RastPort, &Rect );

	Rect.Y -= _pRect->Height - 8;
	Rect.Width = 2;
	Rect.Height = _pRect->Height - 8;
	puRect( _RastPort, &Rect );

	Rect.X += _pRect->Width - 6;
	puRect( _RastPort, &Rect );
}

void puDrawActiveHSliderFrame( PUU32 _RastPort, pusRect* _pRect )
{
	pusRect Rect;

	/* Border, white part */
	puSetFillPen( _RastPort, 255, 255, 255 );
	Rect.X = _pRect->X;
	Rect.Y = _pRect->Y;
	Rect.Width = _pRect->Width;
	Rect.Height = 2;
	puRect( _RastPort, &Rect );

	Rect.Height = _pRect->Height;
	Rect.Width = 2;
	puRect( _RastPort, &Rect );

	/* Border, black part */
	puSetFillPen( _RastPort, 60, 60, 60 );
	Rect.X += 1;
	Rect.Y += _pRect->Height - 2;
	Rect.Width = _pRect->Width - 1;
	Rect.Height = 2;
	puRect( _RastPort, &Rect );

	Rect.X += _pRect->Width - 3;
	Rect.Y = _pRect->Y + 1;
	Rect.Height = _pRect->Height - 1;
	Rect.Width = 2;
	puRect( _RastPort, &Rect );

	/* Fill with gradient */
	Rect.X = _pRect->X + 2;
	Rect.Y = _pRect->Y + 2;
	Rect.Width = _pRect->Width - 4;
	Rect.Height = _pRect->Height - 4;
	puVerGradientRect( _RastPort, &Rect, 255, 255, 255, 150, 190, 228 );
	//puSetFillPen( _RastPort, 230, 255, 255 );
	//puRect( _RastPort, &Rect );
}

void puDrawNormalVSliderFrame( PUU32 _RastPort, pusRect* _pRect )
{
	pusRect Rect;

	/* Border, white part */
	puSetFillPen( _RastPort, 255, 255, 255 );
	Rect.X = _pRect->X;
	Rect.Y = _pRect->Y;
	Rect.Width = _pRect->Width;
	Rect.Height = 2;
	puRect( _RastPort, &Rect );

	Rect.Height = _pRect->Height;
	Rect.Width = 2;
	puRect( _RastPort, &Rect );

	/* Border, black part */
	puSetFillPen( _RastPort, 0, 0, 0 );
	Rect.X += 1;
	Rect.Y += _pRect->Height - 2;
	Rect.Width = _pRect->Width - 1;
	Rect.Height = 2;
	puRect( _RastPort, &Rect );

	Rect.X += _pRect->Width - 3;
	Rect.Y = _pRect->Y + 1;
	Rect.Height = _pRect->Height - 1;
	Rect.Width = 2;
	puRect( _RastPort, &Rect );

	/* Fill with gradient */
	Rect.X = _pRect->X + 2;
	Rect.Y = _pRect->Y + 2;
	Rect.Width = _pRect->Width - 4;
	Rect.Height = _pRect->Height - 4;
	puHorGradientRect( _RastPort, &Rect, 220, 220, 225, 60 + 20, 100 + 20, 128 + 20 );
	//puSetFillPen( _RastPort, 170, 170, 170 );
	//puRect( _RastPort, &Rect );
}

void puDrawActiveVSliderFrame( PUU32 _RastPort, pusRect* _pRect )
{
	pusRect Rect;

	/* Border, white part */
	puSetFillPen( _RastPort, 255, 255, 255 );
	Rect.X = _pRect->X;
	Rect.Y = _pRect->Y;
	Rect.Width = _pRect->Width;
	Rect.Height = 2;
	puRect( _RastPort, &Rect );

	Rect.Height = _pRect->Height;
	Rect.Width = 2;
	puRect( _RastPort, &Rect );

	/* Border, black part */
	puSetFillPen( _RastPort, 60, 60, 60 );
	Rect.X += 1;
	Rect.Y += _pRect->Height - 2;
	Rect.Width = _pRect->Width - 1;
	Rect.Height = 2;
	puRect( _RastPort, &Rect );

	Rect.X += _pRect->Width - 3;
	Rect.Y = _pRect->Y + 1;
	Rect.Height = _pRect->Height - 1;
	Rect.Width = 2;
	puRect( _RastPort, &Rect );

	/* Fill with gradient */
	Rect.X = _pRect->X + 2;
	Rect.Y = _pRect->Y + 2;
	Rect.Width = _pRect->Width - 4;
	Rect.Height = _pRect->Height - 4;
	puHorGradientRect( _RastPort, &Rect, 255, 255, 255, 150, 190, 228 );
	//puSetFillPen( _RastPort, 230, 255, 255 );
	//puRect( _RastPort, &Rect );
}

void puDrawTextEntryFrame( PUU32 _RastPort, PUU32 _State, pusRect* _pRect )
{
	pusRect Rect;

	puSetFillPen( _RastPort, 0, 0, 0 );
	Rect.X = _pRect->X;
	Rect.Y = _pRect->Y;
	Rect.Width = _pRect->Width;
	Rect.Height = 1;
	puRect( _RastPort, &Rect );

	Rect.Height = _pRect->Height;
	Rect.Width = 1;
	puRect( _RastPort, &Rect );

	/* Border, black part */
	//puSetFillPen( _RastPort, 255, 255, 255 );
//	Rect.X += 1;
	Rect.Y += _pRect->Height - 1;
	Rect.Width = _pRect->Width;
	Rect.Height = 1;
	puRect( _RastPort, &Rect );

	Rect.X += _pRect->Width - 1;
	Rect.Y = _pRect->Y;
	Rect.Height = _pRect->Height;
	Rect.Width = 1;
	puRect( _RastPort, &Rect );


	Rect.X = _pRect->X + 1;
	Rect.Y = _pRect->Y + 1;
	Rect.Width = _pRect->Width - 2;
	Rect.Height = 1;
	puSetFillPen( _RastPort, 255, 255, 255 );
	puRect( _RastPort, &Rect );

	Rect.Y += _pRect->Height - 3;
	puRect( _RastPort, &Rect );

	Rect.Y -= _pRect->Height - 4;
	Rect.Width = 1;
	Rect.Height = _pRect->Height - 4;
	puRect( _RastPort, &Rect );

	Rect.X += _pRect->Width - 3;
	puRect( _RastPort, &Rect );


	/* Fill with gradient */
/*	Rect.X = _pRect->X + 1;
	Rect.Y = _pRect->Y + 1;
	Rect.Width = _pRect->Width - 2;
	Rect.Height = _pRect->Height - 2;
	//puHorGradientRect( _RastPort, &Rect, 255, 255, 255, 150, 190, 228 );
	puSetFillPen( _RastPort, 255, 255, 255 );
//	puRect( _RastPort, &Rect );*/
}

void puDrawReadOnlyTextEntryFrame( PUU32 _RastPort, PUU32 _State, pusRect* _pRect )
{
	pusRect Rect;

	puSetFillPen( _RastPort, 0, 0, 0 );
	Rect.X = _pRect->X;
	Rect.Y = _pRect->Y;
	Rect.Width = _pRect->Width;
	Rect.Height = 1;
	puRect( _RastPort, &Rect );

	Rect.Height = _pRect->Height;
	Rect.Width = 1;
	puRect( _RastPort, &Rect );

	/* Border, black part */
	puSetFillPen( _RastPort, 255, 255, 255 );
//	Rect.X += 1;
	Rect.Y += _pRect->Height - 1;
	Rect.Width = _pRect->Width;
	Rect.Height = 1;
	puRect( _RastPort, &Rect );

	Rect.X += _pRect->Width - 1;
	Rect.Y = _pRect->Y;
	Rect.Height = _pRect->Height;
	Rect.Width = 1;
	puRect( _RastPort, &Rect );

	Rect.X = _pRect->X + 1;
	Rect.Y = _pRect->Y + 1;
	Rect.Width = _pRect->Width - 2;
	Rect.Height = 1;
	puSetFillPen( _RastPort, 200, 200, 200 );
	puRect( _RastPort, &Rect );

	Rect.Y += _pRect->Height - 3;
	puRect( _RastPort, &Rect );

	Rect.Y -= _pRect->Height - 4;
	Rect.Width = 1;
	Rect.Height = _pRect->Height - 4;
	puRect( _RastPort, &Rect );

	Rect.X += _pRect->Width - 3;
	puRect( _RastPort, &Rect );


	/* Fill with gradient */
/*	Rect.X = _pRect->X + 1;
	Rect.Y = _pRect->Y + 1;
	Rect.Width = _pRect->Width - 2;
	Rect.Height = _pRect->Height - 2;
	//puHorGradientRect( _RastPort, &Rect, 255, 255, 255, 150, 190, 228 );
	puSetFillPen( _RastPort, 200, 200, 200 );*/
//	puRect( _RastPort, &Rect );
}

void puGetButtonFrameInnerRect( PUU32 _State, pusRect* _pRect )
{
	_pRect->X += 4;
	_pRect->Y += 4;
	_pRect->Height -= 8;
	_pRect->Width -= 8;
}

void puGetButtonFrameOuterRect( PUU32 _State, pusRect* _pRect )
{
	_pRect->X -= 4;
	_pRect->Y -= 4;
	_pRect->Height += 8;
	_pRect->Width += 8;
}

void puGetTextEntryFrameInnerRect( PUU32 _State, pusRect* _pRect )
{
	_pRect->X += 2;
	_pRect->Y += 2;
	_pRect->Height -= 4;
	_pRect->Width -= 4;
}

void puGetTextEntryFrameOuterRect( PUU32 _State, pusRect* _pRect )
{
	_pRect->X -= 2;
	_pRect->Y -= 2;
	_pRect->Height += 4;
	_pRect->Width += 4;
}

void puDrawGroupFrame( PUU32 _RastPort, PUU32 _State, pusRect* _pRect, PUU8* _pLabel )
{
	pusRect Rect, Rect2;
	PUS32 OffsetY = 0;

	Rect.X = _pRect->X;
	Rect.Y = _pRect->Y;
	Rect.Width = _pRect->Width;

	if( _pLabel )
	{
		puTextSize( _RastPort, _pLabel, 0, &Rect2 );
		OffsetY = Rect2.Height / 2;

		Rect.Height = OffsetY;
		puSetFillPen( _RastPort, 170, 170, 170 );
		puRect( _RastPort, &Rect );
	}

	/* Outer part */
	/* Border, black part */
	puSetFillPen( _RastPort, 0, 0, 0 );
	Rect.Y += OffsetY;
	Rect.Width = _pRect->Width;
	Rect.Height = 2;
	puRect( _RastPort, &Rect );

	Rect.Height = _pRect->Height - OffsetY;
	Rect.Width = 2;
	puRect( _RastPort, &Rect );

	/* Border, white part */
	puSetFillPen( _RastPort, 255, 255, 255 );
	Rect.X += 1;
	Rect.Y += _pRect->Height - 2 - OffsetY;
	Rect.Width = _pRect->Width - 1;
	Rect.Height = 2;
	puRect( _RastPort, &Rect );

	Rect.X += _pRect->Width - 3;
	Rect.Y = _pRect->Y + 1 + OffsetY;
	Rect.Height = _pRect->Height - 1 - OffsetY;
	Rect.Width = 2;
	puRect( _RastPort, &Rect );

	/* Inner part */
	/* Border, white part */
//	puSetFillPen( _RastPort, 255, 255, 255 );
	Rect.X = _pRect->X + 2;
	Rect.Y = _pRect->Y + 2 + OffsetY;
	Rect.Width = _pRect->Width - 4;
	Rect.Height = 2;
	puRect( _RastPort, &Rect );

	Rect.Height = _pRect->Height - 4 - OffsetY;
	Rect.Width = 2;
	puRect( _RastPort, &Rect );

	/* Border, black part */
	puSetFillPen( _RastPort, 0, 0, 0 );
	Rect.X += 1;
	Rect.Y += _pRect->Height - 2 - 4 - OffsetY;
	Rect.Width = _pRect->Width - 1 - 4;
	Rect.Height = 2;
	puRect( _RastPort, &Rect );

	Rect.X += _pRect->Width - 3 - 4;
	Rect.Y = _pRect->Y + 1 + 2 + OffsetY;
	Rect.Height = _pRect->Height - 1 - 4 - OffsetY;
	Rect.Width = 2;
	puRect( _RastPort, &Rect );

	Rect.X = _pRect->X + 4;
	Rect.Y = _pRect->Y + 4 + OffsetY;
	Rect.Width = _pRect->Width - 8;
	Rect.Height = 2;

	if( OffsetY )
		Rect.Height = Rect2.Height - OffsetY - 2;

	puSetFillPen( _RastPort, 170, 170, 170 );
	puRect( _RastPort, &Rect );

	Rect.Height = 2;
	Rect.Y += _pRect->Height - 10 - OffsetY;
	puRect( _RastPort, &Rect );

	Rect.Y -= _pRect->Height - 12 - OffsetY;
	Rect.Width = 2;
	Rect.Height = _pRect->Height - 12 - OffsetY;
	puRect( _RastPort, &Rect );

	Rect.X += _pRect->Width - 10;
	puRect( _RastPort, &Rect );

	if( _pLabel )
	{
		puSetFillPen( _RastPort, 170, 170, 170 );
		Rect2.X = _pRect->X + ( _pRect->Width - Rect2.Width ) / 2 - 4;
		Rect2.Y = _pRect->Y;
		Rect2.Width += 8;
		puRect( _RastPort, &Rect2 );

		puDrawText( _RastPort, _pLabel, 0, Rect2.X + 4, Rect2.Y );
	}
}

void puGetGroupFrameInnerRect( PUU32 _RastPort, PUU32 _State, pusRect* _pRect, PUU8* _pLabel )
{
	pusRect Rect;

	_pRect->X += 6;
	_pRect->Width -= 12;

	if( _pLabel )
	{
		puTextSize( _RastPort, _pLabel, 0, &Rect );
		_pRect->Y += Rect.Height + 2;
		_pRect->Height -= Rect.Height + 8;
	}
	else
	{
		_pRect->Y += 6;
		_pRect->Height -= 12;
	}
}

void puGetGroupFrameOuterRect( PUU32 _RastPort, PUU32 _State, pusRect* _pRect, PUU8* _pLabel )
{
	pusRect Rect;

	_pRect->X -= 6;
	_pRect->Width += 12;

	if( _pLabel )
	{
		puTextSize( _RastPort, _pLabel, 0, &Rect );
		_pRect->Y -= Rect.Height + 2;
		_pRect->Height += Rect.Height + 8;
	}
	else
	{
		_pRect->Y -= 6;
		_pRect->Height += 12;
	}
}

void puDrawTitleFrame( PUU32 _RastPort, PUU32 _State, pusRect* _pRect, PUU8* _pLabel )
{
	pusRect Rect, Rect2;
	PUS32 OffsetY = 1;

	Rect.X = _pRect->X;
	Rect.Y = _pRect->Y;
	Rect.Width = _pRect->Width;

	puSetFillPen( _RastPort, 170, 170, 170 );

	if( _pLabel )
	{
		puTextSize( _RastPort, _pLabel, 0, &Rect2 );
		OffsetY = Rect2.Height / 2;
		Rect.Height = OffsetY;
	}
	else
		Rect.Height = _pRect->Height;

	puRect( _RastPort, &Rect );

	puSetFillPen( _RastPort, 0, 0, 0 );
	Rect.Y += OffsetY;
	Rect.Height = 1;
	puRect( _RastPort, &Rect );

	puSetFillPen( _RastPort, 255, 255, 255 );
	Rect.X = _pRect->X;
	Rect.Y = _pRect->Y + 1 + OffsetY;
	Rect.Width = _pRect->Width;
	Rect.Height = 1;
	puRect( _RastPort, &Rect );

	if( _pLabel )
	{
		Rect.X = _pRect->X;
		Rect.Y = _pRect->Y + 2 + OffsetY;
		Rect.Width = _pRect->Width;
		Rect.Height = 2;

		if( OffsetY )
			Rect.Height = Rect2.Height - OffsetY;

		puSetFillPen( _RastPort, 170, 170, 170 );
		puRect( _RastPort, &Rect );

		puSetFillPen( _RastPort, 170, 170, 170 );
		Rect2.X = _pRect->X + ( _pRect->Width - Rect2.Width ) / 2 - 3;
		Rect2.Y = _pRect->Y;
		Rect2.Width += 6;
		puRect( _RastPort, &Rect2 );

		puSetDrawPen( _RastPort, 0, 0, 0 );
		puDrawText( _RastPort, _pLabel, 0, Rect2.X + 3, Rect2.Y );
	}
}

void puGetTitleFrameInnerRect( PUU32 _RastPort, PUU32 _State, pusRect* _pRect, PUU8* _pLabel )
{
	pusRect Rect;

	if( _pLabel )
	{
		puTextSize( _RastPort, _pLabel, 0, &Rect );
		_pRect->Y += Rect.Height + 2;
		_pRect->Height -= Rect.Height + 2;
	}
	else
	{
		_pRect->Y += 4;
		_pRect->Height -= 4;
	}
}

void puGetTitleFrameOuterRect( PUU32 _RastPort, PUU32 _State, pusRect* _pRect, PUU8* _pLabel )
{
	pusRect Rect;

//	_pRect->X -= 6;
//	_pRect->Width += 12;

	if( _pLabel )
	{
		puTextSize( _RastPort, _pLabel, 0, &Rect );
		_pRect->Y -= Rect.Height + 2;
		_pRect->Height += Rect.Height + 2;
	}
	else
	{
		_pRect->Y -= 4;
		_pRect->Height += 4;
	}
}

void puDrawInfoBoxFrame( PUU32 _RastPort, PUU32 _State, pusRect* _pRect )
{
	pusRect Rect;

	/* Border, white part */
	puSetFillPen( _RastPort, 0, 0, 0 );
	Rect.X = _pRect->X;
	Rect.Y = _pRect->Y;
	Rect.Width = _pRect->Width;
	Rect.Height = 2;
	puRect( _RastPort, &Rect );

	Rect.Height = _pRect->Height;
	Rect.Width = 2;
	puRect( _RastPort, &Rect );

	/* Border, black part */
	puSetFillPen( _RastPort, 255, 255, 255 );
	Rect.X += 1;
	Rect.Y += _pRect->Height - 2;
	Rect.Width = _pRect->Width - 1;
	Rect.Height = 2;
	puRect( _RastPort, &Rect );

	Rect.X += _pRect->Width - 3;
	Rect.Y = _pRect->Y + 1;
	Rect.Height = _pRect->Height - 1;
	Rect.Width = 2;
	puRect( _RastPort, &Rect );

	/* Fill */
/*	Rect.X = _pRect->X + 2;
	Rect.Y = _pRect->Y + 2;
	Rect.Width = _pRect->Width - 4;
	Rect.Height = _pRect->Height - 4;
	puSetFillPen( _RastPort, 170, 170, 170 );
	puRect( _RastPort, &Rect );*/
	//puVerGradientRect( _RastPort, &Rect, 220, 220, 225, 60 + 20, 100 + 20, 128 + 20 );

	Rect.X = _pRect->X + 2;
	Rect.Y = _pRect->Y + 2;
	Rect.Width = _pRect->Width - 4;
	Rect.Height = 2;
	puSetFillPen( _RastPort, 170, 170, 170 );
	puRect( _RastPort, &Rect );

	Rect.Y += _pRect->Height - 6;
	puRect( _RastPort, &Rect );

	Rect.Y -= _pRect->Height - 8;
	Rect.Width = 2;
	Rect.Height = _pRect->Height - 8;
	puRect( _RastPort, &Rect );

	Rect.X += _pRect->Width - 6;
	puRect( _RastPort, &Rect );
}

void puGetInfoBoxFrameInnerRect( PUU32 _State, pusRect* _pRect )
{
	_pRect->X += 3;
	_pRect->Y += 3;
	_pRect->Height -= 6;
	_pRect->Width -= 6;
}

void puGetInfoBoxFrameOuterRect( PUU32 _State, pusRect* _pRect )
{
	_pRect->X -= 3;
	_pRect->Y -= 3;
	_pRect->Height += 6;
	_pRect->Width += 6;
}