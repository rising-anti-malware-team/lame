using System;
using System.Collections.Generic;
using System.IO;

namespace lame.dotnetcore.scanner
{
    internal interface IScan
    {
        bool SetParam(string param);
        bool Load();
        void Scan(string path);
        IScan Clone();
        void ShowVersion();
        void ShowLicense();
        void UnLoad();
    }


    internal class Scanner : IScan
    {
        private static readonly object locker = new object();
        private VirusLib vdb = null;
        private Lame lame = new Lame();

        public Scanner(VirusLib vdb)
        {
            this.vdb = vdb;
        }

        public Scanner(Lame lame, VirusLib vdb)
        {
            if (lame == null || vdb == null)
            {
                throw new Exception("Unknown Excepiton");
            }
            this.lame = lame;
            this.vdb = vdb;
        }

        public bool SetParam(string param)
        {
            return lame.SetParameters(param);
        }

        public bool Load()
        {
            try
            {
                if (!vdb.lame_open_vdb(null)) return false;
                if (!lame.Load(vdb)) return false;

                return true;
            }
            catch (Exception ex)
            {
                vdb.lame_close_vdb();
                lame.Unload();
                Console.WriteLine(ex.Message);
            }

            return false;
        }

        public void Scan(string path)
        {
            try
            {
                if (string.IsNullOrEmpty(path)) return;
                if (File.Exists(path))
                {
                    ScanFile(path);
                    return;
                }

                var files = Directory.GetFiles(path);
                foreach (var f in files)
                {
                    ScanFile(f);
                }

                var dirs = Directory.GetDirectories(path);
                foreach (var d in dirs)
                {
                    Scan(d);
                }
            }
            catch (Exception)
            {
            }
        }

        public IScan Clone()
        {
            if (lame == null) return null;

            return new Scanner((Lame) lame.Clone(), vdb);
        }

        public void ShowVersion()
        {
            try
            {
                var info = lame.GetVersion();
                if (info == null) return;

                Console.WriteLine("Engine:" + info.EngineVersion);
                Console.WriteLine("VirusDB:" + info.VirusDBVersion);
            }
            catch (Exception)
            {
            }
        }

        public void ShowLicense()
        {
            try
            {
                var info = lame.GetLicense();
                if (info == null) return;

                Console.WriteLine("Version:" + info.Version);
                Console.WriteLine("Owner:" + info.Owner);
                Console.WriteLine("Date:" + info.Date);
                Console.WriteLine("Authm:" + info.Authm);
            }
            catch (Exception)
            {
            }
        }

        public void UnLoad()
        {
            try
            {
                lame.Unload();
            }
            catch (Exception ex)
            {
                Console.WriteLine(ex.Message);
            }
        }

        private void ScanFile(string file)
        {
            try
            {
                var result = lame.ScanFile(file);
                lock (locker)
                {
                    Console.ForegroundColor = ConsoleColor.Green;
                    Console.Write(file);
                    if (result != null)
                    {
                        Console.ForegroundColor = ConsoleColor.Red;
                        Console.Write("     Infected:" + result.VirusName + " (" + result.EngineID + ")");
                    }

                    Console.WriteLine("");
                }
            }
            catch (Exception)
            {
            }
        }
    }


    internal class ScannerEx : IScan
    {
        public static readonly object locker = new object();
        private VirusLib vdb = null;
        private LameWithEvent lame = new LameWithEvent();

        public ScannerEx(VirusLib vdb)
        {
            this.vdb = vdb;
        }

        public ScannerEx(LameWithEvent lame, VirusLib vdb)
        {
            if (lame == null || vdb == null)
            {
                throw new Exception("Unknown Excepiton");
            }
            this.lame = lame;
            this.vdb = vdb;
        }

        public bool SetParam(string param)
        {
            return lame.SetParameters(param);
        }

        public bool Load()
        {
            try
            {
                if (!vdb.lame_open_vdb(null)) return false;
                if (!lame.Load(vdb)) return false;
                lame.EnterFileEvent = EnterFileEventHandle;
                lame.LeaveFileEvent = LeaveFileEventHandle;
                lame.AlarmEvent = AlarmEventHandle;

                //测试
                //LameWithEvent lm = (LameWithEvent)lame.Clone();
                //lame.Unload();
                //lame = lm;

                return true;
            }
            catch (Exception ex)
            {
                vdb.lame_close_vdb();
                lame.Unload();
                Console.WriteLine(ex.Message);
            }

            return false;
        }

        public IScan Clone()
        {
            if (lame == null) return null;

            return new ScannerEx((LameWithEvent) lame.Clone(), vdb);
        }

