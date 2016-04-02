/* Poly
   Useful
   Lib */

#include <stdlib.h>
#include <pul/pul.h>
#include "vergroupclass.h"
#include "../include/pul.h"

void puVerGroupPreLayout( PULID _Object, pusVerGroupClassData* _pData, PUU32 _Param1, PUU32 _Param2 );
void puVerGroupLayout( PULID _Object, pusVerGroupClassData* _pData, PUU32 _Param1, PUU32 _Param2 );
void puVerGroupRender( PULID _Object, PUU32 _RastPort );
PUU32 puVerGroupSetAttr( PULID _Object, PULID _Class, void* _pData, PUU32 _Attr, PUU32 _Val );


PUU32 puVerGroupClassHandler( PULID _Object, PULID _Class, void* _pData, PUU32 _MethodID, PUU32 _Param1, PUU32 _Param2 )
{
	pusVerGroupClassData* pData;
	pData = ( pusVerGroupClassData* )_pData;

	switch( _MethodID )
	{
		case PUM_NEW:
			pData->NumChild = 0;
			pData->Spacing = 2;
			pData->CenterMode = PU_VGROUP_CENTER;
			pData->bFixedWidth = FALSE;
			pData->bFixedHeight = FALSE;
			return puDoSuperMethod( _Object, _Class, _MethodID, _Param1, _Param2 );

		case PUM_SET:
			puDoSuperMethod( _Object, _Class, _MethodID, _Param1, _Param2 );
			return puVerGroupSetAttr( _Object, _Class, _pData, _Param1, _Param2 );

		case PUM_ADDCHILD:
			pData->NumChild++;
			return puDoSuperMethod( _Object, _Class, _MethodID, _Param1, _Param2 );

		/* Control methods */
		case PUM_CONTROL_CALCMINMAX:
			{
				pusList* pChildList;
				pusNode* pCurrentNode;
				pusNode* pNextNode;

				PUU32 MinWidth = 0, MaxWidth = 0;
				PUU32 MinHeight = 0, MaxHeight = 0;
				PUU32 Temp, NumVisChild = 0;

				pChildList = ( pusList* )puDoMethod( _Object, PUM_GETCHILDLIST, 0, 0 );
				pCurrentNode = pChildList->pFirst;

				while( pNextNode = pCurrentNode->pNext )
				{
					if( !( puGetAttribute( ( PULID )pCurrentNode, PUA_CONTROL_HIDDEN )
						   && !puGetAttribute( ( PULID )pCurrentNode, PUA_CONTROL_KEEPROOM ) ) )
					{
						puDoMethod( ( PULID )pCurrentNode, PUM_CONTROL_CALCMINMAX, _Param1, _Param2 );

						Temp = puGetAttribute( ( PULID )pCurrentNode, PUA_CONTROL_MINWIDTH );
						if( Temp > MinWidth )
							MinWidth = Temp;
						Temp = puGetAttribute( ( PULID )pCurrentNode, PUA_CONTROL_MAXWIDTH );
						if( Temp > MaxWidth )
							MaxWidth = Temp;

						Temp = puGetAttribute( ( PULID )pCurrentNode, PUA_CONTROL_MINHEIGHT );
							MinHeight += Temp;
						Temp = puGetAttribute( ( PULID )pCurrentNode, PUA_CONTROL_MAXHEIGHT );
							MaxHeight += Temp;

						NumVisChild++;
					}

					pCurrentNode = pNextNode;
				}

				MinHeight += ( NumVisChild - 1 ) * pData->Spacing;
				MaxHeight += ( NumVisChild - 1 ) * pData->Spacing;

				if( MaxHeight > 32768 )
					MaxHeight = 32768;

				puSetAttribute( _Object, PUA_CONTROL_MINWIDTH, MinWidth );
				puSetAttribute( _Object, PUA_CONTROL_MINHEIGHT, MinHeight );

				if( pData->bFixedWidth )
					puSetAttribute( _Object, PUA_CONTROL_MAXWIDTH, MinWidth );
				else
					puSetAttribute( _Object, PUA_CONTROL_MAXWIDTH, MaxWidth );

				if( pData->bFixedHeight )
					puSetAttribute( _Object, PUA_CONTROL_MAXHEIGHT, MinHeight );
				else
					puSetAttribute( _Object, PUA_CONTROL_MAXHEIGHT, MaxHeight );

				puDoSuperMethod( _Object, _Class, _MethodID, _Param1, _Param2 );
			}
			break;

		case PUM_CONTROL_PRELAYOUT:
			puVerGroupPreLayout( _Object, pData, _Param1, _Param2 );
			return puDoSuperMethod( _Object, _Class, _MethodID, _Param1, _Param2 );

		case PUM_CONTROL_LAYOUT:
			puVerGroupLayout( _Object, pData, _Param1, _Param2 );
			break;

		case PUM_CONTROL_RENDER:
			puDoSuperMethod( _Object, _Class, _MethodID, _Param1, _Param2 );
			puVerGroupRender( _Object, _Param1 );
			break;

		default:
			return puDoSuperMethod( _Object, _Class, _MethodID, _Param1, _Param2 );
	}

	return 0;
}

