import os
import sys
from ctypes import *
import platform
import signal
import ccolor


class LameScanResult(Structure):
    _fields_ = [
        ("mkclass", c_int),
        ("kclass", c_char  * 16),
        ("kclass_desc", c_char  * 16),
        ("kclass_desc_w", c_wchar  * 16),
        ("engid", c_char  * 32),
        ("vname", c_char  * 256),
        ("vid32", c_int),
        ("vid40", c_longlong),
        ("hitag", c_char  * 32),
        ("treat", c_int)
        ]

class LameVersionInfo(Structure):
    _fields_ = [
        ("engv", c_char * 33),
        ("vdbv", c_char * 33)
    ]

class LameLicenseInfo(Structure):
    _fields_ = [
        ("Version", c_char * 16),
        ("Owner" , c_char * 128),
        ("Date"  , c_char * 64),
        ("Authm" , c_char * 32),
        ("Data"  , c_char * 2048)
    ]

class LameUtity:

    @staticmethod
    def GetFullLameName(lpath):
        if platform.system() == "Windows":
            os.environ['path'] = os.environ['path'] + ';' + lpath
            lamepath = lpath + '\\lame.dll'
        else:
            lamepath = lpath + '/liblame.so'

        return lamepath


class VirusDb:
 
    def __init__(self, lpath):
        _lame_path = LameUtity.GetFullLameName(lpath)
        if not os.path.exists(_lame_path):
            raise Exception("File is not exists.")
        self._lame_dll_handle = CDLL(_lame_path)

        if self._lame_dll_handle is None:
            raise Exception("Faild to load lame library.")


    def OpenVdb(self,vdbf):
        if self._lame_dll_handle is None:
            return False

        self._lame_lib_handle = self._lame_dll_handle.lame_open_vdb(vdbf)
        if self._lame_lib_handle is None:
            return False

        return True 

    def CloseVdb(self):
        if self._lame_dll_handle is None:
            return

        if self._lame_lib_handle is None:
            return

        self._lame_dll_handle.lame_close_vdb(self._lame_lib_handle)
        self._lame_lib_handle = None

    def GetVbdHandle(self):
        return self._lame_lib_handle

class LameBase:

    def __init__(self, lpath):
        self._lame_path = LameUtity.GetFullLameName(lpath)
        if not os.path.exists(self._lame_path):
            raise Exception("File is not exists.")
        self._lame_dll_handle = CDLL(self._lame_path)

        if self._lame_dll_handle is None:
            raise Exception("Faild to load lame library.")

        self._lame_path = lpath
        self._lame_engine_handle = None
        self._params = []

    def SetParam(self, param):
        if self._lame_dll_handle is None:
            return

        if param is None:
            return

        self._params.append(param)

        if self._lame_engine_handle is not None:
            self.SetParameter(param)

        return


    def SetParameter(self, param):
        if 3 == sys.version_info.major:
            p = param.encode('gbk')
        else:
            p = param
        return self._lame_dll_handle.lame_param_set(self._lame_engine_handle, p)


    def Load(self, vdb_object):
        self._vbd_handle = vdb_object.GetVbdHandle()
        if self._vbd_handle is None:
            return False

        self._lame_engine_handle = self._lame_dll_handle.lame_create(self._vbd_handle)
        if self._lame_engine_handle == 0:
            return False

        for p in self._params:
            self.SetParameter(p)

        ret = self._lame_dll_handle.lame_init(self._lame_engine_handle)

        if ret < 0:
            self._lame_dll_handle.lame_destroy(self._lame_engine_handle)
            return False

        return True


    def GetVersion(self):
        try:
            if self._lame_dll_handle is None:
                return None
            _version = LameVersionInfo()
            ret = self._lame_dll_handle.lame_get_version(byref(_version))
            if ret < 0:
                return None
            return _version
        except Exception as e:
            print(e)
            return None


    def GetLicense(self):
        try:
            if self._lame_dll_handle is None:
                return None
            _License = LameLicenseInfo()
            ret = self._lame_dll_handle.lame_get_licence_info(byref(_License))
            if ret < 0:
                return None
            return _License
        except Exception as e:
            print(e)
            return None


    def Unload(self):
        if self._lame_dll_handle is None:
            return

        if self._lame_engine_handle is None:
            return

        self._lame_dll_handle.lame_destroy(self._lame_engine_handle)


