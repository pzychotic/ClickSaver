/* Poly
   Useful
   Lib */

#include <pul/pul.h>
#include <windows.h>
#include "../include/globals.h"
#include "win32.h"

extern int main( int argc, char** argv );

LRESULT CALLBACK puPopupWndProc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam );
LRESULT CALLBACK puWndProc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam );
void puShowWindow( PUU32 _Window );

HINSTANCE g_hInstance;
PUU8 g_Qualifiers = 0;

struct
{
	HWND	hWnd;
	PULID	WindowObj;
} g_WindowLookUpTable[ PUL_MAX_WINDOWS ];

/* Wrapper for that stupid, non portable winmain */
int APIENTRY WinMain( HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int lCmdShow )
{
	char*	argv[1];
	argv[0] = "args not yet implemented";

	g_hInstance = hInstance;

	return main( 1, argv );
}

PUU32 puSysInit()
{
	WNDCLASSEX wndclass;

	wndclass.cbSize = sizeof(WNDCLASSEX);
	wndclass.style = CS_CLASSDC|CS_DBLCLKS;
	wndclass.lpfnWndProc = puWndProc;
	wndclass.cbClsExtra = 0;
	wndclass.cbWndExtra = 0;
	wndclass.hInstance = 0; //hInstance;
	wndclass.hIcon = 0;
	wndclass.hCursor = LoadCursor(NULL, IDC_ARROW);
	wndclass.hbrBackground = 0;

	wndclass.lpszMenuName = NULL;
	wndclass.lpszClassName = "PUL";
	wndclass.hIconSm = 0;
	RegisterClassEx(&wndclass);

	wndclass.lpfnWndProc = puPopupWndProc;
	wndclass.lpszClassName = "PUL_popup";
	RegisterClassEx(&wndclass);

	memset( g_WindowLookUpTable, 0, PUL_MAX_WINDOWS * sizeof( g_WindowLookUpTable[ 0 ] ) );

	return TRUE;
}

PUU32 puCreateWindow( PULID _WindowObj, PUU32 _Parent, char* pTitle, pusRect* _pRect, PUU32 _bNoClose, PUU32 _bIconify, PUU32 _bForceTop, PUU32 _bPopup )
{
	HWND hWnd;
	int i;

	for( i = 0; i < PUL_MAX_WINDOWS; i++ )
	{
		if( !( g_WindowLookUpTable[i].hWnd ) )
			break;
	}

	if( i >= PUL_MAX_WINDOWS )
		return 0;

	hWnd = CreateWindowEx( 
		_Parent ? 0 : ( _bPopup ? WS_EX_TOOLWINDOW : WS_EX_APPWINDOW ),
		_bPopup ? "PUL_popup" : "PUL",
		pTitle,
	//	WS_CHILD,
		_Parent ? ( WS_CHILD | WS_VISIBLE  ) : ( ( _bNoClose ? 0 : WS_SYSMENU ) |
		( _bIconify ? WS_MINIMIZEBOX : 0) |
		( _bPopup ? WS_POPUP : 0 ) |
		WS_SIZEBOX ),
		
	//	0, 0, 50, 50,
	//	1105, 446,

		_pRect->X, _pRect->Y,
		_pRect->Width, _pRect->Height,
		( HWND )_Parent, NULL, 0/*g_hInstance*/, NULL);

	if( hWnd )
	{
		ShowWindow( hWnd, SW_HIDE );
		UpdateWindow( hWnd );

		if( _bForceTop )
			SetWindowPos( hWnd, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE|SWP_NOSIZE );
	}

	g_WindowLookUpTable[i].hWnd = hWnd;
	g_WindowLookUpTable[i].WindowObj = _WindowObj;

	return ( PUU32 )hWnd;
}

void puDestroyWindow( PUU32 _Window )
{
	int i;

	DestroyWindow( ( HWND )_Window );

	for( i = 0; i < PUL_MAX_WINDOWS; i++ )
	{
		if( g_WindowLookUpTable[i].hWnd == ( HWND )_Window )
			g_WindowLookUpTable[i].hWnd = 0;
	}
}