void puVerGroupLayout( PULID _Object, pusVerGroupClassData* _pData, PUU32 _Param1, PUU32 _Param2 )
{
	pusList* pChildList;
	pusNode* pCurrentNode;
	pusNode* pNextNode;
	pusVGroupLayoutInfo* pLayoutInfo;
	PUU32 i, TotalWeight = 0, TotalFreeSpace = 0, NumVisibleChild = 0;
	pusRect Rect;
	PUU32 LayoutState = PULAYOUT_MORESPACE, Size, Pos, Pos2;

	if( _pData->NumChild == 0 )
		return;

	if( !( pLayoutInfo = malloc( _pData->NumChild * sizeof( pusVGroupLayoutInfo ) ) ) )
		return;

	pChildList = ( pusList* )puDoMethod( _Object, PUM_GETCHILDLIST, 0, 0 );
	pCurrentNode = pChildList->pFirst;

	for( i = 0; i < _pData->NumChild; i++ )
	{
		pNextNode = pCurrentNode->pNext;

		if( !puGetAttribute( ( PULID )pCurrentNode, PUA_CONTROL_HIDDEN ) )
		{
			pLayoutInfo[NumVisibleChild].Object = ( PULID )pCurrentNode;
			pLayoutInfo[NumVisibleChild].State = PULAYOUT_FREE;
			pLayoutInfo[NumVisibleChild].Weight = puGetAttribute( ( PULID )pCurrentNode, PUA_CONTROL_WEIGHT );
			pLayoutInfo[NumVisibleChild].MinWidth = puGetAttribute( ( PULID )pCurrentNode, PUA_CONTROL_MINWIDTH );
			pLayoutInfo[NumVisibleChild].MaxWidth = puGetAttribute( ( PULID )pCurrentNode, PUA_CONTROL_MAXWIDTH );
			pLayoutInfo[NumVisibleChild].MinHeight = puGetAttribute( ( PULID )pCurrentNode, PUA_CONTROL_MINHEIGHT );
			pLayoutInfo[NumVisibleChild].MaxHeight = puGetAttribute( ( PULID )pCurrentNode, PUA_CONTROL_MAXHEIGHT );

			TotalWeight += pLayoutInfo[NumVisibleChild].Weight;

			NumVisibleChild++;
		}

		pCurrentNode = pNextNode;
	}

	puDoMethod( _Object, PUM_CONTROL_GETINNERRECT, ( PUU32 )&Rect, 0 );
	TotalFreeSpace = Rect.Height - ( NumVisibleChild - 1 ) * _pData->Spacing;

	/* Outer loop: we stop when inner loop has enumerated every child object
	   and found no contender */
	do
	{
		/* Inner loop */
		for( i = 0; i < NumVisibleChild; i++ )
		{
			if( pLayoutInfo[i].State == PULAYOUT_FREE )
			{
				/* If the object is free, we check the size it will get. If it's
				   lower than its min size or higher than its max size, we block it in
				   min or max state respectively. */
				Size = ( pLayoutInfo[i].Weight * TotalFreeSpace ) / TotalWeight;

				if( Size < pLayoutInfo[i].MinHeight )
				{
					pLayoutInfo[i].State = PULAYOUT_BLOCKEDMIN;
					TotalWeight -= pLayoutInfo[i].Weight;
					TotalFreeSpace -= pLayoutInfo[i].MinHeight;
					LayoutState = PULAYOUT_LESSSPACE;
					break;
				}

				if( Size > pLayoutInfo[i].MaxHeight )
				{
					pLayoutInfo[i].State = PULAYOUT_BLOCKEDMAX;
					TotalWeight -= pLayoutInfo[i].Weight;
					TotalFreeSpace -= pLayoutInfo[i].MaxHeight;
					LayoutState = PULAYOUT_MORESPACE;
					break;
				}
			}
			else
			{
				if( LayoutState == PULAYOUT_MORESPACE && pLayoutInfo[i].State == PULAYOUT_BLOCKEDMIN )
				{
					TotalWeight += pLayoutInfo[i].Weight;
					TotalFreeSpace += pLayoutInfo[i].MinHeight;
					Size = ( pLayoutInfo[i].Weight * TotalFreeSpace ) / TotalWeight;

					if( Size > pLayoutInfo[i].MinHeight )
					{
						pLayoutInfo[i].State = PULAYOUT_FREE;
						break;
					}
					else
					{
						TotalWeight -= pLayoutInfo[i].Weight;
						TotalFreeSpace -= pLayoutInfo[i].MinHeight;
					}
				}

				if( LayoutState == PULAYOUT_LESSSPACE && pLayoutInfo[i].State == PULAYOUT_BLOCKEDMAX )
				{
					TotalWeight += pLayoutInfo[i].Weight;
					TotalFreeSpace += pLayoutInfo[i].MaxHeight;
					Size = ( pLayoutInfo[i].Weight * TotalFreeSpace ) / TotalWeight;

					if( Size < pLayoutInfo[i].MaxHeight )
					{
						pLayoutInfo[i].State = PULAYOUT_FREE;
						break;
					}
					else
					{
						TotalWeight -= pLayoutInfo[i].Weight;
						TotalFreeSpace -= pLayoutInfo[i].MaxHeight;
					}
				}
			}
		}
	} while( i < NumVisibleChild );

	/* Now iterate one last time through objects and setup their size and position */
	Pos = Rect.Y;

	for( i = 0; i < NumVisibleChild; i++, Pos )
	{
		puSetAttribute( pLayoutInfo[i].Object, PUA_CONTROL_POSY, Pos );

		switch( pLayoutInfo[i].State )
		{
			case PULAYOUT_FREE:
				Size = ( pLayoutInfo[i].Weight * TotalFreeSpace ) / TotalWeight;
				break;

			case PULAYOUT_BLOCKEDMIN:
				Size = pLayoutInfo[i].MinHeight;
				break;

			case PULAYOUT_BLOCKEDMAX:
				Size = pLayoutInfo[i].MaxHeight;
				break;
		}

		puSetAttribute( pLayoutInfo[i].Object, PUA_CONTROL_HEIGHT, Size );
		Pos += Size + _pData->Spacing;

		Size = Rect.Width;
		Pos2 = Rect.X;

		if( Size > pLayoutInfo[i].MaxWidth )
		{
			Size = pLayoutInfo[i].MaxWidth;

			switch( _pData->CenterMode )
			{
				case PU_HGROUP_CENTER:
					Pos2 = Rect.X + ( ( Rect.Width - Size ) >> 1 );
					break;

				case PU_HGROUP_TOP:
					Pos2 = Rect.X;
					break;

				case PU_HGROUP_BOTTOM:
					Pos2 = Rect.X + Rect.Width - Size;
					break;
			}
		}

		puSetAttribute( pLayoutInfo[i].Object, PUA_CONTROL_POSX, Pos2 );
		puSetAttribute( pLayoutInfo[i].Object, PUA_CONTROL_WIDTH, Size );

		puDoMethod( pLayoutInfo[i].Object, PUM_CONTROL_LAYOUT, _Param1, _Param2 );
	}

	free( pLayoutInfo );
}


