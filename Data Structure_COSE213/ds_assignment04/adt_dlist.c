#include <stdlib.h> // malloc

#include "adt_dlist.h"

/* internal insert function
	inserts data into a new node
	return	1 if successful
			0 if memory overflow
*/
static int _insert( LIST *pList, NODE *pPre, void *dataInPtr);

/* internal delete function
	deletes data from a list and saves the (deleted) data to dataOut
*/
static void _delete( LIST *pList, NODE *pPre, NODE *pLoc, void **dataOutPtr);

/* internal search function
	searches list and passes back address of node
	containing target and its logical predecessor
	return	1 found
			0 not found
*/
static int _search( LIST *pList, NODE **pPre, NODE **pLoc, void *pArgu);


static int _insert(LIST* pList, NODE* pPre, void* dataInPtr)
{
	NODE* pNew = (NODE*)malloc(sizeof(NODE));
	if (!pNew)
		return 0;

	pNew->dataPtr = dataInPtr;

	//첫 노드 삽입
	if (!(pList->count))
	{
		pNew->llink = NULL;
		pNew->rlink = NULL;
		pList->head = pNew;
		pList->rear = pNew;

		pList->count++;

		return 1;
	}
	else
	{
		//삽입되는 노드가 맨 앞
		if (pPre == NULL)
		{
			pNew->llink = NULL;
			pNew->rlink = pList->head;
			pList->head->llink = pNew;

			pList->head = pNew;

			pList->count++;

			return 1;
		}

		//삽입되는 노드가 맨 뒤
		else if (pPre == pList->rear)
		{
			pNew->rlink = NULL;
			pNew->llink = pPre;
			pPre->rlink = pNew;

			pList->rear = pNew;

			pList->count++;

			return 1;
		}

		//중간에 삽입될 때
		else
		{
			pNew->llink = pPre;
			pNew->rlink = pPre->rlink;
			pPre->rlink->llink = pNew;
			pPre->rlink = pNew;

			pList->count++;
	
			return 1;
		}
	}

	free(pNew);

}


static void _delete(LIST* pList, NODE* pPre, NODE* pLoc, void** dataOutPtr)
{
	//*dataOutPtr = pPre->dataPtr;
	//첫번째 노드가 삭제될 때
	if (pPre == pList->head)
	{
		pList->head = pLoc;
		pLoc->llink = NULL;
		pList->count--;
	}
	//마지막 노드가 삭제될 때
	else if (pPre == pList->rear)
	{
		pList->rear = pPre->llink;
		pPre->llink->rlink = NULL;
		pList->count--;

	}
	//중간 노드가 삭제될 때
	else
	{
		pPre->llink->rlink = pLoc;
		pLoc->llink = pPre->llink;
		pList->count--;
	}

	//pPre노드를 free해줌
	free(pPre);
}

static int _search(LIST* pList, NODE** pPre, NODE** pLoc, void* pArgu)
{
	//search 성공->pPre가 가리키는 노드가 pArgu와 같은 값이 있는 노드
	//search 실패->pPre는 들어갈 위치의 앞 노드, pLoc은 들어갈 위치의 뒤 노드를 가리키는 포인터 
	//리스트가 비어있다면 바로 나감
	if (!(pList->count))
	{
		return 0;
	}

	NODE* search_key = pList->head;
	int cmp = (pList->compare)(search_key->dataPtr, pArgu);

	while (1)
	{
		if (cmp < 0)
		{
			search_key = search_key->rlink;

			//마지막 노드까지 더 큰 값이 없을 때
			if (!search_key)
			{
				*pPre = pList->rear;
				return 0;
			}
		}
		//같은 값 발견
		else if (cmp == 0)
		{
			*pPre = search_key;
			*pLoc = search_key->rlink;

			return 1;
		}
		else //해당 노드의 값이 pArgu보다 클 때
		{
			*pPre = search_key->llink;

			//첫번째 노드가 더 커서 맨 앞 노드에 삽입되어야 할 때
			if (!pPre)
			{
				//head가 가리키고 있던 첫번째 노드
				*pLoc = pList->head;

				return 0;
			}
			else //노드가 중간에 삽입됨
			{
				*pLoc = search_key;
				return 0;
			}
		}

		cmp = (pList->compare)(search_key->dataPtr,pArgu);
	}
}

LIST* createList(int (*compare)(const void*, const void*))
{
	LIST* pList = (LIST*)malloc(sizeof(LIST));

	pList->count = 0;
	pList->head = NULL;
	pList->rear = NULL;
	pList->compare = compare;

	return pList;
}

void destroyList(LIST* pList, void (*callback)(void*))
{
	pList->count = 0;

	NODE* destroy_node = pList->head;

	while (destroy_node)
	{

		callback(destroy_node->dataPtr);

		free(destroy_node->llink);
	
		destroy_node = destroy_node->rlink;

	}

	free(pList->rear);
	//free(pList->compare);
	free(pList);
}

int addNode(LIST* pList, void* dataInPtr, void (*callback)(const void*, const void*))
{
	
	NODE* pPre = NULL;
	NODE* pLoc = NULL;
	int search_success;
	int insert_success;

	//첫번째 이름 추가
	if (!(pList->count))
	{
		insert_success = _insert(pList, pPre, dataInPtr);
		if (!insert_success)
			return 0;
		else
			return 1;
	}

	search_success = _search(pList, &pPre, &pLoc, dataInPtr);

	if (search_success)
	{
		callback(pPre->dataPtr, dataInPtr);
		
		return 2;

	}
	else //새로운 키값이라 노드가 삽입되어야할때
	{
		insert_success = _insert(pList, pPre, dataInPtr);
		if (!insert_success)
		{
			return 0;
		}

			return 1;
		}
}

int removeNode(LIST* pList, void* keyPtr, void** dataOutPtr)
{
	NODE* pPre;
	NODE* pLoc;
	int search_success;

	search_success = _search(pList, &pPre, &pLoc, keyPtr);

	if (search_success)
	{
		//이중포인터 캐스팅 이렇게 맞나
		//*(tName*)dataOutPtr = pPre->dataPtr;
		*dataOutPtr = pPre->dataPtr;
		
		_delete(pList, pPre, pLoc, dataOutPtr);
		return 1;

	}
	else
	{
		//이것도 아니네
		//free(pPre);
		return 0;
	}
}

int searchList(LIST* pList, void* pArgu, void** dataOutPtr)
{
	NODE* pPre;
	NODE* pLoc;

	int search_success = _search(pList, &pPre, &pLoc, pArgu);

	if (search_success)
	{
		//*(tName*)dataOutPtr = pPre->dataPtr;
		*dataOutPtr = pPre->dataPtr;
		return 1;
	}
	else
		return 0;
}

int countList(LIST* pList)
{
	return pList->count;
}


void traverseList(LIST* pList, void (*callback)(const void*))
{
	NODE* traverse = pList->head;

	while (traverse)
	{
		//생각해보니까 역참조 할 때만 캐스팅 해줘도 되지 않나?
		callback(traverse->dataPtr);
		traverse = traverse->rlink;

	}
}

void traverseListR(LIST* pList, void (*callback)(const void*))
{
	NODE* traverse = pList->rear;

	while (traverse)
	{
		callback(traverse->dataPtr);
		traverse = traverse->llink;

	}
}

