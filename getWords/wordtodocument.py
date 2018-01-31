__author__ = 'liupeng'

import os
import re
import optparse
import sys
from Porter import  PorterStemmer

LEN=2665

dicts={}

p = PorterStemmer()
start=1
def getword(filename):
    global start
    print start,filename
    start+=1
    r=open(rfc_path+filename,'r')
    string=r.read()
    s=re.findall("[a-z]{2,}",str.lower(string))
    word_num = len(s)
    for word in s:
        word = p.stem(word, 0,len(word)-1)
        if dicts.has_key(word):
            if dicts[word].has_key(filename):
                dicts[word][filename][0]+=1
            else:
                dicts[word][filename]=[1,word_num]
        else:
            dicts[word]={}
            dicts[word][filename]=[1,word_num]
    r.close()

def writetofile(file_number, document_num, output_file):
    id=1
    f=open(output_file,'w')
    for word in dicts:
        keys=dicts[word].keys()
        lens = len(keys)
        if lens>0 and lens<file_number:
            f.write("%d %s : "%(id,word))
            string=""
            for i in range(lens-1):
                key=keys[i]
                string+=("%s:%d:%d,"%(key,dicts[word][key][0],dicts[word][key][1]))
            key=keys[lens-1]
            string+=("%s:%d:%d"%(key,dicts[word][key][0],dicts[word][key][1]))
            f.write("%s\n"%(string))
            id+=1
    f.close()

if __name__=='__main__':
    argv=sys.argv[1:]
    parser=optparse.OptionParser()
    parser.add_option('-i',action="store" ,dest="input_f",    default="")
    parser.add_option('-o',action="store" ,dest="output_f",    default="")
    parser.add_option('-d',action="store" ,dest="document_num",    default="")
    options,remainder=parser.parse_args()
    document_num = 4000
	
    if options.input_f !="0":
        rfc_path=options.input_f
    else:
        rfc_path = './rfc'
    if options.document_num !="0":
        document_num=int(options.document_num)

    if options.output_f !="0":
        output_file=options.output_f
    else:
        output_file = 'wrodlist.txt'

    lists=os.listdir(rfc_path)
    for i in lists:
        if re.match(r"[0-9]*",i):
            if int(i) <= document_num:
                getword(i)
    file_number = len(lists)
    writetofile(file_number, document_num,output_file)






