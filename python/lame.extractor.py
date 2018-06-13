import os
import sys
import platform
import signal
from lame import *

LAME_PATH = "E:\\work\\svn\\rame\\build\\release"
class LameDumper:

    def __init__(self , lpath , need_dump):
        self.__lame = LameExtractor(lpath)
        self.__vbdf = VirusDb(lpath)
        self.__work_path = os.path.join(lpath,"bin")
        self.__dump = need_dump
        if not os.path.exists(self.__work_path):
            os.mkdir(self.__work_path)

    def Load(self):

        if not self.__vbdf.OpenVdb(None):
            return False

        if not self.__lame.Load(self.__vbdf):
            self.__vbdf.CloseVdb()
            return False

        self.__lame.SetEnterFileCallBack(self.__Enter_File)
        self.__lame.SetLeaveFileCallBack(self.__Leave_File)

        return True

    def Unload(self):
        if self.__vbdf is not None:
            self.__vbdf.CloseVdb()

        if self.__lame is not None:
             return self.__lame.Unload()


    def Extract(self , fname , password):

        if fname is None or password is None:
            return

        return self.__lame.ExtractFile(fname , password)

    def __Enter_File(self, immfile):
        print("FileName: " + immfile.FileName + "   size:" + str(immfile.GetSize()) + "  depth:" + str(immfile.Depth))

        if self.__dump and immfile.Depth > 0:
            self.DumpFile(immfile)
        return LameExtractor.LCT_DO_EXTRACT


    def __Leave_File(self, immfile , lxlvt):
        if lxlvt == LameExtractor.LXLVT_NORMAL:
            pass
        elif lxlvt == LameExtractor.LXLVT_ABORT:
            print("The procedure has be abort.")
        elif lxlvt == LameExtractor.LXLVT_ERROR:
            print("There is error in the procedure.")
        else:
            pass

    def DumpFile(self , immfile):
        '''
        new_path = os.path.join(self.__work_path , immfile.InnerPath)
        if not os.path.exists(new_path):
            os.makedirs(new_path)
        '''
         
        new_file_name = os.path.join(self.__work_path , immfile.FileName)
        with open(new_file_name , 'wb') as handle:

            while True:
                data = immfile.Read(0x1000)
                if data is None:
                    break

                handle.write(data)


def help():
    print("usage:")
    print("     lame.extractor.py [-password password] [-dump] file")

def main(argv):
    

    password = ""
    path_lst = []
    need_dump = False

    vc = len(argv)
    idx = 0
    while idx < vc:
        s = argv[idx].lower().strip()
        idx += 1

        if s == "-password":

            if idx < vc:
                password = argv[idx]
                idx += 1
        elif s == "-dump":
            need_dump= True
        else:
            path_lst.append(s)


    if len(path_lst) == 0:
        help()
        return

    
    # load
    _LameDumper = LameDumper(LAME_PATH , need_dump)
    if not _LameDumper.Load():
        return

    # extract
    for _path in path_lst:
        _LameDumper.Extract(_path , password)

    # unload
    _LameDumper.Unload()



def exit(signum, frame):  
    sys.exit()  
  

if  __name__ == "__main__":
    signal.signal(signal.SIGINT, exit)  
    signal.signal(signal.SIGTERM, exit)
    main(sys.argv[1:])
