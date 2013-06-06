#!/usr/bin/python
#-*- encoding:utf-8 -*-
import re
import sys
import numpy as np
import matplotlib.pyplot as plt

debug = True
g_stat_items = {}
'''
diff files <fft/pin/p2/thread0-trace.out.gz,fft/m5/p2/m5out/X86/m5trace.out.100.gz> with dump file/home/liuyix/Dev/ped3/WORKING_DIR/fft.dump
1534 functions found in dump file!
t1 bbs size = 51277
t2 bbs size = 51291
call trace 1 size = 17468
call trace 2 size = 17643
ProcessCalls:task calltask :(0,17468)=17468,(0,17643)=17643
t1 inst size:469996
t2 inst size:468650
call trace diff:
17376,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
inst trace diff:
0,160780,137675,159148,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
t1 unique size:7127
t2 unique size:5746
call trace sim. size:17376
inst trace sim. size:457603
call trace similarity:99.4733
inst trace similarity:97.3632

diff files <fft/pin/p2/thread1-trace.out.gz,fft/m5/p2/m5out/X86/m5trace.out.101.gz> with dump file/home/liuyix/Dev/ped3/WORKING_DIR/fft.dump
1534 functions found in dump file!
t1 bbs size = 42794
t2 bbs size = 42825
call trace 1 size = 4608
call trace 2 size = 4614
ProcessCalls:task calltask :(0,4608)=4608,(0,4614)=4614
t1 inst size:890037
t2 inst size:889689
call trace diff:
4608,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
inst trace diff:
0,290043,270618,328212,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
t1 unique size:1152
t2 unique size:576
call trace sim. size:4608
inst trace sim. size:888873
call trace similarity:100
inst trace similarity:99.8692

'''

def read_file(filename):
    with open(filename) as fileObj:
        for line in fileObj:
            if line == '\n':
                if stat_item:
                    print_stat_item(stat_item)
                else:
                    if debug:
                        print 'stat item is NONE!'
                stat_item = None
            elif line.find('diff files ') != -1:
                match = re.search('<([a-z]+?|[a-z]+?.[a-z]+?)/pin/p([0-9]+?)/thread([0-9]+?)-', line)
                if match:
                    progname = match.group(1)
                    nthreads = match.group(2)
                    idx_threads = match.group(3)
                    if debug:
                        print 'name: ', progname, 'thread: ', nthreads, 'idx: ', idx_threads
                    stat_item = get_stat_item(progname, int(nthreads), int(idx_threads))
                    # if debug:
                    #     print 'cts: ',stat_item['cts']
                    #     print 'its: ',stat_item['its']
                    #     pass
                else:
                    if debug:
                        print 'pattern not found, msg:'
                        print line                
            elif line.find('error') != -1:
                #print 'found error!'
                #print line
                set_error(stat_item)
            elif line.find('call trace similarity') != -1:
                stat_item['cts'] = float(line.split(':')[1])
            elif line.find("inst trace similarity") != -1:
                stat_item['its'] = float(line.split(':')[1])

                
def get_stat_item(name, nthreads, idx_threads):
    '''
    `name`: fft
    `nthreads`: 4
    `idx_threads`: 0
    '''
    global g_stat_items
    if g_stat_items == None:
        g_stat_items = {}
    if name not in g_stat_items:
        g_stat_items[name] = {}
    if nthreads not in g_stat_items[name]:
        g_stat_items[name][nthreads] = []
        for i in range(nthreads+1):
            tmp = {}
            tmp['cts'] = 0.0
            tmp['its'] = 0.0
            g_stat_items[name][nthreads].append(tmp)
    return g_stat_items[name][nthreads][idx_threads]

def set_error(stat_item):
    #print 'error not implemented!'
    pass

def print_stat_item(stat_item):
    pass

def draw_histo(prog):
    # import numpy as np
    # import matplotlib.pyplot as plt
    # import matplotlib.mlab as mlab
    # fig = plt.figure()
    # ax = fig.subplot(111)
    # datas = []
    # for n in prog.iterkeys():
    #     for i
    pass
def draw_barchart(prog, name):
    cts_datas = []
    its_datas = []
    count = 0
    for n in prog.iterkeys():
        for item in prog[n]:
            if item['cts'] != 0 and item['its'] !=0:
                cts_datas.append(item['cts'])
                its_datas.append(item['its'])
                count += 1
    ind = np.arange(count)
    width = 0.35
    plt.subplot(111)
    # print 'ind: ', len(ind)
    # print 'datas: ',len(cts_datas)
    rects1 = plt.bar(ind, cts_datas, width, color='blue')
    rects2 = plt.bar(ind+width, its_datas, width, color='green')
    plt.title(name, fontsize=14)
    plt.ylabel('Ratio')
    #plt.legend( (rects1[0], rects2[0]), ('call trace', 'inst trace') )
    # autolabel(rects1)
    # autolabel(rects2)
    plt.show()
    #plt.savefig(name+'.png',dpi=100)

def writeCsv():
    """
    csv format:
    progname, total threads, thread idx, cts, its
    """
    import csv
    with open('ped3_stats.csv', 'wb') as csvfile:
        stat_writer = csv.writer(csvfile, delimiter=' ', quotechar='|', quoting=csv.QUOTE_MINIMAL)
        for k in g_stat_items.iterkeys():
            for thread in g_stat_items[k].iterkeys():
                i = 0
                for item in g_stat_items[k][thread]:
                    rowdata = []
                    rowdata.append(k)
                    rowdata.append(thread)
                    rowdata.append(i)
                    rowdata.append(item['cts'])
                    rowdata.append(item['its'])
                    stat_writer.writerow(rowdata)
                    i += 1
    
def autolabel(rects):
    # attach some text labels
    for rect in rects:
        height = rect.get_height()
        plt.text(rect.get_x()+rect.get_width()/2., 1.05*height, '%f'%float(height),
                ha='center', va='bottom')
if __name__ == "__main__":
    if len(sys.argv) < 2:
        print '输入要分析的文件名'
        exit(0)
    read_file(sys.argv[1])
    for k in g_stat_items.iterkeys():
        print 'name: ',k
        for thread in g_stat_items[k].iterkeys():
            print 'thread: ',thread
            i = 0
            for item in g_stat_items[k][thread]:
                print '#', i
                print 'call sim: ', item['cts']
                print 'inst sim: ', item['its']
                i += 1
    # for prog in g_stat_items.iterkeys():
    #     draw_barchart(g_stat_items[prog], prog)
    writeCsv()