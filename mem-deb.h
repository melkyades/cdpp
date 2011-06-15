/**************************************************************************
 * AUTHOR:	Daniel A. Rodriguez
 * DATE:	25/4/1999 (v2)
 ***************************************************************************/

#ifndef MEM_DEBUG_H
#define	MEM_DEBUG_H

#include <malloc.h>
#include <iostream.h>

inline void memory(int tabs = 0)
{
	char	s[10];
	
	strcpy(s, "");

	if (tabs == 1)
		strcpy(s, "\t");

	struct mallinfo	mi;
	mi = mallinfo();

	cout << "\n";
	cout << s << "Bytes Total = " << mi.arena << "\n";
	cout << s << "Total Allocated Space = " << mi.uordblks << "\n";
	cout << s << "Total UnAllocated Space = " << mi.fordblks << "\n";
	cout << "\n";
}

#endif