// Solves the canonical form: maximize c^T x, subject to ax <= b and x >= 0.
template<class T> // T must be of floating type
struct linear_programming_solver_simplex{
  int m, n; vector<int> nn, bb; vector<vector<T>> mat;
  static constexpr T eps = 1e-8, inf = 1/.0;
  linear_programming_solver_simplex(const vector<vector<T>> &a, const vector<T> &b, const vector<T> &c) : m(b.size()), n(c.size()), nn(n+1), bb(m), mat(m+2, vector<T>(n+2)){
    for(int i=0; i<m; i++) for(int j=0; j<n; j++) mat[i][j] = a[i][j];
    for(int i=0; i<m; i++) bb[i] = n + i, mat[i][n] = -1, mat[i][n + 1] = b[i];
    for(int j=0; j<n; j++) nn[j] = j, mat[m][j] = -c[j];
    nn[n] = -1; mat[m + 1][n] = 1;
  }
  void pivot(int r, int s){
    T *a = mat[r].data(), inv = 1 / a[s];
    for(int i=0; i<m+2; i++) if(i != r && abs(mat[i][s]) > eps) {
      T *b = mat[i].data(), inv2 = b[s] * inv;
      for(int j=0; j<n+2; j++) b[j] -= a[j] * inv2;
      b[s] = a[s] * inv2;
    }
    for(int j=0; j<n+2; j++) if(j != s) mat[r][j] *= inv;
    for(int i=0; i<m+2; i++) if(i != r) mat[i][s] *= -inv;
    mat[r][s] = inv; swap(bb[r], nn[s]);
  }
  bool simplex(int phase){
    for(auto x=m+phase-1; ; ){
      int s = -1, r = -1;
      for(auto j=0; j<n+1; j++) if(nn[j] != -phase) if(s == -1 || pair(mat[x][j], nn[j]) < pair(mat[x][s], nn[s])) s = j;
      if(mat[x][s] >= -eps) return true;
      for(auto i=0; i<m; i++){
        if(mat[i][s] <= eps) continue;
        if(r == -1 || pair(mat[i][n + 1] / mat[i][s], bb[i]) < pair(mat[r][n + 1] / mat[r][s], bb[r])) r = i;
      }
      if(r == -1) return false;
      pivot(r, s);
    }
  }
  // Returns -inf if no solution, {inf, a vector satisfying the constraints}
  //   if there are abritrarily good solutions, or {maximum c^T x, x} otherwise.
  // O(n m (# of pivots)), O(2 ^ n) in general.
  pair<T, vector<T>> solve(){
    int r = 0;
    for(int i=1; i<m; i++) if(mat[i][n+1] < mat[r][n+1]) r = i;
    if(mat[r][n+1] < -eps){
      pivot(r, n);
      if(!simplex(2) || mat[m+1][n+1] < -eps) return {-inf, {}};
      for(int i=0; i<m; i++) if(bb[i] == -1){
          int s = 0;
          for(int j=1; j<n+1; j++) if(s == -1 || pair(mat[i][j], nn[j]) < pair(mat[i][s], nn[s])) s = j;
          pivot(i, s);
        }
    }
    bool ok = simplex(1);
    vector<T> x(n);
    for(int i=0; i<m; i++) if(bb[i] < n) x[bb[i]] = mat[i][n + 1];
    return {ok ? mat[m][n + 1] : inf, x};
  }
};