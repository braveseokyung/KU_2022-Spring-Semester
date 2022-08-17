#define SHOW_STEP 1 // 제출시 0
#define BALANCING 1 // 제출시 1

#include <stdlib.h>
#include <stdio.h>
#include <string.h> //strcmp, strdup

#define max(x, y)	(((x) > (y)) ? (x) : (y))

////////////////////////////////////////////////////////////////////////////////
// AVL_TREE type definition
typedef struct node
{
	char		*data;
	struct node	*left;
	struct node	*right;
	int			height;
} NODE;

typedef struct
{
	NODE	*root;
	int		count;  // number of nodes
} AVL_TREE;

////////////////////////////////////////////////////////////////////////////////
// Prototype declarations

/* Allocates dynamic memory for a AVL_TREE head node and returns its address to caller
	return	head node pointer
			NULL if overflow
*/
AVL_TREE *AVL_Create( void);

/* Deletes all data in tree and recycles memory
*/
void AVL_Destroy( AVL_TREE *pTree);
static void _destroy( NODE *root);

/* Inserts new data into the tree
	return	1 success
			0 overflow
*/
int AVL_Insert( AVL_TREE *pTree, char *data);

/* internal function
	This function uses recursion to insert the new data into a leaf node
	return	pointer to new root
*/
static NODE *_insert( NODE *root, NODE *newPtr);

static NODE *_makeNode( char *data);

/* Retrieve tree for the node containing the requested key
	return	address of data of the node containing the key
			NULL not found
*/
char *AVL_Retrieve( AVL_TREE *pTree, char *key);

/* internal function
	Retrieve node containing the requested key
	return	address of the node containing the key
			NULL not found
*/
static NODE *_retrieve( NODE *root, char *key);

/* Prints tree using inorder traversal
*/
void AVL_Traverse( AVL_TREE *pTree);
static void _traverse( NODE *root);

/* Prints tree using inorder right-to-left traversal
*/
void printTree( AVL_TREE *pTree);
/* internal traversal function
*/
static void _infix_print( NODE *root, int level);

/* internal function
	return	height of the (sub)tree from the node (root)
*/
static int getHeight( NODE *root);

/* internal function
	Exchanges pointers to rotate the tree to the right
	updates heights of the nodes
	return	new root
*/
static NODE *rotateRight( NODE *root);

/* internal function
	Exchanges pointers to rotate the tree to the left
	updates heights of the nodes
	return	new root
*/
static NODE *rotateLeft( NODE *root);

////////////////////////////////////////////////////////////////////////////////
int main( int argc, char **argv)
{
	AVL_TREE *tree;
	char str[1024];
	
	if (argc != 2)
	{
		fprintf( stderr, "Usage: %s FILE\n", argv[0]);
		return 0;
	}
	
	// creates a null tree
	tree = AVL_Create();
	
	if (!tree)
	{
		fprintf( stderr, "Cannot create tree!\n");
		return 100;
	}

	FILE *fp = fopen( argv[1], "rt");
	if (fp == NULL)
	{
		fprintf( stderr, "Cannot open file! [%s]\n", argv[1]);
		return 200;
	}

	while(fscanf( fp, "%s", str) != EOF)
	{

#if SHOW_STEP
		fprintf( stdout, "Insert %s>\n", str);
#endif		
		// insert function call
		AVL_Insert( tree, str);

#if SHOW_STEP
		fprintf( stdout, "Tree representation:\n");
		printTree( tree);
#endif
	}
	
	fclose( fp);
	
#if SHOW_STEP
	fprintf( stdout, "\n");

	// inorder traversal
	fprintf( stdout, "Inorder traversal: ");
	AVL_Traverse( tree);
	fprintf( stdout, "\n");

	// print tree with right-to-left infix traversal
	fprintf( stdout, "Tree representation:\n");
	printTree(tree);
#endif
	fprintf( stdout, "Height of tree: %d\n", tree->root->height);
	fprintf( stdout, "# of nodes: %d\n", tree->count);
	
	// retrieval
	char *key;
	fprintf( stdout, "Query: ");
	while( fscanf( stdin, "%s", str) != EOF)
	{
		key = AVL_Retrieve( tree, str);
		
		if (key) fprintf( stdout, "%s found!\n", key);
		else fprintf( stdout, "%s NOT found!\n", str);
		
		fprintf( stdout, "Query: ");
	}
	
	// destroy tree
	AVL_Destroy( tree);

	return 0;
}

/* Allocates dynamic memory for a AVL_TREE head node and returns its address to caller
	return	head node pointer
			NULL if overflow
*/
AVL_TREE *AVL_Create( void)
{
	AVL_TREE* head=(AVL_TREE*)malloc(sizeof(AVL_TREE));
	head->root=NULL;
	head->count=0;

	return head;
}

