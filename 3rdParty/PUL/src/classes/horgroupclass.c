/* Poly
   Useful
   Lib */

#include <stdlib.h>
#include <pul/pul.h>
#include "horgroupclass.h"
#include "../include/pul.h"

void puHorGroupLayout( PULID _Object, pusHorGroupClassData* _pData, PUU32 _Param1, PUU32 _Param2 );
void puHorGroupPreLayout( PULID _Object, pusHorGroupClassData* _pData, PUU32 _Param1, PUU32 _Param2 );
void puHorGroupRender( PULID _Object, PUU32 _RastPort );
PUU32 puHorGroupSetAttr( PULID _Object, PULID _Class, void* _pData, PUU32 _Attr, PUU32 _Val );


PUU32 puHorGroupClassHandler( PULID _Object, PULID _Class, void* _pData, PUU32 _MethodID, PUU32 _Param1, PUU32 _Param2 )
{
	pusHorGroupClassData* pData;
	pData = ( pusHorGroupClassData* )_pData;

	switch( _MethodID )
	{
		case PUM_NEW:
			pData->NumChild = 0;
			pData->Spacing = 2;
			pData->CenterMode = PU_HGROUP_CENTER;
			return puDoSuperMethod( _Object, _Class, _MethodID, _Param1, _Param2 );

		case PUM_SET:
			puDoSuperMethod( _Object, _Class, _MethodID, _Param1, _Param2 );
			return puHorGroupSetAttr( _Object, _Class, _pData, _Param1, _Param2 );

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
						MinWidth += Temp;
						Temp = puGetAttribute( ( PULID )pCurrentNode, PUA_CONTROL_MAXWIDTH );
						MaxWidth += Temp;

						Temp = puGetAttribute( ( PULID )pCurrentNode, PUA_CONTROL_MINHEIGHT );
						if( Temp > MinHeight )
							MinHeight = Temp;
						Temp = puGetAttribute( ( PULID )pCurrentNode, PUA_CONTROL_MAXHEIGHT );
						if( Temp > MaxHeight )
							MaxHeight = Temp;

						NumVisChild++;
					}

					pCurrentNode = pNextNode;
				}

				MinWidth += ( NumVisChild - 1 ) * pData->Spacing;
				MaxWidth += ( NumVisChild - 1 ) * pData->Spacing;

				if( MaxWidth > 32768 )
					MaxWidth = 32768;

				puSetAttribute( _Object, PUA_CONTROL_MINWIDTH, MinWidth );
				puSetAttribute( _Object, PUA_CONTROL_MAXWIDTH, MaxWidth );
				puSetAttribute( _Object, PUA_CONTROL_MINHEIGHT, MinHeight );
				puSetAttribute( _Object, PUA_CONTROL_MAXHEIGHT, MaxHeight );

				puDoSuperMethod( _Object, _Class, _MethodID, _Param1, _Param2 );
			}
			break;

		case PUM_CONTROL_PRELAYOUT:
			puHorGroupPreLayout( _Object, pData, _Param1, _Param2 );
			break;

		case PUM_CONTROL_LAYOUT:
			puHorGroupLayout( _Object, pData, _Param1, _Param2 );
			break;

		case PUM_CONTROL_RENDER:
			puDoSuperMethod( _Object, _Class, _MethodID, _Param1, _Param2 );
			puHorGroupRender( _Object, _Param1 );
			break;

		default:
			return puDoSuperMethod( _Object, _Class, _MethodID, _Param1, _Param2 );
	}

	return 0;
}

