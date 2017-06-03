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
#include <stack>
#include "mainHeader.h"
#include <cstddef>
#include<time.h>
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
	bool Move()
	{
		if(GetCurrentSymbol()!=END)//��ֹ���
		{
			dotPosition++;//�ƶ����λ��
			return true;//�ƶ��ɹ�  ����true
		}
		else
			return false;//�Ѿ�������β��  ����false
	}
	string GetCurrentSymbol()
	{
		if((uint32_t)dotPosition<right.size())
			return right[dotPosition];
		return END;//�������ֹ��
	}
	string GetNextSymbol()
	{
		if((uint32_t)dotPosition+1<right.size())
			return right[dotPosition+1];
		return END;//�������ֹ��
	}
	void printItem()
	{
		printf("%s ---> ",left.c_str());
		for(uint32_t i=0;i<right.size();i++)
		{
			if(dotPosition==i)
				printf(".");
			printf("%s ",right[i].c_str());
		}
		if(dotPosition==right.size())
			printf(". ");
		printf(",%s\n",lookaheadSymbol.c_str());
	}
};

struct PDAedge{//PDA��
		int nextState;//ָ����һ״̬
		string symbol;
	};

struct PDAstate{//PDA״̬
	int id;
	vector<Item> itemSet;

	vector<PDAedge> edges;
	bool ContainEdge(PDAedge input)
	{
		for(u32 i=0;i<edges.size();i++)
		{
			if(edges[i].nextState==input.nextState&&edges[i].symbol==input.symbol)
				return true;
		}
		return false;
	}
	void printState()
	{
		printf("StateID:%d\n",id);
		for(uint32_t i=0;i<itemSet.size();i++)
		{
				cout<<"\t";
				itemSet[i].printItem();
		}
		for(uint32_t i=0;i<edges.size();i++)
		{
			printf("(%d) ---%s---> (%d)\n",id,edges[i].symbol.c_str(),edges[i].nextState);
		}
		printf("\n");
	}
};

struct PDA{
	vector<PDAstate> states;
	void printState()
	{
		for(u32 i=0;i<states.size();i++)
			states[i].printState();
	}
};


set <string> Terminal;//�ս����
set <string> NonTerminal;//���ս����
//set <Item>	 Producers;//����ʽ��
vector <Item>	 Producers;//����ʽ��  ʹ��set��������
PDA globalPDA;
map<string,set<string>> FirstCollection;
struct Element{
		string type;
		int value;
	};
vector<string> TerminalVec;
vector<string> NonTerminalVec;
map<string,int> TerminalIndexMap;
map<string,int> NonTerminalIndexMap;


struct Op{
	string name;
	int priority;
	string type;
};
vector<Op> Operators;


set<string> First(string input)
{
	set<string> ret;
	if(Terminal.count(input)!=0)
		ret.insert(input);
	else
		ret=FirstCollection[input];
	return ret;
}
/*
set<string> First_(string input)
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
*/
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
void PrintFirst(string input)
{
	set<string> res=First(input);
	printf("First(%s)= {",input.c_str());
	for(auto i=res.begin();i!=res.end();i++)
	{
		cout<<*i<<",";
	}
	cout<<"}\n";
}

