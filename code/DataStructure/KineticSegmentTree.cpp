struct line_t{
  ll a, b, v, idx;
  line_t() : line_t(0, nINF) {}
  line_t(ll a, ll b) : line_t(a, b, -1) {}
  line_t(ll a, ll b, ll idx) : a(a), b(b), v(b), idx(idx) {}
  void apply_heat(ll heat){ v += a * heat; }
  void apply_add(ll lz_add){ v += lz_add; }
  ll cross(const line_t &l) const {
    if(a == l.a) return pINF;
    ll p = v - l.v, q = l.a - a;
    if(q < 0) p = -p, q = -q;
    return p >= 0 ? (p + q - 1) / q : -p / q * -1;
  }
  ll cross_after(const line_t &l, ll temp) const {
    ll res = cross(l); return res > temp ? res : pINF;
  }
};
struct range_kinetic_segment_tree{
  struct node_t{
    line_t v;
    ll melt, heat, lz_add;
    node_t() : node_t(line_t()) {}
    node_t(ll a, ll b, ll idx) : node_t(line_t(a, b, idx)) {}
    node_t(const line_t &v) : v(v), melt(pINF), heat(0), lz_add(0) {}
    bool operator < (const node_t &o) const { return tie(v.v,v.a) < tie(o.v.v,o.v.a); }
    ll cross_after(const node_t &o, ll temp) const { return v.cross_after(o.v, temp); }
    void apply_lazy(){ v.apply_heat(heat); v.apply_add(lz_add); melt -= heat; }
    void clear_lazy(){ heat = lz_add = 0; }
    void prop_lazy(const node_t &p){ heat += p.heat; lz_add += p.lz_add; }
    bool have_lazy() const { return heat != 0 || lz_add != 0; }
  };
  node_t T[SZ<<1]; range_kinetic_segment_tree(){ clear(); }
  void clear(){ fill(T, T+SZ*2, node_t()); }
  void pull(int node, int s, int e){
    if(s == e) return;
    const node_t &l = T[node<<1], &r = T[node<<1|1];
    assert(!l.have_lazy() && !r.have_lazy() && !T[node].have_lazy());
    T[node] = max(l, r);
    T[node].melt = min({ l.melt, r.melt, l.cross_after(r, 0) });
  }
  void push(int node, int s, int e){
    if(!T[node].have_lazy()) return; T[node].apply_lazy();
    if(s != e) for(auto c : {node<<1, node<<1|1}) T[c].prop_lazy(T[node]);
    T[node].clear_lazy();
  }
  void build(const vector<line_t> &lines, int node=1, int s=0, int e=SZ-1){
    if(s == e){ T[node] = s < lines.size() ? node_t(lines[s]) : node_t(); return; }
    int m = (s + e) / 2;
    build(lines,node*2,s,m); build(lines,node*2+1,m+1,e);
    pull(node, s, e);
  }
  void update(int x, const line_t &v, int node=1, int s=0, int e=SZ-1){
    push(node, s, e); int m = (s + e) / 2;
    if(s == e){ T[node] = v; return; }
    if(x <= m) update(x, v, node<<1, s, m), push(node<<1|1, m+1, e);
    else update(x, v, node<<1|1, m+1, e), push(node<<1, s, m);
    pull(node, s, e);
  }
  void add(int l, int r, ll v, int node=1, int s=0, int e=SZ-1){
    push(node, s, e); int m = (s + e) / 2;
    if(r < s || e < l) return;
    if(l <= s && e <= r){ T[node].lz_add += v; push(node, s, e); return; }
    add(l,r,v,node*2,s,m); add(l,r,v,node*2+1,m+1,e);
    pull(node, s, e);
  }
  void heaten(int l, int r, ll t, int node=1, int s=0, int e=SZ-1){
    push(node, s, e); int m = (s + e) / 2;
    if(r < s || e < l) return;
    if(l <= s && e <= r){ _heat(t, node, s, e); return; }
    heaten(l,r,t,node*2,s,m); heaten(l,r,t,node*2+1,m+1,e);
    pull(node, s, e);
  }
  void _heat(ll t, int node=1, int s=0, int e=SZ-1){
    push(node, s, e); int m = (s + e) / 2;
    if(T[node].melt > t){ T[node].heat += t; push(node, s, e); return; }
    _heat(t,node*2,s,m); _heat(t,node*2+1,m+1,e);
    pull(node, s, e);
  }
};