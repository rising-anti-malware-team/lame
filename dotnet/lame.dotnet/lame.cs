using System;
using System.Collections.Generic;
using System.IO;
using System.Runtime.InteropServices;

namespace lame.dotnet
{
    public enum TreatResult
    {
        TreatFailFix = -1,
        TreatOkDeleted = 1,
        TreatOkFixed = 2
    }


    public enum VirusType
    {
        Trait = 0,
        Hidden = 1,
        HiddenFU = 2,
        Complier = 3,
        Packer = 4,
        Format = 5,		// 文件格式
        AppType = 6,
        Trusted = 9,

        Malware = 0x10,

        Trojan,
        Backdoor,
        Worm,
        Rootkit,
        Exploit,
        HackTool,
        Adware,
        Stealer,
        Spammer,
        Spyware,
        Virus,
        Joke,
        Junk,
        PUA,
        Downloader,
        Dropper,
        /// 2015-12-21
        Ransom,
        Hoax,
        Riskware,
        // 2016-6-7
        Unwanted,
        Monetizer,				//	套现

        MobileBase = 0xC0,
        // Mobile
        //	摘自《移动互联网恶意代码描述规范》
        Payment = MobileBase,	//	恶意扣费
        Privacy,				//	隐私窃取
        Remote, 				//	远程控制
        Spread, 				//	恶意传播
        Expense, 				//	资费消耗
        System, 				//	系统破坏
        Fraud, 				//	诱骗欺诈
        Rogue,					//	流氓行为

        Attention = 0xFE,		// 注意!
        TypeMax = 0x100
    }



    [StructLayout(LayoutKind.Sequential, CharSet = CharSet.Ansi)]
    public struct ScanResult_
    {
        public VirusType vtype;

        [MarshalAs(UnmanagedType.ByValTStr, SizeConst = 16)]
        public string kclass;

        [MarshalAs(UnmanagedType.ByValTStr, SizeConst = 16)]
        public string kclass_desc_a;

        [MarshalAs(UnmanagedType.ByValTStr, SizeConst = 32)]
        public string kclass_desc_w_notused;

        [MarshalAs(UnmanagedType.ByValTStr, SizeConst = 32)]
        public string engid;

        [MarshalAs(UnmanagedType.ByValTStr, SizeConst = 256)]
        public string vname;
        public int vid32;
        public long vid40;

        [MarshalAs(UnmanagedType.ByValTStr, SizeConst = 32)]
        public string hitag;
        public TreatResult treat;
    }

    [StructLayout(LayoutKind.Sequential, CharSet = CharSet.Ansi)]
    public struct LameLicenceInfo_
    {
        [MarshalAs(UnmanagedType.ByValTStr, SizeConst = 16)]
        public string Version;

        [MarshalAs(UnmanagedType.ByValTStr, SizeConst = 128)]
        public string Owner;

        [MarshalAs(UnmanagedType.ByValTStr, SizeConst = 64)]
        public string Date;

        [MarshalAs(UnmanagedType.ByValTStr, SizeConst = 32)]
        public string Authm;

        [MarshalAs(UnmanagedType.ByValTStr, SizeConst = 2048)]
        public string Data;
    }

    [StructLayout(LayoutKind.Sequential, CharSet = CharSet.Ansi)]
    public struct LameVesionInfo_
    {
        [MarshalAs(UnmanagedType.ByValTStr, SizeConst = 33)]
        public string EngineVersion;

        [MarshalAs(UnmanagedType.ByValTStr, SizeConst = 33)]
        public string VirusDBVersion;
    }

    [StructLayout(LayoutKind.Sequential, CharSet = CharSet.Ansi)]
    public struct LameScanFeedBack
    {
        public delegate int InnerLameScanEnterFile(string fname, uint depth, IntPtr uuserdata);
        public delegate void InnerLameScanLeaveFile(string fname, uint depth, IntPtr uuserdata, int l);
        public delegate int InnerLameScanAlarm(string fname, IntPtr result, IntPtr uuserdata);

        [MarshalAs(UnmanagedType.FunctionPtr)] public InnerLameScanEnterFile EnterFile;

        [MarshalAs(UnmanagedType.FunctionPtr)] public InnerLameScanLeaveFile LeaveFile;

        [MarshalAs(UnmanagedType.FunctionPtr)] public InnerLameScanAlarm alarm;
    }



