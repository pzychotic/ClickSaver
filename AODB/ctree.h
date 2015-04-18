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
#ifndef _CTREE_INC
#define _CTREE_INC

typedef char TEXT, *pTEXT;
typedef unsigned char UTEXT, *pUTEXT;
typedef short COUNT, *pCOUNT;
typedef unsigned short UCOUNT, *pUCOUNT;
typedef long LONG, *pLONG;
typedef unsigned long ULONG, *pULONG;
typedef void *pVOID;
typedef ULONG VRLEN, *pVRLEN;

typedef struct iseg {
    COUNT soffset; /* segment position (offset) */
    COUNT slength; /* segment length */
    COUNT segmode; /* segment mode */
} ISEG, *pISEG;

typedef struct iidx {
    COUNT ikeylen; /* key length */
    COUNT ikeytyp; /* key type */
    COUNT ikeydup; /* duplicate flag */
    COUNT inulkey; /* NULL key flag */
    COUNT iempchr; /* empty character */
    COUNT inumseg; /* number of segments */
    pISEG seg; /* segment information */
    pTEXT ridxnam; /* r-tree symbolic name */
    pTEXT aidxnam; /* optional index file name */
    pCOUNT altseq; /* optional alternate sequence */
    pUTEXT pvbyte; /* optional pointer to pad byte */
} IIDX, *pIIDX;

typedef struct ifil {
    pTEXT pfilnam; /* file name (w/o ext) */
    COUNT dfilno; /* data file number */
    UCOUNT dreclen; /* data record length */
    UCOUNT dxtdsiz; /* data file ext size */
    COUNT dfilmod; /* data file mode */
    COUNT dnumidx; /* number of indices */
    UCOUNT ixtdsiz; /* index file ext size */
    COUNT ifilmod; /* index file mode */
    pIIDX ix; /* index information */
    pTEXT rfstfld; /* r-tree 1st fld name */
    pTEXT rlstfld; /* r-tree last fld name */
    COUNT tfilno; /* temporary file number */
} IFIL, *pIFIL;

#ifdef _CTREE_CPP
#define DECLARE
#else
#define DECLARE extern
#endif

DECLARE COUNT (__cdecl * InitISAM)(COUNT bufs, COUNT fils, COUNT sect);
DECLARE COUNT (__cdecl * CloseISAM)();
DECLARE COUNT (__cdecl * OpenIFile)(pIFIL ifilptr);
DECLARE COUNT (__cdecl * CloseIFile)(pIFIL ifilptr);
DECLARE COUNT (__cdecl * GetRecord)(COUNT keyno, pVOID target, pVOID recptr);
DECLARE COUNT (__cdecl * GetVRecord)(COUNT keyno, pVOID target, pVOID recptr, pVRLEN plen);
DECLARE COUNT (__cdecl * VRecordLength)(COUNT datno);
DECLARE COUNT (__cdecl * ReReadVRecord)(COUNT datno, pVOID recptr, VRLEN bufsiz);

DECLARE COUNT (__cdecl * FirstKey)(COUNT keyno, pVOID idxval);
DECLARE LONG (__cdecl * NextKey)(COUNT keyno, pVOID idxval);
DECLARE COUNT (__cdecl * GetGTEKey)(COUNT keyno, pVOID target, pVOID idxval);
DECLARE COUNT (__cdecl * NextVRecord)(COUNT keyno, pVOID recptr, pVRLEN plen);
DECLARE COUNT (__cdecl * GetGTEVRecord)(COUNT keyno, pVOID Target, pVOID recptr, pVRLEN plen);

DECLARE COUNT (__cdecl * ReWriteVRecord)(COUNT datno, pVOID recptr, VRLEN bufsiz);



DECLARE int CTreeStd_LinkDll(char *DllPath);
DECLARE void CTreeStd_UnlinkDll();

#endif /* _CTREE_INC */
