#!/usr/bin/python
__author__ = 'liupeng'

import os
import re
import math
import pickle
import heapq
import time
import random
import optparse
import sys
from Porter import  PorterStemmer
from CreateInvertibleMatrix import InverMatr
from TPDS2015 import TPDS2015

class PKS():
    def __init__(self,filename,document_num=12000,new_build=0,words_num=12000,dimension=50,extend=20,key_len=10,extend_num=2):
        self.filename          = filename
        self.document_num      = document_num #the number of document
        self.words_num         = words_num #the number of words
        self.index_len         = int(math.ceil(self.words_num/dimension)*dimension)
        self.index             = [0]*self.index_len
        self.index_D_1         = [0]*self.index_len
        self.index_D_2         = [0]*self.index_len
        self.client_index      = {}
        self.id_word           = {}
        self.dimension         = dimension
        self.client_list       = [0]*self.index_len
        self.document          = [0.0]*self.document_num
        self.index_name_e    = "./index/index_w_%d_f_%d_d_%d"%(words_num,document_num,dimension)
        self.client_list_name  = "./index/client_list_%d_%d"%(words_num,document_num)
        self.client_index_name = "./index/client_index_%d_%d"%(words_num,document_num)
	self.tpds_index_file   = "./index/index_tpds2015_%d_%d.pickle"%(document_num,words_num)


        print "start create matrix and S..."
        ma = InverMatr(dimension+key_len*extend_num,self.index_len/dimension)
        ma.createS(self.index_len)
        print "end create"
        ma = InverMatr(dimension+extend+key_len*extend_num,self.index_len/dimension)
	
	

        if not os.path.isfile(self.index_name_e) or not os.path.isfile(self.tpds_index_file) or  new_build:
            start=time.clock()
            print "building original index..."
            self.buildIndex()
            self.tfidf()
            end=time.clock()
            print "building end use time:",(end-start)*1000,"ms"

            print "save %s file..."%(self.index_name_e)
            self.saveIndexToFile(self.index_name_e,self.index,self.index_len, document_num)
            print "save finish.."

            print "save client index to file..."
            self.saveclientindex(self.client_index,words_num,self.client_index_name)
            self.saveclientlist(self.client_list,words_num,self.client_list_name)
            end=time.clock()
            print "end save client , use time:",(end-start)*1000,"ms"
	    #self.tpds5 = TPDS2015(document_num,words_num,self.index,self.client_index,self.client_list,new_build,dimension)
	
    def saveclientindex(self,client_index,word_num,filename):
        f=open(filename,'w')
        lists = [0]*word_num
        for key in client_index:
            lists[client_index[key]] = key
        for i in range(word_num):
            f.write("%s "%(lists[i]))
        f.close()

    def saveclientlist(self,client_list,word_num,filename):
        f=open(filename,'w')
        for i in range(word_num):
            f.write("%.13lf "%(client_list[i]))
        f.close()

    def saveIndexToFile(self,filename,index,words_num,document_num):
        f=open(filename,'w')
        for i in range(words_num):
            f.write("id:%d\n"%(i))
            string="index: "
            for j in range(document_num-1):
                string+='%.13lf '%(index[i][j])
            string+='%.13lf'%(index[i][document_num-1])
            f.write("%s\n"%(string))
        f.close()

    def buildIndex(self):
        print "start build index ... "

        for i in xrange(self.index_len):
            self.index[i]=[0.0]*(self.document_num)

        f   = open(self.filename,'r')
        num = 0
        for line in f:
            if num == self.index_len:
                break
            m = re.match(r"([0-9]*)\s([a-z]*)\s:\s(.*)",line)
            if m:
                word_id=num
                word = m.group(2)
                sta  = m.group(3).split(",")
                file_have_word_number = len(sta)
                self.client_index[word]=word_id
                self.id_word[word_id]=word
                self.client_list[word_id]=math.log(1+self.document_num/file_have_word_number) #record the ID of word and the fi
                for i in range(file_have_word_number):
                    sta[i]=sta[i].split(":")
                    document_id=int (sta[i][0])-1
                    fij=  float(sta[i][1])
                    if document_id >= self.document_num:
                        continue
                    tf=1+math.log(fij)
                    self.document[document_id]+=tf*tf
                    self.index[word_id][document_id]=tf
                num += 1
        f.close()
        print "finish building index"

    def tfidf(self):
        print "start tfidf ...  "
        for i in range(self.document_num):
            self.document[i]=math.sqrt(self.document[i])
        print "--start calculate...\n-- 00% 10% 20% 30% 40% 50% 60% 70% 80% 90% 100%\n--",
        for i in xrange(self.index_len):
            t=self.index_len/100
            if i%t==0 and (i/t)%10==0:
                print "=>>",

            for j in xrange(self.document_num):
                if self.document[j]>0.000001 and self.index[i][j]>0.000001:
                    self.index[i][j]=self.index[i][j]/self.document[j]
        print "\n--end calculate..."
        print "end tfidf"

        f=open("./network",'w')
        for i in xrange(self.index_len):
            f.write("%s: "%(self.id_word[i]))
            string=""
            for j in range(self.document_num):
                if self.index[i][j]>0.000001:
                    string+='%.13lf '%(self.index[i][j])
            f.write("%s\n"%(string))
        f.close()


if __name__=='__main__':
    #(self,filename,document_num=12000,now_build=0,top=20,words_num=12000,dimension=50)
    now=0
    words=1000
    document=1000
    dimension=50
    argv=sys.argv[1:]
    parser=optparse.OptionParser()
    parser.add_option('-n',action="store" ,dest="now",    default="0")
    parser.add_option('-w',action="store" ,dest="words",    default="0")
    parser.add_option('-f',action="store" ,dest="document",    default="0")
    parser.add_option('-d',action="store" ,dest="dimension",    default="0")
    options,remainder=parser.parse_args()

    if options.now =="1":
        now=1
    if options.words!='0':
        words=int(options.words)
    else:
        words=16200
    if options.document!='0':
        document=int(options.document)
    else:
        document=7500

    if options.dimension!='0':
        dimension=int(options.dimension)
    else:
        dimension=50
    p=PKS("./wrodlist.txt",document,now,words,dimension)
