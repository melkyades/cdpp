/****************************************************************************
 * FILE:        MYLIST.H                                                    *
 *                                                                          *
 * AUTHOR:      Daniel A. Rodriguez                                         *
 * 		  Alejandro Troccoli (v2)
 * DATE:        4/6/1999                                                    *
 *		 12/02/2001 (v2)
 ****************************************************************************/

#if !defined(MY_LIST)
#define MY_LIST

#include <stdlib.h>
#include "real.h"
#include "cellpos.h"

typedef struct lNode
{
	CellPosition	element;
	unsigned long	cellIndex;
	lNode		*next;
	
} lNode;


class mList
{
public:
	mList()
	{ firstNode = lastNode = NULL; }
	
	~mList()
	{ this->clear(); }
	
	void clear();
	void add(CellPosition &elem, unsigned long val);
	
	void initCursor()
	{ cursor = firstNode; }
	
	bool endCursor()
	{ return (cursor == NULL)?true:false; }
	
	void next()
	{ cursor = cursor->next; }
	
	void modifyAtCursor(CellPosition &elem, unsigned long val)
	{ cursor->element = elem;
		cursor->cellIndex = val; }
	
	CellPosition elementCell()	// Returns the element pointed by the cursor
	{ return cursor->element; }			
	
	unsigned long elementValue()	// Returns the element pointed by the cursor
	{ return cursor->cellIndex; }			
	
	lNode *exists(const CellPosition &elem);
					// Devuelve un puntero al elemento
					// encontrado si existe. Sino
					// devuelve NULL
	
	void setValue(CellPosition &elem, unsigned long v);
	
	void setValue(const CellPosition &elem, unsigned long v);
	
private:
	lNode	*firstNode;
	lNode	*lastNode;
	lNode	*cursor;
};

inline void mList::clear()
{
	lNode	*auxNode;
	
	while (firstNode != NULL)
	{
		auxNode = firstNode->next;
		delete firstNode;
		
		firstNode = auxNode;
	}
	lastNode = NULL;
	cursor = lastNode;
}

inline void mList::add(CellPosition &elem, unsigned long v)
{
	lNode	*auxNode = new lNode;
	
	auxNode->element = elem;
	auxNode->cellIndex = v;
	auxNode->next = NULL;
	
	if (lastNode == NULL)
	{
		lastNode = auxNode;		// Inserto al principio
		firstNode = auxNode;		// total no hay nada
	}
	else
	{
		lastNode->next = auxNode;	// Relaciono el NEXT
		lastNode = auxNode;		// Hago que el elem sea el ultimo
	}
}

inline lNode *mList::exists(const CellPosition &elem)
{
	lNode	*auxNode = firstNode;
	
	while (auxNode != NULL)
	{
		if (auxNode->element == elem)
			return auxNode;
		
		auxNode = auxNode->next;
	}
	return NULL;
}

inline void mList::setValue(CellPosition &elem, unsigned long v)
{
	lNode	*aux;
	
	if ((aux = this->exists(elem)) != NULL)
	{
		aux->element = elem;
		aux->cellIndex = v;
	}
	else
		add(elem,v);
}

inline void mList::setValue(const CellPosition &elem, unsigned long v)
{
	lNode	*aux;
	
	if ((aux = this->exists(elem)) != NULL)
	{
		aux->element = elem;	// Como existe entonces modifico
		aux->cellIndex = v;
	}
	else
		add(* ((CellPosition *) &elem), v);// Si no existe agrego
}


#endif	// MY_LIST
