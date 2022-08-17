#include <stdio.h>
#include <stdlib.h>	// malloc
#include <string.h>	// strdup
#include <ctype.h>	// isupper, tolower

#define MAX_DEGREE	27 // 'a' ~ 'z' and EOW
#define EOW			'$' // end of word

// used in the following functions: trieInsert, trieSearch, triePrefixList
#define getIndex(x)		(((x) == EOW) ? MAX_DEGREE-1 : ((x) - 'a'))

// TRIE type definition
typedef struct trieNode {
	int 			index; // -1 (non-word), 0, 1, 2, ...
	struct trieNode	*subtrees[MAX_DEGREE];
} TRIE;

////////////////////////////////////////////////////////////////////////////////
// Prototype declarations

/* Allocates dynamic memory for a trie node and returns its address to caller
	return	node pointer
			NULL if overflow
*/
TRIE *trieCreateNode(void);

/* Deletes all data in trie and recycles memory
*/
void trieDestroy( TRIE *root);

/* Inserts new entry into the trie
	return	1 success
			0 failure
*/
// 주의! 엔트리를 중복 삽입하지 않도록 체크해야 함
// 대소문자를 소문자로 통일하여 삽입
// 영문자와 EOW 외 문자를 포함하는 문자열은 삽입하지 않음
int trieInsert( TRIE *root, char *str, int dic_index);

/* Retrieve trie for the requested key
	return	index in dictionary (trie) if key found
			-1 key not found
*/
int trieSearch( TRIE *root, char *str);

/* prints all entries in trie using preorder traversal
*/
void trieList( TRIE *root, char *dic[]);

/* prints all entries starting with str (as prefix) in trie
	ex) "abb" -> "abbas", "abbasid", "abbess", ...
	this function uses trieList function
*/
void triePrefixList( TRIE *root, char *str, char *dic[]);

/* makes permuterms for given str
	ex) "abc" -> "abc$", "bc$a", "c$ab", "$abc"
	return	number of permuterms
*/
int make_permuterms( char *str, char *permuterms[]);

/* recycles memory for permuterms
*/
void clear_permuterms( char *permuterms[], int size);

/* wildcard search
	ex) "ab*", "*ab", "a*b", "*ab*"
	this function uses triePrefixList function
*/
void trieSearchWildcard( TRIE *root, char *str, char *dic[]);

////////////////////////////////////////////////////////////////////////////////
int main(int argc, char **argv)
{
	TRIE *trie;
	TRIE *permute_trie;
	char *dic[100000];

	int ret;
	char str[100];
	FILE *fp;
	char *permuterms[100];
	int num_p;
	int index = 0;
	
	if (argc != 2)
	{
		fprintf( stderr, "Usage: %s FILE\n", argv[0]);
		return 1;
	}
	
	fp = fopen( argv[1], "rt");
	if (fp == NULL)
	{
		fprintf( stderr, "File open error: %s\n", argv[1]);
		return 1;
	}
	
	trie = trieCreateNode(); // original trie
	permute_trie = trieCreateNode(); // trie for permuterm index
	
	while (fscanf( fp, "%s", str) != EOF)
	{	
		//printf("%s\n", str);
		ret = trieInsert( trie, str, index);
		
		if (ret)
		{
			num_p = make_permuterms( str, permuterms);
			
			for (int i = 0; i < num_p; i++)
			{
				
				trieInsert(permute_trie, permuterms[i], index);
			
			}
			
			
			clear_permuterms( permuterms, num_p);
			
			dic[index++] = strdup(str);
		}
	}
	
	fclose( fp);
	
	printf( "\nQuery: ");
	while (fscanf( stdin, "%s", str) != EOF)
	{
		// wildcard search term
		if (strchr( str, '*')) 
		{
			trieSearchWildcard( permute_trie, str, dic);
		}
		// keyword search
		else 
		{
			ret = trieSearch( trie, str);
			if (ret == -1) printf( "[%s] not found!\n", str);
			else printf( "[%s] found!\n", dic[ret]);
		}
		printf( "\nQuery: ");
	}

	for (int i = 0; i < index; i++)
		free( dic[i]);
	
	trieDestroy( trie);
	trieDestroy( permute_trie);
	
	return 0;
}

