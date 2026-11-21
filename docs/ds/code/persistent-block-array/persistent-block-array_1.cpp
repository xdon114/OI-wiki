#include <algorithm>
#include <cstdint>
#include <cstring>
#include <iostream>

constexpr int maxn = 50005, maxq = 50005, block = 240;

int arr[maxn + block];
int bcnt; // 已用的块数
int bval[maxq * 2 + block][block]; // 块上的值
int bsrt[maxq * 2 + block][block]; // 排序的块
int vref[maxq][block]; // 版本上的块表
int vtag[maxq][block]; // 版本上的标记表
int cur, ver, l, r, x;

void sort_block(int id) { // 排序一块
  static_assert(sizeof(*bsrt) == sizeof(*bval));
  memcpy(bsrt[id], bval[id], sizeof(*bval));
  std::sort(bsrt[id], bsrt[id] + block);
}

// 从 ver 到 cur 复制第 bn 块并将 bl~br 加上 x
void copy_block(int bn, int bl, int br) {
  int id = vref[cur][bn] = ++bcnt;
  std::memcpy(bval[id], bval[vref[ver][bn]], sizeof(*bval));
  for (int i = bl; i <= br; ++i)
    bval[id][i] += x;
  sort_block(id);
}

void add() {
  int dl = l / block, dr = r / block;
  if (dl == dr) { // 仅一块
    copy_block(dl, l % block, r % block);
    return;
  }
  copy_block(dl, l % block, block - 1); // 拷贝左侧块
  copy_block(dr, 0,         r % block); // 拷贝右侧块
  for (int bn = dl + 1; bn < dr; ++bn) // 懒标记
    vtag[cur][bn] += x;
}

// cur 第 bn 块中 bl~br 不小于 x 的数量
int count_geq(int bn, int bl, int br) {
  int id = vref[cur][bn], ans = 0;
  for (int i = bl; i <= br; ++i)
    if (bval[id][i] + vtag[cur][bn] >= x)
      ++ans;
  return ans;
}

int rank() { // 仅一块
  int dl = l / block, dr = r / block;
  if (dl == dr)
    return count_geq(dl, l % block, r % block);
  int ans = 0;
  ans += count_geq(dl, l % block, block - 1); // 计算左侧块
  ans += count_geq(dr, 0        , r % block); // 计算右侧块
  for (int bn = dl + 1; bn < dr; ++bn) { // 二分计算整块
    int id = vref[cur][bn];
    ans += bsrt[id] + block - std::lower_bound
          (bsrt[id], bsrt[id] + block, x - vtag[cur][bn]);
  }
  return ans;
}

int main() {
  using std::cin;
  using std::memcpy;
  int n, q;

  cin.tie(nullptr);
  std::ios::sync_with_stdio(false);
  cin >> n >> q;
  for (int i = 0; i < n; ++i)
    cin >> arr[i];

  // 构建初始版本
  for (int bn = 0; bn <= n / block; ++bn) {
    int id = vref[0][bn] = ++bcnt, offset = bn * block;
    for (int i = 0; i < block; ++i)
      bval[id][i] = arr[offset + i];
    sort_block(id);
  }

  // 处理操作
  for (cur = 1; cur <= q; ++cur) {
    int op;
    cin >> op >> ver >> l >> r >> x;
    --l, --r; // 转为 0 基
    // 拷贝引用和标记
    memcpy(vref[cur], vref[ver], sizeof(*vref));
    memcpy(vtag[cur], vtag[ver], sizeof(*vref));
    if (op == 1)
      add();
    else
      std::cout << rank() << '\n';
  }
  return 0;
}
