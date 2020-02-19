////////////////////////////////////////////////////////////////////////
// COMP2521 assignment 1: Textbuffer ADT
//
// DATE: 28/12/2018 
// NAME: OREN LEVY 
// STUDENT ID: z3466301

#include "textbuffer.h"
//#include "test_textbuffer.c"

#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>

#define TRUE 1
#define FALSE -1

// Declare struct textbuffernode
typedef struct textbuffernode {
  char   *line;			 	 // Char array of a string
  struct textbuffernode *prev; // Prev is a pointer to a node in a diff type struct textbuffer
  struct textbuffernode *next; // Next is a pointer to a node in a diff type struct textbuffer
}TBN; 				 // Whenever you say "textBuffer" you mean "struct textbuffer".

// Declare struct textbuffer
typedef struct textbuffer {
  struct textbuffernode *head; 		  // Pointer to first node in textbuffer
  struct textbuffernode *tail;  		  // Pointer to last node in textbuffer
}TB;

//Function Prototypes
void test_function(Textbuffer tb);
TBN *getPosition(Textbuffer tb, size_t pos);
void free_node(Textbuffer tb, TBN *start);
Textbuffer facsimile (Textbuffer tb, size_t from, size_t to);
int findMatch(Textbuffer tb, char *match, bool rev);
static int isValid(Textbuffer tb1, Textbuffer tb2, size_t pos, size_t numLines);
static int isValid2(Textbuffer tb1, size_t from, size_t to);
static void rectifyPointers(Textbuffer tb);
static void merge(Textbuffer tb1, size_t pos, Textbuffer tb2, size_t numLines);

// Create a new textbuffernode
static TBN *CreateNode(char *text) {
  TBN *new;   			   // New is a pointer to type struct textbuffer
  new = malloc(sizeof(TBN)); // Allocate enough memory for size of struct textbuffer node
  assert(new != NULL); 	   // Confirm enough memory space exists
  new->line = strdup(text);  // Returns a pointer to a null-terminated byte string, which is a duplicate of the string pointed to by new.
  new->prev = new->next = NULL; // Set next and prev to point to eachother and NULL
  return new; 			   // Return pointer to new textbuffer
}

// Create a new textbuffer
static TB *CreateTextBuffer() {
  TB *newTB;          // Pointer to type struct textbuffer
  newTB = malloc(sizeof(TB)); // Allocate enough memory for size of struct textbuffer
  assert (newTB != NULL); // Confirm enough memory space exists
  newTB->head = NULL;
  newTB->tail = NULL;
  return newTB;
}

static void addNode(TB *newTB, TBN *newNode) {
  if (newTB->tail == NULL) { // If tail == NULL, first node to be added
    newTB->head = newTB->tail = newNode; // Set head and tail to point to first node
  }
  else {
    newTB->tail->next = newNode; // Append additional node to end of list
    newNode->prev = newTB->tail; // Set Prev to point to the last node (tail) of the textbuffer
    newTB->tail = newNode;		 // Set tail to point to the last node (lattest node added)
  }
}

Textbuffer textbuffer_new(const char *text) {
  TB *newTB = CreateTextBuffer();

  if(strcmp(text, "") == 0) { // Check if string is empty
    return newTB; // Return empty textbuffer
  }

  TBN *newNode;
  char *subString; // newNode is a pointer to an uninitialised type struct textbuffernode
  char *copyText = strdup(text);  // return a pointer to a piece of memory with enough space to support the text
  while(strcmp(copyText, "")) { // Continue while the string is not empty
    subString = strsep(&copyText,"...."); // String sep returns a pointer to the delim searched for (second arguemnt), allowing us to copy text up until new line and add to new node.
    newNode = CreateNode(subString); // Create a new node with the substring pulled from the text
    addNode(newTB, newNode); // Add the newly created node to the linked list
  }
  
  test_function(newTB);
  return newTB;
}