void puGetWindowRect( PUU32 _Window, pusRect* _pWindowRect )
{
	WINDOWPLACEMENT wpla;
//	RECT rect;

//	GetWindowRect( ( HWND )_Window, &rect );
	wpla.length = sizeof( WINDOWPLACEMENT );
	GetWindowPlacement( ( HWND )_Window, &wpla );

	_pWindowRect->X = wpla.rcNormalPosition.left;
	_pWindowRect->Y = wpla.rcNormalPosition.top;
	_pWindowRect->Width = wpla.rcNormalPosition.right - wpla.rcNormalPosition.left;
	_pWindowRect->Height = wpla.rcNormalPosition.bottom - wpla.rcNormalPosition.top;
}

void puGetWindowInnerRect( PUU32 _Window, pusRect* _pWindowRect )
{
	RECT rect;
	POINT point;

	GetClientRect( ( HWND )_Window, &rect );

	point.x = 0;
	point.y = 0;
	ClientToScreen( ( HWND )_Window, &point );

	_pWindowRect->X = point.x;
	_pWindowRect->Y = point.y;
	_pWindowRect->Width = rect.right;
	_pWindowRect->Height = rect.bottom;
}

void puGetScreenRect( pusRect* _pScreenRect )
{
	RECT rect;

	GetWindowRect( GetDesktopWindow(), &rect );

	_pScreenRect->X = 0;
	_pScreenRect->Y = 0;
	_pScreenRect->Width = rect.right - rect.left;
	_pScreenRect->Height = rect.bottom - rect.top;
}

void puSendMouseEvent( PUU32 _ButtonCode, HWND _hWnd, PULID _WindowObj, PUS32 _WheelDelta )
{
	pusMouseEvent MouseEvent;
	POINT MousePos;

	MouseEvent.ButtonCode = _ButtonCode;
	GetCursorPos( &MousePos );
	ScreenToClient( _hWnd, &MousePos );
	MouseEvent.MouseX = MousePos.x;
	MouseEvent.MouseY = MousePos.y;
	MouseEvent.MouseWheel = 0; //_WheelDelta / WHEEL_DELTA;

	puDoMethod( _WindowObj, PUM_WINDOW_MOUSEEVENT, ( PUU32 )&MouseEvent, 0 );
}

void puHandleRawKey( PULID _WindowObj, HWND _hWnd, WPARAM _wParam, LPARAM _lParam )
{
	pusKeyboardEvent KBEvent;
	POINT MousePos;

	switch( _wParam )
	{
		case VK_LEFT:
			KBEvent.Code = PUKEY_LEFT;
			break;

		case VK_RIGHT:
			KBEvent.Code = PUKEY_RIGHT;
			break;

		case VK_UP:
			KBEvent.Code = PUKEY_UP;
			break;

		case VK_DOWN:
			KBEvent.Code = PUKEY_DOWN;
			break;

		case VK_DELETE:
			KBEvent.Code = PUKEY_DEL;
			break;

		case VK_CONTROL:
			g_Qualifiers |= PUQUAL_CTRL;
			return;

		case VK_SHIFT:
			g_Qualifiers |= PUQUAL_SHIFT;
			return;

		default:
			return;
	}

	GetCursorPos( &MousePos );
	ScreenToClient( _hWnd, &MousePos );
	KBEvent.MouseX = MousePos.x;
	KBEvent.MouseY = MousePos.y;
	KBEvent.Qualifiers = g_Qualifiers;
	KBEvent.Type = PUKEYBOARD_RAWKEY;
	puDoMethod( _WindowObj, PUM_WINDOW_KEYBOARDEVENT, ( PUU32 )&KBEvent, 0 );
}

LRESULT CALLBACK puPopupWndProc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam )
{
	int i;
	PULID WindowObj = 0;

	switch( msg )
	{
		case WM_NCPAINT:
		case WM_NCCALCSIZE:

	//	case WM_SYNCPAINT:
	//	case WM_ERASEBKGND:
			return 0;

		case WM_NCACTIVATE:
			if( !wParam )
			{
				for( i = 0; i < PUL_MAX_WINDOWS; i++ )
				{
					if( g_WindowLookUpTable[i].hWnd == hWnd )
						WindowObj = g_WindowLookUpTable[i].WindowObj;
				}

				if( WindowObj )
				{
					puSetAttribute( WindowObj, PUA_WINDOW_CLOSE, TRUE );
				}
			}
			return puWndProc( hWnd, msg, wParam, lParam );

		default:
			return puWndProc( hWnd, msg, wParam, lParam );
	}
}

