/* Poly
   Useful
   Lib */

#ifndef __PUL_H__
#define __PUL_H__

#ifdef __cplusplus
extern "C"
{
#endif

#include <pul/types.h>
#include <pul/list.h>
#include <pul/log.h>
#include <pul/oo.h>
#include <pul/classes.h>
#include <pul/collection.h>
#include <pul/notification.h>
#include <pul/macros.h>

PUU32 puInit();
void puClear();

/* Frame types */
enum
{
	PUFRAME_NONE,
	PUFRAME_BUTTON,
	PUFRAME_BOX,
	PUFRAME_HSLIDER,
	PUFRAME_VSLIDER,
	PUFRAME_TEXTENTRY,
	PUFRAME_READONLYTEXTENTRY,
	PUFRAME_GROUP,
	PUFRAME_TITLE,
	PUFRAME_SMALLBUTTON,
	PUFRAME_INFOBOX
};

/* Font types */
enum
{
	PUFONT_NORMAL,
	PUFONT_FIXED
};

/* Mouse event */
typedef struct
{
	PUS32 MouseX, MouseY, MouseWheel;
	PUU32 ButtonCode;
} pusMouseEvent;

/* Mouse buttons codes */
enum
{
	PUMOUSE_NONE,
	PUMOUSE_LMB,
	PUMOUSE_RMB,
	PUMOUSE_MMB,
	PUMOUSE_WHEEL
};

/* Keyboard event */
typedef struct
{
	PUU32	Type;
	PUS32	MouseX, MouseY;
	PUU8	Code;
	PUU8	Qualifiers;
} pusKeyboardEvent;

/* Keyboard events type */
enum
{
	PUKEYBOARD_ASCII,
	PUKEYBOARD_RAWKEY
};

/* Keyboard key codes */
enum
{
	PUKEY_LEFT,
	PUKEY_RIGHT,
	PUKEY_UP,
	PUKEY_DOWN,
	PUKEY_DEL
};

/* Qualifiers */
#define PUQUAL_CTRL		1
#define PUQUAL_SHIFT	2

#define PU_KEYUP			0x80
#define PU_KEYDOUBLECLICK	0x40

/* Control states */
enum
{
	PUSTATE_NORMAL,
	PUSTATE_DISABLED,
	PUSTATE_HIGHLIGHTED,
	PUSTATE_ACTIVE
};

/* Application messages */
typedef struct
{
	PUU32	Message;
	PUU32	Param;
} pusAppMessage;

/* Application message callback */
typedef PUU32 ( * PUAPPMESSCALLBACK )( PUU32 _Message, PUU32 _Param );

PUU32			puPostAppMessage( PUU32 _Message, PUU32 _Param );
void			puSendAppMessage( PUU32 _Message, PUU32 _Param );
pusAppMessage*	puGetAppMessage();
pusAppMessage*	puWaitAppMessages();
void			puCheckMessages();
void			puSetAppMessCallBack( PUAPPMESSCALLBACK _pAppMessCallBack );

#ifdef __cplusplus
};
#endif

#endif