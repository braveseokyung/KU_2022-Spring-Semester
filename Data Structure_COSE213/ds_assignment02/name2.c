#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_YEAR_DURATION	10	// 기간

// 구조체 선언
typedef struct {
	char	name[20];		// 이름
	char	sex;			// 성별 M or F
	int		freq[MAX_YEAR_DURATION]; // 연도별 빈도
} tName;

typedef struct {
	int		len;		// 배열에 저장된 이름의 수
	int		capacity;	// 배열의 용량 (배열에 저장 가능한 이름의 수)
	tName* data;		// 이름 배열의 포인터
} tNames;

// 함수 원형 선언

// 연도별 입력 파일을 읽어 이름 정보(연도, 이름, 성별, 빈도)를 이름 구조체에 저장
// 이미 구조체에 존재하는(저장된) 이름은 해당 연도의 빈도만 저장
// 새로 등장한 이름은 구조체에 추가
// 주의사항: 동일 이름이 남/여 각각 사용될 수 있으므로, 이름과 성별을 구별해야 함
// 주의사항: 정렬 리스트(ordered list)를 유지해야 함 (qsort 함수 사용하지 않음)
// 이미 등장한 이름인지 검사하기 위해 bsearch 함수를 사용
// 새로운 이름을 저장할 메모리 공간을 확보하기 위해 memmove 함수를 이용하여 메모리에 저장된 내용을 복사
// names->capacity는 2배씩 증가
void load_names(FILE* fp, int start_year, tNames* names);

// 구조체 배열을 화면에 출력
void print_names(tNames* names, int num_year);

// bsearch를 위한 비교 함수
int compare(const void* n1, const void* n2);

// 이진탐색 함수
// return value: key가 발견되는 경우, 배열의 인덱스
//				key가 발견되지 않는 경우, key가 삽입되어야 할 배열의 인덱스
int binary_search(const void* key, const void* base, size_t nmemb, size_t size, int (*compare)(const void*, const void*));

// 함수 정의
void load_names(FILE* fp, int start_year, tNames* names)
{
	char buffer[40];
	
	while (1)
	{
		tName nameinfo;
		int year;
		//char*아님!
		tName* find_name;
		int find_index;
		
		
		char* pstr=fgets(buffer, sizeof(buffer), fp);
		if(pstr==NULL)
			break;

		//&nameinfo.freq[year-start_year]로 하면 이번문장에서 읽은 year가 반영이 안돼서 다른 연도의 빈도에 저장됨
		sscanf(buffer, "%d%s\t%c\t%d",&year,nameinfo.name, &nameinfo.sex, &nameinfo.freq[0]);

		find_name = (tName*)bsearch(nameinfo.name, names->data, names->len, sizeof(tName), compare);

		if (find_name)
		{
			find_name->freq[year - start_year] = nameinfo.freq[0];
		}
		else
		{
			find_index = binary_search(nameinfo.name, names->data, names->len, sizeof(tName), compare);

			names->len++;
			//위치
			if (names->len == names->capacity)
			{
				names->capacity *= 2;
				tName* tmp;
				tmp = (tName*)realloc(names->data, sizeof(tName) * names->capacity);
				if (tmp)
					names->data = tmp;
			}

			memmove(names->data+find_index+1, names->data+find_index, sizeof(tName) * (names->len - find_index-1));

			memset(names->data[find_index].freq, 0, sizeof(int) * 10);
			strcpy(names->data[find_index].name, nameinfo.name);
			names->data[find_index].sex = nameinfo.sex;
			names->data[find_index].freq[year - start_year] = nameinfo.freq[0];

		}
	}
}

void print_names(tNames* names, int num_year)
{
	for (int i = 0; i < names->len; i++)
	{
		printf("%s\t%c", names->data[i].name, names->data[i].sex);

		for (int j = 0; j < num_year; j++)
		{
			printf("\t%d", names->data[i].freq[j]);
		}

		printf("\n");
	}
}

int compare(const void* n1, const void* n2)
{
	//char* 아님!
	char* name1 = ((tName*)n1)->name;
	char* name2 = ((tName*)n2)->name;
	//char* 아님
	char sex1 = ((tName*)n1)->sex;
	char sex2 = ((tName*)n2)->sex;

	if (strcmp(name1, name2) == 0)
		return sex1 - sex2;
	else
		return(strcmp(name1, name2));
}

int binary_search(const void* key, const void* base, size_t nmemb, size_t size, int (*compare)(const void*, const void*))
{
	int start = 0;
	int end = nmemb-1;
	int mid = (start + end) / 2;

	while (start <= end)
	{
		
		if (compare(key, ((tName*)base)[mid].name) < 0)
			end = mid - 1;
		else if (compare(key, ((tName*)base)[mid].name)> 0)
			start = mid + 1;
		else
			return mid;

		mid = (start + end) / 2;
	}

	return start;
}


// 이름 구조체 초기화
// len를 0으로, capacity를 1로 초기화
// return : 구조체 포인터
tNames* create_names(void)
{
	tNames* pnames = (tNames*)malloc(sizeof(tNames));

	pnames->len = 0;
	pnames->capacity = 1;
	pnames->data = (tName*)malloc(pnames->capacity * sizeof(tName));

	return pnames;
}

// 이름 구조체에 할당된 메모리를 해제
void destroy_names(tNames* pnames)
{
	free(pnames->data);
	pnames->len = 0;
	pnames->capacity = 0;

	free(pnames);
}

////////////////////////////////////////////////////////////////////////////////
int main(int argc, char** argv)
{
	tNames* names;
	FILE* fp;

	if (argc != 2)
	{
		fprintf(stderr, "Usage: %s FILE\n\n", argv[0]);
		return 1;
	}

	// 이름 구조체 초기화
	names = create_names();

	fp = fopen(argv[1], "r");
	if (!fp)
	{
		fprintf(stderr, "cannot open file : %s\n", argv[1]);
		return 1;
	}

	fprintf(stderr, "Processing [%s]..\n", argv[1]);

	// 연도별 입력 파일(이름 정보)을 구조체에 저장
	load_names(fp, 2009, names);

	fclose(fp);

	// 이름 구조체를 화면에 출력
	print_names(names, MAX_YEAR_DURATION);

	// 이름 구조체 해제
	destroy_names(names);

	return 0;
}





