LRESULT CALLBACK puWndProc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam )
{
	int i;
	PULID WindowObj = 0;

	if( msg == WM_CHAR )
		i = 0;

	for( i = 0; i < PUL_MAX_WINDOWS; i++ )
	{
		if( g_WindowLookUpTable[i].hWnd == hWnd )
			WindowObj = g_WindowLookUpTable[i].WindowObj;
	}

	switch( msg )
	{
//	case WM_NCPAINT:
//	case WM_SYNCPAINT:
//	case WM_ERASEBKGND:



//	case WM_NCACTIVATE:
//		return 0;
//	case WM_NCACTIVATE:
//		return 1;
//case WM_NCHITTEST:
//	return HTCAPTION;
//		return WVR_ALIGNRIGHT;
//	case WM_NCCREATE:
//		return 1;

		case WM_NCCALCSIZE:
			DefWindowProc( hWnd, msg, wParam, lParam );

			if( wParam )
				return WVR_REDRAW;

			return 0;

		case WM_GETDLGCODE:
			return DLGC_WANTALLKEYS;

		case WM_SETFOCUS:
		case WM_KILLFOCUS:
			return 0;

		case WM_SYSCOMMAND:
			switch( wParam )
			{
				case SC_CLOSE:
					if( WindowObj )
						puSetAttribute( WindowObj, PUA_WINDOW_CLOSE, TRUE );
					break;

				case SC_RESTORE:
					puShowWindow( ( PUU32 )hWnd );

					// Fall through

				default:
					return( DefWindowProc( hWnd, msg, wParam, lParam ) );
			}
			break;

		case WM_PAINT:
			if( WindowObj )
			{
				PAINTSTRUCT Paint;
				RECT Rect;
				pusWin32RastPort RastPort;

				if( GetUpdateRect( hWnd, &Rect, FALSE ) )
				{
					BeginPaint( hWnd, &Paint );

					RastPort.hdc = Paint.hdc;
					puWin32InitRastPort( &RastPort );
					puDoMethod( WindowObj, PUM_WINDOW_RENDER, ( PUU32 )&RastPort, 0 );
					puWin32DeleteRastPort( &RastPort );

					EndPaint( hWnd, &Paint );
				}
			}
			return 0;

		case WM_GETMINMAXINFO:
			if( WindowObj )
			{
				MINMAXINFO* MinMax;
				RECT Rect1, Rect2;
				PUU32 MaxWidth, MaxHeight;
				MinMax = ( MINMAXINFO * )lParam;

				GetClientRect( hWnd, &Rect1 );
				GetWindowRect( hWnd, &Rect2 );

				if( !Rect1.right )
					return TRUE;

				// TODO: prelayout maybe should be called there, so that wanted height
				// is updated depending on the width - will have to check whether WM_SIZE
				// is called during resize when not using solid resize.
				if( WindowObj )
					puDoMethod( WindowObj, PUM_WINDOW_PRELAYOUT, TRUE, 0 );

				MinMax->ptMinTrackSize.x = Rect2.right - Rect2.left - Rect1.right;
				MinMax->ptMinTrackSize.y = Rect2.bottom - Rect2.top - Rect1.bottom;
				MinMax->ptMinTrackSize.x += puGetAttribute( WindowObj, PUA_CONTROL_MINWIDTH );

				MaxWidth = puGetAttribute( WindowObj, PUA_CONTROL_MAXWIDTH );
				MaxHeight = puGetAttribute( WindowObj, PUA_CONTROL_MAXHEIGHT );

				if( MaxWidth < 32768 )
					MinMax->ptMaxTrackSize.x = MaxWidth + Rect2.right - Rect2.left - Rect1.right;

				if( MaxHeight < 32768 )
					MinMax->ptMaxTrackSize.y = MaxHeight + Rect2.bottom - Rect2.top - Rect1.bottom;

			/*	if( MinHeight = puGetAttribute( WindowObj, PUA_CONTROL_WANTEDHEIGHT ) )
				{
					if( MinHeight > MaxHeight )
						MinMax->ptMinTrackSize.y = MinMax->ptMaxTrackSize.y;
					else
					{
						MinMax->ptMinTrackSize.y += MinHeight;
						MinMax->ptMaxTrackSize.y = MinMax->ptMinTrackSize.y;
					}
				}
				else*/
				MinMax->ptMinTrackSize.y += puGetAttribute( WindowObj, PUA_CONTROL_MINHEIGHT );
	
				MaxWidth = puGetAttribute( WindowObj, PUA_CONTROL_MAXWIDTH );
				MaxHeight = puGetAttribute( WindowObj, PUA_CONTROL_MAXHEIGHT );

			}
			return 1;

		case WM_SIZE:
			if( WindowObj )
			{
				RECT Rect;
				puDoMethod( WindowObj, PUM_WINDOW_PRELAYOUT, 0, 0 );

				GetClientRect( hWnd, &Rect );

				if( puGetAttribute( WindowObj, PUA_CONTROL_MINHEIGHT ) != ( PUU32 ) Rect.bottom )
					puCheckWindowSize( ( PUU32 )hWnd );

				puDoMethod( WindowObj, PUM_WINDOW_LAYOUT, 0, 0 );
			}

			InvalidateRect( hWnd, NULL, 1 );
			return 1;

		case WM_CHAR:
			{
				pusKeyboardEvent KBEvent;
				POINT MousePos;

				GetCursorPos( &MousePos );
				ScreenToClient( hWnd, &MousePos );
				KBEvent.MouseX = MousePos.x;
				KBEvent.MouseY = MousePos.y;
	
				KBEvent.Type = PUKEYBOARD_ASCII;
				KBEvent.Code = wParam;
				KBEvent.Qualifiers = g_Qualifiers;

				puDoMethod( WindowObj, PUM_WINDOW_KEYBOARDEVENT, ( PUU32 )&KBEvent, 0 );
			}
			break;

		case WM_KEYDOWN:
			puHandleRawKey( WindowObj, hWnd, wParam, lParam );
			break;

		case WM_KEYUP:			
			switch( wParam )
			{
				case VK_CONTROL:
					g_Qualifiers &= ~PUQUAL_CTRL;
					break;

				case VK_SHIFT:
					g_Qualifiers &= ~PUQUAL_SHIFT;
					break;
			}
			break;

		case WM_MOUSEMOVE:
			puSendMouseEvent( PUMOUSE_NONE, hWnd, WindowObj, 0 );
			break;

		case WM_LBUTTONDOWN:
			puSendMouseEvent( PUMOUSE_LMB, hWnd, WindowObj, 0 );
			break;

		case WM_LBUTTONUP:
			puSendMouseEvent( PUMOUSE_LMB | PU_KEYUP, hWnd, WindowObj, 0 );
			break;

		case WM_LBUTTONDBLCLK:
			puSendMouseEvent( PUMOUSE_LMB | PU_KEYDOUBLECLICK, hWnd, WindowObj, 0 );
			break;

		case WM_RBUTTONDOWN:
			puSendMouseEvent( PUMOUSE_RMB, hWnd, WindowObj, 0 );
			break;

		case WM_RBUTTONUP:
			puSendMouseEvent( PUMOUSE_RMB | PU_KEYUP, hWnd, WindowObj, 0 );
			break;

		case WM_RBUTTONDBLCLK:
			puSendMouseEvent( PUMOUSE_RMB | PU_KEYDOUBLECLICK, hWnd, WindowObj, 0 );
			break;

		/*case WM_MOUSEWHEEL:
			puSendMouseEvent( PUMOUSE_WHEEL, hWnd, WindowObj, GET_WHEEL_DELTA_WPARAM( wParam ) );
			break;*/

		default:
			return( DefWindowProc( hWnd, msg, wParam, lParam ) );
	}

	return 0;
}

