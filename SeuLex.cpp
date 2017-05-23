#include <iostream>
#include <vector>
#include <set>
#include <stack>
#include <map>
#include <stdio.h>
#include <queue>
#include <list>


using namespace std;
#define debug_print printf

char Epsilon=0xff;
int summary=0;
int dfa_summary=0;
set<int> SetDiff(set<int> A,set<int> B);
struct NFAnode {
   int label;
   int acceptstatetag; //�Ƿ�Ϊ����״̬
   bool visited;
   struct {
     char symbol; //���ϵ��ַ�
	 bool edge_visited;
     struct NFAnode *nextnode;//��һ״̬�ڵ� 
   } next_edge1, next_edge2;//ÿ��״̬�ڵ������������������һ�ڵ�
 };
struct NFA  {
    NFAnode *start;
    NFAnode *end;
};


//���¶����dfa�ڵ�
struct DFAedge{bool edge_visited;char symbol;struct DFAnode *nextnode;};
struct DFAnode{
	set<int> origin_state_set;
	bool marked;
	int  label;
	bool acceptstatetag;
	bool visited;
	vector<DFAedge> next_edge;
};
class DFAstates{
public:
	static int static_summary_label;
	vector<DFAnode*> merge_states_list;
	DFAstates(){};
	void mark(set<int> T)
	{
		for(int i=0;i<merge_states_list.size();i++)
		{
			if(SetDiff(merge_states_list[i]->origin_state_set,T).size()==0)
			{
				merge_states_list[i]->marked=true;
				return;
			}
		}
	}
	void AddConnection(set<int> T,char c,set<int> U)
	{
		//T----c---->U
		//�����ҵ�T��Ӧ��DFAnode��Ȼ���ҵ�U���node��Ȼ��Tָ��U
		for(int i=0;i<merge_states_list.size();i++)
		{
			if(SetDiff(merge_states_list[i]->origin_state_set,T).size()==0)
			{
				for(int j=0;j<merge_states_list.size();j++)
				{
					if(SetDiff(merge_states_list[j]->origin_state_set,U).size()==0)
					{
						DFAedge new_edge;
						new_edge.symbol=c;
						new_edge.nextnode=merge_states_list[j];
						new_edge.edge_visited=false;
						merge_states_list[i]->next_edge.push_back(new_edge);
						return;
					}
				}
			}
		}
	}
	set<int> OutFalseSet()
	{
		set<int> result;
		vector<DFAnode*>::iterator it;
		for(it=merge_states_list.begin();it!=merge_states_list.end();it++)
		{
			
			if((*it)->marked==false)
			{
				result=(*it)->origin_state_set;
				//it=merge_states_list.erase(it);Ӧ�ò���ɾ��
				return result;
			}
		}
		return result;
	}
	bool Contain(set<int> U)
	{
		vector<DFAnode*>::iterator it;
		for(it=merge_states_list.begin();it!=merge_states_list.end();it++)
		{
			if(SetDiff((*it)->origin_state_set,U).size()==0)
			{
				return true;
			}
		}
		return false;
	}
	void Add(set<int> A,bool marked)
	{
		set<int> tmpSet;
		tmpSet=A;
		DFAnode *tmpNode=new DFAnode;
		tmpNode->origin_state_set=tmpSet;
		tmpNode->marked=marked;
		tmpNode->label=static_summary_label;
		tmpNode->visited=false;
		static_summary_label++;
		merge_states_list.push_back(tmpNode);
	}
	void printContent()
	{
		vector<DFAnode*>::iterator it;
		for(it=merge_states_list.begin();it!=merge_states_list.end();it++)
		{
			printf("DFA label:%d  Marked:%d  States size:%d\n",(*it)->label,(*it)->marked,(*it)->origin_state_set.size());
		}
	}
	
};

