#include <iostream>
#include <fstream>
#include <iomanip>
#include <string>
#include <cstring>
#include <vector>
#include <set>
#include <map>
#include <cstdlib>
#include <queue>
#include "mainHeader.h"
#include <cstddef>
#define debugprint printf
using namespace std;

ifstream ifile;
ofstream ofile;

class Item{
public:
	string left;//产生式左部
	vector<string> right;//产生式右部
	int dotPosition;//记录点的位置
	set<string> predictSymbol;//可规约预测符号集合
	Item(){}
	void Move()
	{
		if(GetCurrentSymbol()!=END)//防止溢出
			dotPosition++;//移动点的位置
	}
	string GetCurrentSymbol()
	{
		if(dotPosition<right.size())
			return right[dotPosition];
		return END;//定义的终止符
	}
};



struct PDAstate{//PDA状态
	int id;
	vector<Item> itemSet;

	struct PDAedge{//PDA边
	vector<PDAstate* > nextState;//指向下一状态
	vector<PDAstate* > frontState;//指向前一状态
	string symbol;
	};

	vector<PDAedge> edges;
};

struct PDA{
	vector<PDAstate> states;
};


set <string> Terminal;//终结符集
set <string> NonTerminal;//非终结符集
//set <Item>	 Producers;//产生式集
vector <Item>	 Producers;//产生式集  不能用set？？？

struct Op{
	string name;
	int priority;
	string type;
};
vector<Op> Operators;


