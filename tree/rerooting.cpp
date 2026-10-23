// https://github.com/kazu107/kyopro_library/blob/main/tree/rerooting.cpp
// SPDX-License-Identifier: CC0-1.0
#pragma once
#include "Rerooting.hpp"

// 既存の Rerooting r(n); r.add_edge(u,v,w); r.build(root); 用の互換名。
// 各頂点からの最大距離を返す。森対応、再帰なし、構築O(V+E)。
// 独自DPは cp::Rerooting<MyPolicy> を使う。詳細は tree/README.md。
using Rerooting = cp::Rerooting<>;
