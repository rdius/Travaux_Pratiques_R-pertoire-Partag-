#include <vector>
#include <list>
#include <map>
#include <set>
#include <deque>
#include <queue>
#include <stack>
#include <string>
#include <bitset>
#include <algorithm>
#include <functional>
#include <numeric>
#include <utility>
#include <sstream>
#include <iostream>
#include <iomanip>
#include <cstdio>
#include <cmath>
#include <cstdlib>
#include <ctime>
#include <cstring>
#include <climits>
#include <stdlib.h>
#include <stdio.h>
using namespace std;
#define REP(i,n) for(int i=0; i<n; i++)
#define FOR(i,st,end) for(int i=st;i<end;i++)
#define db(x) cout << (#x) << " = " << x << endl;
#define mp make_pair
#define pb push_back
#define mod 1000003
int parent[20005];
typedef long long int ll;

queue<int>q;
int temp,currentState;
int value[20005];
void solve(int n){
	q.push(1);
	parent[1]=0;
	while(!q.empty()){
		currentState=q.front();
		q.pop();
		if(currentState==0){
			stack<int> s;
			while(parent[currentState]){
				s.push(value[currentState]);
				currentState=parent[currentState];
			}
			s.push(1);
			while(!s.empty()){
				printf("%d",s.top());
				s.pop();
			}
			printf("\n");
			break;
		}
		temp=(currentState*10)%n;
		
		if(parent[temp]==-1){
			q.push(temp);
			parent[temp]=currentState;
			value[temp]=0;
		}
		temp=currentState*10+1;
		temp%=n;
		if(parent[temp]==-1){
			q.push(temp);
			parent[temp]=currentState;
			value[temp]=1;
		}
	}
}

int main(){
	int t,n;
	scanf("%d",&t);
	while(t--){
		while(!q.empty()){
			q.pop();
		}
		REP(i,20000)
			parent[i]=-1;
			
		scanf("%d",&n);
		solve(n);
	}
}