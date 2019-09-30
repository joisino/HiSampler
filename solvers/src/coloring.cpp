#include <bits/stdc++.h>
      
#define FOR(i,a,b) for(ll i = (a); i < (ll)(b); i++)
#define REP(i,n) FOR(i,0,n)
#define YYS(x,arr) for(auto& x:arr)

#define pb emplace_back

using namespace std;

using ll = long long int;

ll cnt;

struct ThreeColor{
  using T = tuple<int, int, int>;
  vector<vector<int>> G;
  int n;
  vector<vector<int>> cand;
  vector<int> col;
  vector<int> deg;
  set<T> rank;
  void init(int arg_n){
    n = arg_n;
    G.assign(n, vector<int>(0));
  }
  void add_edge(int a, int b){
    G[a].pb(b);
    G[b].pb(a);
  }
  int pop(){
    if(rank.empty()){
      return -1;
    }
    return get<2>(*rank.begin());
  }
  bool check(int x, int i){
    YYS(w, G[x]){
      if(col[w] == i){
        return false;
      }
    }
    return true;
  }
  T construct_T(int x){
    int cand_rest = max(cand[x][0], 0) + max(cand[x][1], 0) + max(cand[x][2], 0);
    return T(cand_rest, -deg[x], x);
  }
  void fill(int x, int i){
    rank.erase(rank.find(construct_T(x)));
    col[x] = i;
    YYS(w, G[x]){
      if(col[w] == -1){
        rank.erase(rank.find(construct_T(w)));
      }
      deg[w]--;
      cand[w][i]--;
      if(col[w] == -1){
        rank.insert(construct_T(w));
      }
    }
  }
  void release(int x){
    YYS(w, G[x]){
      if(col[w] == -1){
        rank.erase(rank.find(construct_T(w)));
      }
      deg[w]++;
      cand[w][col[x]]++;
      if(col[w] == -1){
        rank.insert(construct_T(w));
      }
    }
    col[x] = -1;
    rank.insert(construct_T(x));
  }
  bool dfs(){
    cnt++;
    int x = pop();
    if(x == -1){
      return true;
    }
    REP(i, 3){
      if(cand[x][i] == 1 && check(x, i)){
        fill(x, i);
        bool res = dfs();
        if(res){
          return true;
        }
        release(x);
      }
    }
    return false;
  }
  bool check(){
    cand.assign(n, vector<int>(3, 1));
    col.assign(n, -1);
    deg.resize(n);
    REP(i, n){
      deg[i] = G[i].size();
      rank.insert(T(3, -deg[i], i));
    }
    REP(c, 2){
      int x = pop();
      if(x == -1){
        return true;
      }
      fill(x, c);
    }
    return dfs();
  }
};

ThreeColor tc;

int n, m;

vector<vector<int>> G;
set<int> ss;

vector<bool> used;

void dfs(int x){
  ss.insert(x);
  used[x] = true;
  YYS(w, G[x]){
    if(!used[w]){
      dfs(w);
    }
  }
}

int main(){

  cin >> n >> m;
  G.assign(n, vector<int>(0));
  REP(i, m){
    int a, b;
    cin >> a >> b;
    G[a].pb(b);
    G[b].pb(a);
  }

  used.assign(n, false);
  
  REP(i, n){
    if(!used[i]){
      ss.clear();
      dfs(i);
      map<int, int> ma;
      int c = 0;
      YYS(w, ss){
        ma[w] = c++;
      }
      tc.init(c);
      YYS(w, ss){
        YYS(u, G[w]){
          if(ma[w] < ma[u]){
            tc.add_edge(ma[w], ma[u]);
          }
        }
      }
      bool f = tc.check();
      if(!f){
        cout << cnt << endl;
        return 0;
      }
    }
  }

  cout << cnt << endl;

  return 0;
}
