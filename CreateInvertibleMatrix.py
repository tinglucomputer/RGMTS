__author__ = 'liupeng'

import numpy
import os
import re
import random
import optparse

class InverMatr():
    def __init__(self,dimension,total):
        self.dimension = dimension
        self.total = total
        self.matrix_file_name="./matrix/matrix_%d_%d"%(dimension,total)
        if not os.path.isfile(self.matrix_file_name+"_0"):
            print "building matrix..."
            self.create_init_matrix(dimension,total)
            self.saveMatrixToFile(self.matrix_file_name,dimension,total,self.matrix)
            print "building end"

    def saveMatrixToFile(self,filename,dimension,total,matrix):
        for n in range(4):
            f=open(filename+"_"+str(n),'w')
            for i in range(total):
                string=""
                for r in range(dimension):
                    for c in range(dimension):
                        if(r==dimension-1 and c==dimension-1):
                            string+= "%.13lf"%(matrix[n][i][r][c])
                        else:
                            string+= "%.13lf "%(matrix[n][i][r][c])
                f.write("%d %s \n"%(i,string))
            f.close()

    def create_init_matrix(self,dimension,total):
        self.matrix=[[],[],[],[]]
        for i in range(self.total):
            t1=numpy.random.rand(dimension,dimension)
            t2=numpy.linalg.inv(t1)
            self.matrix[0].append(t1.T)
            self.matrix[1].append(t2)

            t1=numpy.random.rand(dimension,dimension)
            t2=numpy.linalg.inv(t1)
            self.matrix[2].append(t1.T)
            self.matrix[3].append(t2)

    def createS(self,total):
        file_name = "./index/s_%d"%(total)
        if not os.path.isfile(file_name):
            f=open(file_name,'w')
            s=[0]*total
            for i in range(total):
                s[i]=random.randint(0,1)
                f.write("%d "%(s[i]))
        else:
            f=open(file_name,'r')
            for line in f:
                if len(line)>0:
                    line=line.split(" ")
                    for i in range(total):
                        line[i]=int(line[i])
                    break;
            s=line[:]
        return s


if __name__ == '__main__':
    parser=optparse.OptionParser()
    parser.add_option('-d',action="store" ,dest="demsion",    default="0")
    parser.add_option('-a',action="store" ,dest="total",    default="0")
    options,remainder=parser.parse_args()
    if options.demsion!='0':
        demsion=int(options.demsion)
    if options.total!='0':
        total=int(options.total)
    ma = InverMatr(demsion,total)

