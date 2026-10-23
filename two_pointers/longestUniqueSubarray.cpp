// https://github.com/kazu107/kyopro_library/blob/main/two_pointers/longestUniqueSubarray.cpp
// SPDX-License-Identifier: CC0-1.0
/*
できること: 要素が重複しない連続部分配列の最大長を、しゃくとり法で求める。
使い方:
  vector<int> a = {1,2,1,3}; int length = longestUniqueSubarray(a); // 3。
前提: stdの名前解決を利用側で用意する。空配列なら0。元配列の値は変更しない。
計算量: ハッシュ操作が平均O(1)なら時間O(N)、メモリO(N)。
標準ヘッダは提出用テンプレート側で用意する。
*/
// 依存: なし

// 配列内の要素が全て異なる連続部分配列の最大長を求める
// 計算量 O(N)
int longestUniqueSubarray(vector<int>& nums) {
    unordered_set<int> st;
    int maxLen = 0;
    int left = 0;
    for (int right = 0; right < (int)nums.size(); right++) {
        while (st.find(nums[right]) != st.end()) {
            st.erase(nums[left]);
            left++;
        }
        st.insert(nums[right]);
        maxLen = max(maxLen, right - left + 1);
    }
    return maxLen;
}