/* Deletes all data in tree and recycles memory
*/
void AVL_Destroy( AVL_TREE *pTree)
{
	destroy(pTree->root);
	free(pTree);
}

static void _destroy( NODE *root)
{
	if (root)
	{
		if (root->left)
		{
			_destroy(root->left);
		}
		if (root->right)
		{
			_destroy(root->right);
		}
		free(root->data);
		root->left = NULL;
		root->right = NULL;
		free(root);
		
	}
}

/* Inserts new data into the tree
	return	1 success
			0 overflow
*/
int AVL_Insert( AVL_TREE *pTree, char *data)
{
	//strdup를 쓰니가 문제가 해결되는 마법이 일어났따
	char* dupdata=strdup(data);
	NODE* newNode=_makeNode(dupdata);
	
	if(!newNode)
		return 0;

	pTree->root=_insert(pTree->root,newNode);
	pTree->count++;

	return 1;
}

/* internal function
	This function uses recursion to insert the new data into a leaf node
	return	pointer to new root
*/
static NODE *_insert( NODE *root, NODE *newPtr)
{
	int b_factor;
	
	//왜 root에서만 프린트가 제대로 안되지..?
	//같은 자리 숫자를 넣어야만 숫자에서도 정렬됨
	if(!root)
	{
		return newPtr;
	}

	root->height=max(getHeight(root->left),getHeight(root->right))+1;

	if(strcmp(newPtr->data,root->data)<0)
	{
		root->left=_insert(root->left,newPtr);

		if(BALANCING)
		{
			//	printf("%d %d\n",getHeight(root->left),getHeight(root->right));
			b_factor=getHeight(root->left)-getHeight(root->right);	
		
		//printf("b_factor is %d",b_factor);
		if(b_factor>1)
		{
			//오른쪽 자식이 없는 LL
			if(!root->left->right)
			{
				return rotateRight(root);
			}
			//왼쪽 자식이 없는 RL
			if(!root->left->left)
			{
				root->left=rotateLeft(root->left);
				return rotateRight(root);	
			}
				
			if(getHeight(root->left->left)>getHeight(root->left->right))
			{
				return rotateRight(root);
			}
				else
			{
				//root가 아니고 root->left
				root->left=rotateLeft(root->left);
				return rotateRight(root);	
			}
		}
		if(b_factor<-1)
		{
				//왼쪽 자식이 없는 RR
			if(!root->right->left)
			{
				return rotateLeft(root);
			}
			//오른쪽 자식이 없는 LR
			if(!root->right->right)
			{
				root->right=rotateRight(root->right);
				rotateLeft(root);
			}

			//RR
			if(getHeight(root->right->right)>getHeight(root->right->left))
			{
				return rotateLeft(root);
			}
				//LR
			else
			{
				root->right=rotateRight(root->right);
				return rotateLeft(root);	
			}	
		}
			
		}
	}
	else
	{
		root->right=_insert(root->right, newPtr);



		if(BALANCING)
		{
			//if(root->left&&root->right)
			//printf("%d %d\n",getHeight(root->left),getHeight(root->right));
	
			
			b_factor=getHeight(root->left)-getHeight(root->right);
			
			//printf("b_factor is %d\n",b_factor);
			if(b_factor>1)
			{
			//오른쪽 자식이 없는 LL
				if(!root->left->right)
				{
					return rotateRight(root);
				}
				//왼쪽 자식이 없는 RL
				if(!root->left->left)
				{
					root->left=rotateLeft(root->left);
					return rotateRight(root);	
				}
				
				if(getHeight(root->left->left)>getHeight(root->left->right))
				{
					return rotateRight(root);
				}
				else
				{
				//root가 아니고 root->left
					root->left=rotateLeft(root->left);
					return rotateRight(root);	
				}
			}
			if(b_factor<-1)
			{
				//왼쪽 자식이 없는 RR
				if(!root->right->left)
				{
					return rotateLeft(root);
				}
			//오른쪽 자식이 없는 LR
				if(!root->right->right)
				{
					root->right=rotateRight(root->right);
					rotateLeft(root);
				}

			//RR
				if(getHeight(root->right->right)>getHeight(root->right->left))
				{
					return rotateLeft(root);
				}
				//LR
				else
				{
					root->right=rotateRight(root->right);
					return rotateLeft(root);	
				}	
			}
		}
	}
		
	root->height=max(getHeight(root->left),getHeight(root->right))+1;
	

	
	// if(BALANCING)
	// {
		
	// 	b_factor=getHeight(root->left)-getHeight(root->right);
		
	// 	if(b_factor>1)
	// 	{
	// 		//오른쪽 자식이 없는 LL
	// 		if(!root->left->right)
	// 		{
	// 			return rotateRight(root);
	// 		}
	// 		//왼쪽 자식이 없는 RL
	// 		if(!root->left->left)
	// 		{
	// 			root->left=rotateLeft(root->left);
	// 			return rotateRight(root);	
	// 		}
				
	// 		if(getHeight(root->left->left)>getHeight(root->left->right))
	// 		{
	// 			return rotateRight(root);
	// 		}
	// 			else
	// 		{
	// 			//root가 아니고 root->left
	// 			root->left=rotateLeft(root->left);
	// 			return rotateRight(root);	
	// 		}
	// 	}
	// 	//printf("b factor is %d",b_factor);
	// 	if(b_factor<-1)
	// 	{
			
	// 			//왼쪽 자식이 없는 RR
	// 		if(!root->right->left)
	// 		{
	// 			return rotateLeft(root);
	// 		}
	// 		//오른쪽 자식이 없는 LR
	// 		if(!root->right->right)
	// 		{
	// 			root->right=rotateRight(root->right);
	// 			return rotateLeft(root);
	// 		}

	// 		//RR
	// 		if(getHeight(root->right->right)>getHeight(root->right->left))
	// 		{
	// 			return rotateLeft(root);
	// 		}
	// 		//LR
	// 		else
	// 		{
	// 			root->right=rotateRight(root->right);
	// 			return rotateLeft(root);	
	// 		}
	// 	}
	// }

	
	//root->height++;
	return root;
}

