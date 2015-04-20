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
#include <windows.h>
#include "AODB.h"
#include "ctree.h"


/* Variables */
char strAODBpath[MAX_PATH];
IFIL udtAODBfile;
IIDX udtAODBindex;
ISEG udtAODBseg[2];


/* Initialise AO Database */
long InitAODatabase( char* strAOFolder )
{
    COUNT iErrVal;

    /* Build full path/file */
    wsprintf( strAODBpath, "%s%s", strAOFolder, "\\cd_image\\data\\db\\ResourceDatabase" );

    /* Link the Ctree DLL */
    if( !CTreeStd_LinkDll( "ctreestd.dll" ) )
    {
        iErrVal = AODB_ERR_NODLL;
        goto InitAODatabase_Err_nodll;
    }

    /* Initialise Ctree */
    if( InitISAM( 6, 4, 32 ) )
    {
        iErrVal = AODB_ERR_NOINIT;
        goto InitAODatabase_Err_noinit;
    }

    /* Build file details */
    udtAODBfile.pfilnam = strAODBpath;
    udtAODBfile.dfilno = -1;
    udtAODBfile.dreclen = 8;
    udtAODBfile.dxtdsiz = 4096;
    udtAODBfile.dfilmod = 6;
    udtAODBfile.dnumidx = 1;
    udtAODBfile.ixtdsiz = 4096;
    udtAODBfile.ifilmod = 2;
    udtAODBfile.ix = &udtAODBindex;
    udtAODBfile.rfstfld = "Type";
    udtAODBfile.rlstfld = "Blob";
    udtAODBfile.tfilno = 0;
    udtAODBindex.ikeylen = 8;
    udtAODBindex.ikeytyp = 0;
    udtAODBindex.ikeydup = 0;
    udtAODBindex.inulkey = 0;
    udtAODBindex.iempchr = 0;
    udtAODBindex.inumseg = 2;
    udtAODBindex.seg = udtAODBseg;
    udtAODBindex.ridxnam = "ItemIdx";
    udtAODBindex.aidxnam = 0;
    udtAODBindex.altseq = 0;
    udtAODBindex.pvbyte = 0;
    udtAODBseg[ 0 ].soffset = 0;
    udtAODBseg[ 0 ].slength = 4;
    udtAODBseg[ 0 ].segmode = 1;
    udtAODBseg[ 1 ].soffset = 4;
    udtAODBseg[ 1 ].slength = 4;
    udtAODBseg[ 1 ].segmode = 1;

    /* Open the AO database */
    if( OpenIFile( &udtAODBfile ) )
    {
        iErrVal = AODB_ERR_NODB;
        goto InitAODatabase_Err_nodb;
    }

    return AODB_ERR_NOERROR;

InitAODatabase_Err_nodb:
    CloseISAM();
InitAODatabase_Err_noinit:
    CTreeStd_UnlinkDll();
InitAODatabase_Err_nodll:
    return iErrVal;
}


/* Release the AO Database */
void ReleaseAODatabase( void )
{
    CloseIFile( &udtAODBfile );
    CloseISAM();
    CTreeStd_UnlinkDll();
}
