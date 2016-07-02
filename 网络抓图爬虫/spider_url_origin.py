'''
Created on 2016-06-29
@author: 闫日亮
@mailto: 495432810@163.com
运行环境:Python3
'''
#coding:utf-8	#编码规则utf-8
import sys
import re
import urllib.request
from html.parser import HTMLParser
from html.parser import HTMLParseError
import os
import threading
import time
import chardet

class MyHtmlParser(HTMLParser):
    def __init__(self):
        HTMLParser.__init__(self)
        self.url = []
        self.img = []
        self.title = []
    def handle_starttag(self, tag, attrs):
        if tag == "a":
            for i in attrs:
                if i[0] == "href":
                    self.url.append(i[1])
        elif tag == "title":
            self.title = 1
        for i in attrs:
            if re.match('http://.+\.(jpg|jepg|png)',str(i[1])):
                self.img.append(i[1])

    def handle_data(self, data):
        if self.title == 1:
            self.title = data
        findimg = re.findall('http://.+?\.jpg',data)
        for i in range(0,len(findimg)):
                    findimg[i] = findimg[i]
        self.img += findimg

    def handle_startendtag(self, tag, attrs):
        if tag == "a":
            for i in attrs:
                if i[1] == "href":
                    self.url.append(i[1])
        for i in attrs:
            if re.match('http://.+\.(jpg|jepg|png)',str(i[1])):
                self.img.append(i[1])

class ScratchFactory(threading.Thread):
    def __init__(self,url):
        threading.Thread.__init__(self)
        self.url = url
        self.tempImgs = []
        self.tempUrls = []
        self.title = []
        global seed
        match = re.search(seed + '.*/',url)
        #if match:
        #    self.pwd = match.group()

    def addHeader(self,data):
        global seed
        for i in range(0,len(data)):
            if re.match("http.+", data[i]) == None:
                if re.match("/.*",data[i]):
                    data[i] = seed + data[i]
                #elif re.match('./.*',data[i]):
                #    data[i] = self.pwd + data[i][2:]
                #else:
                #    data[i] = self.pwd + data[i]
        return data
    def run(self):
        try:
            conect = urllib.request.urlopen(self.url)    #下载网页数据
            data = conect.read()
            conect.close()
            htmlx = MyHtmlParser()
            t = chardet.detect(data)                     #获得html编码
            if t['encoding']:
                charset = t['encoding']
            else:
                charset = 'utf-8'
            htmlx.feed(data.decode(charset,'ignore'))
            self.title = htmlx.title
            self.tempUrls = self.addHeader(htmlx.url)    #给相对路径链接加上头
            self.tempImgs = self.addHeader(htmlx.img)
            htmlx.close()
            self.clearData()                             #过滤无用链接
            threading.Thread(target = \
                             self.saveImages,args = () ).start()  #下载图片
        except HTMLParseError as e:
            print("####Error : 1 ######:",e , '--->',  self.url)
        except Exception as e:
            print("####Error : 2 ######:",e , '--->' , self.url)

        global UrlSrc,UrlDiged,mLock
        mLock.acquire()
        t = []
        for temp in self.tempUrls:
            if not UrlDiged.__contains__(temp):
                t.append(temp)
        l = []
        for temp in t:
            if not UrlSrc.__contains__(temp):
                l.append(temp)
        UrlSrc += l
        mLock.release()

    def clearData(self):
        #去除重复链接
        self.tempUrls = set(self.tempUrls)
        self.tempImgs = set(self.tempImgs)
        global seed
        t = []
        for temp in self.tempUrls:                    #<-链接过滤,正则表达式
            if re.match(seed + "/.*", temp):
                t.append(temp)
        self.tempUrls = t

        t = []
        for temp in self.tempImgs:                    #<-图片过滤,正则表达式
            if re.match(".+.(gif|jpg|png)",temp):
                t.append(temp)
        self.tempImgs = t
        self.title = re.split('(-|_)',self.title)[0]  #<-页面标题分隔，截取title中关键字
        #去除title中非法字符
        self.title = self.title.replace(' ','')
        self.title = self.title.replace('/','')
        self.title = self.title.replace('\\','')
        self.title = self.title.replace(':','')
        self.title = self.title.replace('|','')
        self.title = self.title.replace('?','')
        self.title = self.title.replace('*','')
        self.title = self.title.replace('<','')
        self.title = self.title.replace('>','')
        self.title = self.title.replace('\r','')
        self.title = self.title.replace('\n','')
        self.title = self.title.replace('\t','')

    def save(self,path,url):
        global MinSize
        try:
            req = urllib.request.Request(url)
            req.add_header("User-Agent", "Mozilla/5.0 (Windows NT 6.1) \
            AppleWebKit/537.11 (KHTML, like Gecko) \
            Chrome/23.0.1271.64 Safari/537.11")
            req.add_header("Referer",self.url)         #有些网站防盗链，所以自己加上头
            conect = urllib.request.urlopen(req)
            t = conect.read()
            conect.close()
            if t.__len__() < MinSize:
                return
            #if not os.path.exists(path):
            #    os.mkdir(path)
            f = open(path + self.title + time.strftime("%H%M%S",\
                     time.localtime()) + ".jpg","wb")
            f.write(t)
            f.close()
        except HTMLParseError as e:
            print("####Error : 3 ######:",e , '--->',  url)
        except Exception as e:
            print("####Error : 4 ######:",e , '--->',  url)

    def saveImages(self):
        global IMG_TIME
        global SAVE_PATH
        if len(self.tempImgs) == 0:
            return
        path = SAVE_PATH + self.title
        print("Downdow------->",self.title)
        while len(self.tempImgs) != 0:
            t = threading.Thread(target=self.save,args=\
                                 (path,self.tempImgs.pop(0)))
            if len(self.tempImgs) != 0:
                t.start()
                time.sleep(IMG_TIME)
            else:
                t.start()
                t.join()
                if os.path.exists(path) and len(os.listdir(path)) == 0:
                    os.rmdir(path)


