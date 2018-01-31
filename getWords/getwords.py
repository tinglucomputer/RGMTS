# -*- coding:utf-8 -*-
__author__ = 'liupeng'
import os
import re
import optparse
import sys
'''get all the words from the txt files!
    从原始文件中提取出每个文档的单词并保存到一个单独的文件中
'''

start = 0

def getword(data_path,word_path):
    global start
    start+=1
    if start %100 == 0:
        print start,"->", data_path
    r=open(data_path,'r')
    w=open(word_path+str(start),'w')
    string=r.read()
    w.write(string)
    w.close()

def get_document(data_path, word_path):
    if os.path.isdir(data_path):
        lists=os.listdir(data_path)
        for i in lists:
            get_document(data_path+"/"+i, word_path)
    else:
        getword(data_path,word_path)

if __name__=='__main__':
    argv=sys.argv[1:]
    parser=optparse.OptionParser()
    parser.add_option('-i',action="store" ,dest="input_f",    default="")
    parser.add_option('-o',action="store" ,dest="output_f",    default="")
    options,remainder=parser.parse_args()

    if options.input_f !="0":
        data_path=options.input_f
    if options.output_f !='0':
        word_path=options.output_f

    lists=os.listdir(data_path)
    for i in lists:
        if os.path.isdir(data_path+"/"+i):
            dirs_child = data_path+"/"+i+"/all_documents"
            if os.path.exists(dirs_child):
                get_document(dirs_child,word_path+"/")
        else:
            getword(data_path+"/"+i,word_path+"/")