void test_function(Textbuffer tb) {
    TBN *curr = tb->head;
    while(curr != NULL) {
        printf("%s ", curr->line);
        curr = curr->next;
    }

}

void textbuffer_drop(Textbuffer tb) {

  if(tb == NULL) { // If TB = NULL Abort
    printf("TEXT BUFFER = NULL\n");
    abort();
  }

  TBN *curr, *prev;
  curr = tb->head;
  while (curr != NULL) { // Traverse the list freeing each node until we reach the end
    prev = curr;
    curr = curr->next; // Move to next node so we dont lose pointer when we free prev / access a freed piece of memory
    free(prev->line); // free the memory claimed by the text using strdup
    free(prev); // free the memory calimed by the node using malloc
  }
  free(tb); // free the memory calimed by the textbuffer using malloc
  return;
}

void free_node(Textbuffer tb, TBN *start) {
  TBN *node = start;
  if(tb == NULL){
    return;
  } else if(node->prev == NULL && node->next == NULL) { // One node in list. Free The node but do not free the textbuffer
        tb->head = NULL;
        tb->tail = NULL;
        free(node->line);
        free(node);
  } else if(node->prev == NULL) { // Deletion begins from first node in list
        node->next->prev = NULL; // Set the prev pointer, of next node pointed to by first, to point to NULL, as it is now the head of the list
        tb->head = node->next; // Set the next node pointed to by first to be the head of the list
        free(node->line);
        free(node);
  } else if(node->next == NULL) { // Deletion begins from last node in list
        node->prev->next = NULL; // Set the next pointer, of the node behind tail, to point to NULL, as it is now the last (tail) node of the list
        tb->tail = node->prev; // Set the prev node pointed to by last to be the tail of the list
        free(node->line);
        free(node);
  } else {                        // Deletion begins in middle of the list
        node->next->prev = node->prev; // set the prev pointer of the next node pointed to by current to point to the prev node of curr
        node->prev->next = node->next; // set the next pointer of the prev node pointed to by current to point to the next node of curr
        free(node->line);
        free(node);
  }
}

size_t textbuffer_lines (Textbuffer tb) {
  size_t lineCount = 0; // Line count starts at 0 to better suit "from" and "to" 0 indexing in specific functions

  if(tb == NULL){
    return 0;
  }
  // Go through the text buffer incrementing count until you hit NULL
  TBN *curr = tb->head;
  while (curr != NULL) {
    lineCount++;
    curr = curr->next;
  }
  return lineCount;
}

size_t textbuffer_bytes (Textbuffer tb) { // Return number of bytes in TB per line
  size_t byteCount = 0;

  if(tb == NULL){
    return 0;
  }

  TBN *curr = tb->head;
  while (curr != NULL) {
    byteCount = byteCount + strlen(curr->line); // add the byte count of each line
    curr = curr->next;
  }
  return byteCount + textbuffer_lines(tb);
}

char *textbuffer_to_str (Textbuffer tb) {
  if(tb->head == NULL) {
    return NULL;
  }

  TBN *curr = tb->head;
  char *tbString = NULL;

  if(strcmp(curr->line, "") == 0) { // Check if string is empty
    return curr->line; // Return empty string
  }

  while(curr != NULL) {
    if(tbString == NULL) { // First node
      char *s = malloc(strlen(curr->line)+2); // Initialise a new string with enough memory for current line + newline character + cushioning
      tbString = s; // Let tbString point to the newly created string
      strcpy(tbString,curr->line); // Copy the current line into the string.
      strcat(tbString,"\n"); // Add the newline character to the end of the string
    } else { // Node after first
      char *s = realloc(tbString, strlen(tbString) + strlen(curr->line)+2); // Find a new piece of memory large enough to hold old string and new string and move them there
      assert(s != NULL); // Make sure there is enough free memory
      tbString = s; // Reorient tbString pointer to point to new piece of memory
      strcat(tbString, curr->line); // Append new string to old string
      strcat(tbString,"\n"); // Add newLine character to mark the position of old and new string start and end
    }
    curr = curr->next;
  }
  return tbString;
}