void puWaitMessages()
{
	MSG msg;
	int RetCode;

	while( RetCode = GetMessage( &msg, NULL, 0, 0 ) )
	{
		if( RetCode != -1 )
		{
			TranslateMessage( &msg );
			DispatchMessage( &msg ); 
		}

		if( g_Globals.NumAppMessages )
			return;
	}
}

void puCheckMessages()
{
	MSG msg;

	while( PeekMessage( &msg, NULL,  0, 0, PM_REMOVE ) ) 
	{
		TranslateMessage( &msg );
		DispatchMessage( &msg );
	}
}

void puSendAppMessage( PUU32 _Message, PUU32 _Param )
{
	int i;

	puPostAppMessage( _Message, _Param );

	for( i = 0; i < PUL_MAX_WINDOWS; i++ )
	{
		if( g_WindowLookUpTable[i].hWnd )
		{
			PostMessage( g_WindowLookUpTable[i].hWnd, WM_APP, 0, 0 );
			return;
		}
	}
}

void puRedrawWindowRect( PUU32 _Window, pusRect* _pRect )
{
	if( _pRect == NULL )
	{
		InvalidateRect( ( HWND )_Window, NULL, 1 );
		UpdateWindow( ( HWND )_Window );
	}
	else
	{
		RECT Rect;
		Rect.left = _pRect->X;
		Rect.top = _pRect->Y;
		Rect.right = Rect.left + _pRect->Width;
		Rect.bottom = Rect.top + _pRect->Height;
		InvalidateRect( ( HWND )_Window, &Rect, 0 );
		UpdateWindow( ( HWND )_Window );
	}
}

