#include <stdlib.h> // malloc
#include <string.h> // strchr
#include <stdio.h>
#include <string.h> // strdup, strcmp
#include <ctype.h> // toupper

#define QUIT			1
#define FORWARD_PRINT	2
#define BACKWARD_PRINT	3
#define SEARCH			4
#define DELETE			5
#define COUNT			6

// User structure type definition
typedef struct
{
	char* name;
	char	sex;
	int		freq;
} tName;

////////////////////////////////////////////////////////////////////////////////
// LIST type definition
typedef struct node
{
	tName* dataPtr;
	struct node* llink;
	struct node* rlink;
} NODE;

typedef struct
{
	int		count;
	NODE* head;
	NODE* rear;
} LIST;

////////////////////////////////////////////////////////////////////////////////
// Prototype declarations

/* Allocates dynamic memory for a list head node and returns its address to caller
	return	head node pointer
			NULL if overflow
*/
LIST* createList(void);

/* Deletes all data in list and recycles memory
*/
void destroyList(LIST* pList);

/* Inserts data into list
	return	0 if overflow
			1 if successful
			2 if duplicated key
*/
int addNode(LIST* pList, tName* dataInPtr);

/* Removes data from list
	return	0 not found
			1 deleted
*/
int removeNode(LIST* pList, tName* keyPtr, tName** dataOutPtr);

/* interface to search function
	Argu	key being sought
	dataOut	contains found data
	return	1 successful
			0 not found
*/
int searchList(LIST* pList, tName* pArgu, tName** dataOutPtr);

/* returns number of nodes in list
*/
int countList(LIST* pList);

/* returns	1 empty
			0 list has data
*/
int emptyList(LIST* pList);

/* traverses data from list (forward)
*/
void traverseList(LIST* pList, void (*callback)(const tName*));

/* traverses data from list (backward)
*/
void traverseListR(LIST* pList, void (*callback)(const tName*));

/* internal insert function
	inserts data into a new node
	return	1 if successful
			0 if memory overflow
*/
static int _insert(LIST* pList, NODE* pPre, tName* dataInPtr);

/* internal delete function
	deletes data from a list and saves the (deleted) data to dataOut
*/
static void _delete(LIST* pList, NODE* pPre, NODE* pLoc, tName** dataOutPtr);

/* internal search function
	searches list and passes back address of node
	containing target and its logical predecessor
	return	1 found
			0 not found
*/
static int _search(LIST* pList, NODE** pPre, NODE** pLoc, tName* pArgu);

////////////////////////////////////////////////////////////////////////////////
/* Allocates dynamic memory for a name structure, initialize fields(name, freq) and returns its address to caller
	return	name structure pointer
			NULL if overflow
*/
tName* createName(char* str, char sex, int freq);

/* Deletes all data in name structure and recycles memory
*/
void destroyName(tName* pNode);

////////////////////////////////////////////////////////////////////////////////
/* gets user's input
*/
int get_action()
{
	char ch;
	scanf("%c", &ch);
	ch = toupper(ch);
	switch (ch)
	{
	case 'Q':
		return QUIT;
	case 'P':
		return FORWARD_PRINT;
	case 'B':
		return BACKWARD_PRINT;
	case 'S':
		return SEARCH;
	case 'D':
		return DELETE;
	case 'C':
		return COUNT;
	}
	return 0; // undefined action
}

// compares two names in name structures
// for createList function
int cmpName(const tName* pName1, const tName* pName2)
{
	int ret = strcmp(pName1->name, pName2->name);
	if (ret == 0) return pName1->sex - pName2->sex;
	else return ret;
}

// prints contents of name structure
// for traverseList and traverseListR functions
void print_name(const tName* dataPtr)
{
	printf("%s\t%c\t%d\n", dataPtr->name, dataPtr->sex, dataPtr->freq);
}

// increases freq in name structure
// for addNode function
void increase_freq(tName* dataOutPtr, const tName* dataInPtr)
{
	dataOutPtr->freq += dataInPtr->freq;
}

// splits name and sex from string
// ex) "Zoe/M"
void split_name_sex(char* str, char* sex)
{
	char* p;

	p = strchr(str, '/');
	if (p == NULL)
	{
		fprintf(stderr, "unexpected name/sex format\n");
		return;
	}
	else
	{
		*p++ = 0;
		*sex = *p;
	}
}

LIST* createList(void)
{
	LIST* pList = (LIST*)malloc(sizeof(LIST));

	pList->count = 0;
	pList->head = NULL;
	pList->rear = NULL;

	return pList;

}

void destroyList(LIST* pList)
{
	pList->count = 0;
	
	NODE* destroy_node = pList->head;
	//free(pList->head);
	
	while (destroy_node)
	{
		//NODE* temp = destroy_node;

		destroyName(destroy_node->dataPtr);
		//node도 free해줘야되는구나
		free(destroy_node->llink);
		//temp = destroy_node->rlink;
		//free(destroy_node);
		destroy_node = destroy_node->rlink;
		//free(temp);
	}

	free(pList->rear);
	//free(pList);
	//pList->count = 0;
	//free(pList->head);
	//free(pList->rear);
	free(pList);

}

