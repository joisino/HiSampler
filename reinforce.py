#!/usr/bin/env python3

import os
import time

import matplotlib
matplotlib.use('Agg')

import numpy as np
import chainer
import chainer.functions as F
import chainer.links as L
import matplotlib.pyplot as plt
import yaml

from util import calc_reward, makedir, output_graph, output_distribution, load_conf

class MLP(chainer.Chain):
    def __init__(self, channels, bias_final):
        super(MLP, self).__init__()
        self.n_layers = len(channels) - 1
        self.channels = channels
        bias = [0 for i in range(self.n_layers)]
        bias[-1] = bias_final
        for i in range(self.n_layers):
            self.add_link('l{}'.format(i), L.Linear(channels[i], channels[i+1], initial_bias=bias[i]))

    def z(self, batch):
        return self.xp.random.randn(batch, self.channels[0]).astype('f')

    def __call__(self, x):
        for i in range(self.n_layers):
            x = self['l{}'.format(i)](x)
            if i + 1 == self.n_layers:
                x = F.sigmoid(x)
            else:
                x = F.relu(x)
        return x

def gen_edges(n, p, xp):
    EPS = 1e-6
    a = xp.random.binomial(1, p.data, n * (n-1) // 2)
    lp = F.sum(a * F.log(p + EPS) + (1 - a) * F.log(1 - p + EPS))
    a_cpu = chainer.cuda.to_cpu(a)
    edges = np.array(np.tril_indices(n, -1)).T[np.where(a_cpu == 1)]
    return a, edges, lp
    
def calc_lp(n, p, a, xp):
    EPS = 1e-6
    lp = F.sum(a * F.log(p + EPS) + (1 - a) * F.log(1 - p + EPS))
    return lp

def train():
    conf = load_conf()

    no_replay = conf['noreplay']
    
    solver = conf['solver']
    n = conf['n']
    eps = conf['eps']

    savedir = conf['dirname']
    makedir(savedir)
    tmpdir = os.path.join(savedir, 'tmp')
    makedir(tmpdir)

    np.random.seed(conf['seed'])

    logfile = os.path.join(savedir, 'log')

    ave = 0
    aves = []
    ma = 0
    global_ma = 0
  
    channels = [10, 100, 500, n*(n-1)//2]
    if 'channels' in conf:
        channels = conf['channels']
        channels.append(n*(n-1)//2)

    bias = - np.log(1.0 / conf['p']  - 1)
    net = MLP(channels, bias)

    if conf['gpu'] != -1:
        chainer.cuda.get_device_from_id(conf['gpu']).use()
        net.to_gpu()
    
    if conf['opt'] == 'SGD':
        opt = chainer.optimizers.SGD(lr=conf['lr'])
    elif conf['opt'] == 'Adam':
        opt = chainer.optimizers.Adam(alpha=conf['lr'])
    opt.setup(net)

    stop = 0

    pool_size = 10
    start_training = 20
    r_bests = []
    edges_bests = []
    z_bests = []

    if no_replay:
        pool_size = 1
        start_training = 1e9

    iteration = 0
    from_restart = 0

    start_time = time.time()

    while True:
        iteration += 1
        from_restart += 1

        z = net.z(1)
        x = net(z)[0]
        edges_li, edges, lp = gen_edges(n, x, net.xp)
        r = calc_reward(n, edges, solver, tmpdir)

        entropy = F.mean(x * F.log(x + 1e-6) + (1 - x) * F.log(1 - x + 1e-6))

        if no_replay:
            loss = - r * lp

            net.cleargrads()
            loss.backward()
            opt.update()

        if r > ma:
            ma = r
            stop = 0
        else:
            stop += 1
        if r > global_ma:
            global_ma = r
            output_graph(os.path.join(savedir, 'output_{}.txt'.format(r)), n, edges)
            output_distribution(os.path.join(savedir, 'distribution_{}.txt'.format(r)), n, x.data)
            chainer.serializers.save_npz(os.path.join(savedir, 'snapshot_at_reward_{}'.format(r)), net)

        elapsed = time.time() - start_time

        ave = ave * (1 - conf['eps']) + r * conf['eps']
        aves.append(ave)
        with open(logfile, 'a') as f:
            print(savedir, iteration, elapsed, r, len(edges), entropy.data, global_ma, ma, ave, flush=True)
            print(iteration, elapsed, r, len(edges), entropy.data, global_ma, ma, ave, flush=True, file=f)

        f = False
        for es in edges_bests:
            if (es == edges_li).all():
                f = True
        if not f:
            r_bests.append(r)
            edges_bests.append(edges_li)
            z_bests.append(z)
        
        while len(r_bests) > pool_size:
            mi = 0
            for j in range(len(r_bests)):
                if r_bests[j] < r_bests[mi]:
                    mi = j
            r_bests.pop(mi)
            edges_bests.pop(mi)
            z_bests.pop(mi)

        if from_restart >= start_training:
            ind = np.random.randint(len(r_bests))
            x = net(z_bests[ind])[0]
            lp = calc_lp(n, x, edges_bests[ind], net.xp)

            loss = - r_bests[ind] * lp

            net.cleargrads()
            loss.backward()
            opt.update()

        if stop >= conf['restart']:
            stop = 0
            ma = 0
            r_bests = []
            edges_bests = []
            z_bests = []
            from_restart = 0
            net = MLP(channels, bias)
            if conf['gpu'] != -1:
                chainer.cuda.get_device_from_id(conf['gpu']).use()
                net.to_gpu()
            if conf['opt'] == 'SGD':
                opt = chainer.optimizers.SGD(lr=conf['lr'])
            elif conf['opt'] == 'Adam':
                opt = chainer.optimizers.Adam(alpha=conf['lr'])
            opt.setup(net)
            continue

        if iteration % 100 == 0:
            plt.clf()
            plt.plot(range(len(aves)), aves)
            plt.savefig(os.path.join(savedir, 'graph.png'))
        if iteration % 1000 == 0:
            plt.savefig(os.path.join(savedir, 'graph_{}.png'.format(iteration)))
            plt.savefig(os.path.join(savedir, 'graph_{}.eps'.format(iteration)))
            chainer.serializers.save_npz(os.path.join(savedir, 'snapshot_{}'.format(iteration)), net)
            chainer.serializers.save_npz(os.path.join(savedir, 'opt_{}'.format(iteration)), opt)

if __name__ == '__main__':
    train()