void puHorGroupPreLayout( PULID _Object, pusHorGroupClassData* _pData, PUU32 _Param1, PUU32 _Param2 )
{
	pusList* pChildList;
	pusNode* pCurrentNode;
	pusNode* pNextNode;
	pusHGroupLayoutInfo* pLayoutInfo;
	PUU32 i, TotalWeight = 0, TotalFreeSpace = 0, NumVisibleChild = 0;
	pusRect Rect;
	PUU32 LayoutState = PULAYOUT_MORESPACE, Size, Pos;

	if( _pData->NumChild == 0 )
		return;

	if( !( pLayoutInfo = malloc( _pData->NumChild * sizeof( pusHGroupLayoutInfo ) ) ) )
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

			TotalWeight += pLayoutInfo[NumVisibleChild].Weight;

			NumVisibleChild++;
		}

		pCurrentNode = pNextNode;
	}

	puDoMethod( _Object, PUM_CONTROL_GETINNERRECT, ( PUU32 )&Rect, 0 );
	TotalFreeSpace = Rect.Width - ( NumVisibleChild - 1 ) * _pData->Spacing;

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

				if( Size < pLayoutInfo[i].MinWidth )
				{
					pLayoutInfo[i].State = PULAYOUT_BLOCKEDMIN;
					TotalWeight -= pLayoutInfo[i].Weight;
					TotalFreeSpace -= pLayoutInfo[i].MinWidth;
					LayoutState = PULAYOUT_LESSSPACE;
					break;
				}

				if( Size > pLayoutInfo[i].MaxWidth )
				{
					pLayoutInfo[i].State = PULAYOUT_BLOCKEDMAX;
					TotalWeight -= pLayoutInfo[i].Weight;
					TotalFreeSpace -= pLayoutInfo[i].MaxWidth;
					LayoutState = PULAYOUT_MORESPACE;
					break;
				}
			}
			else
			{
				if( LayoutState == PULAYOUT_MORESPACE && pLayoutInfo[i].State == PULAYOUT_BLOCKEDMIN )
				{
					TotalWeight += pLayoutInfo[i].Weight;
					TotalFreeSpace += pLayoutInfo[i].MinWidth;
					Size = ( pLayoutInfo[i].Weight * TotalFreeSpace ) / TotalWeight;

					if( Size > pLayoutInfo[i].MinWidth )
					{
						pLayoutInfo[i].State = PULAYOUT_FREE;
						break;
					}
					else
					{
						TotalWeight -= pLayoutInfo[i].Weight;
						TotalFreeSpace -= pLayoutInfo[i].MinWidth;
					}
				}

				if( LayoutState == PULAYOUT_LESSSPACE && pLayoutInfo[i].State == PULAYOUT_BLOCKEDMAX )
				{
					TotalWeight += pLayoutInfo[i].Weight;
					TotalFreeSpace += pLayoutInfo[i].MaxWidth;
					Size = ( pLayoutInfo[i].Weight * TotalFreeSpace ) / TotalWeight;

					if( Size < pLayoutInfo[i].MaxWidth )
					{
						pLayoutInfo[i].State = PULAYOUT_FREE;
						break;
					}
					else
					{
						TotalWeight -= pLayoutInfo[i].Weight;
						TotalFreeSpace -= pLayoutInfo[i].MaxWidth;
					}
				}
			}
		}
	} while( i < NumVisibleChild );

	/* Now iterate one last time through objects and setup their size and position */
	Pos = Rect.X;

	for( i = 0; i < NumVisibleChild; i++, Pos )
	{
		puSetAttribute( pLayoutInfo[i].Object, PUA_CONTROL_POSX, Pos );

		switch( pLayoutInfo[i].State )
		{
			case PULAYOUT_FREE:
				Size = ( pLayoutInfo[i].Weight * TotalFreeSpace ) / TotalWeight;
				break;

			case PULAYOUT_BLOCKEDMIN:
				Size = pLayoutInfo[i].MinWidth;
				break;

			case PULAYOUT_BLOCKEDMAX:
				Size = pLayoutInfo[i].MaxWidth;
				break;
		}

		puSetAttribute( pLayoutInfo[i].Object, PUA_CONTROL_WIDTH, Size );
		Pos += Size + _pData->Spacing;

		puDoMethod( pLayoutInfo[i].Object, PUM_CONTROL_PRELAYOUT, _Param1, _Param2 );
	}

	free( pLayoutInfo );
}