class Lame(LameBase):

    def __init__(self, lpath):
        LameBase.__init__(self, lpath)

    def ScanFile(self,fname):

        if self._lame_dll_handle is None:
            return None

        if self._lame_engine_handle is None:
            return None

        if fname is None:
            return None

        if not os.path.exists(fname):
            return None

        _result = LameScanResult()

        if 3 == sys.version_info.major:
            scan_file = self._lame_dll_handle.lame_scan_file_w
        else:
            scan_file = self._lame_dll_handle.lame_scan_file

        ret = scan_file(self._lame_engine_handle, fname, byref(_result))
        if ret < 0:
            return None

        return _result

    def ScanMem(self, data):
        if self._lame_dll_handle is None:
            return None

        if self._lame_engine_handle is None:
            return None

        if data is None:
            return None

        _result = LameScanResult()
        ret = self._lame_dll_handle.lame_scan_mem(self._lame_engine_handle, data, len(data), byref(_result))
        if ret < 0:
            return None

        return _result

    def Clone(self):
        _lame = LameWithFeedback(self._lame_path) 

        for s in self._params:
            _lame.SetParam(s)
        
        if not _lame.Load(self._vbd_handle):
            return None

        return _lame


if os.name == 'posix':
    WINFUNCTYPE = CFUNCTYPE

LAMESCANENTERFILE = WINFUNCTYPE(c_int, c_char_p, c_int, c_void_p)
def LameScanEnterFile(fname, depth, userdata):
    _lame = cast(userdata, py_object).value
    return _lame.EnterFile(fname, depth, _lame)

LAMESCANLEAVEFILE = WINFUNCTYPE(None, c_char_p, c_int, c_void_p, c_int)
def LameScannerLeaveFile(fname, depth, userdata, l):
    _lame = cast(userdata, py_object).value
    _lame.LeaveFile(fname, depth, _lame, l)

LAMESCANALARM = WINFUNCTYPE(None, c_char_p, POINTER(LameScanResult), c_void_p)
def LameScannerAlarm(fname, result, userdata):
    _result = None
    if  hasattr(result, "contents"):
        _result = result.contents
    _lame = cast(userdata, py_object).value
    return _lame.Alarm(fname, _result, _lame)

class LameScanFeedback(Structure):
    _fields_ = [
        ("lame_scan_enter_file", LAMESCANENTERFILE),
        ("lame_scan_leave_file", LAMESCANLEAVEFILE),
        ("lame_scan_alarm", LAMESCANALARM),
    ]

class LameWithFeedback(LameBase):

    LSCT_CONTINUE = 0x01
    LSCT_ABORT    = 0x02

    def __init__(self, lpath):
        LameBase.__init__(self, lpath)
        self.EnterFile = None
        self.LeaveFile = None
        self.Alarm = None
        self._this_obj_ref = c_void_p()
        self._this_obj_ref.value = id(self)
        self._virus_name = None
        self._feedback = LameScanFeedback()
        self._feedback.lame_scan_enter_file = LAMESCANENTERFILE(LameScanEnterFile)
        self._feedback.lame_scan_leave_file = LAMESCANLEAVEFILE(LameScannerLeaveFile)
        self._feedback.lame_scan_alarm = LAMESCANALARM(LameScannerAlarm)

    def SetCallack(self, enter_file, leave_file, alram):
        self.EnterFile = enter_file
        self.LeaveFile = leave_file
        self.Alarm = alram

    def ScanFile(self, fname):

        if self._lame_dll_handle is None:
            return False

        if self._lame_engine_handle is None:
            return False

        if fname is None:
            return False

        if 3 == sys.version_info.major:
            scan_file_with_callback = self._lame_dll_handle.lame_scan_file_with_callback_w
        else:
            scan_file_with_callback = self._lame_dll_handle.lame_scan_file_with_callback

        scan_file_with_callback(self._lame_engine_handle, fname, byref(self._feedback), self._this_obj_ref)
        return True

    def ScanMem(self, data):
        if self._lame_dll_handle is None:
            return False

        if self._lame_engine_handle is None:
            return False

        if data is None:
            return False

        self._lame_dll_handle.lame_scan_mem_with_callback(self._lame_engine_handle, data, len(data), byref(self._feedback), self._this_obj_ref)

        return True


    def Clone(self):
        _lame = LameWithFeedback(self._lame_path) 

        for s in self._params:
            _lame.SetParam(s)

        if not _lame.Load(self._vbd_handle):
            return None

        _lame.EnterFile = self.EnterFile
        _lame.LeaveFile = self.EnterFile
        _lame.Alarm = self.Alarm

        return _lame