void GenerateAllFirstWithOutRecursion()
{
	int totalSize=0;
	int lastTotalSize=0;
	printf("Begin generate\n");
	//���ȳ�ʼ��ÿ�����ս����SetΪ��
	for(auto nonTer=NonTerminal.begin();nonTer!=NonTerminal.end();nonTer++)
		FirstCollection[*nonTer]=set<string>();
	//������ʼ
	do{
		lastTotalSize=totalSize;
		totalSize=0;
		for(auto producer=Producers.begin();producer!=Producers.end();producer++)
		{
			//����Ƿ����ս��
			if(Terminal.count(producer->right[0]))
				FirstCollection[producer->left].insert(producer->right[0]);//����Ҳ���һ�����ս���������
			else//�Ƿ��ս�� ��ô�����ս��X��First(x)��������
			{
				for(auto eachFirst=FirstCollection[producer->right[0]].begin();eachFirst!=FirstCollection[producer->right[0]].end();eachFirst++)
				{
					FirstCollection[producer->left].insert(*eachFirst);//���Է���ÿһ��first���뵽����
				}
			}
			totalSize+=FirstCollection[producer->left].size();
		}
	}
	while(totalSize!=lastTotalSize);
	printf("End generate\n");
}
//��ʱ��
bool ItemContained(vector<Item> input0,Item input1)
{
	for(uint32_t i=0;i<input0.size();i++)
	{
		if(input0[i].left==input1.left&&input0[i].dotPosition==input1.dotPosition&&input0[i].right.size()==input1.right.size()&&input0[i].lookaheadSymbol==input1.lookaheadSymbol)
		{
			bool flag=true;
			for(uint32_t j=0;j<input0[i].right.size();j++)
			{
				if(input0[i].right[j]!=input1.right[j]){flag=false;break;}
			}
			if(flag==true)
				return true;
		}
	}
	return false;
}
int GetProducerIndex(Item input1)
{
	for(uint32_t i=0;i<Producers.size();i++)
	{
		if(Producers[i].left==input1.left&&Producers[i].right.size()==input1.right.size())
		{
			bool flag=true;
			for(uint32_t j=0;j<Producers[i].right.size();j++)
			{
				if(Producers[i].right[j]!=input1.right[j])
				{
					flag=false;break;
				}
			}
			if(flag==true)
				return i;
		}
	}
	return -1;
}
bool StateEqual(PDAstate A,PDAstate B)//�ж�����״̬�Ƿ���ȣ����ʱ�临�Ӷȡ�����
{
	if(A.itemSet.size()!=B.itemSet.size())
		return false;//�����С����ͬ  ����״̬�϶�����ͬ
	for(int i=0;i<B.itemSet.size();i++)
	{
		if(!ItemContained(A.itemSet,B.itemSet[i]))//����κ�һ��B�е�״̬��������A����ô�϶������
			return false;
	}
	return true;
}
int PDAstateContained(PDA input0,PDAstate input1)//�ж�һ��PDAstate�Ƿ������PDA��
{
	for(int i=0;i<input0.states.size();i++)
	{
		//�ж�ÿ��״̬�Ƿ��뵱ǰ���״̬��ͬ
		if(StateEqual(input0.states[i],input1))
			return i;//ֻҪinput0����һ��״̬��input1״̬��ȣ���ôinput1�Ͱ�����input0��
	}
	return -1;
}