def save():
    global mLock
    global UrlSrc
    #global ImgDiged
    #global iLock
    global SAVE_PATH
    mLock.acquire()
    #iLock.acquire()
    try:
        f = open(SAVE_PATH + r"/UrlDiged.txt",'w')
        for i in UrlDiged:
            f.write(i + '\n')
        f.close()

        f = open(SAVE_PATH +r"/UrlSrc.txt",'w')
        for i in UrlSrc:
            f.write(i + '\n')
        f.close()

        print("********************* Saved **********************")
    except Exception as e:
        print (e)
    finally:
        mLock.release()


def readBackup():
    global UrlDiged
    global UrlSrc
    try:
        f = open(SAVE_PATH + r"/UrlDiged.txt",'r')
        while True:
            t = f.readline()
            if t == '':
                break
            t = t.replace('\n','')
            UrlDiged.append(t)
        f.close()
        f = open(SAVE_PATH + r"/UrlSrc.txt",'r')
        while True:
            t = f.readline()
            if t == '':
                break
            t = t.replace('\n','')
            UrlSrc.append(t)
        f.close()
    except Exception as e:
        print(e)


#*****************************start********************************


if __name__ == '__main__':

    #timeout = 20
    #socket.setdefaulttimeout(timeout)
    #seed = "http://www.27270.com/ent/meinvtupian/"    #<-站点的根页面
    seed = sys.argv[2]
    SAVE_PATH = sys.argv[1]      	#<-存储目录
    THREAD_NUM = 35                 #<-限制线程数，以控制下载速度，防止出现类DDos攻击
    SLEEP_TIME = 2.5                #<-每次请求链接的时间间隔(秒)，太快不一定好哟！
    MinSize = 72000                 #<-过滤小图片，初始32k
    IMG_TIME = 1.5                  #<-下载图片速度，初始1.5秒一张
    UrlSrc = [seed]                 #存储获得的未遍历过的链接
    UrlDiged = []                   #存储遍历过的链接
    mLock = threading.Lock()        #UrlSrc和UrlDiged的同步锁
    savetime = time.time()

    if not os.path.exists(SAVE_PATH):
        os.mkdir(SAVE_PATH)
    if seed[-1:] == '/':
        seed = seed[:-1]

    #读取上一次运行的现场
    if not os.path.exists(SAVE_PATH + r'/UrlDiged.txt'):
        try:
            f = open(SAVE_PATH + r'/UrlDiged.txt','w')
            f.close()
            f = open(SAVE_PATH + r'/UrlSrc.txt','w')
            f.close()
        except Exception as e:
            print(e)
    else:
        readBackup()


    while True:
        if len(threading.enumerate()) > THREAD_NUM:
            continue
        mLock.acquire()
        if UrlSrc.__len__():
            temp = UrlSrc.pop(0)
            t = ScratchFactory(temp)
            UrlDiged.append(temp)
            t.start()
        mLock.release()
        #打印当前连接数、线程数、urlsrc+urldiged表长
        #print("Conections:",UrlSrc.__len__(),"*****threads:",\
        #      len(threading.enumerate()),"****TableLength:",\
        #      (len(UrlSrc)+len(UrlDiged))/1000)
        if time.localtime().tm_min%2 == 0 \
        and time.time() - savetime > 60 :
            save()                 #保存现场
            savetime = time.time()
        time.sleep(SLEEP_TIME)

