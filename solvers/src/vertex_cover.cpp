#include <bits/stdc++.h>
      
#define FOR(i,a,b) for(ll i = (a); i < (ll)(b); i++)
#define REP(i,n) FOR(i,0,n)
#define YYS(x,arr) for(auto& x:arr)

#define pb emplace_back

using namespace std;

using ll = long long int;

ll cnt;

struct VertexCover{
  vector<vector<int>> G;
  vector<int> cand;
  int n;
  void init(int arg_n){
    n = arg_n;
    G.assign(n, vector<int>(0));
  }
  void add_edge(int a, int b){
    G[a].emplace_back(b);
    G[b].emplace_back(a);
  }
  template<class T>
  vector<T> diff(vector<T> a, vector<T> b){
    int ib = 0;
    vector<T> res(0);
    for(T x: a){
      while(ib < b.size() && b[ib] < x){
        ib++;
      }
      if(ib == b.size() || x != b[ib]){
        res.push_back(x);
      }
    }
    return res;
  }
  template<class T>
  void push_back_set(vector<T> &a, T x){
    if(a.empty() || a.back() != x){
      a.push_back(x);
    }
  }
  template<class T>
  vector<T> uni(vector<T> a, vector<T> b){
    int ib = 0;
    vector<T> res(0);
    for(T x: a){
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
  template<class T>
  vector<T> intersect(vector<T> a, vector<T> b){
    int ib = 0;
    vector<T> res(0);
    for(T x: a){
      while(ib < b.size() && b[ib] < x){
        ib++;
      }
      if(ib < b.size() && x == b[ib]){
        res.push_back(x);
      }
    }
    return res;
  }
  vector<pair<int, int>> incident(int a){
    vector<pair<int, int>> res(0);
    for(int v: G[a]){
      res.emplace_back(min(a, v), max(a, v));
    }
    auto res2 = res;
    sort(res2.begin(), res2.end());
    assert(res == res2);
    return res;
  }
  int hstar(vector<pair<int, int>> P){
    int res = 0;
    while(!P.empty()){
      int a = P[0].first;
      int b = P[0].second;
      vector<pair<int, int>> looked = uni(incident(a), incident(b));
      P = diff(P, looked);
      res++;
    }
    return res;
  }
  void dfs(vector<int> R, vector<int> Q, vector<pair<int, int>> P){
    cnt++;
    if(Q.empty()){
      if(P.empty() && R.size() < cand.size()){
        cand = R;
      }
      return;
    }
    if(R.size() + hstar(P) >= cand.size()){
      return;
    }
    int must = -1;
    for(pair<int, int> e: P){
      vector<int> r = intersect(Q, {e.first, e.second});
      if(r.size() == 0){
        assert(false);
        return;
      } else if(r.size() == 1){
        must = r[0];
        break;
      }
    }
    if(must >= 0){
      vector<int> nQ = diff(Q, {must});
      vector<int> nR = uni(R, {must});
      vector<pair<int, int>> nP = diff(P, incident(must));
      dfs(nR, nQ, nP);
      return;
    }
    auto cmp = [&](int i, int j){
      return diff(P, incident(i)).size() < diff(P, incident(j)).size();
    };
    int v = *min_element(Q.begin(), Q.end(), cmp);
    vector<int> nQ = diff(Q, {v});
    vector<int> nR = uni(R, {v});
    vector<pair<int, int>> nP = diff(P, incident(v));
    dfs(nR, nQ, nP);
    dfs(R, nQ, P);
  }
  vector<int> vertex_cover(){
    cand.clear();
    for(int i = 0; i < n; i++){
      cand.push_back(i);
    }
    for(int i = 0; i < n; i++){
      sort(G[i].begin(), G[i].end());
    }
    vector<int> R(0);
    vector<int> Q(0);
    for(int i = 0; i < n; i++){
      Q.push_back(i);
    }
    vector<pair<int, int>> P(0);
    for(int i = 0; i < n; i++){
      P = uni(P, incident(i));
    }
    dfs(R, Q, P);
    return cand;
  }
  vector<int> vertex_cover_naive(){
    cand.clear();
    for(int i = 0; i < n; i++){
      cand.push_back(i);
    }
    for(int mask = 0; mask < (1<<n); mask++){
      vector<int> cur(0);
      for(int i = 0; i < n; i++){
        if(mask & (1<<i)){
          cur.push_back(i);
        }
      }
      if(cur.size() >= cand.size()){
        continue;
      }
      bool ok = true;
      for(int i = 0; i < n; i++){
        for(int v: G[i]){
          if(!(mask & (1<<i)) && !(mask & (1<<v))){
            ok = false;
          }
        }
      }
      if(ok){
        cand = cur;
      }
    }
    return cand;
  }
};

VertexCover vc;

int n, m;

int main(){

  cin >> n >> m;
  vc.init(n);
  REP(i, m){
    int a, b;
    cin >> a >> b;
    vc.add_edge(a, b);
  }

  auto res1 = vc.vertex_cover();
  /*
  auto res2 = vc.vertex_cover_naive();

  for(int v: res1){
    cout << v << " ";
  }
  cout << endl;
  for(int v: res2){
    cout << v << " ";
  }
  cout << endl;

  assert(res1.size() == res2.size());
  */
  
  cout << cnt << endl;

  return 0;
}
