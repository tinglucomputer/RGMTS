__author__ = 'liupeng'


import os
import time
import math
from CreateInvertibleMatrix import InverMatr

class Node(object):

    def __init__(self,index=[],index_1=[],index_2=[],lchild=None,rchild=None,document_id=-1):
        self.index        = index
        self.index_1      = index_1
        self.index_2      = index_2
        self.document_id  = document_id
        self.left         = lchild
        self.right        = rchild

class TPDS2015():
    def __init__(self,document_num,words_num,index,client_index,client_list,new_build=0,dimesion = 50):
        self.index        = index
        self.words_num    = words_num
        self.document_num = document_num
        self.encrypt_num  = 0
        self.all          = 0
        self.client_index = client_index
        self.client_list  = client_list
        self.dimesion     = dimesion
        self.index_file   = "./index/index_tpds2015_%d_%d.pickle"%(document_num,words_num)

        ma = InverMatr(words_num,1)

        #if not os.path.isfile(self.index_file) or  new_build:
	if not os.path.isfile(self.index_file):
            start=time.clock()
            print "building tpds2015 index..."
            print "-- build index"
            self.head         = self.build_index(document_num,words_num)
            print "-- save unencrypted index"
            self.sava_index_to_file(self.index_file+"_ue",self.head,self.all)
            print "-- save finish"

            os.system("./encrypt -f %d -w %d -d %d -k 2 -n %d"%(document_num,words_num,dimesion,new_build))
            os.remove(self.index_file+"_ue")


    def save_node(self,node,f,dimension):
        f.write("id:%d\n"%(node.document_id))
        string="index: "
        for i in range(dimension-1):
            string+='%.13lf '%(node.index[i])
        string+='%.13lf'%(node.index[dimension-1])
        f.write("%s\n"%(string))

    def sava_index_to_file(self,file_name,head,node_num):
        f = open(file_name,"w")
        dimension = self.words_num
        queue=[]
        temp=[]
        queue.append(head)
        f.write("all:%d\n"%(node_num))
        while queue:
            for node in queue:
                self.save_node(node,f,dimension)
                if node.left != None:
                    temp.append(node.left)
                if node.right != None:
                    temp.append(node.right)
            queue = temp[:]
            temp  = []
        f.close()

    def build_index(self,document_num,words_num):
        nodes=[]
        for i in range(document_num):
            index_value=[0.0]*words_num
            for j in range(words_num):
                index_value[j]=self.index[j][i]
            node = Node(index=index_value[:],document_id=i)
            self.all+=1
            nodes.append(node)

        while nodes:
            lens=len(nodes)
            t=2**int(math.log(lens,2))
            left = nodes[t:]
            nodes = nodes[:t]
            temp=[]
            for i in xrange(0,t,2):
                index_value=[0.0]*words_num
                for j in range(words_num):
                    index_value[j]=max(nodes[i].index[j],nodes[i+1].index[j])
                node = Node(index=index_value[:],lchild=nodes[i],rchild=nodes[i+1])
                self.all+=1
                temp.append(node)
            nodes=temp[:]+left
            if len(nodes)==1:
                return nodes[0]