/*void puSetCaptureCallBack( PUCAPTURECALLBACK* _pCallBack )
{

}
*/
void puSetCapture( PUU32 _Window )
{
	if( _Window )
		SetCapture( ( HWND )_Window );
	else
		ReleaseCapture();
}

void puCheckWindowSize( PUU32 _Window )
{
	RECT Rect;
	GetWindowRect( ( HWND )_Window, &Rect );
	MoveWindow( ( HWND )_Window, Rect.left, Rect.top, Rect.right - Rect.left, Rect.bottom - Rect.top, TRUE );
	InvalidateRect( ( HWND )_Window, NULL, 0 );
}

void puSetWindowPos( PUU32 _Window, PUU32 _XPos, PUU32 _YPos )
{
	RECT Rect;
	GetWindowRect( ( HWND )_Window, &Rect );
	MoveWindow( ( HWND )_Window, _XPos, _YPos, Rect.right - Rect.left, Rect.bottom - Rect.top, TRUE );
	InvalidateRect( ( HWND )_Window, NULL, 0 );
}

void puShowWindow( PUU32 _Window )
{
	RECT Rect;
	GetWindowRect( ( HWND )_Window, &Rect );

	if( GetParent( ( HWND )_Window ) )
		MoveWindow( ( HWND )_Window, 0, 0, Rect.right - Rect.left, Rect.bottom - Rect.top, TRUE );
	else
		MoveWindow( ( HWND )_Window, Rect.left, Rect.top, Rect.right - Rect.left, Rect.bottom - Rect.top, TRUE );

	InvalidateRect( ( HWND )_Window, NULL, 0 );
	ShowWindow( ( HWND )_Window, SW_SHOWNORMAL );
}

void puIconifyWindow( PUU32 _Window )
{
	ShowWindow( ( HWND )_Window, SW_MINIMIZE );
}

PUU8* puGetClipboardText()
{
	PUU8*	pResult = NULL;
	HGLOBAL hglb;
	LPTSTR	lptstr;

	if ( !IsClipboardFormatAvailable( CF_TEXT ) ) 
		return NULL;

	if( !OpenClipboard( NULL ) ) 
		return NULL; 
 
	if( hglb = GetClipboardData( CF_TEXT ) )
	{
		if( lptstr = GlobalLock(hglb) )
		{
			if( pResult = malloc( strlen( lptstr ) + 1 ) )
				strcpy( pResult, lptstr );

			GlobalUnlock( hglb );
		}
	}

	CloseClipboard(); 

	return pResult;
}

void puPutClipboardText( PUU8* _pText, PUU32 _TextSize )
{
	HGLOBAL hglbCopy;
	LPTSTR	lptstrCopy;
	PUU32	TextSize;

	if( _TextSize )
		TextSize = _TextSize;
	else
		TextSize = strlen( _pText );

	// Open the clipboard
	if( !OpenClipboard( NULL ) )
		return;

    EmptyClipboard(); 

	if( !( hglbCopy = GlobalAlloc( GMEM_MOVEABLE, TextSize + 1 ) ) )
	{
		CloseClipboard(); 
		return; 
	}
 
    // Lock the handle and copy the text to the buffer. 
    lptstrCopy = GlobalLock( hglbCopy ); 
    memcpy( lptstrCopy, _pText, TextSize ); 
    lptstrCopy[ TextSize ] = (TCHAR) 0;    // null character

    GlobalUnlock( hglbCopy ); 

    // Place the handle on the clipboard. 
    SetClipboardData( CF_TEXT, hglbCopy );

	CloseClipboard();
}