/*
ClickSaver -  Anarchy Online mission helper
Copyright (C) 2002 Morb

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
*/
#ifndef _AODB_INC
#define _AODB_INC


/* Global Constants */
#define AODB_ERR_NOERROR    0
#define AODB_ERR_NODLL      1
#define AODB_ERR_NOINIT     2
#define AODB_ERR_NODB       3
#define AODB_TYP_ITEM       0xf4254
#define AODB_TYP_TEXTURE    0xf6954
#define AODB_TYP_ICON       0xf6958

#define AODB_TYP_PF         0xF4241

/* Functions */
long InitAODatabase( char* strAOFolder );
void ReleaseAODatabase( void );


#endif /* _AODB_INC */
