import os
import sys
import platform
import signal
import ccolor
from lame import *

def format_bstring(bstr, CodePage=None):
    if 3 == sys.version_info.major:
        if CodePage is None:
            return bstr.decode()
        else:
            return bstr.decode(CodePage)
    else:
        return bstr

class LameScanner:

    def __init__(self, lpath):
        self._lame = Lame(lpath)
        self._color = ccolor.ConsoleColor()

    def Load(self, vdb):
        if self._lame is None:
            return
        if not self._lame.Load(vdb):
            return
        return True

    def SetParam(self, param):
        if param is None:
            return
        if self._lame is None:
            return
        self._lame.SetParam(param)
        return


    def Scan(self, path_):
        if os.path.isfile(path_):
            self.__ScanFile(path_)
        else:
            self.__ScanDir(path_)
        return

    def Unload(self):
        if self._lame is not None:
            self._lame.Unload()
            self._lame = None
        return

    def ShowLicense(self):
        if self._lame is None:
            return
        _license = self._lame.GetLicense()
        if _license is None:
            print("get licence information failed.")
            return
        print("License:")
        print("     Version: " + format_bstring(_license.version))
        print("     Owner  : " + format_bstring(_license.Owner, 'gb2312'))
        print("     Date   : " + format_bstring(_license.Data))
        print("     Authm  : " + format_bstring(_license.Authm))
        print("     Data   : " + format_bstring(_license.Data) + "\n")
        return

    def ShowVersion(self):
        if self._lame is None:
            return
        _version = self._lame.GetVersion()
        if _version is None:
            return
        print("Engine Version    :" + format_bstring(_version.engv))
        print("Viruse Lib Version:" + format_bstring(_version.vdbv))
        return

    def __ScanFile(self, fname):
        if not os.path.exists(fname):
            return
        if self._lame is None:
            return
        sys.stdout.write(fname)
        _result = self._lame.ScanFile(fname)
        if _result is not None:
            sys.stdout.write(" " + "  Infected: " + format_bstring(_result.vname) + " (" + format_bstring(_result.engid) + ")")
        sys.stdout.write("\n")
        return

    def __ScanDir(self, dir_):
        if not os.path.exists(dir_):
            return
        for dirpath,dirnames,filenames in os.walk(dir_):
            for file in filenames:
                fullpath=os.path.join(dirpath,file)
                self.__ScanFile(fullpath)
        return


class LameScanner2:

    def __init__(self, lpath):
        self._lame = LameWithFeedback(lpath)
        self._lame.SetCallack(self.__EnterFile, self.__LeaveFile, self.__Alarm)
        self._color = ccolor.ConsoleColor()
        self._virus_info_list = []

    def Load(self, vdb):
        if self._lame is None:
            return
        if not self._lame.Load(vdb):
            return
        return True

    def SetParam(self, param):
        if param is None:
            return
        if self._lame is None:
            return
        self._lame.SetParam(param)
        return

    def Unload(self):
        if self._lame is not None:
            self._lame.Unload()
            self._lame = None
        return

    def ShowLicense(self):
        if self._lame is None:
            return
        _license = self._lame.GetLicense()
        if _license is None:
            print("get licence information failed.")
            return
        print("License:")
        print("     Version: " + format_bstring(_license.version))
        print("     Owner  : " + format_bstring(_license.Owner))
        print("     Date   : " + format_bstring(_license.Data))
        print("     Authm  : " + format_bstring(_license.Authm))
        return

    def ShowVersion(self):
        if self._lame is None:
            return
        _version = self._lame.GetVersion()
        if _version is None:
            return
        print("Engine Version    : " + format_bstring(_version.engv))
        print("Viruse Lib Version: " + format_bstring(_version.vdbv))
        return

    def __EnterFile(self, fname, depth):
        self._virus_info_list.append([fname, ""])
        return LameWithFeedback.LSCT_CONTINUE

    def __LeaveFile(self, fname, depth):
        if self._virus_info_list.count == 0:
            return
        vinfo = self._virus_info_list[len(self._virus_info_list) - 1]
        self._virus_info_list.remove(vinfo)
        sys.stdout.write(vinfo[0])
        if len(vinfo[1]) != 0:
            sys.stdout.write(" " + "  Infected: " + vinfo[1])
        sys.stdout.write("\n")
        return

    def __Alarm(self, fname, result):
        if result is None:
            return LameWithFeedback.LSCT_CONTINUE
        self._virus_info_list[len(self._virus_info_list) - 1][1] = result.vname + " (" + result.engid+")"
        return LameWithFeedback.LSCT_CONTINUE

    def Scan(self, path_):
        if os.path.isfile(path_):
            self.__ScanFile(path_)
        else:
            self.__ScanDir(path_)
        return

    def __ScanFile(self, fname):
        if not os.path.exists(fname):
            return
        if self._lame is None:
            return
        self._lame.ScanFile(fname)
        return

    def __ScanDir(self, dir_):
        if not os.path.exists(dir_):
            return
        for dirpath,dirnames,filenames in os.walk(dir_):
            for file in filenames:
                fullpath=os.path.join(dirpath,file)
                self.__ScanFile(fullpath)
        return


def exit(signum, frame):
    sys.exit()

def main(lame_path, argv):
    _lame_path = lame_path
    _param_lst = []
    _scan_path_lst = []

    bShowVersion = False
    bShowLicense = False
    bShowFileList = False

    for _param in argv:
        if _param.startswith('-'):
            if _param.lower() == "-version":
                bShowVersion = True
            elif _param.lower() == "-license":
                bShowLicense = True
            elif _param.lower() == "-show-file-list":
                bShowFileList = True
            else:
                _param_lst.append(_param[1:])
        else:
            _scan_path_lst.append(_param)

    _lame = None
    if bShowFileList:
        _lame = LameScanner2(_lame_path)
    else:
        _lame = LameScanner(_lame_path)

    if bShowLicense:
        _lame.ShowLicense()

    if bShowVersion:
        _lame.ShowVersion()

    if 0 == len(_scan_path_lst):
        return

    _dbf = VirusDb(_lame_path)
    if not _dbf.OpenVdb(None):
        print("Faild to openvdb.")
        return

    for _param in _param_lst:
        _lame.SetParam(_param)

    if not _lame.Load(_dbf):
        _dbf.CloseVdb()
        print("Faild to load lame.")
        return

    for _path in _scan_path_lst:
        _lame.Scan(_path)

    _lame.Unload()
    _dbf.CloseVdb()
    return

# sys.argv.append('D:\\MyJob\\SDK\\produce\\make\\lame-win-x64')
# sys.argv.append('D:\\rsdata\\1\\11')

if  __name__ == "__main__":
    signal.signal(signal.SIGINT, exit)
    signal.signal(signal.SIGTERM, exit)
    main(sys.argv[1], sys.argv[2:])
