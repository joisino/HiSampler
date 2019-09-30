import os
import subprocess
import argparse

import numpy as np
import yaml

def get_output(command):
    return subprocess.check_output(command, shell=True).decode()
    
def output_graph(filename, n, edges):
    with open(filename, 'w') as f:
        f.write('{} {}\n'.format(n, len(edges)))
        for e in edges:
            f.write('{} {}\n'.format(e[0], e[1]))

def output_distribution(filename, n, x):
    P = np.zeros((n, n), dtype='f')
    cnt = 0;
    for i in range(n):
        for j in range(i):
            P[i, j] = x[cnt]
            P[j, i] = x[cnt]
            cnt += 1
    with open(filename, 'w') as f:
        f.write('{}\n'.format(n))
        for i in range(n):
            for j in range(n):
                if j != 0:
                    f.write(' ')
                f.write('{:.3f}'.format(P[i, j]))
            f.write('\n')
    
def calc_reward(n, edges, solver, tmpdir):
    filename = os.path.join(tmpdir, 'calc_reward_{}'.format(os.getpid()))
    output_graph(filename, n, edges)
    reward = float(get_output('{} < {}'.format(solver, filename)))
    os.remove(filename)
    return reward 

def makedir(dirname):
    if not os.path.exists(dirname):
        os.makedirs(dirname)

def load_conf():
    parser = argparse.ArgumentParser()
    parser.add_argument('conf', type=str)
    parser.add_argument('--seed', type=int, default=None)
    parser.add_argument('--solver', type=str, default=None)
    parser.add_argument('--gpu', '-g', type=int, default=None)
    parser.add_argument('--n', type=int, default=None)
    parser.add_argument('--p', type=float, default=None)
    parser.add_argument('--iteration', type=int, default=None)
    parser.add_argument('--lr', type=float, default=None)
    parser.add_argument('--dirname', type=str, default=None)
    parser.add_argument('--eps', type=float, default=None)
    parser.add_argument('--restart', type=int, default=None)
    parser.add_argument('--noreplay', action='store_true')
    parser.add_argument('--opt', type=str, default='Adam')
    args = parser.parse_args()

    with open(args.conf) as f:
        conf = yaml.load(f)

    for key in conf:
        conf[key] = getattr(args, key) if getattr(args, key) is not None else conf[key]

    return conf