void main()
{
	//Step1. 读取yacc文件
	string srcFile = "D:\\minic.y";//Yacc文件
	
	ifile.open(srcFile.c_str(), ios::in);
	ofile.open("D:\\generatedYacc.h", ios::out);
	if (!ifile.is_open()) {
		cerr << "[-]Error occured when open file!" << endl;
		exit(1);
	}
	char c=0x00;
	while(c!='%'&&!ifile.eof())
	{
		//一直读取到第一个%
		c = ifile.get();
	}
	if(c=='%')//读取到了第一个%
	{
		if(!(!ifile.eof()&&ifile.get()=='{'))
			cerr<<"[-]Wrong format!"<<endl;
	}
	bool endState=false;
	char nextC=0x00;
	while(!endState)
	{
		c=ifile.get();
		if(c!='%')
		{
			ofile.put(c);
		}
		else
		{
			nextC=ifile.get();
			if(nextC!='}')
				ofile.put(nextC);
			else
				endState=true;
		}
	}
	std::cout<<"[+]Definition segment completed!\n";
	//开始解析终结符和运算符
	string lineStr;
	char tempLineCStr[500];
	endState=false;
	int currentPriority=0;
	while(!endState)
	{
		ifile.getline(tempLineCStr,500);
		lineStr=tempLineCStr;
		//删除注释部分
		int annoteIndex=lineStr.find("/*");
		if(annoteIndex!=-1)
			lineStr=lineStr.substr(0,annoteIndex);
		//默认每一行的有效标识符从行首开始
		if(lineStr.find("%%")!=string::npos)
		{
			//找到%%标识符 说明定义段结束 跳出
			endState=true;
		}
		else
		{
			string markWord="";//用来存储行首的标识符
			char toSplit[200];
			strcpy(toSplit,lineStr.c_str());
			char *toke;
			char *nextToke;
			if(lineStr[0]=='%')//如果不是空行 则进行处理
			{//toke存储的是行首的标记符 有%token %type %nonassoc %left
				toke=strtok_s(toSplit,"\t \n",&nextToke);
				//std::cout<<"[toke]"<<toke<<"[+]\n";
				if(strcmp(toke,"%token")==0)//此行记录的是token
				{
					while(toke!=0)
					{
						toke=strtok_s(0,"\t \n",&nextToke);
						if(toke!=0)
						{//存储每一个terminal到集合中
							string tempToken=toke;
							Terminal.insert(tempToken);
						}
					}
				}
				else if(strcmp(toke,"%nonassoc")==0)//此行记录的是nonassoc
				{
					while(toke!=0)
					{
						toke=strtok_s(0,"\t \n",&nextToke);
						if(toke!=0)
						{//存储每一个nonassoc op到Operators中
							Op newOp;
							newOp.name=toke;
							newOp.priority=currentPriority;
							newOp.type="nonassoc";
							Operators.push_back(newOp);
						}
					}
					currentPriority++;
				}
				else if(strcmp(toke,"%left")==0)//此行记录的是nonassoc
				{
					while(toke!=0)
					{
						toke=strtok_s(0,"\t \n",&nextToke);
						if(toke!=0)
						{//存储每一个nonassoc op到Operators中
							Op newOp;
							newOp.name=toke;
							newOp.priority=currentPriority;
							newOp.type="left";
							Operators.push_back(newOp);
						}
					}
					currentPriority++;
				}
				else if(strcmp(toke,"%type")==0)//此行记录的是type
				{
				}
				else if(strcmp(toke,"%union")==0)//此行记录的是type
				{
				}
			}
		}
	}
	//开始解析规则段
	endState=false;
	bool newLeftMark=true;
	string tempLeft="";
	vector<string> tempRight;
	while(!endState)
	{
		ifile.getline(tempLineCStr,500);
		lineStr=tempLineCStr;
		//删除注释部分
		int annoteIndex=lineStr.find("/*");
		if(annoteIndex!=-1)
			lineStr=lineStr.substr(0,annoteIndex);
		lineStr=lineStr+'\0';
		//默认每一行的有效标识符从行首开始
		if(lineStr.find("%%")!=-1)
		{
			//找到%%标识符 说明规则段结束 跳出
			endState=true;
		}
		else
		{
			char toSplit[200];
			strcpy(toSplit,lineStr.c_str());
			char *toke=NULL;
			char *nextToke=NULL;
			//查找当前是否包含：  如果有 说明出现了新的左部
			if(lineStr.find(":")!=-1)
			{
				newLeftMark=true;//记录当前遇到了新的左部
			}
			//debugprint("%s\n",lineStr.c_str());
			toke=strtok_s(toSplit,"\t \n",&nextToke);
			while(toke!=NULL)
			{
				if(newLeftMark)//如果我遇到了leftMark 很显然toke是左部
				{
					newLeftMark=false;
					tempLeft=toke;
					if(Terminal.count(string(toke))==0)//如果当前的toke不是终结符 那么一定是非终结符
						NonTerminal.insert(toke);
					toke=strtok_s(NULL,"\t \n",&nextToke);
					continue;
				}
				//否则toke是右部或者 ：
				else if(string(toke)==":")
				{
					//遇到 ： 或者 | 或者 ；直接快进
					toke=strtok_s(NULL,"\t \n",&nextToke);
					continue;
				}
				else if(string(toke)=="|"||string(toke)==";")
				{//此时上一个产生式的右部已经采集完毕
					toke=strtok_s(NULL,"\t \n",&nextToke);
					Item tempProducer;
					tempProducer.dotPosition=0;
					tempProducer.left=tempLeft;
					for(int i=0;i<tempRight.size();i++)
					{
						tempProducer.right.push_back(tempRight[i]);
					}
					Producers.push_back(tempProducer);//将刚发现的产生式加入到Producer中
					tempRight.clear();//清除上一次的右部
				}
				else//当前遇到的是右部
				{
					tempRight.push_back(string(toke));
					if(Terminal.count(string(toke))==0)//如果当前的toke不是终结符 那么一定是非终结符
						NonTerminal.insert(toke);
					toke=strtok_s(NULL,"\t \n",&nextToke);
				}
			}
		}
	}

	//开始拷贝子程序段

	//输出部分结果
	std::cout<<"[+]Print Terminal Set\n";
	for(set<string>::iterator it=Terminal.begin();it!=Terminal.end();it++)
	{
		//debugprint("%s\t",*it->c_str());
		cout<<"<"<<*it<<">\t";
	}
	cout<<endl;
	std::cout<<"[+]Print NonTerminal Set\n";
	for(set<string>::iterator it=NonTerminal.begin();it!=NonTerminal.end();it++)
	{
		cout<<"<"<<*it<<">\t";
	}
	cout<<endl;
	cout<<"[+]Print Producers\n";
	for(int i=0;i<Producers.size();i++)
	{
		cout<<"("<<Producers[i].left<<") ----> ";
		for(int j=0;j<Producers[i].right.size();j++)
			cout<<"("<<Producers[i].right[j]<<") ";
		cout<<endl;
	}
	std::cout<<"\n[+]Print Operator\n";
	for(int i=0;i<Operators.size();i++)
	{
		std::cout<<Operators[i].name<<"\t"<<Operators[i].type<<"\t"<<Operators[i].priority<<"\t"<<endl;
	}
	ifile.close();
	ofile.close();
	std::cout<<"Done\n";
}


void main_()
{
	string lineStr="abc";
	lineStr=("ff");
	cout<<"[]"<<lineStr<<"[]"<<endl;
	
}