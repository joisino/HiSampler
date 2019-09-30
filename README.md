# HiSampler: Learning to Sample Hard Instances for Graph Algorithms

This algorithm learns generating hard instances of graph problems automatically.

https://arxiv.org/abs/1902.09700

## Getting Started

```
pip install -r requirements.txt
make solver/coloring
python3 ./hisampler.py yamls/coloring.yaml
ls results/coloring
```

## Train the Model with Your Own Solver

Your solver must take input of the following format from the standard input.

```
n m
a_1 b_1
a_2 b_2
...
a_m b_m
```

`n` is the number of nodes, `m` is the number of edges, and `(a_i, b_i)` is the endpoints of the i-th edge.
The indices of the vertices are numbered as 0, 1, 2, ..., n-1.

Your solver must output a single value which represents the hardness value (e.g., the time consumption or the number of recursive calls).

You can reuse the off-the-shelf configuration file such as `yamls/coloring.yaml` by chaning the solver path.
