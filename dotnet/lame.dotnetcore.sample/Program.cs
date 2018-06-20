using System;
using System.IO;

namespace lame.dotnetcore.sample
{
    internal class Program
    {
        private static void Main(string[] args)
        {
            if (args.Length == 0) return;

            var vdb = new VirusLib();
            if (!vdb.lame_open_vdb(null))
            {
                Console.WriteLine("Faild to load virus lib.");
                return;
            }

            LameTest(vdb, args[0]);

            LameWithEventTest(vdb, args[0]);

            vdb.lame_close_vdb();
        }

        private static void PrintScanResult(string path, LameScanResult result)
        {
            if (result == null) return;

            Console.Write(path);
            if (result != null)
            {
                Console.Write("     Infected:" + result.VirusName + " (" + result.EngineID + ")");
            }
            Console.WriteLine("");
        }

        private static void LameTest(VirusLib vdb, string path)
        {
            if (vdb == null) return;


            // 1. load
            var _lame = new Lame();
            if (!_lame.Load(vdb)) return;

            //2. scan
            if (File.Exists(path))
            {
                var _result = _lame.ScanFile(path);
                PrintScanResult(path, _result);
            }
            else if (Directory.Exists(path))
            {
                var files = Directory.GetFiles(path);
                foreach (var f in files)
                {
                    var _result = _lame.ScanFile(f);
                    PrintScanResult(f, _result);
                }

                //travel dir......
            }


            //3.
            _lame.Unload();
        }

        private static LSCT AlarmEventHandle(string file, LameScanResult result)
        {
            Console.ForegroundColor = ConsoleColor.Green;
            Console.Write(file);
            if (result != null)
            {
                Console.ForegroundColor = ConsoleColor.Red;
                Console.Write("     Infected:" + result.VirusName + " (" + result.EngineID + ")");
            }

            Console.WriteLine("");
            return LSCT.CONTINUE;
        }

        private static LSCT EnterFileEventHandle(string fname, uint depth)
        {
            return LSCT.CONTINUE;
        }

        private static void LeaveFileEventHandle(string fname, uint depth)
        {
        }

        private static void LameWithEventTest(VirusLib vdb, string path)
        {
            if (vdb == null) return;

            // 1. load
            var _lame = new LameWithEvent();
            _lame.EnterFileEvent = EnterFileEventHandle;
            _lame.LeaveFileEvent = LeaveFileEventHandle;
            _lame.AlarmEvent = AlarmEventHandle;

            if (!_lame.Load(vdb)) return;

            //2. scan
            if (File.Exists(path))
            {
                _lame.ScanFile(path);
            }
            else if (Directory.Exists(path))
            {
                var files = Directory.GetFiles(path);
                foreach (var f in files)
                {
                    _lame.ScanFile(f);
                }

                //travel dir......
            }

            //3.
            _lame.Unload();
        }
    }
}