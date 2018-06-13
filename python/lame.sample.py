import os
import sys
from lame import *

LAME_PATH = "E:\\work\\svn\\rame\\build\\release"


def PrintLameScanResult(fname , result):
    sys.stdout.write(fname)
    if result is not None:
        sys.stdout.write(" " + "  Infected: " + result.vname + " (" + result.engid+")")
    
    sys.stdout.write("\n")
     
def lame_test(dbf , path):

    _lame = Lame(LAME_PATH)


    #_lame.SetParam("kill")
    
    if not _lame.Load(dbf):
        return

    _license = _lame.GetLicense()
    if _license is not None:
        print("License:")
        print("     Version: " + _license.version)
        print("     Owner  : " + _license.Owner)
        print("     Date   : " + _license.Data)
        print("     Authm  : " + _license.Authm)


    _version = _lame.GetVersion()
    if _version is not None: 
        print("Engine Version    :" + _version.engv)
        print("Viruse Lib Version: " + _version.vdbv)

    if os.path.isfile(path) :
        _result = _lame.ScanFile(path)
        PrintLameScanResult(path  , _result)
    else:
        for dirpath,dirnames,filenames in os.walk(path):
            for file in filenames:
                fullpath=os.path.join(dirpath,file)
                _result = _lame.ScanFile(fullpath)
                PrintLameScanResult(fullpath  , _result)



    _lame.Unload()

def LameCallBack(fname , result):
    sys.stdout.write(fname)
    if result is not None:
        sys.stdout.write(" " + "  Infected: " + result.vname + " (" + result.engid+")")
    
    sys.stdout.write("\n")

def lame_with_feedback_test(dbf , path):

    if dbf is None:
        return

    _lame = LameWithFeedback(LAME_PATH)
    _lame.SetCallack(LameCallBack)

    #_lame.SetParam("kill")

    if not _lame.Load(dbf):
        return

    _license = _lame.GetLicense()
    if _license is not None:
        print("License:")
        print("     Version: " + _license.version)
        print("     Owner  : " + _license.Owner)
        print("     Date   : " + _license.Data)
        print("     Authm  : " + _license.Authm)


    _version = _lame.GetVersion()
    if _version is not None: 
        print("Engine Version    :" + _version.engv)
        print("Viruse Lib Version: " + _version.vdbv)

    if os.path.isfile(path) :
        _lame.ScanFile(path)
    else:
        for dirpath,dirnames,filenames in os.walk(path):
            for file in filenames:
                fullpath=os.path.join(dirpath,file)
                _lame.ScanFile(fullpath)

    _lame.Unload()   
    
def test(path):

    if path is None:
        return

    _scan_path_count = len(path)
    if _scan_path_count == 0:
        return

    _dbf = VirusDb(LAME_PATH)
    if not _dbf.OpenVdb(None):
        print("Faild to openvdb.")
        return


    lame_test(_dbf , path[0])
    lame_with_feedback_test(_dbf , path[0])

    _dbf.CloseVdb()



def exit(signum, frame):  
    sys.exit()  

if  __name__ == "__main__":
    signal.signal(signal.SIGINT, exit)  
    signal.signal(signal.SIGTERM, exit)
    test(sys.argv[1:])
    