void puHorGroupLayout( PULID _Object, pusHorGroupClassData* _pData, PUU32 _Param1, PUU32 _Param2 )
{
	pusList* pChildList;
	pusNode* pCurrentNode;
	pusNode* pNextNode;
	pusHGroupLayoutInfo* pLayoutInfo;
	PUU32 i, TotalWeight = 0, TotalFreeSpace = 0, NumVisibleChild = 0;
	pusRect Rect;
	PUU32 LayoutState = PULAYOUT_MORESPACE, Size, Pos, Pos2;

	if( _pData->NumChild == 0 )
		return;

	if( !( pLayoutInfo = malloc( _pData->NumChild * sizeof( pusHGroupLayoutInfo ) ) ) )
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
	TotalFreeSpace = Rect.Width - ( NumVisibleChild - 1 ) * _pData->Spacing;

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

				if( Size < pLayoutInfo[i].MinWidth )
				{
					pLayoutInfo[i].State = PULAYOUT_BLOCKEDMIN;
					TotalWeight -= pLayoutInfo[i].Weight;
					TotalFreeSpace -= pLayoutInfo[i].MinWidth;
					LayoutState = PULAYOUT_LESSSPACE;
					break;
				}

				if( Size > pLayoutInfo[i].MaxWidth )
				{
					pLayoutInfo[i].State = PULAYOUT_BLOCKEDMAX;
					TotalWeight -= pLayoutInfo[i].Weight;
					TotalFreeSpace -= pLayoutInfo[i].MaxWidth;
					LayoutState = PULAYOUT_MORESPACE;
					break;
				}
			}
			else
			{
				if( LayoutState == PULAYOUT_MORESPACE && pLayoutInfo[i].State == PULAYOUT_BLOCKEDMIN )
				{
					TotalWeight += pLayoutInfo[i].Weight;
					TotalFreeSpace += pLayoutInfo[i].MinWidth;
					Size = ( pLayoutInfo[i].Weight * TotalFreeSpace ) / TotalWeight;

					if( Size > pLayoutInfo[i].MinWidth )
					{
						pLayoutInfo[i].State = PULAYOUT_FREE;
						break;
					}
					else
					{
						TotalWeight -= pLayoutInfo[i].Weight;
						TotalFreeSpace -= pLayoutInfo[i].MinWidth;
					}
				}

				if( LayoutState == PULAYOUT_LESSSPACE && pLayoutInfo[i].State == PULAYOUT_BLOCKEDMAX )
				{
					TotalWeight += pLayoutInfo[i].Weight;
					TotalFreeSpace += pLayoutInfo[i].MaxWidth;
					Size = ( pLayoutInfo[i].Weight * TotalFreeSpace ) / TotalWeight;

					if( Size < pLayoutInfo[i].MaxWidth )
					{
						pLayoutInfo[i].State = PULAYOUT_FREE;
						break;
					}
					else
					{
						TotalWeight -= pLayoutInfo[i].Weight;
						TotalFreeSpace -= pLayoutInfo[i].MaxWidth;
					}
				}
			}
		}
	} while( i < NumVisibleChild );

	/* Now iterate one last time through objects and setup their size and position */
	Pos = Rect.X;

	for( i = 0; i < NumVisibleChild; i++, Pos )
	{
		puSetAttribute( pLayoutInfo[i].Object, PUA_CONTROL_POSX, Pos );

		switch( pLayoutInfo[i].State )
		{
			case PULAYOUT_FREE:
				Size = ( pLayoutInfo[i].Weight * TotalFreeSpace ) / TotalWeight;
				break;

			case PULAYOUT_BLOCKEDMIN:
				Size = pLayoutInfo[i].MinWidth;
				break;

			case PULAYOUT_BLOCKEDMAX:
				Size = pLayoutInfo[i].MaxWidth;
				break;
		}

		puSetAttribute( pLayoutInfo[i].Object, PUA_CONTROL_WIDTH, Size );
		Pos += Size + _pData->Spacing;

		Size = Rect.Height;
		Pos2 = Rect.Y;

		if( Size > pLayoutInfo[i].MaxHeight )
		{
			Size = pLayoutInfo[i].MaxHeight;

			switch( _pData->CenterMode )
			{
				case PU_HGROUP_CENTER:
					Pos2 = Rect.Y + ( ( Rect.Height - Size ) >> 1 );
					break;

				case PU_HGROUP_TOP:
					Pos2 = Rect.Y;
					break;

				case PU_HGROUP_BOTTOM:
					Pos2 = Rect.Y + Rect.Height - Size;
					break;
			}
		}

		puSetAttribute( pLayoutInfo[i].Object, PUA_CONTROL_POSY, Pos2 );
		puSetAttribute( pLayoutInfo[i].Object, PUA_CONTROL_HEIGHT, Size );

		puDoMethod( pLayoutInfo[i].Object, PUM_CONTROL_LAYOUT, _Param1, _Param2 );
	}

	free( pLayoutInfo );
}

