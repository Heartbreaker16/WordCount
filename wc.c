#include<stdio.h>
#include<stdlib.h>
#include<string.h>

typedef enum { false = 0, true = 1 } bool;

void count_char(char*, char*);
void count_word(char*, char*);
void count_line(char*, char*);
void count_null(char *, char*);
void output(char*, char*, char*, int);
void stopList(char * , char *, char * );
void wordsave(char **, char *, char *);
int Listsize(char *, char *);
FILE* openfile(char*, char *);

void main(int argc, char *argv[])
{
	struct
	{
		bool flag;//函数执行开关
		void(*p)(char*, char*);//指向函数的指针
	}function[4];//每一个结构体成员代表一种函数

	for (int i = 0; i < 4; i++)
		function[i].flag = false;//初始化：函数开关全部关闭

	//初始化指向函数
	function[0].p = count_char;
	function[1].p = count_word;
	function[2].p = count_line;
	function[3].p = count_null;

	char *filename = "", *txtname = "output.txt", *stopfile="";

	for (int i = 1; i < argc; i++) 
	{
		//根据用户在终端的输入来打开相应开关和初始化文件名称
		if (strcmp(argv[i], "-c")==0) function[0].flag = true;
		else if (strcmp(argv[i], "-w")==0) function[1].flag = true;
		else if (strcmp(argv[i],"-l")==0) function[2].flag = true;
		else if (strcmp(argv[i], "-a") == 0) function[3].flag = true;
		else if (strcmp(argv[i], "-o") == 0) txtname = argv[ (i++) + 1];
		else if (strcmp(argv[i], "-e") == 0) stopfile = argv[(i++) + 1];
		else filename = argv[i];
	}
	for (int i = 0; i < 4; i++)//按优先级顺序执行函数，i 越小优先级越大
	{
		if (function[i].flag)
		{
		if (i == 1 && strcmp(stopfile, "") != 0) //调用了停用词表，则执行带停用词表的统计函数
			{
				stopList(filename, stopfile, txtname);
				continue;
			}
			function[i].p(filename, txtname);
		}
	}
}

//统计字符数
void count_char(char * filename, char * txtname)
{
	FILE *fp = openfile(filename, "r");
	if (fp == NULL) return;
	int count = 0;
	while (fgetc(fp) != EOF)//逐字符扫描并统计
		count++;
	output(filename, txtname, "字符数：", count);
	fclose(fp);
}

//统计单词数
void count_word(char * filename, char * txtname)
{
	//调用统计单词个数的函数Listsize和输出函数output
	output(filename, txtname, "单词数：", Listsize(filename, ", \n\t"));
}

//统计行数
void count_line(char * filename, char * txtname)
{
	FILE *fp = openfile(filename, "r");
	if (fp == NULL) return;
	int count = 1;
	while (!feof(fp))
	{
		if(fgetc(fp)=='\n')//文本行数=换行符个数+1
			count++;
	}
	output(filename,txtname, "行数：",count);
	fclose(fp);
}

//统计空行数
void count_null(char * filename, char*txtname)
{
	FILE *fp = openfile(filename, "r");
	if (fp == NULL) return;
	int count = 0;
	bool isNull = true;//标记当前行是否为空行
	char ch;
	while ((ch = fgetc(fp)) != EOF)
	{
		if (ch == '\n')
		{
			if (isNull)//上一行是空行
				count++;
			else isNull = true;//设置标记：当前行是空行
		}
		else if (strchr("\n\t{} ", ch) == NULL)//当前字符是代码字符
			isNull = false;//设置标记：当前行不是空行
	}
	fclose(fp);
	
	if (isNull)//文件最后一行是空行，则最后结果加一
		count ++;

	output(filename, txtname, "空行数：", count);
}

