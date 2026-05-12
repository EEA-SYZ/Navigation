## 运行泊松磁盘采样测试（后续完善成对生成的图测试）

**对应参数**
- `-n` 表示生成的点数
- `-r` 表示点间的距离下界
- `--seed` 表示随机数种子

```bash
    make points_test

    # 或者单纯用管道
    g++ -std=c++17 -O2 poisson_points.cpp -o poisson_points.exe

    poisson_points.exe -n 150 --seed 1549 -r 60 | python poisson_preview.py --stdin --radius 70 --save poisson_pipe_preview.png --no-show
```