// Valid check if 2 tb are involved
static int isValid(Textbuffer tb1, Textbuffer tb2, size_t pos, size_t numLines) {
  size_t zero = 0;
  if(pos < zero || numLines < pos) { // If pos is negative or greater than number of nodes, out of range, abort.
    fprintf(stderr, "Not a valid position: Out of range\n");
    abort();
  }
  if(tb1->head == NULL || tb2->head == NULL) { // If either tb1 or tb2 is empty, there is no action to execture, return original tb
    printf("Empty list, nothing to merge\n");
    return FALSE;
  }
  return TRUE;
}
// Valid check if only 1 tb is involved
static int isValid2(Textbuffer tb1, size_t from, size_t to) {

  if(tb1 == NULL) {
    return FALSE;
  }

  size_t numLines = textbuffer_lines(tb1);
  size_t zero = 0;
  numLines = numLines-1;
  if(numLines < from || numLines < to || from < zero) { // If the starting position is greater than the number of nodes or the end position is greater than the number of nodes, our of range
    return FALSE;
    //fprintf(stderr, "Not a valid position: Out of range\n");
    //abort();
  }

  return TRUE;
}
// Go through the list and make each nodes prev point to the node before it
// to rectify the prev pointer connections. Mainly used for TB_SWAP
static void rectifyPointers(Textbuffer tb){
  TBN *curr = tb->head;
  TBN *currPrev = NULL;
  while(curr->next != NULL) {
    currPrev = curr;
    curr = curr->next;
    curr->prev = currPrev;
  }
  return;
}

void textbuffer_swap (Textbuffer tb, size_t pos1, size_t pos2) {

  int valid = isValid2(tb, pos1, pos2);
  if(valid == FALSE || pos1 == pos2) {
    return;
  }

  TBN *p1 = getPosition(tb, pos1);
  TBN *p1Prev = p1->prev;
  TBN *p2 = getPosition(tb, pos2);
  TBN *p2Prev = p2->prev;

  // Next few lines of structual logic was inspired by GeeksforGeeks
  // If p1 is not head of linked list
  if (p1Prev != NULL)
  p1Prev->next = p2;
  else // If p1 is head of linked list, make p2 new head
  tb->head = p2;
  // If p2 is not head of linked list
  if (p2Prev != NULL)
  p2Prev->next = p1;
  else  // If p2 is head of linked list, make p2 new head
  tb->head = p1;
  // Swap next and prev pointers
  TBN *temp1 = p2->next;

  p2->next = p1->next;
  p2->prev = p1->prev;

  if(p2->next != NULL) {
    p2->next->prev = p2;
  }

  p1->next = temp1;
  p1->prev = p2Prev;
  if(temp1 != NULL) {
    temp1->prev = p1;
  }

  if(p1->next == NULL) {
    tb->tail = p1;
  }
  rectifyPointers(tb);
}

static void merge(Textbuffer tb1, size_t pos, Textbuffer tb2, size_t numLines) {

  if(pos == numLines) {   // Insert at end of tb1
    tb1->tail->next = tb2->head; // The last node of tb1 points to the first node of tb2
    tb2->head->prev = tb1->tail; // The prev pointer of the first node in tb2 points to the last node of tb1
    tb1->tail = tb2->tail; // Update tail to be the end of tb2
  } else if(pos == 0) {     // Insert at start of tb1
    tb1->head->prev = tb2->tail; // The prev pointer of first node in tb1 points to the last node in tb2
    tb2->tail->next = tb1->head; // The next pointer of the last node in tb2 points to the first node in tb1
    tb1->head = tb2->head; // Update the head to be first node in tb2
  } else { // Insert in the middle of the list
    TBN *p = getPosition(tb1, pos);
    p->prev->next = tb2->head; // Prev pointer of current node points to first node of tb2
    tb2->head->prev = p->prev; // Prev pointer of tb2 first node points to prev of current node
    tb2->tail->next = p; // Next pointer of last node of tb2 points to the current node
    p->prev = tb2->tail; // Prev pointer of curent node points to last node of tb2
  }

  return;
}

