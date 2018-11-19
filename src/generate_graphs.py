#!/usr/bin/env python3
import os, subprocess, argparse
from pathlib import Path

HOME = str(Path.home())
NAUTY_DIR = HOME + '/src/nauty26r10/'
FNULL = open(os.devnull, 'w')

if __name__ == '__main__':
    parser = argparse.ArgumentParser(description='Generate graphs.')
    parser.add_argument('vertices', help='number of vertices', type=int, nargs=1)
    parser.add_argument('edges', help='number of edges', type=int, nargs=1)

    args = parser.parse_args()

    vertices = args.vertices[0]
    edges = args.edges[0]

    geng = subprocess.Popen((NAUTY_DIR+'geng', '-cd3', str(vertices), '%d:%d' % (edges, edges)),
                            stdout=subprocess.PIPE, stderr=FNULL)
    showg = subprocess.Popen((NAUTY_DIR+'showg', '-e', '-l0'), stdin=geng.stdout, stderr=FNULL, stdout=subprocess.PIPE)

    line_count = -1
    count = 0
    for line in showg.stdout:
        if line_count % 4 == 2:
            graph_encoding = line.decode('ascii').rstrip()
            print(vertices, edges, '   ', graph_encoding, '    g_%d_%d_%d' % (vertices, edges, count+1))
            count += 1
        line_count += 1
