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
	string left;//����ʽ��
	vector<string> right;//����ʽ�Ҳ�
	int dotPosition;//��¼���λ��
	string lookaheadSymbol;//�ɹ�ԼԤ����ż��� ÿ��item����Ϊһ��
	Item(){dotPosition=0;}
	void Move()
	{
		if(GetCurrentSymbol()!=END)//��ֹ���
			dotPosition++;//�ƶ����λ��
	}
	string GetCurrentSymbol()
	{
		if(dotPosition<right.size())
			return right[dotPosition];
		return END;//�������ֹ��
	}
	string GetNextSymbol()
	{
		if(dotPosition+1<right.size())
			return right[dotPosition+1];
		return END;//�������ֹ��
	}
	void printItem()
	{
		printf("%s ---> ",left.c_str());
		for(int i=0;i<right.size();i++)
		{
			if(dotPosition==i)
				printf(".");
			printf("%s ",right[i].c_str());
		}
		printf(",%s\n",lookaheadSymbol.c_str());
	}
};



struct PDAstate{//PDA״̬
	int id;
	vector<Item> itemSet;

	struct PDAedge{//PDA��
	vector<PDAstate* > nextState;//ָ����һ״̬
	vector<PDAstate* > frontState;//ָ��ǰһ״̬
	string symbol;
	};

	vector<PDAedge> edges;
};

struct PDA{
	vector<PDAstate> states;
};


set <string> Terminal;//�ս����
set <string> NonTerminal;//���ս����
//set <Item>	 Producers;//����ʽ��
vector <Item>	 Producers;//����ʽ��  ʹ��set��������
PDA myPDA;


struct Op{
	string name;
	int priority;
	string type;
};
vector<Op> Operators;

set<string> First(string input)
{
	set<string> ret;
	if(input==END)return ret;
	if(Terminal.count(input)!=0)
	{
		ret.insert(input);
		return ret;
	}
	for(auto i=Producers.begin();i!=Producers.end();i++)
	{
		if(i->left==input)
		{
			if(Terminal.count(i->right[0])!=0)
				ret.insert(i->right[0]);//�����ǰinput���Ҳ���һ�����ս�� ��ô��������
			else
			{
				set<string> res=First(i->right[0]);//�ݹ��ѯfirst
				for(auto j=res.begin();j!=res.end();j++)
				{
					ret.insert(*j);//���µ�first�ϲ����������
				}
			}
		}
	}
	return ret;
}

set<string> FirstDouble(string input0,string input1)
{
	set<string> ret;
	string input="";
	if(input0==END)
		input=input1;
	else
		input=input0;
	ret = First(input);
	return ret;
}

bool ItemContained(vector<Item> input0,Item input1)
{
	for(int i=0;i<input0.size();i++)
	{
		if(input0[i].left==input1.left&&input0[i].dotPosition==input1.dotPosition&&input0[i].right.size()==input1.right.size()&&input0[i].lookaheadSymbol==input1.lookaheadSymbol)
		{
			bool flag=true;
			for(int j=0;j<input0[i].right.size();j++)
			{
				if(input0[i].right[j]!=input1.right[j]){flag=false;break;}
			}
			if(flag==true)
				return true;
		}
	}
	return false;
}

void Closure(PDAstate& input)
{//����ע����Ҫ��I�в�ͣ��������Ҫ���ǰ�����ε����Ƿ��ٴβ����µĲ���ʽ��������
	vector<Item> I=input.itemSet;
	int lastSize;
	do
	{
		lastSize=I.size();//��¼I�Ĵ�С ��������
		string A;
		string alpha,B,beta;
		string a;
		for(int itemIndex=0;itemIndex<I.size();itemIndex++)//(auto eachItem=I.begin();eachItem!=I.end();eachItem++)
		{
			Item eachItem=I[itemIndex];
			B=eachItem.GetCurrentSymbol();//B�ǵ�ǰ��Ŀ�ĵ���ָ�����ΪEND��ʾΪ��
			beta=eachItem.GetNextSymbol();//beta��B����ķ���
			a=eachItem.lookaheadSymbol;
			//�������ҵ�������B�����Ĳ���ʽ
			if(B!=END)
			{
				//����������B��Ϊ�󲿵Ĳ���ʽ
				for(auto eachp=Producers.begin();eachp!=Producers.end();eachp++)
				{
					if(eachp->left==B)
					{
						set<string> firstBetAlp=FirstDouble(beta,a);
						//printf("[+]Size of first(%s,%s)=%d\n",beta.c_str(),a.c_str(),firstBetAlp.size());
						for(auto eachfirst=firstBetAlp.begin();eachfirst!=firstBetAlp.end();eachfirst++)
						{
							Item toAddItem;
							toAddItem.left=eachp->left;
							toAddItem.right=eachp->right;
							toAddItem.dotPosition=0;
							toAddItem.lookaheadSymbol=*eachfirst;
							//���²����Ĳ���ʽ���뵽����I��
							cout<<"Add new item to I:";
							toAddItem.printItem();
							//�жϵ�ǰ��I���Ƿ����toAddItem
							if(!ItemContained(I,toAddItem))
								I.push_back(toAddItem);
						}
					}
				}
			}
		}
	}
	while(I.size()!=lastSize);
	input.itemSet=I;
}