map<string,vector<Item>> GlobalProducerMap;
void GenerateProducerMap()
{
	cout<<"Test begin";
	for(auto i=NonTerminal.begin();i!=NonTerminal.end();i++)
	{
		GlobalProducerMap[*i]=vector<Item>();
	}
	for(auto pro=Producers.begin();pro!=Producers.end();pro++)
	{
		GlobalProducerMap[pro->left].push_back(*pro);
	}
	for(auto i=GlobalProducerMap.begin();i!=GlobalProducerMap.end();i++)
	{
		for(int j=0;j<i->second.size();j++)
			i->second[j].printItem();
	}
	cout<<"Test end";
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
		for(uint32_t itemIndex=0;itemIndex<I.size();itemIndex++)//(auto eachItem=I.begin();eachItem!=I.end();eachItem++)
		{
			Item eachItem=I[itemIndex];
			B=eachItem.GetCurrentSymbol();//B�ǵ�ǰ��Ŀ�ĵ���ָ�����ΪEND��ʾΪ��
			beta=eachItem.GetNextSymbol();//beta��B����ķ���
			a=eachItem.lookaheadSymbol;
			//�������ҵ�������B�����Ĳ���ʽ
			if(B!=END)
			{
				vector<Item> eachPs=GlobalProducerMap[B];
				for(auto eachp=eachPs.begin();eachp!=eachPs.end();eachp++)
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
							//cout<<"Add new item to I:";
							//toAddItem.printItem();
							//�жϵ�ǰ��I���Ƿ����toAddItem
							if(!ItemContained(I,toAddItem))
								I.push_back(toAddItem);
						}
				}

				/*
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
							//cout<<"Add new item to I:";
							//toAddItem.printItem();
							//�жϵ�ǰ��I���Ƿ����toAddItem
							if(!ItemContained(I,toAddItem))
								I.push_back(toAddItem);
						}
					}
				}

				*/
			}
		}
	}
	while(I.size()!=lastSize);//����˴εĴ�С���ϴδ�С��ͬ ˵�����ٲ����µ�produce ��������
	input.itemSet=I;
}
PDAstate GOTO(PDAstate I,string X)
{
	PDAstate J;//��J��ʼ��Ϊ�ռ�
	for(uint32_t itemIndex=0;itemIndex<I.itemSet.size();itemIndex++)//(auto eachItem=I.begin();eachItem!=I.end();eachItem++)
	{//����I�е�ÿ���[A->alpha. X beta,a]
		//����[A->alpha X .beta,a]���뵽J��
		Item eachItem=I.itemSet[itemIndex];
		if(eachItem.GetCurrentSymbol()==X)
		{
			eachItem.Move();//�����ǰ����X ��ô�ƶ��㵽����
			J.itemSet.push_back(eachItem);//������뵽J��
		}
	}
	
	Closure(J);//��J���бհ�����
	
	//debugprint("[=]");
	return J;
}
void ParseYaccFile(string filename)
{
	string srcFile = filename; //Yacc�ļ�
	
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
							//�������������ս������
							Terminal.insert(string(toke));//new add
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

		annoteIndex=lineStr.find("{#");//����Ƿ������嶯��
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
					for(uint32_t i=0;i<tempRight.size();i++)
					{
						tempProducer.right.push_back(tempRight[i]);
					}
					Producers.push_back(tempProducer);//���շ��ֵĲ���ʽ���뵽Producer��
					tempRight.clear();//�����һ�ε��Ҳ�
				}
				else//��ǰ���������Ҳ�
				{
					//if(string(toke).find("{")==-1)//�ų����嶯��SemantAction
					//{
						tempRight.push_back(string(toke));
						if(Terminal.count(string(toke))==0)//�����ǰ��toke�����ս�� ��ôһ���Ƿ��ս��
							NonTerminal.insert(toke);
					//}
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
	for(uint32_t i=0;i<Producers.size();i++)
	{
		cout<<"("<<Producers[i].left<<") ----> ";
		for(uint32_t j=0;j<Producers[i].right.size();j++)
			cout<<"("<<Producers[i].right[j]<<") ";
		cout<<endl;
	}
	std::cout<<"\n[+]Print Operator\n";
	for(uint32_t i=0;i<Operators.size();i++)
	{
		std::cout<<Operators[i].name<<"\t"<<Operators[i].type<<"\t"<<Operators[i].priority<<"\t"<<endl;
	}
	ifile.close();
	ofile.close();

	//Ϊ�˷������  ��set<string> �� terminal �� nonterminal ת�� vector<string>
	Terminal.insert("$");//���ս���в��������
	GenerateProducerMap();//����Producer����
	GenerateAllFirstWithOutRecursion();//�������з��ս����First
	for(auto i=Terminal.begin();i!=Terminal.end();i++)
	{
		TerminalIndexMap[*i]=TerminalVec.size();
		TerminalVec.push_back(*i);
	}
	for(auto i=NonTerminal.begin();i!=NonTerminal.end();i++)
	{
		NonTerminalIndexMap[*i]=NonTerminalVec.size();
		NonTerminalVec.push_back(*i);
	}

}
PDA GeneratePDA()
{
	PDA myPDA;
	//Ĭ��Producer�д�ŵĵ�0������ʽ���ǳ�ʼ����ʽ С��bug
	PDAstate S0;//���ó�ʼ״̬
	Item item0;
	int globalStateId=0;
	
	//���ó�ʼ״̬
	S0.id=globalStateId++;
	if(Producers.size()==0)return myPDA;//�����ǰ���в���ʽ �ǾͲ�����
	Producers[0].lookaheadSymbol="$";//�׸����ż���$��Ϊ�ս��ʾ
	S0.itemSet.push_back(Producers[0]);//����0������ʽ�����ʼ״̬��
	Closure(S0);
	myPDA.states.push_back(S0);
	debugprint("[+]***************PDA BEGIN*****************\n");

	uint32_t lastSize=myPDA.states.size();
	do
	{
		lastSize=myPDA.states.size();
		for(int i=0;i<myPDA.states.size();i++)
		{
			//����ÿһ��״̬��ʵʩGOTO X ��Ϊ
			PDAstate newPDAstate;
			cout<<globalStateId<<"\t"<<i<<"\n";
			//�����ǶԵ�ǰ��״̬ʩ�л����ս���ͷ��ս����GOTO����
			//��� GOTO�Ľ���ǿղ���Ŀǰ��״̬���в����� ��ô������Ϊһ����״̬
			//cout<<"1";
			
			
			for(auto ter=Terminal.begin();ter!=Terminal.end();ter++)
			{
				
				newPDAstate=GOTO(myPDA.states[i],*ter);
				
				if(newPDAstate.itemSet.size()!=0)
				{
					int stateId=PDAstateContained(myPDA,newPDAstate);
					if(stateId==-1)//�Ҳ������״̬��˵������״̬
					{
						newPDAstate.id=globalStateId++;
						myPDA.states.push_back(newPDAstate);
						//Ϊ����µ�״̬������
						PDAedge newEdge;
						newEdge.nextState=newPDAstate.id;//�±�ָ���״̬�ĺ���
						newEdge.symbol=*ter;//�±��ϵķ��ű��
						myPDA.states[i].edges.push_back(newEdge);
					}
					else
					{
						PDAedge newEdge;
						newEdge.nextState=stateId;//�±�ָ���״̬�ĺ���
						newEdge.symbol=*ter;//�±��ϵķ��ű��
						if(myPDA.states[i].ContainEdge(newEdge)==false)//��������������� �ӽ�ȥ
							myPDA.states[i].edges.push_back(newEdge);
					}
				}
			}
			
			for(auto ter=NonTerminal.begin();ter!=NonTerminal.end();ter++)
			{
				newPDAstate=GOTO(myPDA.states[i],*ter);
				if(newPDAstate.itemSet.size()!=0)
				{
					int stateId=PDAstateContained(myPDA,newPDAstate);
					if(stateId==-1)//�Ҳ������״̬��˵������״̬
					{
						newPDAstate.id=globalStateId++;
						myPDA.states.push_back(newPDAstate);
						//Ϊ����µ�״̬������
						PDAedge newEdge;
						newEdge.nextState=newPDAstate.id;//�±�ָ���״̬�ĺ���
						newEdge.symbol=*ter;//�±��ϵķ��ű��
						myPDA.states[i].edges.push_back(newEdge);
					}
					else
					{
						PDAedge newEdge;
						newEdge.nextState=stateId;//�±�ָ���״̬�ĺ���
						newEdge.symbol=*ter;//�±��ϵķ��ű��
						if(myPDA.states[i].ContainEdge(newEdge)==false)//��������������� �ӽ�ȥ
							myPDA.states[i].edges.push_back(newEdge);
					}
				}
			}
			//cout<<" S";
			//cout<<endl<<endl<<endl;
		}
	}
	while(myPDA.states.size()!=lastSize);//�ظ�����һֱ��PDA�Ĵ�С���ٱ仯
	//myPDA.printState();
	debugprint("\n[+]Generate PDA test finish\n");
	return myPDA;
}
void GenerateAnalaysingTable(PDA & myPDA,vector<vector<Element>> &ACTIONtable,vector<vector<Element>>& GOTOtable,string filename="Table.txt")
{
	map<string,string> ElementType;
	map<string,int> ElementValue;
	
	int stateRows=myPDA.states.size();
	int actionCols=Terminal.size();
	int gotoCols=NonTerminal.size();
	
	ofstream ofile(filename.c_str(),ios::out);
	
	//���ȼ�¼S',ȷ����һ������ʽ�ǿ�ʼ����һ��
	string startSymbol=Producers[0].left;
	
	for(int row=0;row<stateRows;row++)
	{
		//��ÿ��״̬����ACTION �� GOTO������
		//������ACTION��
		vector<Element> tempActionRow(actionCols);
		vector<Element> tempGotoRow(gotoCols);

		for(int colAction=0;colAction<TerminalVec.size();colAction++)
		{
			//�����������Ԫ��Ϊ��error
			tempActionRow[colAction].type="ERROR";
			tempActionRow[colAction].value=0;
		}
		for(int i=0;i<myPDA.states[row].edges.size();i++)
		{
			//�����е�����������뵽���� Sj
			string terSym=myPDA.states[row].edges[i].symbol;
			if(Terminal.count(terSym)!=0)
			{
				int terIndex=TerminalIndexMap[terSym];
				tempActionRow[terIndex].type="S";
				tempActionRow[terIndex].value=myPDA.states[row].edges[i].nextState;
			}
		}
		for(int i=0;i<myPDA.states[row].itemSet.size();i++)
		{
			//�����еĹ�Լ�������뵽���� Rj
			Item thisItem=myPDA.states[row].itemSet[i];
			if(thisItem.GetCurrentSymbol()==END)
			{
				//��ǰ��һ��ﵽ��ĩβ
				if(thisItem.left==startSymbol)
				{//�����S' ��ô���Ϊaccept
					int terIndex=TerminalIndexMap["$"];//��ֹ�����±�
					tempActionRow[terIndex].type="ACC";
					tempActionRow[terIndex].value=0;
				}
				else
				{
					//�������ǰ���Item����ʽ��Producer�еڼ���
					int itemIndex=GetProducerIndex(thisItem);
					int terIndex=TerminalIndexMap[thisItem.lookaheadSymbol];
					tempActionRow[terIndex].type="R";
					tempActionRow[terIndex].value=itemIndex;
				}
			}
		}
		//���濪ʼ����GOTO��
		for(int colGoto=0;colGoto<NonTerminalVec.size();colGoto++)
		{
			//�����������Ԫ��Ϊ��error
			tempGotoRow[colGoto].type="ERROR";
			tempGotoRow[colGoto].value=0;
		}
	
		for(int i=0;i<myPDA.states[row].edges.size();i++)
		{
			//�����е�GOTO�������뵽���� j
			string nonTerSym=myPDA.states[row].edges[i].symbol;
			if(NonTerminal.count(nonTerSym)!=0)
			{
				int nonTerIndex=NonTerminalIndexMap[nonTerSym];
				tempGotoRow[nonTerIndex].type="GOTO";
				tempGotoRow[nonTerIndex].value=myPDA.states[row].edges[i].nextState;
			}
		}
		ACTIONtable.push_back(tempActionRow);
		GOTOtable.push_back(tempGotoRow);
	}
	//��飡
	//��ӡ���ű�
	//���ȴ�ӡACTION table
	printf("\n****************ACTION table******************\n");
	printf(" \t");
	int rows=ACTIONtable.size();
	int colOfActionTable=ACTIONtable[0].size();
	int colOfGotoTable=GOTOtable[0].size();
	ofile<<rows<<"\t"<<colOfActionTable<<"\t"<<colOfGotoTable<<"\n";//�洢���ļ���
	ofile<<"ACTION\n";

	/*//��ӡ�ս��
	for(int i=0;i<TerminalVec.size();i++)
		printf("  %s \t",TerminalVec[i].c_str());
	printf("\n");
	*/
	for(int i=0;i<ACTIONtable.size();i++)
	{
		//printf("%d:\t",i);
		for(int j=0;j<ACTIONtable[i].size();j++)
		{
			ofile<<ACTIONtable[i][j].type<<" "<<ACTIONtable[i][j].value<<"\t";
			/*
			if(ACTIONtable[i][j].type=="ERROR")
				printf("(    )\t");
			else if(ACTIONtable[i][j].type=="ACC")
				printf("(%s )\t",ACTIONtable[i][j].type.c_str());
			else
				printf("(%s%d  )\t",ACTIONtable[i][j].type.c_str(),ACTIONtable[i][j].value);
				*/
		}
		ofile<<"\n";
		//printf("\n");
	}
	//��ӡGOTO table
	ofile<<"GOTO\n";
	printf("\n****************GOTO table******************\n");
	//printf(" \t");
	//for(int i=0;i<NonTerminalVec.size();i++)
	//	printf("  %s \t",NonTerminalVec[i].c_str());
	//printf("\n");
	for(int i=0;i<GOTOtable.size();i++)
	{
		//printf("%d:\t",i);
		for(int j=0;j<GOTOtable[i].size();j++)
		{
			ofile<<GOTOtable[i][j].type<<" "<<GOTOtable[i][j].value<<"\t";
			/*
			if(GOTOtable[i][j].type=="ERROR")
				printf("(   )\t");
			else
				printf("( %d )\t",GOTOtable[i][j].value);
				*/
		}
		ofile<<"\n";
		//printf("\n");
	}

}
void GenerateAnalaysingTableFromFile(vector<vector<Element>> &ACTIONtable,vector<vector<Element>>& GOTOtable,string filename)
{
	ifstream ifile;
	ifile.open(filename.c_str(), ios::in);
	int rows=0;
	int colOfActionTable=0;
	int colOfGotoTable=0;
	ifile>>rows>>colOfActionTable>>colOfGotoTable;//���ļ���ȡ���ڴ�
	//printf("rows=%d\ncolOfActionTable=%d\ncolofGotoTable=%d\n",rows,colOfActionTable,colOfGotoTable);
	string segmentType;
	ifile>>segmentType;
	printf("[read file]*************ACTION TABLE*************\n");
	for(int row=0;row<rows;row++)
	{
		string type="";
		int value=0;
		Element tuple;
		vector<Element> tuples;
		for(int col=0;col<colOfActionTable;col++)
		{
			ifile>>type>>value;
			//printf("(%s,%d)\t",type.c_str(),value);
			tuple.type=type;
			tuple.value=value;
			tuples.push_back(tuple);
		}
		ACTIONtable.push_back(tuples);
		//printf("\n");
	}
	printf("[read file]*************GOTO TABLE*************\n");
	ifile>>segmentType;//�ָ���
	for(int row=0;row<rows;row++)
	{
		string type="";
		int value=0;
		Element tuple;
		vector<Element> tuples;
		for(int col=0;col<colOfGotoTable;col++)
		{
			ifile>>type>>value;
			//printf("(%s,%d)\t",type.c_str(),value);
			tuple.type=type;
			tuple.value=value;
			tuples.push_back(tuple);
		}
		GOTOtable.push_back(tuples);
		//printf("\n");
	}
	ifile.close();

}
void GrammarRun(vector<vector<Element>> ACTIONtable,vector<vector<Element>> GOTOtable,vector<string> grammarInput)
{
	string a;//���w$�е�һ������
	stack<int> stateStack;//���״̬��ջ
//#define DOLLAR -1
	//stateStack.push(DOLLAR);
	stateStack.push(0);//���׸�״̬ѹ��ջ��ע����ܻ��bug
	int inputPointer=0;
	a=grammarInput[inputPointer++];//��aΪ�����ַ�������λ
	int t;
	printf("\n[+]************Grammar Test Begin**************\n");
	printf("Input token serials= ");
	for(int i=0;i<grammarInput.size();i++)
		cout<<grammarInput[i]<<" ";
	printf("\n[+]Print reduce prodctions\n");
	while(1)
	{
		int s=stateStack.top();//��s��ջ����״̬
		if(s==-1){}//ע�������и�dollar����
		Element tempAction=ACTIONtable[s][TerminalIndexMap[a]];
		if(tempAction.type=="S")//���ACTION[s,a]Ϊ����t
		{
			t=tempAction.value;
			stateStack.push(t);//��tѹ��ջ��
			if(inputPointer<grammarInput.size())
			{
				a=grammarInput[inputPointer++];//��aΪ��һ���������
			}
			else
			{
				printf("[-]Grammar error!\n");
				break;
			}
		}
		else if(tempAction.type=="R")//���ACTION[s,a]Ϊ��ԼA->beta
		{
			int value=tempAction.value;
			int lengthOfBeta=Producers[value].right.size();
			bool errorFlag=false;
			for(int i=0;i<lengthOfBeta;i++)
			{
				if(stateStack.size()!=0)
					stateStack.pop();//��ջ�е���|beta|������
				else
				{
					errorFlag=true;
					break;
				}
			}
			if(errorFlag)
			{
				printf("[-]Grammar error!\n");
				break;
			}
		    t=stateStack.top();//��tΪ��ǰջ����״̬
			Element tempGoto=GOTOtable[t][NonTerminalIndexMap[Producers[value].left]];//tempGoto=GOTO[t,A]
			stateStack.push(tempGoto.value);//��GOTO[t,A]ѹ��ջ��
			Producers[value].printItem();//���A->beta�Ĳ���ʽ
		}
		else if(tempAction.type=="ACC")//����״̬
		{
			printf("[+]Grammar correct.Accept!");
			break;//�﷨�������
		}
		else
		{
			printf("[-]Grammar error!\n");
			break;
		}
	}
	printf("\n[+]GrammarDone!\n");
}

