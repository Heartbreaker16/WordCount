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
		bool flag;//����ִ�п���
		void(*p)(char*, char*);//ָ������ָ��
	}function[4];//ÿһ���ṹ���Ա����һ�ֺ���

	for (int i = 0; i < 4; i++)
		function[i].flag = false;//��ʼ������������ȫ���ر�

	//��ʼ��ָ����
	function[0].p = count_char;
	function[1].p = count_word;
	function[2].p = count_line;
	function[3].p = count_null;

	char *filename = "", *txtname = "output.txt", *stopfile="";

	for (int i = 1; i < argc; i++) 
	{
		//�����û����ն˵�����������Ӧ���غͳ�ʼ���ļ�����
		if (strcmp(argv[i], "-c")==0) function[0].flag = true;
		else if (strcmp(argv[i], "-w")==0) function[1].flag = true;
		else if (strcmp(argv[i],"-l")==0) function[2].flag = true;
		else if (strcmp(argv[i], "-a") == 0) function[3].flag = true;
		else if (strcmp(argv[i], "-o") == 0) txtname = argv[ (i++) + 1];
		else if (strcmp(argv[i], "-e") == 0) stopfile = argv[(i++) + 1];
		else filename = argv[i];
	}
	for (int i = 0; i < 4; i++)//�����ȼ�˳��ִ�к�����i ԽС���ȼ�Խ��
	{
		if (function[i].flag)
		{
		if (i == 1 && strcmp(stopfile, "") != 0) //������ͣ�ôʱ���ִ�д�ͣ�ôʱ��ͳ�ƺ���
			{
				stopList(filename, stopfile, txtname);
				continue;
			}
			function[i].p(filename, txtname);
		}
	}
}

//ͳ���ַ���
void count_char(char * filename, char * txtname)
{
	FILE *fp = openfile(filename, "r");
	if (fp == NULL) return;
	int count = 0;
	while (fgetc(fp) != EOF)//���ַ�ɨ�貢ͳ��
		count++;
	output(filename, txtname, "�ַ�����", count);
	fclose(fp);
}

//ͳ�Ƶ�����
void count_word(char * filename, char * txtname)
{
	//����ͳ�Ƶ��ʸ����ĺ���Listsize���������output
	output(filename, txtname, "��������", Listsize(filename, ", \n\t"));
}

//ͳ������
void count_line(char * filename, char * txtname)
{
	FILE *fp = openfile(filename, "r");
	if (fp == NULL) return;
	int count = 1;
	while (!feof(fp))
	{
		if(fgetc(fp)=='\n')//�ı�����=���з�����+1
			count++;
	}
	output(filename,txtname, "������",count);
	fclose(fp);
}

//ͳ�ƿ�����
void count_null(char * filename, char*txtname)
{
	FILE *fp = openfile(filename, "r");
	if (fp == NULL) return;
	int count = 0;
	bool isNull = true;//��ǵ�ǰ���Ƿ�Ϊ����
	char ch;
	while ((ch = fgetc(fp)) != EOF)
	{
		if (ch == '\n')
		{
			if (isNull)//��һ���ǿ���
				count++;
			else isNull = true;//���ñ�ǣ���ǰ���ǿ���
		}
		else if (strchr("\n\t{} ", ch) == NULL)//��ǰ�ַ��Ǵ����ַ�
			isNull = false;//���ñ�ǣ���ǰ�в��ǿ���
	}
	fclose(fp);
	
	if (isNull)//�ļ����һ���ǿ��У����������һ
		count ++;

	output(filename, txtname, "��������", count);
}

//������
void output(char *filename, char *txtname, char *title, int count)
{
		char buffer[50];
		printf("%-10s%-15s%d\n", filename, title, count);//�����������ն�
		sprintf_s(buffer, 50,"%-10s%-15s%d\n", filename, title, count);//�����д��buffer����
		FILE *fp = openfile(txtname, "a+");
		if (fp==NULL) return;
		fseek(fp, 0, SEEK_END);
		fwrite(buffer, strlen(buffer), 1, fp);//��buffer����д���ļ�
		fclose(fp);
}