int addNode(LIST* pList, tName* dataInPtr)
{
	//메모리 할당 안해줘도 되나?

	NODE* pPre=NULL;
	NODE* pLoc=NULL;
	int search_success;
	int insert_success;

	//if (!(pList->count))
	//{
	//	//NODE* first_node = (NODE*)malloc(sizeof(NODE));
	//	//if (!first_node)
	//		//return 0;

	//	first_node->dataPtr = dataInPtr;
	//	first_node->llink = NULL;
	//	first_node->rlink = NULL;
	//	pList->head = first_node;
	//	pList->rear = first_node;

	//	pList->count++;

	//	
	//	//first_node free
	//	//free(first_node);

	//	return 1;
	//}
	
	if (!(pList->count))
	{
		insert_success = _insert(pList, pPre, dataInPtr);
		//free(dataInPtr);
		if (!insert_success)
			return 0;
		else
			return 1;
	}

	
		

	search_success = _search(pList, &pPre, &pLoc, dataInPtr);

	if (search_success)
	{
		increase_freq(pPre->dataPtr, dataInPtr);
		//free(pPre);
		//free(pLoc);
		return 2;
	}
	else //새로운 키값이라 노드가 삽입되어야할때
	{
		insert_success = _insert(pList, pPre, dataInPtr);
		//free(pPre);
		//free(pLoc);
		
		//free(dataInPtr);
		//destroyName(dataInPtr);
		if (!insert_success)
		{
			//free(pPre);
			return 0;
		}

		return 1;
	}
	
}