void puVerGroupPreLayout( PULID _Object, pusVerGroupClassData* _pData, PUU32 _Param1, PUU32 _Param2 )
{
	pusList* pChildList;
	pusNode* pCurrentNode;
	pusNode* pNextNode;
	pusVGroupLayoutInfo* pLayoutInfo;
	PUU32 i, TotalWeight = 0, TotalFreeSpace = 0, NumVisibleChild = 0;
	pusRect Rect;
	PUU32 LayoutState = PULAYOUT_MORESPACE, Size, Pos, Pos2;

	if( _pData->NumChild == 0 )
		return;

	if( !( pLayoutInfo = malloc( _pData->NumChild * sizeof( pusVGroupLayoutInfo ) ) ) )
		return;

	pChildList = ( pusList* )puDoMethod( _Object, PUM_GETCHILDLIST, 0, 0 );
	pCurrentNode = pChildList->pFirst;

	for( i = 0; i < _pData->NumChild; i++ )
	{
		pNextNode = pCurrentNode->pNext;

		if( !puGetAttribute( ( PULID )pCurrentNode, PUA_CONTROL_HIDDEN ) )
		{
			pLayoutInfo[NumVisibleChild].Object = ( PULID )pCurrentNode;
			pLayoutInfo[NumVisibleChild].Weight = puGetAttribute( ( PULID )pCurrentNode, PUA_CONTROL_WEIGHT );
			pLayoutInfo[NumVisibleChild].MinWidth = puGetAttribute( ( PULID )pCurrentNode, PUA_CONTROL_MINWIDTH );
			pLayoutInfo[NumVisibleChild].MaxWidth = puGetAttribute( ( PULID )pCurrentNode, PUA_CONTROL_MAXWIDTH );

			NumVisibleChild++;
		}

		pCurrentNode = pNextNode;
	}

	puDoMethod( _Object, PUM_CONTROL_GETINNERRECT, ( PUU32 )&Rect, 0 );

	/* Now iterate one last time through objects and setup their size and position */
	for( i = 0; i < NumVisibleChild; i++, Pos )
	{
		Size = Rect.Width;
		Pos2 = Rect.X;

		if( Size > pLayoutInfo[i].MaxWidth )
		{
			Size = pLayoutInfo[i].MaxWidth;

			switch( _pData->CenterMode )
			{
				case PU_HGROUP_CENTER:
					Pos2 = Rect.X + ( ( Rect.Width - Size ) >> 1 );
					break;

				case PU_HGROUP_TOP:
					Pos2 = Rect.X;
					break;

				case PU_HGROUP_BOTTOM:
					Pos2 = Rect.X + Rect.Width - Size;
					break;
			}
		}

		puSetAttribute( pLayoutInfo[i].Object, PUA_CONTROL_POSX, Pos2 );
		puSetAttribute( pLayoutInfo[i].Object, PUA_CONTROL_WIDTH, Size );

		puDoMethod( pLayoutInfo[i].Object, PUM_CONTROL_PRELAYOUT, _Param1, _Param2 );
	}

	free( pLayoutInfo );

	{
		pusList* pChildList;
		pusNode* pCurrentNode;
		pusNode* pNextNode;

		PUU32 MinHeight = 0, MaxHeight = 0;
		PUU32 Temp, NumVisChild = 0;

		pChildList = ( pusList* )puDoMethod( _Object, PUM_GETCHILDLIST, 0, 0 );
		pCurrentNode = pChildList->pFirst;

		while( pNextNode = pCurrentNode->pNext )
		{
			if( !( puGetAttribute( ( PULID )pCurrentNode, PUA_CONTROL_HIDDEN )
				   && !puGetAttribute( ( PULID )pCurrentNode, PUA_CONTROL_KEEPROOM ) ) )
			{
				Temp = puGetAttribute( ( PULID )pCurrentNode, PUA_CONTROL_MINHEIGHT );
					MinHeight += Temp;
				Temp = puGetAttribute( ( PULID )pCurrentNode, PUA_CONTROL_MAXHEIGHT );
					MaxHeight += Temp;

				NumVisChild++;
			}

			pCurrentNode = pNextNode;
		}

		MinHeight += ( NumVisChild - 1 ) * _pData->Spacing;
		MaxHeight += ( NumVisChild - 1 ) * _pData->Spacing;

		if( MaxHeight > 32768 )
			MaxHeight = 32768;

		puSetAttribute( _Object, PUA_CONTROL_MINHEIGHT, MinHeight );

		if( _pData->bFixedHeight )
			puSetAttribute( _Object, PUA_CONTROL_MAXHEIGHT, MinHeight );
		else
			puSetAttribute( _Object, PUA_CONTROL_MAXHEIGHT, MaxHeight );
	}
}