TBN *getPosition(Textbuffer tb, size_t pos) {
  TBN *curr = tb->head;
  TBN *p = NULL;
  size_t i = 0;

  while(curr != NULL) { // Find the position of the node to be manipulated
    if(i == pos) { // Use i as a counter to traverse the list until it reaches the desired position, updating current at each iteration so we can return a pointer to the appropriate node
      p = curr;
      break; // When we find the node equal to the position we are searching for, stop looking and break out of loop
    }
    i++;
    curr = curr->next;
  }
  return p; // Return a pointer to the node in the linked list equal to the position of the list given by a function with from or to
}

void textbuffer_insert (Textbuffer tb1, size_t pos, Textbuffer tb2) {
  size_t numLines = textbuffer_lines(tb1); // Get the number of lines in the tb by counting the number of nodes
  int valid = isValid(tb1,tb2, pos, numLines); // Check if position is in range and tb1 is not NULL
  if(valid == FALSE) {
    return;
  }
  merge(tb1, pos, tb2, numLines);
}

void textbuffer_paste (Textbuffer tb1, size_t pos, Textbuffer tb2) {
  size_t numLines = textbuffer_lines(tb1);
  int valid = isValid(tb1,tb2, pos, numLines); // Check position is in range
  if(valid == FALSE) {
    return;
  }

  TBN *curr = tb2->head;
  while(curr != NULL) {
    TBN *newNode = CreateNode(curr->line); // Create a new node with the string from tb2 and append to the end of tb1
    addNode(tb1, newNode); // Append the new node with string from tb2 to the end of tb1
    curr = curr->next;
  }

  // After all the nodes from tb2 have been added to the end of tb1, begin to rearrange
  // the nodes to the position chosen given.

  // If the position is equal to the number of lines, return,
  // as the new nodes from tb2 are initially added to the end of tb1
  if(pos == numLines) {
    return;
  } else if(pos == 0){ // Rearrange the nodes added from tb2 to the begining of tb1
    TBN *p = getPosition(tb1, numLines-1);
    tb1->tail->next = tb1->head; // The last node of tb1 was the last node to have been added from tb2. Thus make last node point to head of tb1 to head of tb1, so we can connect the first node of tb1 with the last node of tb2
    tb1->head->prev = tb1->tail; // The prev pointer of the first node in tb1 points to the last node of tb2
    tb1->tail = p; // Update tail
    tb1->head = p->next;
    p->next->prev = NULL;
    p->next = NULL;
  } else { // tb2 nodes position is somewhere in the middle of tb1
    TBN *p1 = getPosition(tb1, pos);
    TBN *p2 = getPosition(tb1, numLines);
    TBN *temp = p2->prev;

    p1->prev->next = p2;
    p2->prev = p1->prev;
    tb1->tail->next = p1;
    p1->prev = tb1->tail;
    temp->next = NULL;
    tb1->tail = temp;
  }

  return;
}

Textbuffer textbuffer_cut (Textbuffer tb, size_t from, size_t to) {
  int valid = isValid2(tb, from, to);
  if(valid == FALSE) {
    return tb;
  }

  // Cut function calls a function to create and return a new tb
  // with the nodes needed starting at "from" and ending at "to"
  // Then the function deletes those same nodes from the original
  // text buffer.

  Textbuffer newTb = facsimile (tb, from, to); // Make a new tb that copies all nodes starting at position "from"
  textbuffer_delete (tb, from, to); // delete the nodes out of the original tb at positions "from" and "to"

  return newTb;
}

