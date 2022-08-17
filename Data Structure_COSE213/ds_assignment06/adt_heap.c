#include <stdio.h>
#include <stdlib.h> // malloc, realloc

#include "adt_heap.h"

/* Reestablishes heap by moving data in child up to correct location heap array
*/
static void _reheapUp(HEAP* heap, int index)
{
	int child = index;
	int parent;
	void* temp;
	
	if (child)
	{
		parent = (child - 1) / 2;
		
		if ((heap->compare)((heap->heapArr)[child], (heap->heapArr)[parent])>0)
		{
			
			temp = heap->heapArr[parent];
			heap->heapArr[parent] = heap->heapArr[child];
			heap->heapArr[child] = temp;
			
			_reheapUp(heap, parent);
		}		

	}

}


/* Reestablishes heap by moving data in root down to its correct location in the heap
*/
static void _reheapDown(HEAP* heap, int index)
{
	int root, left_child, right_child,larger_child;
	root = index;
	left_child = root * 2 + 1;
	right_child = root * 2 + 2;
	
	if (right_child < heap->last)
	{
		//왼쪽 자식이 오른쪽 자식보다 크면
		if (heap->compare((heap->heapArr)[left_child], (heap->heapArr)[right_child])>0)
		{
			larger_child = left_child;
		}
		else
			larger_child = right_child;

		//root가 자식보다 작으면
		if (heap->compare((heap->heapArr)[root], (heap->heapArr)[larger_child])<0)
		{
			void* temp = heap->heapArr[root];
			(heap->heapArr)[root] = (heap->heapArr)[larger_child];
			(heap->heapArr)[larger_child] = temp;

			_reheapDown(heap, larger_child);
		}

	}

}

/* Allocates memory for heap and returns address of heap head structure
if memory overflow, NULL returned
*/
HEAP* heap_Create(int capacity, int (*compare) (void* arg1, void* arg2))
{
	HEAP* adt_Heap;
	adt_Heap = (HEAP*)malloc(sizeof(HEAP));

	adt_Heap->last = -1;
	adt_Heap->capacity = capacity;
	adt_Heap->compare = compare;

	adt_Heap->heapArr = malloc(sizeof(void*)*(adt_Heap->capacity));

	return adt_Heap;
}

/* Free memory for heap
*/
void heap_Destroy(HEAP* heap)
{
	for (int i = 0; i <= heap->last; i++)
	{
		free(heap->heapArr[i]);
	}
	free(heap->heapArr);
	free(heap);
}

/* Inserts data into heap
return 1 if successful; 0 if heap full
*/
int heap_Insert(HEAP* heap, void* dataPtr)
{
	
	heap->last++;

	if ((heap->last) == (heap->capacity))
	{
		(heap->capacity) *= 2;

		heap->heapArr = realloc(heap->heapArr, (heap->capacity) * sizeof(void*));

		if (!heap->heapArr)
			return 0;
	}

	heap->heapArr[heap->last] = dataPtr;
	_reheapUp(heap, heap->last);
	

	return 1;

}

/* Deletes root of heap and passes data back to caller
return 1 if successful; 0 if heap empty
*/
int heap_Delete(HEAP* heap, void** dataOutPtr)
{

	if (heap_Empty(heap))
		return 0;

	*dataOutPtr = heap->heapArr[0];
	heap->heapArr[0] = heap->heapArr[heap->last];
	heap->heapArr[heap->last] = *dataOutPtr;

	_reheapDown(heap, 0);
	(heap->last)--;

	return 1;

}

/*
return 1 if the heap is empty; 0 if not
*/
int heap_Empty(HEAP* heap)
{
	if (heap->last == -1)
		return 1;

	return 0;
}

/* Print heap array */
void heap_Print(HEAP* heap, void (*print_func) (void* data))
{
	for (int i = 0; i <= heap->last; i++)
	{
		print_func(heap->heapArr[i]);
		
	}
	printf("\n");
}

