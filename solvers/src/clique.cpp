// verified
// http://codeforces.com/contest/839/problem/E
// https://abc002.contest.atcoder.jp/tasks/abc002_4

#include <bits/stdc++.h>
      
#define FOR(i,a,b) for(ll i = (a); i < (ll)(b); i++)
#define REP(i,n) FOR(i,0,n)
#define YYS(x,arr) for(auto& x:arr)

#define pb emplace_back

using namespace std;

using ll = long long int;

ll cnt;

struct MaxClique{
  vector<vector<int>> G;
  vector<int> cand;
  int n;
  void init(int arg_n){
    n = arg_n;
    G.assign(n, vector<int>(0));
  }
  void add_edge(int a, int b){
    G[a].pb(b);
    G[b].pb(a);
  }
  vector<int> diff(vector<int> a, vector<int> b){
    int ib = 0;
    vector<int> res(0);
    for(int x: a){
      while(ib < b.size() && b[ib] < x){
        ib++;
      }
      if(ib == b.size() || x != b[ib]){
        res.push_back(x);
      }
    }
    return res;
  }
  void push_back_set(vector<int> &a, int x){
    if(a.empty() || a.back() != x){
      a.push_back(x);
    }
  }
  vector<int> uni(vector<int> a, vector<int> b){
    int ib = 0;
    vector<int> res(0);
    for(int x: a){
      while(ib < b.size() && b[ib] < x){
        push_back_set(res, b[ib]);
        ib++;
      }
      push_back_set(res, x);
    }
    for(int i = ib; i < b.size(); i++){
      push_back_set(res, b[i]);
    }
    return res;
  }
  vector<int> intersect(vector<int> a, vector<int> b){
    int ib = 0;
    vector<int> res(0);
    for(int x: a){
      while(ib < b.size() && b[ib] < x){
        ib++;
      }
      if(ib < b.size() && x == b[ib]){
        res.push_back(x);
      }
    }
    return res;
  }
  void dfs(vector<int> R, vector<int> P, vector<int> X){
    cnt++;
    if(P.empty()){
      if(X.empty() && R.size() > cand.size()){
        cand = R;
      }
      return;
    }
    if(R.size() + P.size() <= cand.size()){
      return;
    }
    int piv = P[0];
    for(int v: P){
      if(diff(P, G[v]).size() < diff(P, G[piv]).size()){
        piv = v;
      }
    }
    vector<int> search = diff(P, G[piv]);
    for(int v: search){
      vector<int> nR = uni(R, {v});
      vector<int> nP = intersect(P, G[v]);
      vector<int> nX = intersect(X, G[v]);
      dfs(nR, nP, nX);
      P = diff(P, {v});
      X = uni(X, {v});
    }
  }
  vector<int> max_clique(){
    cand.clear();
    vector<int> R(0);
    vector<int> P(0);
    vector<int> X(0);
    for(int i = 0; i < n; i++){
      P.push_back(i);
    }
    for(int i = 0; i < n; i++){
      sort(G[i].begin(), G[i].end());
    }
    dfs(R, P, X);
    return cand;
  }
};

MaxClique mc;

int n, m;

int main(){

  cin >> n >> m;
  mc.init(n);
  REP(i, m){
    int a, b;
    cin >> a >> b;
    mc.add_edge(a, b);
  }

  mc.max_clique();
  
  cout << cnt << endl;

  return 0;
}