void ParseYaccFile()
{
	string srcFile = "D:\\test.y";//Yacc�ļ�
	
	ifile.open(srcFile.c_str(), ios::in);
	ofile.open("D:\\generatedYacc.h", ios::out);
	if (!ifile.is_open()) {
		cerr << "[-]Error occured when open file!" << endl;
		exit(1);
	}
	char c=0x00;
	while(c!='%'&&!ifile.eof())
	{
		//һֱ��ȡ����һ��%
		c = ifile.get();
	}
	if(c=='%')//��ȡ���˵�һ��%
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
	//��ʼ�����ս���������
	string lineStr;
	char tempLineCStr[500];
	endState=false;
	int currentPriority=0;
	while(!endState)
	{
		ifile.getline(tempLineCStr,500);
		lineStr=tempLineCStr;
		//ɾ��ע�Ͳ���
		int annoteIndex=lineStr.find("/*");
		if(annoteIndex!=-1)
			lineStr=lineStr.substr(0,annoteIndex);
		//Ĭ��ÿһ�е���Ч��ʶ�������׿�ʼ
		if(lineStr.find("%%")!=string::npos)
		{
			//�ҵ�%%��ʶ�� ˵������ν��� ����
			endState=true;
		}
		else
		{
			string markWord="";//�����洢���׵ı�ʶ��
			char toSplit[200];
			strcpy(toSplit,lineStr.c_str());
			char *toke;
			char *nextToke;
			if(lineStr[0]=='%')//������ǿ��� ����д���
			{//toke�洢�������׵ı�Ƿ� ��%token %type %nonassoc %left
				toke=strtok_s(toSplit,"\t \n",&nextToke);
				//std::cout<<"[toke]"<<toke<<"[+]\n";
				if(strcmp(toke,"%token")==0)//���м�¼����token
				{
					while(toke!=0)
					{
						toke=strtok_s(0,"\t \n",&nextToke);
						if(toke!=0)
						{//�洢ÿһ��terminal��������
							string tempToken=toke;
							Terminal.insert(tempToken);
						}
					}
				}
				else if(strcmp(toke,"%nonassoc")==0)//���м�¼����nonassoc
				{
					while(toke!=0)
					{
						toke=strtok_s(0,"\t \n",&nextToke);
						if(toke!=0)
						{//�洢ÿһ��nonassoc op��Operators��
							Op newOp;
							newOp.name=toke;
							newOp.priority=currentPriority;
							newOp.type="nonassoc";
							Operators.push_back(newOp);
						}
					}
					currentPriority++;
				}
				else if(strcmp(toke,"%left")==0)//���м�¼����nonassoc
				{
					while(toke!=0)
					{
						toke=strtok_s(0,"\t \n",&nextToke);
						if(toke!=0)
						{//�洢ÿһ��nonassoc op��Operators��
							Op newOp;
							newOp.name=toke;
							newOp.priority=currentPriority;
							newOp.type="left";
							Operators.push_back(newOp);
						}
					}
					currentPriority++;
				}
				else if(strcmp(toke,"%type")==0)//���м�¼����type
				{
				}
				else if(strcmp(toke,"%union")==0)//���м�¼����type
				{
				}
			}
		}
	}
	//��ʼ���������
	endState=false;
	bool newLeftMark=true;
	string tempLeft="";
	vector<string> tempRight;
	while(!endState)
	{
		ifile.getline(tempLineCStr,500);
		lineStr=tempLineCStr;
		//ɾ��ע�Ͳ���
		int annoteIndex=lineStr.find("/*");
		if(annoteIndex!=-1)
			lineStr=lineStr.substr(0,annoteIndex);
		lineStr=lineStr+'\0';
		//Ĭ��ÿһ�е���Ч��ʶ�������׿�ʼ
		if(lineStr.find("%%")!=-1)
		{
			//�ҵ�%%��ʶ�� ˵������ν��� ����
			endState=true;
		}
		else
		{
			char toSplit[200];
			strcpy(toSplit,lineStr.c_str());
			char *toke=NULL;
			char *nextToke=NULL;
			//���ҵ�ǰ�Ƿ������  ����� ˵���������µ���
			if(lineStr.find(":")!=-1)
			{
				newLeftMark=true;//��¼��ǰ�������µ���
			}
			//debugprint("%s\n",lineStr.c_str());
			toke=strtok_s(toSplit,"\t \n",&nextToke);
			while(toke!=NULL)
			{
				if(newLeftMark)//�����������leftMark ����Ȼtoke����
				{
					newLeftMark=false;
					tempLeft=toke;
					if(Terminal.count(string(toke))==0)//�����ǰ��toke�����ս�� ��ôһ���Ƿ��ս��
						NonTerminal.insert(toke);
					toke=strtok_s(NULL,"\t \n",&nextToke);
					continue;
				}
				//����toke���Ҳ����� ��
				else if(string(toke)==":")
				{
					//���� �� ���� | ���� ��ֱ�ӿ��
					toke=strtok_s(NULL,"\t \n",&nextToke);
					continue;
				}
				else if(string(toke)=="|"||string(toke)==";")
				{//��ʱ��һ������ʽ���Ҳ��Ѿ��ɼ����
					toke=strtok_s(NULL,"\t \n",&nextToke);
					Item tempProducer;
					tempProducer.dotPosition=0;
					tempProducer.left=tempLeft;
					for(int i=0;i<tempRight.size();i++)
					{
						tempProducer.right.push_back(tempRight[i]);
					}
					Producers.push_back(tempProducer);//���շ��ֵĲ���ʽ���뵽Producer��
					tempRight.clear();//�����һ�ε��Ҳ�
				}
				else//��ǰ���������Ҳ�
				{
					if(string(toke).find("{")==-1)//�ų����嶯��SemantAction
					{
						tempRight.push_back(string(toke));
						if(Terminal.count(string(toke))==0)//�����ǰ��toke�����ս�� ��ôһ���Ƿ��ս��
							NonTerminal.insert(toke);
					}
					toke=strtok_s(NULL,"\t \n",&nextToke);
				}
			}
		}
	}

	//��ʼ�����ӳ����

	//������ֽ��
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
	
}
void GeneratePDA()
{
	//Ĭ��Producer�д�ŵĵ�0������ʽ���ǳ�ʼ����ʽ С��bug
	PDAstate initState;//���ó�ʼ״̬
	Item item0;
	int stateIndex=0;
	Terminal.insert("$");//���ս���в��������
	//���ó�ʼ״̬
	initState.id=stateIndex++;
	if(Producers.size()==0)return;//�����ǰ���в���ʽ �ǾͲ�����
	Producers[0].lookaheadSymbol="$";//�׸����ż���$��Ϊ�ս��ʾ
	initState.itemSet.push_back(Producers[0]);//����0������ʽ�����ʼ״̬��
	Closure(initState);
	debugprint("[+]After closure ,s0 contain:\n");
	for(auto i=initState.itemSet.begin();i!=initState.itemSet.end();i++)
		i->printItem();
	debugprint("[+]Generate PDA test finish\n");
}

void main()
{
	//Step1. ��ȡyacc�ļ�
	ParseYaccFile();
	//Step2. ���������Զ���
	GeneratePDA();
	//Step3. 
	std::cout<<"Done\n";
}


void main_()
{
	string lineStr="abc";
	lineStr=("ff");
	cout<<"[]"<<lineStr<<"[]"<<endl;
	/*
	string firstEle="S";
	set<string> res=First(firstEle);
	printf("First(%s) = ",firstEle.c_str());
	for(auto i=res.begin();i!=res.end();i++)
		cout<<*i<<"\t";
	cout<<endl;
	*/	
}