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
    showg = subprocess.check_output((NAUTY_DIR+'showg', '-e', '-l0'), stdin=geng.stdout, stderr=FNULL)
    geng.wait()
    showg_output = showg.decode('ascii')
    lines = showg_output.split('\n')

    assert lines[0] == '' and lines[-1] == ''
    del lines[0]
    assert len(lines) % 4 == 0

    num_graphs = int(len(lines)/4)

    for n in range(0, num_graphs):
        graph_encoding = lines[4*n + 2]
        print(graph_encoding, '    g_%d_%d_%d' % (vertices, edges, n+1))
