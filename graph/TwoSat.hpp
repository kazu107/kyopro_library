// https://github.com/kazu107/kyopro_library/blob/main/graph/TwoSat.hpp
// SPDX-License-Identifier: CC0-1.0
#pragma once
#include "scc.hpp"

namespace cp {
// TwoSat sat(n); sat.add_clause(i,f,j,g); bool ok=sat.satisfiable();
// 各節は(x[i]==f)||(x[j]==g)。force(i,f)で値を固定できる。
// answer()は直近のsatisfiableがtrueの場合のみ利用可。0変数も可。
// 条件追加後は再計算する。計算O(変数数+節数)、メモリも同じ。
class TwoSat {
    int n_;
    Graph<int> graph_;
    bool solved_ = false, possible_ = false;
    std::vector<bool> assignment_;
    int literal(int i, bool value) const { assert(0 <= i && i < n_); return 2 * i + (value ? 1 : 0); }
public:
    explicit TwoSat(int n = 0) : n_(n), graph_(2 * n, true) { assert(n >= 0); }
    void add_clause(int i, bool f, int j, bool g) {
        const int a = literal(i, f), b = literal(j, g);
        graph_.add_edge(a ^ 1, b); graph_.add_edge(b ^ 1, a); solved_ = false;
    }
    void force(int i, bool value) { add_clause(i, value, i, value); }
    void add_implication(int i, bool f, int j, bool g) { add_clause(i, !f, j, g); }
    bool satisfiable() {
        const auto result = strongly_connected_components(graph_);
        assignment_.assign(n_, false); solved_ = true; possible_ = true;
        for (int i = 0; i < n_; ++i) {
            if (result.component[2 * i] == result.component[2 * i + 1]) { possible_ = false; return false; }
            assignment_[i] = result.component[2 * i] < result.component[2 * i + 1];
        }
        return true;
    }
    const std::vector<bool>& answer() const { assert(solved_ && possible_); return assignment_; }
};
} // namespace cp
