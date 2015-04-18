/* Poly
   Useful
   Lib */

#ifndef __PUL_LIST_H__
#define __PUL_LIST_H__

#include <pul/types.h>

typedef struct _pusNode pusNode;

struct _pusNode
{
	pusNode* pNext;
	pusNode* pPrev;
};

typedef struct
{
	pusNode* pFirst;
	pusNode* pDummy;
	pusNode* pLast;
} pusList;

// Initialize a list
static __inline void puInitList( pusList* _pList )
{
	_pList->pDummy = NULL;
	_pList->pFirst = ( pusNode* )&_pList->pDummy;
	_pList->pLast = ( pusNode* )_pList;
}

// Insert a node at the end of a list
static __inline void puAddTail( pusList* _pList, pusNode* _pNode )
{
	_pNode->pPrev = _pList->pLast;
	_pNode->pNext = _pList->pLast->pNext;
	_pList->pLast->pNext = _pNode;
	_pList->pLast = _pNode;
}

// Insert a node at the begin of a list
static __inline void puAddHead( pusList* _pList, pusNode* _pNode )
{
	_pNode->pNext = _pList->pFirst;
	_pNode->pPrev = _pList->pFirst->pPrev;
	_pList->pFirst->pPrev = _pNode;
	_pList->pFirst = _pNode;
}

// Insert a node after another node
static __inline void puInsertNode( pusNode* _pNode, pusNode* _pPrev )
{
	_pNode->pPrev = _pPrev;
	_pNode->pNext = _pPrev->pNext;
	_pPrev->pNext = _pNode;
	_pNode->pNext->pPrev = _pNode;
}

// Remove a node from a list
static __inline void puRemoveNode( pusNode* _pNode )
{
	_pNode->pPrev->pNext = _pNode->pNext;
	_pNode->pNext->pPrev = _pNode->pPrev;
}

#endif