void puVerGroupRender( PULID _Object, PUU32 _RastPort )
{
	pusList* pChildList;
	pusNode* pCurrentNode;
	pusNode* pNextNode;
	pusRect Rect;
	pusRect Rect2;
	pusRect TmpRect;

	PUS32 LastPos;

	puDoMethod( _Object, PUM_CONTROL_GETINNERRECT, ( PUU32 )&Rect, 0 );

	LastPos = Rect.Y;

	pChildList = ( pusList* )puDoMethod( _Object, PUM_GETCHILDLIST, 0, 0 );
	pCurrentNode = pChildList->pFirst;

	while( pNextNode = pCurrentNode->pNext )
	{
		if( !puGetAttribute( ( PULID )pCurrentNode, PUA_CONTROL_HIDDEN ) )
		{
			puDoMethod( ( PULID )pCurrentNode, PUM_CONTROL_GETOUTERRECT, ( PUU32 )&Rect2, 0 );

			if( Rect2.X > Rect.X )
			{
				TmpRect.X = Rect.X;
				TmpRect.Width = Rect2.X - Rect.X;
				TmpRect.Y = Rect2.Y;
				TmpRect.Height = Rect2.Height;

				puDoMethod( _Object, PUM_CONTROL_CLEARBG, _RastPort, ( PUU32 )&TmpRect );
			}

			if( ( Rect2.X + Rect2.Width ) < ( Rect.X + Rect.Width ) )
			{
				TmpRect.X = Rect2.X + Rect2.Width;
				TmpRect.Width = Rect.X + Rect.Width - TmpRect.X;
				TmpRect.Y = Rect2.Y;
				TmpRect.Height = Rect2.Height;

				puDoMethod( _Object, PUM_CONTROL_CLEARBG, _RastPort, ( PUU32 )&TmpRect );
			}

			if( Rect2.Y > LastPos )
			{
				TmpRect.X = Rect.X;
				TmpRect.Width = Rect.Width;
				TmpRect.Y = LastPos;
				TmpRect.Height = Rect2.Y - LastPos;

				puDoMethod( _Object, PUM_CONTROL_CLEARBG, _RastPort, ( PUU32 )&TmpRect );
			}

			LastPos = Rect2.Y + Rect2.Height;
		}

		pCurrentNode = pNextNode;
	}

	if( ( Rect.Y + Rect.Height ) > LastPos )
	{
		TmpRect.X = Rect.X;
		TmpRect.Width = Rect.Width;
		TmpRect.Y = LastPos;
		TmpRect.Height = Rect.Y + Rect.Height - LastPos;

		puDoMethod( _Object, PUM_CONTROL_CLEARBG, _RastPort, ( PUU32 )&TmpRect );
	}

}

PUU32 puVerGroupSetAttr( PULID _Object, PULID _Class, void* _pData, PUU32 _Attr, PUU32 _Val )
{
	pusVerGroupClassData* pData;
	pData = ( pusVerGroupClassData* )_pData;

	switch( _Attr )
	{
		case PUA_VERGROUP_CENTERMODE:
			pData->CenterMode = _Val;
			break;

		case PUA_VERGROUP_SPACING:
			pData->Spacing = _Val;
			break;

		case PUA_VERGROUP_FIXEDWIDTH:
			pData->bFixedWidth = _Val;
			break;

		case PUA_VERGROUP_FIXEDHEIGHT:
			pData->bFixedHeight = _Val;
			break;
	}

	return TRUE;
}