vector<string> SplitString(const string &s, const string &seperator){
    vector<string> result;
    typedef string::size_type string_size;
    string_size i = 0;
     
    while(i != s.size()){
        //�ҵ��ַ������׸������ڷָ�������ĸ��
        int flag = 0;
        while(i != s.size() && flag == 0){
            flag = 1;
            for(string_size x = 0; x < seperator.size(); ++x)
               if(s[i] == seperator[x]){
                   ++i;
                   flag= 0;
                    break;
                  }
		}
         
        //�ҵ���һ���ָ������������ָ���֮����ַ���ȡ����
        flag = 0;
        string_size j = i;
        while(j != s.size() && flag == 0){
            for(string_size x = 0; x < seperator.size(); ++x)
              if(s[j] == seperator[x]){
                   flag = 1;
                break;
                }
            if(flag == 0)
            ++j;
        }
        if(i != j){
            result.push_back(s.substr(i, j-i));
            i = j;
        }
    }
    return result;
}

void main()//��һ�����У�������Table�ļ�
{
	//��������﷨�����ַ���
	string input="VOID IDENTIFIER '(' VOID ')' '{'   '}' $";
	vector<string> testString=SplitString(input," \t\n");
	//Step1. ��ȡyacc�ļ�
	ParseYaccFile("D:\\myyacc.y");
	//Step2. ���������Զ���
	PDA myPDA=GeneratePDA();
	//Step3. ���ɷ�����
	vector<vector<Element>> ACTIONtable;
	vector<vector<Element>> GOTOtable;
	GenerateAnalaysingTable(myPDA,ACTIONtable,GOTOtable,"D:\\myyacc.y.Table.txt");
	//Step4. ����LR��������
	GrammarRun(ACTIONtable,GOTOtable,testString);
	std::cout<<"**********************Done*******************\n";
}



void main_()//�����й������ļ�֮��  ����ֱ�����ж�ȡ�ļ��е�Table
{
	//��������﷨�����ַ���
	//testString.push_back("id");
	//testString.push_back("=");
	string input="VOID IDENTIFIER '(' VOID ')' '{'  INT IDENTIFIER ';'  '}' $";
	vector<string> testString=SplitString(input," \t\n");
	//Step1. ��ȡyacc�ļ�
	ParseYaccFile("D:\\myyacc.y");
	//Step2. ���������Զ���
	//PDA myPDA=GeneratePDA();
	//Step3. ���ɷ�����
	vector<vector<Element>> ACTIONtable;
	vector<vector<Element>> GOTOtable;
	GenerateAnalaysingTableFromFile(ACTIONtable,GOTOtable,"D:\\myyacc.y.Table.txt");
	//Step4. ����LR��������
	GrammarRun(ACTIONtable,GOTOtable,testString);
	std::cout<<"**********************Done*******************\n";

}