Textbuffer facsimile (Textbuffer tb, size_t from, size_t to) {
  Textbuffer newTb = CreateTextBuffer();
  TBN *curr = getPosition(tb, from);
  size_t i = from;

  // Begin added new nodes to a tb as normal, return a pointer to this tb
  // as the tb for cut function to reutrn.
  while(i <= to) {
    TBN *newNode = CreateNode(curr->line);
    addNode(newTb, newNode);
    i++;
    curr = curr->next;
  }
  return newTb;
}

Textbuffer textbuffer_copy (Textbuffer tb, size_t from, size_t to){
  int valid = isValid2(tb, from, to);
  if(valid == FALSE) {
    return tb;
  }

  // Call the facsimile function to create a new tb copy without
  // modifying the original tb
  Textbuffer new_tb_copy = facsimile (tb, from, to);
  return new_tb_copy;
}

void textbuffer_delete (Textbuffer tb, size_t from, size_t to) {
  int valid = isValid2(tb, from, to);
  if(valid == FALSE) {
    return;
  }

// This function remove lines `from` through `to` inclusive from textbuffer `tb`.

  TBN *curr = getPosition(tb, from);
  size_t start = from;
  TBN *prev;

  while(start <= to) {
    prev = curr;
    curr = curr->next;
    free_node(tb, prev);
    start++;
  }
  return;
}

int findMatch(Textbuffer tb, char *match, bool rev) {
  int lineNum = 1;
  TBN *curr;
  if(rev == TRUE) { // Begin at end of tb
    curr = tb->tail;
  } else {
    curr = tb->head; // Begin at start of tb
  }

  // Search through tb looking for a match.
  // Return line number (node position in list) if we find a match
  // Otherwise return FALSE as no match was found
  while(curr != NULL) {
    if(strcmp(match, curr->line) == 0) {
      return lineNum;
    }
    lineNum++;
    if(rev == TRUE) { // Traverse backwards until the starting node
      curr = curr->prev;
    } else {
      curr = curr->next; // Traverse forwards if reverse is false
    }
  }

  return FALSE;
}

ssize_t textbuffer_search (Textbuffer tb, char *match, bool rev) {
  if(tb == NULL || match == NULL) {
    fprintf(stderr, "Hello from textbuffer_search function: String or TB passed is NULL\n");
    abort();
  }

  if(strcmp(match, "") == 0) { // Check if string is empty
    return FALSE;
  }

  // Call to a function to find whether a match exists or not
  int hit = findMatch(tb, match, rev);

  return hit;
}

void textbuffer_replace (Textbuffer tb, char *match, char *replace)
{
  char *subString;
  size_t i, count = 0;
  size_t replace_len = strlen(replace);
  size_t match_len = strlen(match);
  
  // Next few lines of structual logic was inspired by GeeksforGeeks
  TBN *curr = tb->head;
  while(curr != NULL) {
    char *string_copy = strdup(curr->line);
    for (i = 0; string_copy[i] != '\0'; i++) // Keep searching until the end of the array
    {
      // Continue to increment count so long as substring exists
      if (strstr(&string_copy[i], match) == &string_copy[i]) //
      {
        count++;
        i += match_len - 1; // I will equal the length of the string
      }
    }
    // Reserve enough memory so that the old string can support the substring
    subString = (char *)malloc(i + count * (replace_len - match_len) + 2);
    i = 0;
    while (*string_copy) // While not NULL
    {
      // compare the substring with the result
      if (strstr(string_copy, match) == string_copy)
      {
        // Copy the replacement string into the correct index of the new char array
        strcpy(&subString[i], replace);
        i += replace_len;
        string_copy += match_len;
      }
      // Continue to increment until we find the correct position to insert
      // the replacement string
      else
      subString[i++] = *string_copy++;
    }

    subString[i] = '\0';
    free(curr->line); // Free the memory pointed to by the old string
    curr->line = subString; // Let the modified string be pointed to by curr
    curr = curr->next; // Continue proccess through each node in the list
  }
  return;
}