//输出结果
void output(char *filename, char *txtname, char *title, int count)
{
		char buffer[50];
		printf("%-10s%-15s%d\n", filename, title, count);//将结果输出到终端
		sprintf_s(buffer, 50,"%-10s%-15s%d\n", filename, title, count);//将结果写入buffer数组
		FILE *fp = openfile(txtname, "a+");
		if (fp==NULL) return;
		fseek(fp, 0, SEEK_END);
		fwrite(buffer, strlen(buffer), 1, fp);//将buffer内容写入文件
		fclose(fp);
}

//统计不在停用词表中的单词数
void stopList(char * filename, char *stoptext, char * txtname)
{
	int count = 0;
	//获取输入文件和停用词表中的单词个数
	int countword = Listsize(filename, ", \n\t"), countstop = Listsize(stoptext, " ");
	if (countword *countstop < 0) return;

	//动态申请存储输入文件中所有单词的二维数组
	char **wordList = (char**)malloc(countword * sizeof(char*));
	for (int i = 0; i < countword; i++)
		wordList[i] = (char*)malloc(200);
	//动态申请存储停用词表中所有单词的二维数组
	char **stopList = (char**)malloc(countstop * sizeof(char*));
	for (int i = 0; i < countstop; i++)
		stopList[i] = (char*)malloc(200);

	//二维字符数组初始化
	for (int i = 0; i < countword; i++)
		for (int j = 0; j < 200; j++)
		{
			wordList[i][j] = '\0';
		}
	for (int i = 0; i < countstop; i++)
		for (int j = 0; j < 200; j++)
		{
			stopList[i][j] = '\0';
		}

	wordsave(wordList, filename, ", \n\t");
	wordsave(stopList, stoptext, " ");

	//统计重复的单词个数
	for (int i = 0; i < countstop; i++)
		for (int j = 0; j < countword; j++)
		{
			if (strcmp(stopList[i], wordList[j]) == 0)
				count++;
		}

	//释放动态申请的内存空间
	for (int i = 0; i<countword; i++)
		free(wordList[i]);
	free(wordList);

	for (int i = 0; i<countstop; i++)
		free(stopList[i]);
	free(stopList);

	output(filename, txtname, "单词数：", countword - count);
}

//获取文件中所有单词
void wordsave(char **list, char *file, char *filter)
{
	FILE *fp = openfile(file, "r");
	if (fp == NULL) return;
	char ch;
	int i = 0, j = 0;
	bool afterWord = false;//标记当前字符是否在单词字符后面
	while (!feof(fp))
	{
		ch = fgetc(fp);
		if (strchr(filter, ch) != NULL && afterWord)//当前字符是分隔符且前一个字符是单词字符
		{
			afterWord = false; //设置标记：当前字符是分隔字符
			i++; //当前单词记录结束，开始记录下一单词
			j = 0;//新单词从首位开始记录
		}
		else if (strchr(filter, ch) == NULL && ch != EOF)//当前字符是单词字符
		{
			list[i][j++] = ch;//将当前字符存储在该单词中
			afterWord = true;//设置标记：当前字符是单词字符
		}
	}
	fclose(fp);
}

//统计单词个数
int Listsize(char *file, char *filter)
{
	FILE *fp = openfile(file, "r");
	if (fp == NULL) return -1;
	char ch;
	int count = 0;
	bool afterWord = false;//标记当前字符是否在单词字符后面

	while (!feof(fp))
	{
		ch = fgetc(fp);
		if (strchr(filter, ch) != NULL && afterWord)//当前字符是分隔符且前一个字符是单词字符
		{
			count++;
			afterWord = false;//设置标记：当前字符是分隔符
		}
		else if (strchr(filter, ch) == NULL && ch != EOF)//当前字符是单词字符
			afterWord = true;//设置标记：当前字符是单词字符
	}
	fclose(fp);

	if (afterWord)//文件最后一个字符是单词字符，则最后结果加一
		count++;

	return count;
}

//打开文件
FILE *openfile(char* filename, char * type)
{
	FILE *fp;
	if (fopen_s(&fp, filename, type) != 0)
	{
		printf("找不到%s文件\n", filename);
		return NULL;
	}
	return fp;
}