static NODE *_makeNode( char *data)
{
	NODE* newNode=(NODE*)malloc(sizeof(NODE));

	newNode->data=(char*)malloc(sizeof(char)*(strlen(data)+1));
	newNode->data=data;
	newNode->left=NULL;
	newNode->right=NULL;
	newNode->height=1;

	return newNode;
}

/* Retrieve tree for the node containing the requested key
	return	address of data of the node containing the key
			NULL not found
*/
char *AVL_Retrieve( AVL_TREE *pTree, char *key)
{
	//char* dupkey=strdup(key);

	return _retrieve(pTree->root,key)->data;
}

/* internal function
	Retrieve node containing the requested key
	return	address of the node containing the key
			NULL not found
*/
static NODE *_retrieve( NODE *root, char *key)
{
	//char* dupkey=strdup(key);
	if(root)
	{
		if(strcmp(key,root->data)<0)
		{
			_retrieve(root->left,key);
		}
		else if(strcmp(key,root->data)<0)
		{
			_retrieve(root->right,key);
		}
		else
			return root;
	}

	return NULL;
}

/* Prints tree using inorder traversal
*/
void AVL_Traverse( AVL_TREE *pTree)
{
	_traverse(pTree->root);
}
static void _traverse( NODE *root)
{
	if(root)
	{
		if(root->left)
		{
			_traverse(root->left);
		}
		printf("%s ",root->data);
		if(root->right)
		{
			_traverse(root->right);
		}
	}
}

/* Prints tree using inorder right-to-left traversal
*/
void printTree( AVL_TREE *pTree)
{
	int level=0;
	_infix_print(pTree->root,level);
}
/* internal traversal function
*/
static void _infix_print( NODE *root, int level)
{
	if(root)
	{
		if(root->right)
		{
			_infix_print(root->right,level+1);
		}
		for(int i=0;i<level;i++)
		{
			printf("\t");
			
		}
		printf("%s\n",root->data);
		if(root->left)
		{
			_infix_print(root->left,level+1);
		}
		
	}
}

/* internal function
	return	height of the (sub)tree from the node (root)
*/
static int getHeight( NODE *root)
{
	if(!root)
		return 0;
	return root->height;
	
}

/* internal function
	Exchanges pointers to rotate the tree to the right
	updates heights of the nodes
	return	new root
*/
static NODE *rotateRight( NODE *root)
{
	NODE* newRoot;
	newRoot=root->left;
	root->height=getHeight(newRoot->right)+1;
	root->left=newRoot->right;
	newRoot->right=root;
	newRoot->height=getHeight(root)+1;
	//root->left->right=root;
	//root->left=temp;

	//root->height=getHeight(root)-1;
	//newRoot->height=getHeight(newRoot)+1;
	//root->height=getHeight(newRoot->right)+1;
	//newRoot->height=getHeight(root)+1;

	return newRoot;
}

/* internal function
	Exchanges pointers to rotate the tree to the left
	updates heights of the nodes
	return	new root
*/
static NODE *rotateLeft( NODE *root)
{
	NODE* newRoot;
	newRoot=root->right;
	root->height=getHeight(newRoot->left)+1;
	root->right=newRoot->left;
	newRoot->left=root;
	newRoot->height=getHeight(root)+1;

	//root->height=getHeight(newRoot->left)+1;
	//root->height=getHeight(root)-1;
	//newRoot->height=getHeight(newRoot)+1;	
	return newRoot;
}