/* Allocates dynamic memory for a trie node and returns its address to caller
	return	node pointer
			NULL if overflow
*/
TRIE* trieCreateNode(void)
{
	TRIE* trie = (TRIE*)malloc(sizeof(TRIE));
	if (trie) {
		trie->index = -1;
		for (int i = 0; i < MAX_DEGREE; i++)
		{

			trie->subtrees[i] = NULL;
		}
	}

	return trie;
}

/* Deletes all data in trie and recycles memory
*/
void trieDestroy(TRIE* root)
{

	for (int i = 0; i < 27; i++)
	{
		if (root->subtrees[i])
		{
			trieDestroy(root->subtrees[i]);
			//free(root);
		}
		
	}

	free(root);
}

/* Inserts new entry into the trie
	return	1 success
			0 failure
*/
// 주의! 엔트리를 중복 삽입하지 않도록 체크해야 함
// 대소문자를 소문자로 통일하여 삽입
// 영문자와 EOW 외 문자를 포함하는 문자열은 삽입하지 않음
int trieInsert(TRIE* root, char* str, int dic_index)
{
	if (!root)
		return 0;
	int ith_index;

	for (int i = 0; i < strlen(str); i++)
	{
		if (!isalpha(str[i]))
			if(str[i]!=EOW)
				return 0;
		
		if (isupper(str[i]))
			str[i] = tolower(str[i]);
		
	}


	for (int i = 0; i < strlen(str); i++)
	{
		ith_index = getIndex(str[i]);
		

		if (root->subtrees[ith_index]==NULL)
		{
			root->subtrees[ith_index] = trieCreateNode();
			//return 1;
		}
		root = root->subtrees[ith_index];

	
	}

	root->index = dic_index;

	return 1;

}

/* Retrieve trie for the requested key
	return	index in dictionary (trie) if key found
			-1 key not found
*/
int trieSearch(TRIE* root, char* str)
{
	TRIE* Root=root;
	int ith_index;

	for (int i = 0; i < strlen(str); i++)
	{
		if (!isalpha(str[i])&&str[i]!='&')
			return -1;

		if (isupper(str[i]))
			str[i] = tolower(str[i]);
	}

	
	for (int i = 0; i < strlen(str); i++)
	{

		ith_index = getIndex(str[i]);

		if (!Root->subtrees[ith_index])
			return -1;

		Root = Root->subtrees[ith_index];
	
	}

	if (!Root)
		return -1;
	return Root->index;

}

/* prints all entries in trie using preorder traversal
*/
void trieList(TRIE* root, char* dic[])
{
	TRIE* Root = root;

	if (!Root)
		return;

	if (Root->index >= 0)
	{
		printf("%s\n", dic[Root->index]);
	}

	for (int i = 0; i < 27; i++)
	{
		if (Root->subtrees[i])
		{
			trieList(Root->subtrees[i], dic);
		}
	}
	
	return;
}

/* prints all entries starting with str (as prefix) in trie
	ex) "abb" -> "abbas", "abbasid", "abbess", ...
	this function ucdcdses trieList function
*/
void triePrefixList(TRIE* root, char* str, char* dic[])
{
	if (!root)	return;
	if (!str)	return;

	//printf("%d", root->index);
	TRIE* Root = root;
	//int ith_index;
	int length = strlen(str);
	//char check_str[100];
	//int idx = 0;
	//int found = 1;


	for (int i = 0; i < length-1; i++)
	{

		//ith_index = getIndex(str[i]);

		if (Root->subtrees[getIndex(str[i])])
			Root = Root->subtrees[getIndex(str[i])];
			
		else
		{
	
			return;
		}

	}
	
	
	trieList(Root, dic);

	return;
	
}