void visit_nfa_nodes(NFAnode *start)
{
	queue<NFAnode*> waitForVisit;
	waitForVisit.push(start);
	NFAnode *tmpNode;
	while(waitForVisit.size()!=0)
	{
		tmpNode=waitForVisit.front();
		waitForVisit.pop();
		tmpNode->visited=true;
		if(tmpNode->next_edge1.nextnode!=NULL&&tmpNode->next_edge1.edge_visited==false)
		{
			waitForVisit.push(tmpNode->next_edge1.nextnode);
			tmpNode->next_edge1.edge_visited=true;
			printf("S%d ----%c----> S%d\n",tmpNode->label,tmpNode->next_edge1.symbol,tmpNode->next_edge1.nextnode->label);
		}
		if(tmpNode->next_edge2.nextnode!=NULL&&tmpNode->next_edge2.edge_visited==false)
		{
			waitForVisit.push(tmpNode->next_edge2.nextnode);
			tmpNode->next_edge2.edge_visited=true;
			printf("S%d ----%c----> S%d\n",tmpNode->label,tmpNode->next_edge2.symbol,tmpNode->next_edge2.nextnode->label);
		}
	}
}
int DFAstates::static_summary_label=0;
NFAnode create_new_nfa_node(int symbolInput=-1)//�����µ�nfa�ڵ�
{
    NFAnode new_NFA_node;
    summary+=1;
    if(symbolInput==-1)
        new_NFA_node.label=summary;//summary��ͣ���� ��֤��ÿ��nfa�ڵ��label��һ��
    else
        new_NFA_node.label=symbolInput;
    new_NFA_node.acceptstatetag=0;//��ʼ��Ϊ�ǽ���״̬
    new_NFA_node.next_edge1.symbol='\0';
    new_NFA_node.next_edge1.nextnode=NULL;
	new_NFA_node.next_edge1.edge_visited=false;
    new_NFA_node.next_edge2.symbol='\0';
    new_NFA_node.next_edge2.nextnode=NULL;
	new_NFA_node.next_edge2.edge_visited=false;
	new_NFA_node.visited=false;
    return new_NFA_node;
}
void create_new_nfa_edge(NFAnode& origin_node,char symbolInput,NFAnode &nextnode)
{
	if(origin_node.next_edge1.symbol=='\0')
	{
		origin_node.next_edge1.symbol=symbolInput;
		origin_node.next_edge1.nextnode=&nextnode;
	}
	else if(origin_node.next_edge2.symbol=='\0')
	{
		origin_node.next_edge2.symbol=symbolInput;
		origin_node.next_edge2.nextnode=&nextnode;
	}	
	else
	{
		debug_print("[-]Warning:nfa node edges full!Insert action stopped!\n"); 
	}
}

