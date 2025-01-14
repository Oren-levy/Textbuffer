////////////////////////////////////////////////////////////////////////
// COMP2521 19T0 ... assignment 1: Textbuffer ADT submission tests

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "textbuffer.h"

int main (void)
{
	//textbuffer_insert (tb1, 14, tb2);
#define TEST_1 "line1\nline2\nline3\nline4\nline5\n"
#define TEST_2	"line6\nline7\nline8\nline9\nline10\n"

	Textbuffer tb1 = textbuffer_new (TEST_PASTE_1);
	assert (tb1 != NULL);

	Textbuffer tb2 = textbuffer_new (TEST_PASTE_2);
	assert (tb2 != NULL);

    textbuffer_paste (tb1, 0, tb2);

	textbuffer_drop (tb1);
	////////////////////////////////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////	
	tb1 = textbuffer_new (TEST_PASTE_1);
	assert (tb1 != NULL);

	tb2 = textbuffer_new (TEST_PASTE_2);
	assert (tb2 != NULL);

    textbuffer_paste (tb1, 5, tb2);

	textbuffer_drop (tb1);
	////////////////////////////////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////
	tb1 = textbuffer_new (TEST_PASTE_1);
	assert (tb1 != NULL);

	tb2 = textbuffer_new (TEST_PASTE_2);
	assert (tb2 != NULL);

    textbuffer_paste (tb1, 1, tb2);

	textbuffer_drop (tb1);
	////////////////////////////////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////	
	tb1 = textbuffer_new (TEST_PASTE_1);
	assert (tb1 != NULL);

	tb2 = textbuffer_new (TEST_PASTE_2);
	assert (tb2 != NULL);

    textbuffer_paste (tb1, 2, tb2);

	textbuffer_drop (tb1);
	////////////////////////////////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////	
	tb1 = textbuffer_new (TEST_PASTE_1);
	assert (tb1 != NULL);

	tb2 = textbuffer_new (TEST_PASTE_2);
	assert (tb2 != NULL);

    textbuffer_paste (tb1, 3, tb2);

	textbuffer_drop (tb1);
	////////////////////////////////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////
	tb1 = textbuffer_new (TEST_PASTE_1);
	assert (tb1 != NULL);

	tb2 = textbuffer_new (TEST_PASTE_2);
	assert (tb2 != NULL);

    textbuffer_paste (tb1, 4, tb2);

	textbuffer_drop (tb1);
	

	return EXIT_SUCCESS;
}