int removeNode(LIST* pList, tName* keyPtr, tName** dataOutPtr)
{
	NODE* pPre;
	NODE* pLoc;
	int search_success;

	search_success = _search(pList, &pPre, &pLoc, keyPtr);

	if (search_success)
	{
		//*dataOutPtr = keyPtr;
		*dataOutPtr = pPre->dataPtr;
		//main함수에서 dataOutPtr이 가리키는 tName destroy

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

int searchList(LIST* pList, tName* pArgu, tName** dataOutPtr)
{
	NODE* pPre;
	NODE* pLoc;

	int search_success = _search(pList, &pPre, &pLoc, pArgu);

	if (search_success)
	{
		//*dataOutPtr = pArgu;
		*dataOutPtr = pPre->dataPtr;
		return 1;
	}
	else
		return 0;

	//해줘야 되나
	//free(pPre);
}

int countList(LIST* pList)
{
	return pList->count;
}


void traverseList(LIST* pList, void (*callback)(const tName*))
{
	NODE* traverse = pList->head;

	while (traverse)
	{
		//순서
		callback(traverse->dataPtr);
		traverse = traverse->rlink;
		
	}
}

void traverseListR(LIST* pList, void (*callback)(const tName*))
{
	NODE* traverse = pList->rear;

	while (traverse)
	{
		callback(traverse->dataPtr);
		traverse = traverse->llink;
		
	}
}

static int _insert(LIST* pList, NODE* pPre, tName* dataInPtr)
{
	NODE* pNew = (NODE*)malloc(sizeof(NODE));
	if (!pNew)
		return 0;
	
	//strcpy(pNew->dataPtr->name,dataInPtr->name);
	//*(pNew->dataPtr->sex) = *(dataInPtr->sex);
	//*(pNew->dataPtr->freq) = *(dataInPtr->freq);*/

	pNew->dataPtr = dataInPtr;
	//destroyName(dataInPtr);

	//첫 노드 삽입
	if(!(pList->count))
	{
		pNew->llink=NULL;
		pNew->rlink=NULL;
		pList->head=pNew;
		pList->rear=pNew;
		
		pList->count++;
		
		//free(pPre);
		//free(pLoc);
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
			//pNew free
			//free(pNew);
			//free(pPre);
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
			//pNew free
			//free(pNew);
			//free(pPre);
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
			//pNew free
			//free(pNew);
			//free(pPre);
			return 1;
		}
	}

	//insert를 한 후에 free해야 겠다
	//free(pPre);
	free(pNew);
}

static void _delete(LIST* pList, NODE* pPre, NODE* pLoc, tName** dataOutPtr)
{
	//*dataOutPtr = pPre->dataPtr;
	//NODE* temp;
	//temp = pPre;

	//첫번째 노드가 삭제될 때
	if (pPre==pList->head)
	{
		pList->head = pLoc;
		//pList->head = pPre->rlink;
		pLoc->llink = NULL;
		//pPre->rlink->llink = NULL;
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
		//pPre->llink->rlink = pLoc;
		//pPre->llink->rlink = pPre->rlink;
		//pLoc->llink = pPre->llink;
		//pPre->rlink->llink = pPre->llink;

		//free(pPre->llink);
		//free(pPre->rlink);
		pPre->llink->rlink = pLoc;
		pLoc->llink = pPre->llink;
		pList->count--;
	}
	
	//있어야 하네
	free(pPre);
	//free(pLoc);
	//이거 없으면 segmentation fault
	//zoe/f->zoe/M순서로 삭제하면 오류나고(전 노드의 메모리에 접근할 수 없어서)
	//zoe/m->zoe/f 순서로 삭제하면 오류가 안났음
}

static int _search(LIST* pList, NODE** pPre, NODE** pLoc, tName* pArgu)
{
	
	//NODE** ppPre=pPre;
	//NODE** ppLoc=pLoc;
	
	//리스트가 비어있다면 바로 나감
	if (!(pList->count))
	{
		//free(*pPre);
		//free(*pLoc);
		return 0;
	}

	NODE* search_key = pList->head;
	int cmp = cmpName(search_key->dataPtr, pArgu);
	//위치 실수 이거 반복해야됨
	//int cmp = cmpName(search_key->dataPtr,pArgu );

	while (1)
	{


		if (cmp < 0)
		{
			search_key = search_key->rlink;


			//마지막 노드까지 더 큰 값이 없을 때
			if (!search_key)
			{
				*pPre = pList->rear;



				//키값 노드 free
				//free(search_key);
				return 0;
			}


		}
		else if (cmp == 0)
		{
			*pPre = search_key;
			//*pLoc=NULL;
			*pLoc = search_key->rlink;

			//키값 노드 free
			// 메모리 할당도 안했는데 free하면 어카냐!
			//free(search_key);
			return 1;
		}
		else //해당 노드의 값이 pArgu보다 클 때
		{
			*pPre = search_key->llink;

			//첫번째 노드가 더 커서 맨 앞 노드에 삽입되어야 할 때
			if (!pPre)
			{
				//head 위치 자체
				//pPre = &(pList->head);
				//head가 가리키고 있던 첫번째 노드
				*pLoc = pList->head;

				//키값 노드 free
				//free(search_key);
				return 0;
			}
			else
			{
				*pLoc = search_key;

				//키값 노드 free
				//free(search_key);
				return 0;
			}
		}

		cmp = cmpName(search_key->dataPtr, pArgu);
	}
}


tName* createName(char* str, char sex, int freq)
{
	tName* pname = (tName*)malloc(sizeof(tName));
	/*char* pstr = (char*)malloc(1024 * sizeof(str));
	strcpy(pstr, str);
	pname->name = pstr;*/
	pname->name= (char*)malloc(1024 * sizeof(str));
	strcpy(pname->name, str);
	pname->sex = sex;
	pname->freq = freq;

	//pstr free
	//free(pstr);

	//pname은 main에서 free
	return pname;
}

void destroyName(tName* pNode)
{
	//이거 꼭 해줘야 하나?
	free(pNode->name);
	//pNode->sex = { NULL };
	//pNode->sex = '\n';
	//pNode->sex=NULL;
	pNode->freq = 0;

	free(pNode);
}

////////////////////////////////////////////////////////////////////////////////
int main(int argc, char** argv)
{
	LIST* list;

	char str[1024];
	//char name[100];
	char sex;
	int freq;

	tName* pName;
	int ret;
	FILE* fp;

	if (argc != 2) {
		fprintf(stderr, "usage: %s FILE\n", argv[0]);
		return 1;
	}

	fp = fopen(argv[1], "rt");
	if (!fp)
	{
		fprintf(stderr, "Error: cannot open file [%s]\n", argv[1]);
		return 2;
	}

	// creates an empty list
	list = createList();
	if (!list)
	{
		printf("Cannot create list\n");
		return 100;
	}

	while (fscanf(fp, "%*d\t%s\t%c\t%d", str, &sex, &freq) != EOF)
	{
		pName = createName(str, sex, freq);

		ret = addNode(list, pName);

		if (ret == 2) // duplicated
		{
			destroyName(pName);
		}
	}

	fclose(fp);

	fprintf(stderr, "Select Q)uit, P)rint, B)ackward print, S)earch, D)elete, C)ount: ");

	while (1)
	{
		tName* p;
		int action = get_action();

		switch (action)
		{
		case QUIT:
			destroyList(list);
			return 0;

		case FORWARD_PRINT:
			traverseList(list, print_name);
			break;

		case BACKWARD_PRINT:
			traverseListR(list, print_name);
			break;

		case SEARCH:
			fprintf(stderr, "Input a name/sex to find: ");
			fscanf(stdin, "%s", str);
			split_name_sex(str, &sex);

			pName = createName(str, sex, 0);

			if (searchList(list, pName, &p)) print_name(p);
			else fprintf(stdout, "%s not found\n", str);

			destroyName(pName);
			break;

		case DELETE:
			fprintf(stderr, "Input a name/sex to delete: ");
			fscanf(stdin, "%s", str);
			split_name_sex(str, &sex);

			pName = createName(str, sex, 0);

			if (removeNode(list, pName, &p))
			{
				fprintf(stdout, "(%s, %c, %d) deleted\n", p->name, p->sex, p->freq);
				destroyName(p);
			}
			else fprintf(stdout, "%s not found\n", str);

			destroyName(pName);
			break;

		case COUNT:
			fprintf(stdout, "%d\n", countList(list));
			break;
		}

		if (action) fprintf(stderr, "Select Q)uit, P)rint, B)ackward print, S)earch, D)elete, C)ount: ");
	}
	return 0;
}