void reset_nfa_visit_state(NFAnode *start)
{
	queue<NFAnode*> waitForVisit;
	waitForVisit.push(start);
	NFAnode *tmpNode;
	while(waitForVisit.size()!=0)
	{
		tmpNode=waitForVisit.front();
		waitForVisit.pop();
		tmpNode->visited=false;
		if(tmpNode->next_edge1.nextnode!=NULL&&tmpNode->next_edge1.edge_visited==true)
		{
			waitForVisit.push(tmpNode->next_edge1.nextnode);
			tmpNode->next_edge1.edge_visited=false;
		}
		if(tmpNode->next_edge2.nextnode!=NULL&&tmpNode->next_edge2.edge_visited==true)
		{
			waitForVisit.push(tmpNode->next_edge2.nextnode);
			tmpNode->next_edge2.edge_visited=false;
		}
	}
}
set<char> get_nfa_edge_symbols(NFAnode *start)
{
	set<char> result;
	queue<NFAnode*> waitForVisit;
	waitForVisit.push(start);
	NFAnode *tmpNode;
	while(waitForVisit.size()!=0)
	{
		tmpNode=waitForVisit.front();
		waitForVisit.pop();
		tmpNode->visited=true;
		if(tmpNode->next_edge1.nextnode!=NULL&&tmpNode->next_edge1.edge_visited==false)
		{
			waitForVisit.push(tmpNode->next_edge1.nextnode);
			tmpNode->next_edge1.edge_visited=true;
			if(tmpNode->next_edge1.symbol!=Epsilon)
			result.insert(tmpNode->next_edge1.symbol);
		}
		if(tmpNode->next_edge2.nextnode!=NULL&&tmpNode->next_edge2.edge_visited==false)
		{
			waitForVisit.push(tmpNode->next_edge2.nextnode);
			tmpNode->next_edge2.edge_visited=true;
			if(tmpNode->next_edge2.symbol!=Epsilon)
			result.insert(tmpNode->next_edge2.symbol);
		}
	}
	reset_nfa_visit_state(start);
	return result;
}
set<int> SetDiff(set<int> A,set<int> B){  //������Ҫ������Բ������
    set<int>::iterator it;  
	set<int> result;
    it = A.begin();  
    while(it != A.end()){  
        if(B.find(*it) == B.end()) result.insert(*it);  
        it++;  
    }  
	it = B.begin();  
    while(it != B.end()){  
        if(A.find(*it) == A.end()) result.insert(*it);  
        it++;  
    }  
	return result;
}  
map<int, NFAnode*> NFANodeStateMap(NFAnode *start)
{
    map<int, NFAnode*> state_nodes; //�±���nfa״̬�ڵ�Ķ�Ӧ
    queue<NFAnode*> waitForVisit;
    waitForVisit.push(start);
    NFAnode *tmpNode;
    while(waitForVisit.size()!=0)
    {
        //ȡ������ͷ���Ľڵ�
        tmpNode=waitForVisit.front();
        waitForVisit.pop();
        tmpNode->visited=true;
        state_nodes.insert(pair<int, NFAnode*>(tmpNode->label,tmpNode));
        //��Ҫ�����������·�Ƿ��нڵ���� �Լ��ýڵ��Ƿ񱻷��ʹ�
		if(tmpNode->next_edge1.nextnode!=NULL&&tmpNode->next_edge1.nextnode->visited==false)
        {
            waitForVisit.push(tmpNode->next_edge1.nextnode);//ѹ����У��Ƚ��ȳ�
            tmpNode->next_edge1.nextnode->visited=true;//���ýڵ�Ϊ�ѷ���״̬
        }
        if(tmpNode->next_edge2.nextnode!=NULL&&tmpNode->next_edge2.nextnode->visited==false)
        {
            waitForVisit.push(tmpNode->next_edge2.nextnode);//ѹ����У��Ƚ��ȳ�
            tmpNode->next_edge2.nextnode->visited=true;//���ýڵ�Ϊ�ѷ���״̬
        }
    }
    return state_nodes;
}
map<int,DFAnode*> DFANodeStateMap(DFAnode *start)
{
	map<int, DFAnode*> state_nodes; //�±���nfa״̬�ڵ�Ķ�Ӧ
    queue<DFAnode*> waitForVisit;
    waitForVisit.push(start);
    DFAnode *tmpNode;
    while(waitForVisit.size()!=0)
    {
        //ȡ������ͷ���Ľڵ�
        tmpNode=waitForVisit.front();
        waitForVisit.pop();
        tmpNode->visited=true;
        state_nodes.insert(pair<int, DFAnode*>(tmpNode->label,tmpNode));
        //��Ҫ������г��Ƚڵ��Ƿ��нڵ���� �Լ��ýڵ��Ƿ񱻷��ʹ�
		for(int i=0;i<tmpNode->next_edge.size();i++)
		{
			if(tmpNode->next_edge[i].nextnode!=NULL&&tmpNode->next_edge[i].nextnode->visited==false)
			{

				waitForVisit.push(tmpNode->next_edge[i].nextnode);
			}
		}
    }
    return state_nodes;
}
set<int> EpsilonClosure(set<int> T,map<int, NFAnode*>node_map)
{
	//node_map�д洢��label_index��NFAnode��ӳ��
    stack<int> states_stack;
	set<int> result=T;
    //����T��������ת����stack
    for(set<int>::iterator set_iter=T.begin();set_iter!=T.end();set_iter++)
		states_stack.push(*set_iter);
    while(!states_stack.empty())//��ջ��Ԫ�طǿյ�ʱ��
    {
		int t=states_stack.top();
		states_stack.pop();
		//�ҵ�������״̬t�����ı�ΪEpsilon��״̬u
		map<int, NFAnode*>::iterator iters;
		iters=node_map.find(t);
		//��ȡ״̬��Ϊt����Ӧ��nfa�ڵ�ָ��,������ܻ�������ָ��Ĵ���,��ҪС��
		NFAnode *tmpnode=iters->second;
		if(tmpnode->next_edge1.symbol==Epsilon)
		{
			if(result.count(tmpnode->next_edge1.nextnode->label)==0)//�����ǰ��epsilon���ӽڵ㲻��T��
			{
				result.insert(tmpnode->next_edge1.nextnode->label);//����ǰ��epsilon���ӽڵ���뵽�������
				states_stack.push(tmpnode->next_edge1.nextnode->label);//����ǰ��epsilon���ӽڵ�ѹ��ջ��
			}
		}
		if(tmpnode->next_edge2.symbol==Epsilon)
		{
			if(result.count(tmpnode->next_edge2.nextnode->label)==0)//�����ǰ��epsilon���ӽڵ㲻��T��
			{
				result.insert(tmpnode->next_edge2.nextnode->label);//����ǰ��epsilon���ӽڵ���뵽�������
				states_stack.push(tmpnode->next_edge2.nextnode->label);//����ǰ��epsilon���ӽڵ�ѹ��ջ��
			}
		}
    }
	return result;
}
set<int> Move(set<int> T,char a,map<int, NFAnode*>node_map)
{
	set<int> result;
	set<int>::iterator it;
    for(it=T.begin();it!=T.end();it++) 
	{
		if(node_map[*it]->next_edge1.symbol==a)
			result.insert(node_map[*it]->next_edge1.nextnode->label);
		if(node_map[*it]->next_edge2.symbol==a)
			result.insert(node_map[*it]->next_edge2.nextnode->label);
	}
	return result;
}
void debug_printset(set<int> T)
{
	set<int>::iterator it;
	cout<<"{";
	for(it=T.begin();it!=T.end();it++)
	{
		cout<<*it<<" ";
	}
	cout<<"} ";
}
DFAstates NFA2DFA(NFAnode *start)
{
	DFAstates D_States;
	map<int,NFAnode*> nfa_node_map=NFANodeStateMap(start);
	set<char> input=get_nfa_edge_symbols(start);
	set<int> start_set;start_set.insert(start->label);//������һ��nfa�ڵ��״̬����ʵ���Ͼ���һ������
	set<int> A=EpsilonClosure(start_set,nfa_node_map);
	D_States.Add(A,false);
	set<int> T;
	T=D_States.OutFalseSet();//��ͣ�Ĵ�dfa������ȡ����ǰ���Ϊfalse�Ľڵ�
	while(T.size()!=0)
	{
		set<char>::iterator c;
		D_States.mark(T);
		for(c=input.begin();c!=input.end();c++)
		{
			printf("Move('%c',",*c);debug_printset(T);cout<<")";
			set<int> tmpMove=Move(T,*c,nfa_node_map);
			cout<<" Resluts in:";debug_printset(tmpMove);cout<<endl;
			set<int> U=EpsilonClosure(tmpMove,nfa_node_map);
			printf("Epsilon(");debug_printset(tmpMove);cout<<")";cout<<"Create new DFA:";debug_printset(U);cout<<endl;

			if(U.size()!=0)
			{
				if(D_States.Contain(U)==false)
				{
					D_States.Add(U,false);
				}
				D_States.AddConnection(T,*c,U);//���Ӻ��� ����·�� T----c---->U
				debug_printset(T);cout<<"->";debug_printset(U);cout<<" on "<<*c<<endl;
			}

		}
		T=D_States.OutFalseSet();
	}
	return D_States;
}
vector<int> GetDFAedgePointedState(DFAnode *A,char symbol)
{//��ȡһ��dfa�ڵ��Ƿ�ͨ��ĳ������ָ����һ��dfa�ڵ�
	vector<int> result(2);
	for(int i=0;i<A->next_edge.size();i++)
	{
		if(A->next_edge[i].symbol==symbol)
		{
			result[0]=i;
			result[1]=A->next_edge[i].nextnode->label;
			return result;
		}
	}
	result[0]=-1;
	result[1]=-1;
	return result;
}
vector<int> GetDFAedgePointedEdge(DFAnode *A,DFAnode *B)
{//��ȡһ��dfa�ڵ��Ƿ�ͨ��ĳ������ָ����һ��dfa�ڵ�
	vector<int> result(2);
	for(int i=0;i<A->next_edge.size();i++)
	{
		if(A->next_edge[i].nextnode->label==B->label)
		{
			result[0]=i;
			result[1]=A->next_edge[i].symbol;
			return result;
		}
	}
	result[0]=-1;
	result[1]=-1;
	return result;
}
DFAstates MinimizeDFA(DFAstates &A)
{
	DFAstates result;
	map<int,set<int>> reverseStateSet;
	map<int,DFAnode*> DFAnodeMap=DFANodeStateMap(A.merge_states_list[0]);
	set<int> nonAcceptNode,acceptNode;
	set<set<int>> groupSet;
	bool partionContinue=false;
	for(map<int,DFAnode*>::iterator it=DFAnodeMap.begin();it!=DFAnodeMap.end();it++)
	{
		if(it->second->next_edge.size()==0)//����Ϊ0��˵���϶��ǽ���̬�ڵ�
			acceptNode.insert(it->first);
		else if(it->second->next_edge.size()==1&&it->second->next_edge[0].nextnode->label==it->second->label)//ָ�����һ�ڵ�Ϊ�Լ���ʱ�� Ҳ˵���ǽ���̬
			acceptNode.insert(it->first);
		else
			nonAcceptNode.insert(it->first);
		for(int j=0;j<it->second->next_edge.size();j++)
		{
			//�ҵ����е�ĳ����Լ�֮���ǰ���
			if(it->second->next_edge[j].nextnode->label!=it->second->label)
			{
				if(reverseStateSet.count(it->second->next_edge[j].nextnode->label)!=0)
				{
					reverseStateSet.at(it->second->next_edge[j].nextnode->label).insert(it->second->label);
				}
				else
				{
					set<int> tmpset;
					tmpset.insert(it->second->label);
					reverseStateSet.insert(pair<int, set<int>>(it->second->next_edge[j].nextnode->label, tmpset));
				}
			}
		}
	}
 	groupSet.insert(nonAcceptNode);
	groupSet.insert(acceptNode);
	
	do
	{
		partionContinue=false;
		stack<set<int>> tmpStack;
		set<set<int>>::iterator group=groupSet.begin();
		for(;group!=groupSet.end();group++)
		{
			bool outloop=false;
			if(group->size()<=1)
				continue;
			set<int>::iterator i=group->begin();
			for(;i!=group->end();i++)
			{
				int s=*i;
				DFAnode *tmpDFAnode=DFAnodeMap[s];
				for(int j=0;j<tmpDFAnode->next_edge.size();j++)
				{
					//�����ǰ״̬����������ǰ�ڵ����һ״̬�ڵ�
					if(group->count(tmpDFAnode->next_edge[j].nextnode->label)==0)
					{
						//add new group set
						set<int> newGroup;
						newGroup.insert(s);
						tmpStack.push(newGroup);
						partionContinue=true;

						//remove i;
						set<int> newOriginalGroup=*group;
						newOriginalGroup.erase(s);
						groupSet.erase(group);
						groupSet.insert(newOriginalGroup);
						outloop=true;
						break;
					}
				}
				if(outloop)break;
			}
			if(outloop)break;
		}
		while(!tmpStack.empty())
		{
			groupSet.insert(tmpStack.top());
			tmpStack.pop();
		}
	}
	while(partionContinue);

	set<set<int>>::iterator groupSetIt;
	for(groupSetIt=groupSet.begin();groupSetIt!=groupSet.end();groupSetIt++)
	{
		//���������ֹһ�� ˵����Ҫ�ϲ�
		//������ʽת�� 
		if(groupSetIt->size()>1)
		{
			DFAnode *newDFAnode=new DFAnode;
			for(set<int>::iterator groupIt=groupSetIt->begin();groupIt!=groupSetIt->end();groupIt++)
			{
				//���ںϲ�����ÿ���ڵ� ��ȡ��ǰ��ͺ���ڵ� ��ָ���µĽڵ���
				//groupIt�洢��ǰ��Ҫ���滻����״̬��
				//frontStatesΪǰ�����нڵ�״̬��
				DFAnode *currentNode=DFAnodeMap.at(*groupIt);
				newDFAnode->label=currentNode->label;
				set<int> frontStates=reverseStateSet.at(currentNode->label);
				//������ǰ��ڵ��ָ���������µĽڵ�
				for(set<int>::iterator frontStateIt=frontStates.begin();frontStateIt!=frontStates.end();frontStateIt++)
				{
					//�ҵ���������ָ���˵�ǰ�ȴ�ɾ���ĵ�
					vector<int> tmpVec=GetDFAedgePointedEdge(DFAnodeMap.at(*frontStateIt),currentNode);
					if(tmpVec[0]!=-1)
					{
						//����ǰ��ڵ��ĳ���ߵ���һ��
						//�ж��´����Ľڵ����Ƿ��Ѿ����������ǰ��ڵ�
						vector<int> tmpVec2=GetDFAedgePointedEdge(DFAnodeMap.at(*frontStateIt),newDFAnode);
						if(tmpVec2[0]!=-1)
						{
							//˵����ǰ��ڵ㻹δ��ָ���´����Ľڵ�,�����ԭָ��
							DFAnodeMap.at(*frontStateIt)->next_edge[tmpVec2[0]].nextnode=newDFAnode;
						}
					}
				}
				//�����е�ԭ�нڵ�ĺ���ڵ��������½ڵ��ָ��
				for(int j=0;j<currentNode->next_edge.size();j++)
				{
					//�����ǰ�ڵ��ĳ����ָ��Ĳ����Լ� ���Ҹ�ָ�����һ�ڵ���δ�������½ڵ���
					if(currentNode->next_edge[j].nextnode->label!=currentNode->label)
					{
						vector<int> tmpVec=GetDFAedgePointedEdge(newDFAnode,currentNode->next_edge[j].nextnode);
						if(tmpVec[0]==-1)//˵���½ڵ���δ���ӵ�����ڵ�
						{
							DFAedge newEdge;
							newEdge.nextnode=currentNode->next_edge[j].nextnode;
							newEdge.edge_visited=false;
							newEdge.symbol=currentNode->next_edge[j].symbol;
							newDFAnode->next_edge.push_back(newEdge);
						}
					}
					else
					{
						//���ԭ�нڵ�ı�ָ�������� ��ô�µı�ҲҪָ������
						vector<int> tmpVec=GetDFAedgePointedEdge(newDFAnode,newDFAnode);
						if(tmpVec[0]==-1)
						{//��ֹ�ظ�ָ���Լ�
							DFAedge newEdge;
							newEdge.nextnode=newDFAnode;
							newEdge.symbol=currentNode->next_edge[j].symbol;
							newDFAnode->next_edge.push_back(newEdge);
						}
					}
				}
			}
			result.merge_states_list.push_back(newDFAnode);
		}
		else
		{
			set<int>::iterator it=groupSetIt->begin();
			result.merge_states_list.push_back(DFAnodeMap.at(*it));
		}
	}
	

	return result;
}

