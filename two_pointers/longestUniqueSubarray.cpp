// https://github.com/kazu107/kyopro_library/blob/main/two_pointers/longestUniqueSubarray.cpp
// SPDX-License-Identifier: CC0-1.0
// 配列内の要素が全て異なる部分列の最大長を求める
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