class LameImmediateFile:

    def __init__(self, dhanle, fhandle):
        self._lame_dll_handle = dhanle
        self._file_handle = fhandle
        self.FullName = ""
        self.FileName = ""
        self.Depth = 0
        self.InnerPath = ""
        self.Seek(0,0)

    def GetSize(self):
        if not self.__Is_Valid_Handle():
            return 0
        return self._lame_dll_handle.lame_file_get_size(self._file_handle)

    def Seek(self, offset, seekOrigin):
        if not self.__Is_Valid_Handle():
            return False

        if (seekOrigin >2 or seekOrigin < 0):
            return False

        ret = self._lame_dll_handle.lame_file_seek(self._file_handle, offset, seekOrigin)
        if ret != 0:
            return False
        return True


    def Tell(self):
        if not self.__Is_Valid_Handle():
            return 0
        return self._lame_dll_handle.lame_file_tell(self._file_handle)


    def Read(self, size):
        if not self.__Is_Valid_Handle():
            return 0

        _file_size = self.GetSize()
        _pos = self.Tell()

        _remain_bytes = _file_size - _pos
        _need_read_bytes = 0
        if _remain_bytes > size:
            _need_read_bytes = size
        else: 
            _need_read_bytes = _remain_bytes

        if _need_read_bytes == 0:
            return None

        buf = (c_byte * _need_read_bytes)()
        _real_read_bytes = self._lame_dll_handle.lame_file_read(self._file_handle, buf, _need_read_bytes)

        if _need_read_bytes != _real_read_bytes:
            return None
        return buf

    def __Is_Valid_Handle(self):
        if self._lame_dll_handle is None:
            return False
        if self._file_handle is None:
            return False
        return True


EXTRACTENTERCALLBACK = WINFUNCTYPE(c_int,c_char_p, c_char_p, c_uint32, c_uint64, c_void_p, c_void_p)

def ExtractEnterCallBack(fname, fmt, depth, flag, fhandle, userdata):
    _lame = cast(userdata, py_object).value

    _imm_file = LameImmediateFile(_lame._lame_dll_handle, fhandle)
    _imm_file.FullName = fname
    _imm_file.Depth = depth

    return _lame._Enter_File(_imm_file)

EXTRACTLEAVECALLBACK = WINFUNCTYPE(None,c_char_p, c_char_p, c_uint32, c_uint64, c_void_p, c_void_p, c_int)
def ExtractLeaveCallBack(fname, fmt, depth, flag, fhandle, userdata, lxlvt):
    _lame = cast(userdata, py_object).value

    _imm_file = LameImmediateFile(_lame._lame_dll_handle, fhandle)
    _imm_file.FullName = fname
    _imm_file.Depth = depth

    return _lame._Leave_File(_imm_file, lxlvt)


class LameFeedback(Structure):
    _fields_ = [
        ("lame_extract_enter_file", EXTRACTENTERCALLBACK),
        ("lame_extract_leave_file", EXTRACTLEAVECALLBACK),
    ]

class LameExtractor(LameBase):
    LCT_CONTINUE = 0x01
    LCT_DO_EXTRACT = 0x02
    LCT_ABORT = 0x03

    LXLVT_NORMAL = 0x01
    LXLVT_ABORT  = 0x02
    LXLVT_ERROR  = 0x03

    def  __init__(self, lpath):
        LameBase.__init__(self, lpath)
        self._this_obj_ref = c_void_p()
        self._this_obj_ref.value = id(self)
        self._Report = None
        self._feedback = LameFeedback()
        self._feedback.lame_extract_enter_file = EXTRACTENTERCALLBACK(ExtractEnterCallBack)
        self._feedback.lame_extract_leave_file = EXTRACTLEAVECALLBACK(ExtractLeaveCallBack)

    def Load(self, vdbf):
        if vdbf is None:
            return False
        return LameBase.Load(self, vdbf)

    def Unload(self):
        LameBase.Unload(self)

    def ExtractFile(self, fname, password):
        self._lame_dll_handle.lame_extract_file(self._lame_engine_handle, fname, password,byref(self._feedback), self._this_obj_ref)

    def SetEnterFileCallBack(self, cb):
        self._Enter_File = cb

    def SetLeaveFileCallBack(self, cb):
        self._Leave_File = cb