DFAstates MinimizeDFA2(DFAstates &A)
{
	DFAstates result;
	map<int,set<int>> reverseStateSet;
	map<int,DFAnode*> DFAnodeMap=DFANodeStateMap(A.merge_states_list[0]);
	set<int> nonAcceptNode,acceptNode;
	set<set<int>> groupSet;
	bool partionContinue=false;
	for(map<int,DFAnode*>::iterator it=DFAnodeMap.begin();it!=DFAnodeMap.end();it++)
	{
		if(it->second->next_edge.size()==0)//����Ϊ0��˵���϶��ǽ���̬�ڵ�
			acceptNode.insert(it->first);
		else if(it->second->next_edge.size()==1&&it->second->next_edge[0].nextnode->label==it->second->label)//ָ�����һ�ڵ�Ϊ�Լ���ʱ�� Ҳ˵���ǽ���̬
			acceptNode.insert(it->first);
		else
			nonAcceptNode.insert(it->first);
		for(int j=0;j<it->second->next_edge.size();j++)
		{
			//�ҵ����е�ĳ����Լ�֮���ǰ���
			if(it->second->next_edge[j].nextnode->label!=it->second->label)
			{
				if(reverseStateSet.count(it->second->next_edge[j].nextnode->label)!=0)
				{
					reverseStateSet.at(it->second->next_edge[j].nextnode->label).insert(it->second->label);
				}
				else
				{
					set<int> tmpset;
					tmpset.insert(it->second->label);
					reverseStateSet.insert(pair<int, set<int>>(it->second->next_edge[j].nextnode->label, tmpset));
				}
			}
		}
	}
 	groupSet.insert(nonAcceptNode);
	groupSet.insert(acceptNode);
	
	do
	{
		partionContinue=false;
		stack<set<int>> tmpStack;
		set<set<int>>::iterator group=groupSet.begin();
		for(;group!=groupSet.end();group++)
		{
			bool outloop=false;
			if(group->size()<=1)
				continue;
			set<int>::iterator i=group->begin();
			for(;i!=group->end();i++)
			{
				int s=*i;
				DFAnode *tmpDFAnode=DFAnodeMap[s];
				for(int j=0;j<tmpDFAnode->next_edge.size();j++)
				{
					//�����ǰ״̬����������ǰ�ڵ����һ״̬�ڵ�
					if(group->count(tmpDFAnode->next_edge[j].nextnode->label)==0)
					{
						//add new group set
						set<int> newGroup;
						newGroup.insert(s);
						tmpStack.push(newGroup);
						partionContinue=true;

						//remove i;
						set<int> newOriginalGroup=*group;
						newOriginalGroup.erase(s);
						groupSet.erase(group);
						groupSet.insert(newOriginalGroup);
						outloop=true;
						break;
					}
				}
				if(outloop)break;
			}
			if(outloop)break;
		}
		while(!tmpStack.empty())
		{
			groupSet.insert(tmpStack.top());
			tmpStack.pop();
		}
	}
	while(partionContinue);

	set<set<int>>::iterator groupSetIt;
	for(groupSetIt=groupSet.begin();groupSetIt!=groupSet.end();groupSetIt++)
	{
		//���������ֹһ�� ˵����Ҫ�ϲ�
		//������ʽת�� 
		if(groupSetIt->size()>1)
		{
			DFAnode *newDFAnode=new DFAnode;
			for(set<int>::iterator groupIt=groupSetIt->begin();groupIt!=groupSetIt->end();groupIt++)
			{
				//���ںϲ�����ÿ���ڵ� ��ȡ��ǰ��ͺ���ڵ� ��ָ���µĽڵ���
				//groupIt�洢��ǰ��Ҫ���滻����״̬��
				//frontStatesΪǰ�����нڵ�״̬��
				DFAnode *currentNode=DFAnodeMap.at(*groupIt);
				newDFAnode->label=currentNode->label;
				set<int> frontStates=reverseStateSet.at(currentNode->label);
				//������ǰ��ڵ��ָ���������µĽڵ�
				for(set<int>::iterator frontStateIt=frontStates.begin();frontStateIt!=frontStates.end();frontStateIt++)
				{
					//�ҵ���������ָ������
					for(int j=0;j<DFAnodeMap.at(*frontStateIt)->next_edge.size();j++)
					{
						if(DFAnodeMap.at(*frontStateIt)->next_edge[j].nextnode->label==currentNode->label)
						{
							//�ı䵱ǰ�����ߵ�ָ��
							DFAnodeMap.at(*frontStateIt)->next_edge[j].nextnode=newDFAnode;
							break;
						}
					}
				}
				//�����е�ԭ�нڵ�ĺ���ڵ��������½ڵ��ָ��
				for(int j=0;j<currentNode->next_edge.size();j++)
				{
					if(currentNode->next_edge[j].nextnode->label!=currentNode->label)
					{
						DFAedge newEdge=currentNode->next_edge[j];
						newDFAnode->next_edge.push_back(newEdge);
					}
					else
					{
						//���ԭ�нڵ�ı�ָ�������� ��ô�µı�ҲҪָ������
						DFAedge newEdge;
						newEdge.nextnode=newDFAnode;
						newDFAnode->next_edge.push_back(newEdge);
					}
				}
			}
			result.merge_states_list.push_back(newDFAnode);
		}
		else
		{
			set<int>::iterator it=groupSetIt->begin();
			result.merge_states_list.push_back(DFAnodeMap.at(*it));
		}
	}
	

	return result;
}

int main()
{
    vector<int> a;
    printf("NFA test begin\n");
    NFA nfa1;
    NFAnode s0,s1,s2,s3,s4,s5,s7;
   
    s1=create_new_nfa_node(1);
    s2=create_new_nfa_node(2);
    s3=create_new_nfa_node(3);
    s4=create_new_nfa_node(4);
    s5=create_new_nfa_node(5);

	s3.acceptstatetag=1;
	s5.acceptstatetag=1;
	create_new_nfa_edge(s1,Epsilon,s2);
	create_new_nfa_edge(s1,Epsilon,s4);
	create_new_nfa_edge(s2,'a',s3);
	create_new_nfa_edge(s3,'b',s3);
	create_new_nfa_edge(s4,'a',s4);
	create_new_nfa_edge(s4,'b',s5);
	printf("Start\n");
	DFAstates dfa0= NFA2DFA(&s1);
	MinimizeDFA(dfa0);


    return 0;
}                                          