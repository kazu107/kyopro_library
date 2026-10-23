// https://github.com/kazu107/kyopro_library/blob/main/tree/rerooting.cpp
// SPDX-License-Identifier: CC0-1.0
/*
できること: 既存のグローバル名Rerootingで、各頂点からの最大距離を求める。
使い方:
  Rerooting dp(n); dp.add_edge(u, v, cost); auto answer = dp.build(root);
  // 独自のDP演算を指定する場合はcp::Rerooting<MyPolicy>を使う。
前提: 0始まり、無向の木・森。自分自身までの距離0も含む。N/gの公開フィールドへの直接アクセスは互換対象外。
計算量: O(V+E)、メモリO(V+E)。本体は依存先の汎用Rerooting。
標準ヘッダは提出用テンプレート側で用意する。
*/
// 依存: tree/Rerooting.hpp


// 既存の Rerooting r(n); r.add_edge(u,v,w); r.build(root); 用の互換名。
// 各頂点からの最大距離を返す。森対応、再帰なし、構築O(V+E)。
// 独自DPは cp::Rerooting<MyPolicy> を使う。詳細は tree/README.md。
using Rerooting = cp::Rerooting<>;