    [StructLayout(LayoutKind.Sequential, CharSet = CharSet.Ansi)]
    public struct LameExtractFeedBack
    {
        public delegate int InnerExtractEnterFileEvent(string fname, string fmt, uint depth, ulong flag, IntPtr fhandle, IntPtr userdata);
        public delegate void InnerExtractLeaveFileEvent(string fname, string fmt, uint depth, ulong flag, IntPtr fhandle, IntPtr userdata, uint l);
        [MarshalAs(UnmanagedType.FunctionPtr)] public InnerExtractEnterFileEvent EnterFile;
        [MarshalAs(UnmanagedType.FunctionPtr)] public InnerExtractLeaveFileEvent LeaveFile;
    }


    public class LameLicenceInfo
    {
        public string Version { set; get; }
        public string Owner { set; get; }
        public string Date { set; get; }
        public string Authm { set; get; }
        public string Data { set; get; }
    }


    public class LameVesionInfo
    {
        public string EngineVersion { set; get; }
        public string VirusDBVersion { set; get; }

    }


    public class LameScanResult
    {
        public VirusType VirusType { set; get; }
        public string VirusTypeName { set; get; }
        public string VirusTypeDesc { set; get; }
        public string EngineID { set; get; }
        public string VirusName { set; get; }
        public int VirusID32 { set; get; }
        public long VirusID40 { set; get; }
        public string HitTag { set; get; }
        public TreatResult TreatResult { set; get; }

    }




    internal class LameUtity
    {
        //public delegate void ScanInternalFileEvent_(string fname, IntPtr result, IntPtr zero);
        //public delegate int InnerExtractFileEvent(string fname, string fmt, uint depth, int flag, IntPtr fhandle, IntPtr userdata);

        [DllImport("lame.dll", EntryPoint = "lame_create", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi)]
        public static extern IntPtr lame_create_(IntPtr vdb);

        [DllImport("lame.dll", EntryPoint = "lame_param_set", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi)]
        public static extern int lame_param_set_(IntPtr pEngine, string pFileName);

        [DllImport("lame.dll", EntryPoint = "lame_init", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi)]
        public static extern int lame_init_(IntPtr pEngine);

        [DllImport("lame.dll", EntryPoint = "lame_scan_file", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi)]
        public static extern int lame_scan_file_(IntPtr pEngine, string pFileName, ref ScanResult_ result);

        [DllImport("lame.dll", EntryPoint = "lame_scan_file_with_callback", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi)]
        public static extern int lame_scan_file_with_callback_(IntPtr pEngine, string pFileName, ref LameScanFeedBack cb, IntPtr user_data);

        [DllImport("lame.dll", EntryPoint = "lame_scan_mem", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi)]
        public static extern int lame_scan_mem_(IntPtr pEngine, byte[] data, uint uSize, ref ScanResult_ result);

        [DllImport("lame.dll", EntryPoint = "lame_scan_mem_with_callback", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi)]
        public static extern int lame_scan_mem_with_callback_(IntPtr pEngine, byte[] data, uint uSize, ref LameScanFeedBack cb, IntPtr user_data);

        [DllImport("lame.dll", EntryPoint = "lame_destroy", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi)]
        public static extern void lame_destroy_(IntPtr pEngine);

        [DllImport("lame.dll", EntryPoint = "lame_get_licence_info", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi)]
        public static extern int lame_get_licence_info_(ref LameLicenceInfo_ license);

        [DllImport("lame.dll", EntryPoint = "lame_get_version", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi)]
        public static extern int lame_get_version_(ref LameVesionInfo_ ver);


        [DllImport("lame.dll", EntryPoint = "lame_extract_file", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi)]
        public static extern int lame_extract_file_(IntPtr pEngine, string fname, string password, ref LameExtractFeedBack cb, IntPtr user_data);

        [DllImport("lame.dll", EntryPoint = "lame_file_get_size", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi)]
        public static extern int lame_file_get_size_(IntPtr pEngine);

        [DllImport("lame.dll", EntryPoint = "lame_file_seek", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi)]
        public static extern int lame_file_seek_(IntPtr pEngine, int offset, int method);

        [DllImport("lame.dll", EntryPoint = "lame_file_tell", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi)]
        public static extern int lame_file_tell_(IntPtr pEngine);

        [DllImport("lame.dll", EntryPoint = "lame_file_read", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi)]
        public static extern int lame_file_read_(IntPtr pEngine, byte[] data, uint uSize);
    }

    public class LameBase
    {

        protected List<string> params_lst = new List<string>();

        protected IntPtr pEngine = IntPtr.Zero;

        protected VirusLib _vlib = null;

