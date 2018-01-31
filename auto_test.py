#!/usr/bin/python
__author__ = 'liupeng'

import os
#[document_num , word_num, dimension]
#lists = [[4000,7000,80,50],[4000,10000,80,50],[4000,12000,80,50]]
#lists = [[1000,960,80],[1000,1760,80]]
lists = [[6000,4000,80,50],[8000,4000,80,50],[10000,4000,80,50],[12000,4000,80,50]]
#lists = [[4000,4000,80,70],[4000,4000,80,90],[4000,4000,80,110]]
for node in lists:
    #os.system("python ./CreateInvertibleMatrix.py -d %d -a %d"%(node[2]+10,node[1]/80 ))
    #os.system("python ./CreateInvertibleMatrix.py -d %d -a %d"%(node[2]+30,node[1]/80 ))
    #os.system("python ./CreateInvertibleMatrix.py -d %d -a %d"%(node[2]+40,node[1]/80 ))
    print "start create_index.py -f %d -w %d -n 0 -d %d"%(node[0],node[1],node[2])
    os.system("./create_index.py -f %d -w %d -n 0 -d %d"%(node[0],node[1],node[2]))
    print "end create_index.py -f %d -w %d -n 0 -d %d"%(node[0],node[1],node[2])

    print "./encrypt -f %d  -w %d  -d %d -s 1 -u 100 -n 0 -t %d -e 2 -y 10 -m 1"%(node[0],node[1],node[2],node[3])
    #print "./encrypt -f %d  -w %d  -d %d -s 1 -u 100 -n 0 -t %d -e 2 -y 10 -m 1 >>log.txt"%(node[0],node[1],node[2],node[3])
    os.system("./encrypt -f %d  -w %d  -d %d -s 1 -u 100 -n 0 -t %d -e 2 -y 10 -m 1"%(node[0],node[1],node[2],node[3]))
    #print "./encrypt -f %d  -w %d  -d %d -s 1 -u 100 -n 0 -t 50 -e 1 -y 10 -m 2 >> log.txt"%(node[0],node[1],node[2])
    #os.system("./encrypt -f %d  -w %d  -d %d -s 1 -u 100 -n 0 -t 50 -e 1 -y 10 -m 2 >> log.txt"%(node[0],node[1],node[2]))
    print "-------end---------\n\n"


