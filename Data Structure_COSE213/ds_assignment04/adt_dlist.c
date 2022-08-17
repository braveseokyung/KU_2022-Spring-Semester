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

	//ù ��� ����
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
		//���ԵǴ� ��尡 �� ��
		if (pPre == NULL)
		{
			pNew->llink = NULL;
			pNew->rlink = pList->head;
			pList->head->llink = pNew;

			pList->head = pNew;

			pList->count++;

			return 1;
		}

		//���ԵǴ� ��尡 �� ��
		else if (pPre == pList->rear)
		{
			pNew->rlink = NULL;
			pNew->llink = pPre;
			pPre->rlink = pNew;

			pList->rear = pNew;

			pList->count++;

			return 1;
		}

		//�߰��� ���Ե� ��
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
	//ù��° ��尡 ������ ��
	if (pPre == pList->head)
	{
		pList->head = pLoc;
		pLoc->llink = NULL;
		pList->count--;
	}
	//������ ��尡 ������ ��
	else if (pPre == pList->rear)
	{
		pList->rear = pPre->llink;
		pPre->llink->rlink = NULL;
		pList->count--;

	}
	//�߰� ��尡 ������ ��
	else
	{
		pPre->llink->rlink = pLoc;
		pLoc->llink = pPre->llink;
		pList->count--;
	}

	//pPre��带 free����
	free(pPre);
}

static int _search(LIST* pList, NODE** pPre, NODE** pLoc, void* pArgu)
{
	//search ����->pPre�� ����Ű�� ��尡 pArgu�� ���� ���� �ִ� ���
	//search ����->pPre�� �� ��ġ�� �� ���, pLoc�� �� ��ġ�� �� ��带 ����Ű�� ������ 
	//����Ʈ�� ����ִٸ� �ٷ� ����
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

			//������ ������ �� ū ���� ���� ��
			if (!search_key)
			{
				*pPre = pList->rear;
				return 0;
			}
		}
		//���� �� �߰�
		else if (cmp == 0)
		{
			*pPre = search_key;
			*pLoc = search_key->rlink;

			return 1;
		}
		else //�ش� ����� ���� pArgu���� Ŭ ��
		{
			*pPre = search_key->llink;

			//ù��° ��尡 �� Ŀ�� �� �� ��忡 ���ԵǾ�� �� ��
			if (!pPre)
			{
				//head�� ����Ű�� �ִ� ù��° ���
				*pLoc = pList->head;

				return 0;
			}
			else //��尡 �߰��� ���Ե�
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

	//ù��° �̸� �߰�
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
	else //���ο� Ű���̶� ��尡 ���ԵǾ���Ҷ�
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
		//���������� ĳ���� �̷��� �³�
		//*(tName*)dataOutPtr = pPre->dataPtr;
		*dataOutPtr = pPre->dataPtr;
		
		_delete(pList, pPre, pLoc, dataOutPtr);
		return 1;

	}
	else
	{
		//�̰͵� �ƴϳ�
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
		//�����غ��ϱ� ������ �� ���� ĳ���� ���൵ ���� �ʳ�?
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