        public bool Load(VirusLib lib)
        {
            try
            {
                if (lib == null) throw new Exception("Invalid Param.");

                if (lib.vdb_handle == IntPtr.Zero) throw new Exception("Invalid VirusLib.");

                pEngine = LameUtity.lame_create_(lib.vdb_handle);
                if (pEngine == IntPtr.Zero) throw new Exception("Faild to create lame.");

                foreach (var s in params_lst)
                {
                    LameUtity.lame_param_set_(pEngine, s);
                }

                if (LameUtity.lame_init_(pEngine) < 0)
                {
                    LameUtity.lame_destroy_(pEngine);
                    pEngine = IntPtr.Zero;
                    throw new Exception("Faild to init lame.");
                }

                _vlib = lib;
                return true;
            }
            catch (Exception e)
            {
                throw e;
            }

        }

        public bool SetParameters(string param)
        {
            if (string.IsNullOrEmpty(param)) return false;
            params_lst.Add(param);
            return true;
        }

        public void Unload()
        {
            try
            {
                if (pEngine == IntPtr.Zero) return;
                LameUtity.lame_destroy_(pEngine);
                pEngine = IntPtr.Zero;
            }
            catch (Exception)
            {
            }

        }

        public LameLicenceInfo GetLicense()
        {

            try
            {
                var lame_info_ = new LameLicenceInfo_();

                if (LameUtity.lame_get_licence_info_(ref lame_info_) < 0) return null;

                var lame_info = new LameLicenceInfo();
                lame_info.Authm = lame_info_.Authm;
                lame_info.Data = lame_info_.Data;
                lame_info.Date = lame_info_.Date;
                lame_info.Owner = lame_info_.Owner;
                lame_info.Version = lame_info_.Version;

                return lame_info;
            }
            catch (Exception)
            {
                //throw e;
            }

            return null;
        }

        public LameVesionInfo GetVersion()
        {
            try
            {
                var ver_ = new LameVesionInfo_();
                if (LameUtity.lame_get_version_(ref ver_) < 0) return null;

                var ver = new LameVesionInfo();
                ver.EngineVersion = ver_.EngineVersion;
                ver.VirusDBVersion = ver_.VirusDBVersion;
                return ver;
            }
            catch (Exception)
            {

            }

            return null;
        }
    }

    public class Lame : LameBase
    {
        public LameScanResult ScanFile(string sFile)
        {
            try
            {
                if (string.IsNullOrEmpty(sFile)) throw new Exception("Invalid param");
                if (pEngine == IntPtr.Zero) throw new Exception("Invalid lame");


                var _result = new ScanResult_();
                if (LameUtity.lame_scan_file_(pEngine, sFile, ref _result) < 0) return null;

                var result = new LameScanResult();
                result.EngineID = _result.engid;
                result.VirusName = _result.vname;
                result.VirusTypeName = _result.kclass;
                result.VirusTypeDesc = _result.kclass_desc_a;
                result.HitTag = _result.hitag;
                result.VirusID32 = _result.vid32;
                result.VirusID40 = _result.vid40;
                result.VirusType = _result.vtype;
                result.TreatResult = _result.treat;

                return result;

            }
            catch (Exception e)
            {
                throw e;
            }
        }
        public LameScanResult ScanMem(byte[] bytes)
        {
            try
            {
                if (bytes == null || bytes.Length == 0) throw new Exception("Invalid param");

                if (pEngine == IntPtr.Zero) throw new Exception("Invalid lame");

                var _result = new ScanResult_();
                if (LameUtity.lame_scan_mem_(pEngine, bytes, (uint)bytes.Length, ref _result) < 0) return null;

                var result = new LameScanResult();

                result.EngineID = _result.engid;
                result.VirusName = _result.vname;
                result.HitTag = _result.hitag;
                result.VirusID32 = _result.vid32;
                result.VirusID40 = _result.vid40;
                result.VirusType = _result.vtype;
                result.TreatResult = _result.treat;

                return result;
            }
            catch (Exception e)
            {
                throw e;
            }

        }
        public object Clone()
        {
            Lame _lame = null;
            try
            {
                _lame = new Lame();
                foreach (var s in params_lst)
                {
                    _lame.SetParameters(s);
                }

                if (!_lame.Load(_vlib))
                {
                    _lame.Unload();
                    return null;
                }

                return _lame;
            }
            catch (Exception)
            {

            }

            return null;
        }
    }


