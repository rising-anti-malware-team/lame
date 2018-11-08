import os
import sys
from lame import *

def format_bstring(bstr, CodePage=None):
    if 3 == sys.version_info.major:
        if CodePage is None:
            return bstr.decode()
        else:
            return bstr.decode(CodePage)
    else:
        return bstr

def PrintLameScanResult(fname, result):
    sys.stdout.write(fname)
    if result is not None:
        sys.stdout.write(" " + "  Infected: " + format_bstring(result.vname) + " (" + format_bstring(result.engid) + ")")
    sys.stdout.write("\n")

def print_lame_licence_info(_lame):
    _license = _lame.GetLicense()
    if _license is not None:
        print("License:")
        print("     Version: " + format_bstring(_license.Version))
        print("     Owner  : " + format_bstring(_license.Owner,'gb2312'))
        print("     Date   : " + format_bstring(_license.Date))
        print("     Authm  : " + format_bstring(_license.Authm))
        print("     Data  :  " + format_bstring(_license.Data) + "\n")
    return

def print_lame_version_info(_lame):
    _version = _lame.GetVersion()
    if _version is not None:
        print("Engine Version    : " + format_bstring(_version.engv))
        print("Viruse Lib Version: " + format_bstring(_version.vdbv) + "\n")
    return

def lame_test(dbf, lame_path, scan_path):
    _lame = Lame(lame_path)
    #_lame.SetParam("kill")

    if not _lame.Load(dbf):
        sys.stdout.write("load viruslib failed.")
        return

    print_lame_licence_info(_lame)
    print_lame_version_info(_lame)

    if os.path.isfile(scan_path) :
        _result = _lame.ScanFile(scan_path)
        PrintLameScanResult(scan_path, _result)
    else:
        for dirpath,dirnames,filenames in os.walk(scan_path):
            for file in filenames:
                fullpath=os.path.join(dirpath,file)
                _result = _lame.ScanFile(fullpath)
                PrintLameScanResult(fullpath, _result)
    _lame.Unload()
    return

def enter_file(fname, depth, _lame):
    _lame._virus_name = None
    return LameWithFeedback.LSCT_CONTINUE

def leave_file(fname, depth, l, _lame):
    sys.stdout.write(format_bstring(fname))
    if _lame._virus_name is not None:
        sys.stdout.write(" " + "  Infected: " + format_bstring(_lame._virus_name))
        _lame._virus_name = None
    sys.stdout.write("\n")
    return

def alarm(fname, result, _lame):
    if result is not None:
        _lame._virus_name = result.vname
    return LameWithFeedback.LSCT_CONTINUE

def lame_with_feedback_test(dbf, lame_path, scan_path):
    if dbf is None:
        return

    _lame = LameWithFeedback(lame_path)
    _lame.SetCallack(enter_file, leave_file, alarm)

    #_lame.SetParam("kill")

    if not _lame.Load(dbf):
        return

    print_lame_licence_info(_lame)
    print_lame_version_info(_lame)

    if os.path.isfile(scan_path) :
        _lame.ScanFile(scan_path)
    else:
        for dirpath,dirnames,filenames in os.walk(scan_path):
            for file in filenames:
                fullpath=os.path.join(dirpath,file)
                _lame.ScanFile(fullpath)
    _lame.Unload()
    return


def main(path):
    if path is None:
        return

    _scan_path_count = len(path)
    if _scan_path_count == 0:
        return

    print('\n---- test start ----\n')

    lame_path = path[0]
    scan_path = path[1]

    _dbf = VirusDb(lame_path)
    if not _dbf.OpenVdb(None):
        print('Faild to openvdb.')
        return

    print('\n---- normal sample ----\n')
    lame_test(_dbf, lame_path, scan_path)

    print('\n---- callback sample ----\n')
    lame_with_feedback_test(_dbf, lame_path, scan_path)

    _dbf.CloseVdb()

    print('\n---- test end ----\n')
    return



def exit(signum, frame):
    sys.exit()

# sys.argv.append('D:\\MyJob\\SDK\\produce\\make\\lame-win-x64')
# sys.argv.append('D:\\rsdata\\1\\11')

if  __name__ == "__main__":
    signal.signal(signal.SIGINT, exit)
    signal.signal(signal.SIGTERM, exit)
    main(sys.argv[1:])

