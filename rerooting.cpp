struct Rerooting {
    using DP = long long; // DPの型

    struct Edge {
        int to;
        long long w;
    };

    int N;
    vector<vector<Edge>> g;
    vector<DP> down;
    vector<DP> ans;

    Rerooting(int n) : N(n), g(n), down(n), ans(n) {
        DP id = identity();
        fill(down.begin(), down.end(), id);
        fill(ans.begin(), ans.end(), id);
    }

    void add_edge(int u, int v, long long w = 1) {
        g[u].push_back(Edge{v, w});
        g[v].push_back(Edge{u, w});
    }

    vector<DP> build(int root = 0) {
        dfs1(root, -1);
        dfs2(root, -1, identity());
        return ans;
    }

private:
    DP identity() const { return 0; } // mergeの単位元
    DP merge(const DP &a, const DP &b) const { return max(a, b); } // DP同士のマージ
    DP apply_edge(const DP &dp_from_neighbor, long long w, int from, int to) const {
        return dp_from_neighbor + w;
    } // 辺(from->to)をまたぐときの変換
    DP add_root(const DP &acc, int v) const { return acc; } // 頂点vでの最終処理

    void dfs1(int v, int p) {
        DP acc = identity();
        for (auto &e : g[v]) {
            int to = e.to;
            if (to == p) continue;
            dfs1(to, v);
            DP child_contrib = apply_edge(down[to], e.w, to, v);
            acc = merge(acc, child_contrib);
        }
        down[v] = add_root(acc, v);
    }

    void dfs2(int v, int p, DP from_parent) {
        int deg = (int)g[v].size();
        vector<DP> pref(deg + 1, identity()), suff(deg + 1, identity());

        for (int i = 0; i < deg; i++) {
            auto &e = g[v][i];
            int to = e.to;
            DP val;
            if (to == p) {
                val = apply_edge(from_parent, e.w, p, v);
            } else {
                val = apply_edge(down[to], e.w, to, v);
            }
            pref[i + 1] = merge(pref[i], val);
        }

        for (int i = deg - 1; i >= 0; i--) {
            auto &e = g[v][i];
            int to = e.to;
            DP val;
            if (to == p) {
                val = apply_edge(from_parent, e.w, p, v);
            } else {
                val = apply_edge(down[to], e.w, to, v);
            }
            suff[i] = merge(suff[i + 1], val);
        }

        DP all = pref[deg];
        ans[v] = add_root(all, v);

        for (int i = 0; i < deg; i++) {
            auto &e = g[v][i];
            int to = e.to;
            if (to == p) continue;
            DP without = merge(pref[i], suff[i + 1]);
            DP nxt = add_root(without, v);
            dfs2(to, v, nxt);
        }
    }
};