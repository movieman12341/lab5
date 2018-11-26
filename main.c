/*
Luke Owens
11/27/18

to run my code, type:
gcc -o exe main.c
./exe

brief summary/requirements:

Design, code, and test a C program to determine strongly connected components for a directed graph. 

a. Input is to be read from standard input (like the first four assignments):
1. The first line is two integer values: n, the number of vertices, and m, the number of edges.
2. The remaining m lines will each contain two values defining an edge: a tail name (string of no more than 25
characters) and a head name (another string).
b. While reading the input, new vertex names should be stored in a double hash table along with consecutively assigned
vertex numbers. The first line of your output should indicate the size of your double hash table.
In addition to the double hash table, a table of vertex names will be needed. This is needed when printing your results
for the end user.
If the input does not have exactly n different names, give a disparaging message and stop.
The assigned vertex numbers are used to build compressed adjacency lists (Notes 14).
c. Perform Kosaraju’s SCC algorithm (Notes 14). The elements of each SCC must be output using the vertex names, not
numbers.

*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define WHITE 0
#define GRAY 1
#define BLACK 2

//protos
int addAscii(char string[]);
int getPrimeVal(int nVal);
int getHash(int hasharr[], char str[], int asciiVal, int m);



int n;  // number of nodes
int e;  // number of edges
struct edge {
	int tail, head, type;
};
typedef struct edge edgeType;
edgeType *edgeTab;
int *firstEdge;  // Table indicating first in range of edges with a common tail

int *vertexStatus, *secondDFSrestarts;

int tailThenHead(const void* xin, const void* yin)
// Used in calls to qsort() and bsearch() for read_input_file()
{
	int result;
	edgeType *x, *y;

	x = (edgeType*)xin;
	y = (edgeType*)yin;
	result = x->tail - y->tail;
	if (result != 0)
		return result;
	else
		return x->head - y->head;
}

//globals
int m;
char **strTable;//string table
static int count = 0;


void read_input_file()
{
	int a, b, i, j;
	char str1[25], str2[25];
	edgeType work;
	edgeType *ptr;

	scanf("%d %d", &n, &e);
	edgeTab = (edgeType*)malloc(e * sizeof(edgeType));
	if (!edgeTab)
	{
		printf("edgeTab malloc failed %d\n", __LINE__);
		exit(0);
	}
	m = getPrimeVal(n);
	int *hash = (int*)calloc(m, sizeof(int));//hash table
	strTable = (char**)calloc(m, sizeof(char*));//string table

	for (i = 0; i < m; i++)
	{
		strTable[i] = (char*)calloc(26, sizeof(char));//fills up empty 2d cost array
		hash[i] = (-1); // Flag all slots as never-used
	}


	if (!hash || !strTable) {
		printf("malloc failed\n");
		exit(0);
	}

	
	// read edges, NOW AS STRING. end result = a  and b store values after hash table is done
	for (i = 0; i<e; i++)
	{
		scanf("%s %s", &str1, &str2);
		int asciiVal1 = addAscii(str1); //gets ascii key for the string
		int asciiVal2 = addAscii(str2);//gets ascii key for the string
		int hash1Val = getHash(hash, str1, asciiVal1, m); //gets unique key for the string
		int hash2Val = getHash(hash, str2, asciiVal2, m); //gets unique key for the string

		a = hash1Val;
		b = hash2Val;


		if (a<0 || a >= n || b<0 || b >= n)
		{
			printf("Invalid input %d %d at %d\n", a, b, __LINE__);
			exit(0);
		}
		edgeTab[i].tail = a;
		edgeTab[i].head = b;
	}

	// sort edges
	qsort(edgeTab, e, sizeof(edgeType), tailThenHead);

	// Coalesce duplicates into a single edge
	j = 0;
	for (i = 1; i<e; i++)
		if (edgeTab[j].tail == edgeTab[i].tail
			&& edgeTab[j].head == edgeTab[i].head)
			;
		else
		{
			j++;
			edgeTab[j].tail = edgeTab[i].tail;
			edgeTab[j].head = edgeTab[i].head;
		}
	e = j + 1;

	// For each vertex as a tail, determine first in range of edgeTab entries
	firstEdge = (int*)malloc((n + 1) * sizeof(int));
	if (!firstEdge)
	{
		printf("malloc failed %d\n", __LINE__);
		exit(0);
	}
	j = 0;
	for (i = 0; i<n; i++)
	{
		firstEdge[i] = j;
		for (;
			j<e && edgeTab[j].tail == i;
			j++)
			;
	}
	firstEdge[n] = e;
}

int finishIndex;

void DFSvisit(int u)
{
	int i, v;

	vertexStatus[u] = GRAY;

	for (i = firstEdge[u]; i<firstEdge[u + 1]; i++)
	{
		v = edgeTab[i].head;
		if (vertexStatus[v] == WHITE)
			DFSvisit(v);
	}
	vertexStatus[u] = BLACK;
	secondDFSrestarts[--finishIndex] = u;
}

void reverseEdges()
{
	int a, b, i, j;
	edgeType work;
	edgeType *ptr;

	for (i = 0; i<e; i++)
	{
		a = edgeTab[i].tail;
		b = edgeTab[i].head;
		edgeTab[i].tail = b;
		edgeTab[i].head = a;
	}

	// sort edges
	qsort(edgeTab, e, sizeof(edgeType), tailThenHead);

	// For each vertex as a tail, determine first in range of edgeTab entries
	if (!firstEdge)
	{
		printf("malloc failed %d\n", __LINE__);
		exit(0);
	}
	j = 0;
	for (i = 0; i<n; i++)
	{
		firstEdge[i] = j;
		for (;
			j<e && edgeTab[j].tail == i;
			j++)
			;
	}
	firstEdge[n] = e;
}

void DFSvisit2(int u)
{
	int i, v;

	printf("%s\n", strTable[u]); // Indicate that u is in SCC for this restart 
	vertexStatus[u] = GRAY;

	for (i = firstEdge[u]; i<firstEdge[u + 1]; i++)
	{
		v = edgeTab[i].head;
		if (vertexStatus[v] == WHITE)
			DFSvisit2(v);
	}
	vertexStatus[u] = BLACK;
}

int main()
{
	int u, i, j, k, nextDFS;
	int SCCcount = 0;

	read_input_file();

	vertexStatus = (int*)malloc(n * sizeof(int));
	secondDFSrestarts = (int*)malloc(n * sizeof(int));
	if (!vertexStatus || !secondDFSrestarts)
	{
		printf("malloc failed\n");
		exit(0);
	}
	// DFS code
	for (u = 0; u<n; u++)
		vertexStatus[u] = WHITE;
	finishIndex = n;
	for (u = 0; u<n; u++)
		if (vertexStatus[u] == WHITE)
			DFSvisit(u);

	reverseEdges();


	//other print double hash table????

	// DFS code
	for (u = 0; u<n; u++)
		vertexStatus[u] = WHITE;
	for (i = 0; i<n; i++)
		if (vertexStatus[secondDFSrestarts[i]] == WHITE)
		{
			SCCcount++;
			printf("SCC %d\n", SCCcount);
			DFSvisit2(secondDFSrestarts[i]);
		}

	free(edgeTab);
	free(firstEdge);
	free(vertexStatus);
	free(secondDFSrestarts);
	return 0;
}

int addAscii(char string[])
{
	int i = 0, key = 0, count = 0;
	for (; string[i] != 0; i++)
	{
		key = (count * 10 + string[i]) % m; //gets ascii values and puts them in key
	}
	return key;
}

int getPrimeVal(int nVal)
{
	double compare = nVal;
	int i, compare1, tmp, j;
	for (i = nVal + 1; i <= nVal + 100; i++)
	{
		compare1 = 0;
		for (j = 2; j < i; j++)
		{
			if (i % j == 0)
			{
				compare1 = 1;
				break;
			}
		}
		if (compare1 == 0)
		{
			if (compare / i < .90)//to get a load factor no bigger than 90%
			{
				return i;
			}
			else
			{
				continue;
			}
		}
	}
}

int getHash(int hasharr[], char str[], int asciiVal, int m)
{
	int i, h1, h2, strcmpVal = 5;
	// Scan probe sequence until unused slot or key is found.
	i = h1 = asciiVal % m;
	h2 = asciiVal % (m - 1) + 1;

	if (hasharr[i] == -1) //empty table value to fill
	{
		hasharr[i] = count;			//count stores the index of the string table array
		strcpy(strTable[count], str); //copies the input string to the string table
		count++;
		return hasharr[i];
	}
	else //not a free slot, so loop until free
	{
		while (1)
		{
			if (hasharr[i] == -1) //empty table value to fill
			{
				hasharr[i] = count;			//count stores the index of the string table array
				strcpy(strTable[count], str); //copies the input string to the string table
				count++;
				return hasharr[i];
			}

			else if (hasharr[i] != -1 && strcmp(str, strTable[hasharr[i]]) == 0) //not an empty slot.
															//		strings are the same, so return same
													//index value
			{
				return hasharr[i];
			}
			else//skip hash table values of i with h2 values
			{
				i = (i + h2) % m;
			}
			
		}
	}
}