/* makes permuterms for given str
	ex) "abc" -> "abc$", "bc$a", "c$ab", "$abc"
	return	number of permuterms
*/
int make_permuterms(char* str, char* permuterms[])
{
	//char* str1;
	////str1 = (char*)malloc(sizeof(char));
	////char* str2 = (char*)malloc(sizeof(char));
	////str에 직접 손대면 이상하게 나와
	////char* input;

	////str1 = strdup(str);
	//str1=strdup(str);
	//str1[strlen(str)] = EOW;
	//int length = strlen(str1);

	//permuterms[0] = str1;

	//for (int i = 1; i <length; i++)
	//{
	//	strcpy(str1,permuterms[i - 1]);

	//	char temp = str1[0];
	//	for (int j = 1; j < length; j++)
	//	{
	//		str1[j-1] = str1[j];
	//	}

	//	str1[length] = temp;

	//	permuterms[i] = str1;
	//}

	//return length + 1;


	int length = strlen(str);
	str[length] = EOW;
	permuterms[0] = str;
	for (int i = length + 1; i < 100; i++)
	{
		str[i] = '\0';
	}
	//str[length] = EOW;
	
	

	
	
	
	

	//char* input = strdup(permute_str);
	

	for (int i = 1; i <= length; i++)
	{
		
		char* permute_str;
		//permute_str = strdup(permuterms[i-1]);
		permute_str= permuterms[i - 1];
		//input = strdup(permute_str);
		//char* input;

		char temp = permute_str[0];

		for (int j = 1; j <= length; j++)
		{
			permute_str[j - 1] = permute_str[j];
		}
		permute_str[length] = temp;

		permuterms[i] = strdup(permute_str);

		//free(input);
	
		//free(input);
		
	}

	
	//free(permute_str);
	 

	
	//permuterms[length] =strdup( permute_str);
	//free(input);

	//복구

	/*for (int i = 0; i < length + 1; i++)
	{
		printf("%s\n", permuterms[i]);
	}*/


	return length+1;
}

/* recycles memory for permuterms
*/
void clear_permuterms(char* permuterms[], int size)
{
	for (int i = 0; i < size; i++)
	{
		free(permuterms[i]); 
		//permuterms[i] = NULL;
	}
	
	//free(permuterms);
}

/* wildcard search
	ex) "ab*", "*ab", "a*b", "*ab*"
	this function uses triePrefixList function
*/
void trieSearchWildcard(TRIE* root, char* str, char* dic[])
{

	char wildcard_str[100];
	char* input;
	int length = strlen(str);

	if (str[0] == '*') 
	{
		int idx = 0;

		for (int i = 0; i < length; i++)
		{
			if (str[i] != '*')
			{
				wildcard_str[idx] = str[i];
				idx++;
			}
		}

		if (str[length-1] == '*')
		{
			
			//triePrefixList(root, input, dic);
		}
		else
		{
			wildcard_str[strlen(wildcard_str)] = EOW;

			//triePrefixList(root, wildcard_str,dic);
		}
	}
	else if (str[length - 1] == '*')
	{
		//wildcard_str = strtok(str, "*");

		
		
		wildcard_str[0] = EOW;
		for (int i = 1; i <= length; i++)
		{
			wildcard_str[i] = str[i-1];
		}
		//printf("%s", wildcard_str);
		//triePrefixList(root, wildcard_str,dic);
	}
	else
	{
		int idx = 0;
		int pre_length = 0;
		for (int i = 0; i < length; i++)
		{
			if (str[i] == '*')
			{
				break;
			}
			else
				pre_length++;
		}
		pre_length++;

		for (int j = pre_length + 1; j < length; j++)
		{
			wildcard_str[idx] = str[j];
			idx++;
		}
		wildcard_str[idx++] = EOW;
		for (int k = 0; k < pre_length; k++)
		{
			wildcard_str[idx] = str[k];
			idx++;
		}
	

		//triePrefixList(root, wildcard_str, dic);
	}

	input = strdup(wildcard_str);
	triePrefixList(root, input, dic);
	free(input);
	return;
	//triePrefixList(root, wildcard_str, dic);
	//free(wildcard_str);


}