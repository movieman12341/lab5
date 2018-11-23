// Strongly connected components using Kosaraju's DFS technique
// BPW, 4/2003

#include <stdio.h>
#include <stdlib.h>

#define WHITE 0
#define GRAY 1
#define BLACK 2

//protos
int addAscii(char string[]);
int getPrimeVal(int nVal);




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

	// read edges, NOW AS STRING. end result = a  and b store values after hash table is done
	for (i = 0; i<e; i++)
	{
		scanf("%s %s", &str1, &str2);
		int asciiVal1 = addAscii(&str1);
		int asciiVal2 = addAscii(&str2);
		int m = getPrimeVal(n);





		/*if (a<0 || a >= n || b<0 || b >= n)
		{
			printf("Invalid input %d %d at %d\n", a, b, __LINE__);
			exit(0);
		}
		edgeTab[i].tail = a;
		edgeTab[i].head = b;*/
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

	printf("%d\n", u); // Indicate that u is in SCC for this restart
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
	int i = 0, sum = 0, len = strlen(string);
	for (; i < len; i++)
	{
		sum += string[i]; //sums all the ascii values together for the given string
	}
	return sum;
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