    public enum LSCT
    {
        CONTINUE = 0x01,
        ABORT = 0x02
    }
    public delegate LSCT LameScannerAlarm(string fname, LameScanResult result);
    public delegate LSCT LameScannerEnterFileEvent(string fname, uint depth);
    public delegate void LameScannerLeaveFileEvent(string fname, uint depth);
    public class LameWithEvent : LameBase
    {
        public LameScannerEnterFileEvent EnterFileEvent;
        public LameScannerLeaveFileEvent LeaveFileEvent;
        public LameScannerAlarm AlarmEvent;
        private LameScanFeedBack cbs = new LameScanFeedBack();
        public LameWithEvent()
        {
            cbs.EnterFile = LameScanEnterFile;
            cbs.LeaveFile = LameScanLeaveFile;
            cbs.alarm = LameScanAlarm;

            EnterFileEvent = delegate { return LSCT.CONTINUE; };
            LeaveFileEvent = delegate { };
            AlarmEvent = delegate { return LSCT.CONTINUE; };
        }

        public void ScanFile(string sFile)
        {
            try
            {
                if (string.IsNullOrEmpty(sFile)) throw new Exception("Invalid param");
                if (pEngine == IntPtr.Zero) throw new Exception("Invalid lame");
                if (LameUtity.lame_scan_file_with_callback_(pEngine, sFile, ref cbs, IntPtr.Zero) < 0) return;

            }
            catch (Exception e)
            {
                throw e;
            }
        }

        public void ScanMem(byte[] bytes)
        {
            try
            {
                if (bytes == null || bytes.Length == 0) throw new Exception("Invalid param");
                if (pEngine == IntPtr.Zero) throw new Exception("Invalid lame");

                if (LameUtity.lame_scan_mem_with_callback_(pEngine, bytes, (uint)bytes.Length, ref cbs, IntPtr.Zero) < 0) return;

            }
            catch (Exception e)
            {
                throw e;
            }
        }


        public object Clone()
        {
            LameWithEvent _lame = null;
            try
            {
                _lame = new LameWithEvent();
                foreach (var s in params_lst)
                {
                    _lame.SetParameters(s);
                }

                if (!_lame.Load(_vlib))
                {
                    _lame.Unload();
                    return null;
                }

                _lame.EnterFileEvent = EnterFileEvent;
                _lame.LeaveFileEvent = LeaveFileEvent;
                _lame.AlarmEvent = AlarmEvent;

                return _lame;
            }
            catch (Exception)
            {

            }

            return null;
        }
        private LameScanResult FetchResult(IntPtr resp)
        {
            try
            {
                var _result = (ScanResult_) Marshal.PtrToStructure(resp, typeof(ScanResult_));


                var result = new LameScanResult();
                result.EngineID = _result.engid;
                result.VirusName = _result.vname;
                result.VirusTypeName = _result.kclass;
                result.VirusTypeDesc = _result.kclass_desc_a;
                result.HitTag = _result.hitag;
                result.VirusID32 = _result.vid32;
                result.VirusID40 = _result.vid40;
                result.VirusType = _result.vtype;
                result.TreatResult = _result.treat;

                return result;
            }
            catch (Exception)
            {

            }

            return null;
        }

        private int LameScanEnterFile(string fname, uint depth, IntPtr uuserdata)
        {
            return (int)EnterFileEvent(fname, depth);
        }
        private void LameScanLeaveFile(string fname, uint depth, IntPtr uuserdata, int l)
        {
            LeaveFileEvent(fname, depth);
        }
        private int LameScanAlarm(string fname, IntPtr resp, IntPtr uuserdata)
        {
            LameScanResult result = null;

            if (resp != IntPtr.Zero) { result = FetchResult(resp); }
            if (result == null) return (int)LSCT.CONTINUE;
            return (int)AlarmEvent(fname, result);
        }

    }


    public enum FileAttritue
    {
        Data,
        Package,
        Document,
        Program,
        MMedia,
        Script,
        Email,
        DiskImg
    }

    public enum LCT
    {
        Continue = 1,
        Extract = 2,
        Abort = 3
    }


    public class LameFile
    {
        public class ImmediateFile
        {
            public enum SeekOriginal
            {
                Begin = 0x00,
                Current = 0x01,
                End = 0x02
            }
            private IntPtr _handle = IntPtr.Zero;
            public ImmediateFile(IntPtr handle)
            {
                if (handle == IntPtr.Zero) throw new Exception("Invalid file handle.");
                _handle = handle;
            }