void puHorGroupRender( PULID _Object, PUU32 _RastPort )
{
	pusList* pChildList;
	pusNode* pCurrentNode;
	pusNode* pNextNode;
	pusRect Rect;
	pusRect Rect2;
	pusRect TmpRect;

	PUS32 LastPos;

	puDoMethod( _Object, PUM_CONTROL_GETINNERRECT, ( PUU32 )&Rect, 0 );

	LastPos = Rect.X;

	pChildList = ( pusList* )puDoMethod( _Object, PUM_GETCHILDLIST, 0, 0 );
	pCurrentNode = pChildList->pFirst;

	while( pNextNode = pCurrentNode->pNext )
	{
		if( !puGetAttribute( ( PULID )pCurrentNode, PUA_CONTROL_HIDDEN ) )
		{
			puDoMethod( ( PULID )pCurrentNode, PUM_CONTROL_GETOUTERRECT, ( PUU32 )&Rect2, 0 );

			if( Rect2.Y > Rect.Y )
			{
				TmpRect.Y = Rect.Y;
				TmpRect.Height = Rect2.Y - Rect.Y;
				TmpRect.X = Rect2.X;
				TmpRect.Width = Rect2.Width;

				puDoMethod( _Object, PUM_CONTROL_CLEARBG, _RastPort, ( PUU32 )&TmpRect );
			}

			if( ( Rect2.Y + Rect2.Height ) < ( Rect.Y + Rect.Height ) )
			{
				TmpRect.Y = Rect2.Y + Rect2.Height;
				TmpRect.Height = Rect.Y + Rect.Height - TmpRect.Y;
				TmpRect.X = Rect2.X;
				TmpRect.Width = Rect2.Width;

				puDoMethod( _Object, PUM_CONTROL_CLEARBG, _RastPort, ( PUU32 )&TmpRect );
			}

			if( Rect2.X > LastPos )
			{
				TmpRect.Y = Rect.Y;
				TmpRect.Height = Rect.Height;
				TmpRect.X = LastPos;
				TmpRect.Width = Rect2.X - LastPos;

				puDoMethod( _Object, PUM_CONTROL_CLEARBG, _RastPort, ( PUU32 )&TmpRect );
			}

			LastPos = Rect2.X + Rect2.Width;
		}

		pCurrentNode = pNextNode;
	}

	if( ( Rect.X + Rect.Width ) > LastPos )
	{
		TmpRect.Y = Rect.Y;
		TmpRect.Height = Rect.Height;
		TmpRect.X = LastPos;
		TmpRect.Width = Rect.X + Rect.Width - LastPos;

		puDoMethod( _Object, PUM_CONTROL_CLEARBG, _RastPort, ( PUU32 )&TmpRect );
	}

}


PUU32 puHorGroupSetAttr( PULID _Object, PULID _Class, void* _pData, PUU32 _Attr, PUU32 _Val )
{
	pusHorGroupClassData* pData;
	pData = ( pusHorGroupClassData* )_pData;

	switch( _Attr )
	{
		case PUA_HORGROUP_CENTERMODE:
			pData->CenterMode = _Val;
			break;

		case PUA_HORGROUP_SPACING:
			pData->Spacing = _Val;
			break;
	}

	return TRUE;
}