        public void Scan(string path)
        {
            try
            {
                if (string.IsNullOrEmpty(path)) return;
                if (File.Exists(path))
                {
                    ScanFile(path);
                    return;
                }

                var files = Directory.GetFiles(path);
                foreach (var f in files)
                {
                    ScanFile(f);
                }

                var dirs = Directory.GetDirectories(path);
                foreach (var d in dirs)
                {
                    Scan(d);
                }
            }
            catch (Exception)
            {
            }
        }

        public void ShowVersion()
        {
            try
            {
                var info = lame.GetVersion();
                if (info == null) return;

                Console.WriteLine("Engine:" + info.EngineVersion);
                Console.WriteLine("VirusDB:" + info.VirusDBVersion);
            }
            catch (Exception)
            {
            }
        }

        public void ShowLicense()
        {
            try
            {
                var info = lame.GetLicense();
                if (info == null) return;

                Console.WriteLine("Version:" + info.Version);
                Console.WriteLine("Owner:" + info.Owner);
                Console.WriteLine("Date:" + info.Date);
                Console.WriteLine("Authm:" + info.Authm);
            }
            catch (Exception)
            {
            }
        }

        public void UnLoad()
        {
            try
            {
                lame.Unload();
            }
            catch (Exception ex)
            {
                Console.WriteLine(ex.Message);
            }
        }

        private void ScanFile(string file)
        {
            try
            {
                lame.ScanFile(file);
            }
            catch (Exception)
            {
            }
        }

        private void ScanMem(string file)
        {
            try
            {
                var bytes = File.ReadAllBytes(file);
                lame.ScanMem(bytes);
            }
            catch (Exception)
            {
            }
        }

        private LSCT AlarmEventHandle(string file, LameScanResult result)
        {
            lock (locker)
            {
                Console.ForegroundColor = ConsoleColor.Green;
                Console.Write(file);
                if (result != null)
                {
                    Console.ForegroundColor = ConsoleColor.Red;
                    Console.Write("     Infected:" + result.VirusName + " (" + result.EngineID + ")");
                }

                Console.WriteLine("");
            }

            return LSCT.CONTINUE;
        }

        private LSCT EnterFileEventHandle(string fname, uint depth)
        {
            return LSCT.CONTINUE;
        }

        private void LeaveFileEventHandle(string fname, uint depth)
        {
        }
    }


    internal class Program
    {
        private static void Main(string[] args)
        {
            if (args.Length == 0)
            {
                Console.WriteLine("Invalid Param.");
                Help();
                return;
            }

            VirusLib vdb = null;
            IScan _Scanner = null;
            try
            {
                vdb = new VirusLib();
                if (!vdb.lame_open_vdb(null))
                {
                    Console.WriteLine("Faild to load virus lib.");
                    return;
                }

                var path_list = new List<string>();
                var param_list = new List<string>();

                var bShowVerion = false;
                var bShowLicense = false;
                var bShowFileList = false;
                var bHold = false;
                var bMd5List = false;
                foreach (var s in args)
                {
                    var vl = s.ToLower();
                    if (s.StartsWith("-"))
                    {
                        if (vl == "-version")
                        {
                            bShowVerion = true;
                        }
                        else if (vl == "-license")
                        {
                            bShowLicense = true;
                        }
                        else if (vl == "-hold")
                        {
                            bHold = true;
                        }
                        else if (vl == "-show-file-list")
                        {
                            bShowFileList = true;
                        }
                        else if (vl == "-md5-list")
                        {
                            bMd5List = true;
                        }
                        else
                        {
                            param_list.Add(s.Substring(1));
                        }
                    }
                    else
                    {
                        path_list.Add(s);
                    }
                }

                if (bShowFileList)
                {
                    _Scanner = new ScannerEx(vdb);
                }
                else
                {
                    _Scanner = new Scanner(vdb);
                }


                if (bShowVerion) _Scanner.ShowVersion();

                if (bShowLicense) _Scanner.ShowLicense();


                foreach (var s in param_list)
                {
                    _Scanner.SetParam(s);
                }

                if (!_Scanner.Load()) return;

                foreach (var s in path_list)
                {
                    _Scanner.Scan(s);
                }


                if (bHold) Console.Read();
            }
            catch (Exception ex)
            {
                Console.WriteLine(ex.Message);
            }
            finally
            {
                if (_Scanner != null) _Scanner.UnLoad();
                if (vdb != null) vdb.lame_close_vdb();
            }
        }

        private static void Help()
        {
            Console.WriteLine("Usage:\n");
            Console.WriteLine("	  -version	  : show scanner info.");
            Console.WriteLine("	  -license	  : show scanner license.");
            Console.WriteLine("	  -hold		  : hold , press return to exit.");
            Console.WriteLine("	  -show-file-list : show internal files of scan.");

            Console.WriteLine("Example:");
            Console.WriteLine("	  : lame.scanner d:\\samples");
            Console.WriteLine("	  : lame.scanner -lame.v1 -hold d:\\samples");
        }
    }
}