//ͳ�Ʋ���ͣ�ôʱ��еĵ�����
void stopList(char * filename, char *stoptext, char * txtname)
{
	int count = 0;
	//��ȡ�����ļ���ͣ�ôʱ��еĵ��ʸ���
	int countword = Listsize(filename, ", \n\t"), countstop = Listsize(stoptext, " ");
	if (countword *countstop < 0) return;

	//��̬����洢�����ļ������е��ʵĶ�ά����
	char **wordList = (char**)malloc(countword * sizeof(char*));
	for (int i = 0; i < countword; i++)
		wordList[i] = (char*)malloc(200);
	//��̬����洢ͣ�ôʱ������е��ʵĶ�ά����
	char **stopList = (char**)malloc(countstop * sizeof(char*));
	for (int i = 0; i < countstop; i++)
		stopList[i] = (char*)malloc(200);

	//��ά�ַ������ʼ��
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

	//ͳ���ظ��ĵ��ʸ���
	for (int i = 0; i < countstop; i++)
		for (int j = 0; j < countword; j++)
		{
			if (strcmp(stopList[i], wordList[j]) == 0)
				count++;
		}

	//�ͷŶ�̬������ڴ�ռ�
	for (int i = 0; i<countword; i++)
		free(wordList[i]);
	free(wordList);

	for (int i = 0; i<countstop; i++)
		free(stopList[i]);
	free(stopList);

	output(filename, txtname, "��������", countword - count);
}

//��ȡ�ļ������е���
void wordsave(char **list, char *file, char *filter)
{
	FILE *fp = openfile(file, "r");
	if (fp == NULL) return;
	char ch;
	int i = 0, j = 0;
	bool afterWord = false;//��ǵ�ǰ�ַ��Ƿ��ڵ����ַ�����
	while (!feof(fp))
	{
		ch = fgetc(fp);
		if (strchr(filter, ch) != NULL && afterWord)//��ǰ�ַ��Ƿָ�����ǰһ���ַ��ǵ����ַ�
		{
			afterWord = false; //���ñ�ǣ���ǰ�ַ��Ƿָ��ַ�
			i++; //��ǰ���ʼ�¼��������ʼ��¼��һ����
			j = 0;//�µ��ʴ���λ��ʼ��¼
		}
		else if (strchr(filter, ch) == NULL && ch != EOF)//��ǰ�ַ��ǵ����ַ�
		{
			list[i][j++] = ch;//����ǰ�ַ��洢�ڸõ�����
			afterWord = true;//���ñ�ǣ���ǰ�ַ��ǵ����ַ�
		}
	}
	fclose(fp);
}

//ͳ�Ƶ��ʸ���
int Listsize(char *file, char *filter)
{
	FILE *fp = openfile(file, "r");
	if (fp == NULL) return -1;
	char ch;
	int count = 0;
	bool afterWord = false;//��ǵ�ǰ�ַ��Ƿ��ڵ����ַ�����

	while (!feof(fp))
	{
		ch = fgetc(fp);
		if (strchr(filter, ch) != NULL && afterWord)//��ǰ�ַ��Ƿָ�����ǰһ���ַ��ǵ����ַ�
		{
			count++;
			afterWord = false;//���ñ�ǣ���ǰ�ַ��Ƿָ���
		}
		else if (strchr(filter, ch) == NULL && ch != EOF)//��ǰ�ַ��ǵ����ַ�
			afterWord = true;//���ñ�ǣ���ǰ�ַ��ǵ����ַ�
	}
	fclose(fp);

	if (afterWord)//�ļ����һ���ַ��ǵ����ַ������������һ
		count++;

	return count;
}

//���ļ�
FILE *openfile(char* filename, char * type)
{
	FILE *fp;
	if (fopen_s(&fp, filename, type) != 0)
	{
		printf("�Ҳ���%s�ļ�\n", filename);
		return NULL;
	}
	return fp;
}