            public int GetFileSize()
            {
                try
                {
                    return LameUtity.lame_file_get_size_(_handle);
                }
                catch (Exception)
                {

                }

                return 0;
            }

            public int Read(byte[] bytes)
            {
                try
                {
                    return LameUtity.lame_file_read_(_handle, bytes, (uint)bytes.Length);
                }
                catch (Exception)
                {

                }
                return 0;
            }

            public int Tell()
            {

                try
                {
                    return LameUtity.lame_file_tell_(_handle);
                }
                catch (Exception)
                {

                }
                return 0;
            }

            public int Seek(int offset, SeekOriginal orginal)
            {
                try
                {
                    return LameUtity.lame_file_seek_(_handle, offset, (int)orginal);
                }
                catch (Exception)
                {

                }
                return 0;
            }
        }


        private string _fname;
        private string _full_name;
        private string _inner_path;
        private string _fmt;
        private FileAttritue _flag;
        private ImmediateFile _file;
        private uint _depth = 0;
        public LameFile(string fname, string fmt, uint depth, FileAttritue flag, IntPtr fhandle)
        {
            try
            {
                _full_name = fname;

                _fname = Path.GetFileName(_full_name);
                _inner_path = _full_name.Substring(0, _full_name.Length - _fname.Length);

                _fmt = fmt;
                _flag = flag;
                _depth = depth;
                _file = new ImmediateFile(fhandle);
            }
            catch (Exception ex)
            {
                throw ex;
            }

        }

        public string FileName
        {
            get
            {
                return _fname;
            }
        }

        public string FullFileName
        {
            get
            {
                return _full_name;
            }
        }

        public string InnerPath
        {
            get
            {
                return _inner_path;
            }
        }

        public string FileFormat
        {
            get
            {
                return _fmt;
            }
        }

        public FileAttritue FileAttritue
        {
            get
            {
                return _flag;
            }
        }

        public ImmediateFile File
        {
            get
            {
                return _file;
            }
        }
        public uint Depth
        {
            get
            {
                return _depth;
            }
        }
    }
    public delegate LCT ExtractEnterFileEvent(LameFile file);
    public delegate void ExtractLeaveFileEvent(LameFile file);

    public class LameExtractor : LameBase
    {
        private LameExtractFeedBack fd;
        public ExtractEnterFileEvent OnExtractEnterFileEvent;
        public ExtractLeaveFileEvent OnExtractLeaveFileEvent;

        public LameExtractor()
        {
            fd.EnterFile = ExtractEnterFileHandle;
            fd.LeaveFile = ExtractLeaveFileHandle;
            OnExtractEnterFileEvent = delegate { return LCT.Continue; };
            OnExtractLeaveFileEvent = delegate { };
        }
        public bool Extract(string fname, string password)
        {
            if (pEngine == IntPtr.Zero) throw new Exception("Invalid lame");
            if (string.IsNullOrEmpty(fname)) return false;

            LameUtity.lame_extract_file_(pEngine, fname, password, ref fd, IntPtr.Zero);

            return true;
        }
        private int ExtractEnterFileHandle(string fname, string fmt, uint depth, ulong flag, IntPtr handle, IntPtr userdata)
        {
            try
            {
                var _file = new LameFile(fname, fmt, depth, GetPackFlage(flag), handle);
                if (_file == null) return (int)LCT.Continue;
                return (int)OnExtractEnterFileEvent(_file);
            }
            catch (Exception)
            {

            }

            return (int)LCT.Continue;
        }

        private void ExtractLeaveFileHandle(string fname, string fmt, uint depth, ulong flag, IntPtr handle, IntPtr userdata, uint lv)
        {
            try
            {
                var _file = new LameFile(fname, fmt, depth, GetPackFlage(flag), handle);
                if (_file == null) return;
                OnExtractLeaveFileEvent(_file);
            }
            catch (Exception)
            {

            }

        }
        private FileAttritue GetPackFlage(ulong flag)
        {
            if ((flag & 0x00000010) > 0) return FileAttritue.Program;
            if ((flag & 0x00000020) > 0) return FileAttritue.Document;
            if ((flag & 0x00000040) > 0) return FileAttritue.Package;
            if ((flag & 0x00000080) > 0) return FileAttritue.MMedia;
            if ((flag & 0x00000100) > 0) return FileAttritue.Script;
            if ((flag & 0x00000200) > 0) return FileAttritue.Email;
            if ((flag & 0x00000400) > 0) return FileAttritue.DiskImg;
            return FileAttritue.Data;
        }
    }
}
