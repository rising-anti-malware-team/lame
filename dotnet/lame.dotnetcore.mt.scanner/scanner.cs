using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.IO;
using System.Threading;

namespace lame.dotnetcore.mt.scanner
{
    internal class MtdUtility
    {
        private static readonly object _locker = new object();

        public static void ShowScanResult(string path, LameScanResult result)
        {
            lock (_locker)
            {
                Console.ForegroundColor = ConsoleColor.Green;
                Console.Write(path);
                if (result != null)
                {
                    Console.ForegroundColor = ConsoleColor.Red;
                    Console.Write("     Infected:" + result.VirusName + " (" + result.EngineID + ")");
                }

                Console.WriteLine("");
            }
        }
    }

    internal class SFQueue
    {
        private readonly object _locker = new object();
        private readonly uint _max_queue_size = 100;
        private readonly Queue<string> _queue = new Queue<string>();

        public bool Enqueue(string s, bool force = true)
        {
            if (force)
                while (true)
                {
                    lock (_locker)
                    {
                        if (_queue.Count < _max_queue_size)
                        {
                            _queue.Enqueue(s);
                            return true;
                        }
                    }

                    Thread.Sleep(10);
                }

            lock (_locker)
            {
                if (_queue.Count >= _max_queue_size) return false;
                _queue.Enqueue(s);
                return true;
            }
        }

        public string Dequeue()
        {
            lock (_locker)
            {
                if (_queue.Count == 0) return null;
                return _queue.Dequeue();
            }
        }

        public int Count()
        {
            lock (_locker)
            {
                return _queue.Count;
            }
        }
    }


    internal class DirTravelWorker
    {
        private readonly ManualResetEvent _manual_event = new ManualResetEvent(false);
        private readonly SFQueue _scan_obj_queue;
        private readonly SFQueue _scan_path = new SFQueue();
        private readonly Thread _travel_worker;

        public DirTravelWorker(SFQueue q)
        {
            _scan_obj_queue = q;
            _travel_worker = new Thread(_run);
            _travel_worker.Start();
        }


        public void Run()
        {
            _manual_event.Set();
        }

        public void AppendScanPath(string path)
        {
            if (string.IsNullOrEmpty(path)) return;
            _scan_path.Enqueue(path);
        }

        public void Wait()
        {
            _manual_event.WaitOne();
        }

        private void _run()
        {
            _manual_event.WaitOne();
            _manual_event.Reset();

            while (true)
            {
                var path = _scan_path.Dequeue();
                if (string.IsNullOrEmpty(path))
                {
                    _scan_obj_queue.Enqueue("exit");
                    break;
                }

                travel(path);
            }

            _manual_event.Set();
        }

        private void travel(string s)
        {
            if (string.IsNullOrEmpty(s)) return;

            if (File.Exists(s))
            {
                _scan_obj_queue.Enqueue(s);
                return;
            }

            if (!Directory.Exists(s)) return;

            var files = Directory.GetFiles(s);
            foreach (var f in files) _scan_obj_queue.Enqueue(f);

            var dirs = Directory.GetDirectories(s);
            foreach (var d in dirs) travel(d);
        }
    }


    internal class ScanWorker
    {
        private Lame _lame = new Lame();
        private readonly ManualResetEvent _mannul_event = new ManualResetEvent(false);
        private readonly SFQueue _scan_objs;
        private readonly Thread _thread;

        public ScanWorker(List<string> param, SFQueue scan_path)
        {
            _scan_objs = scan_path;
            _thread = new Thread(scan);
            _thread.Start();

            if (param == null) return;

            foreach (var s in param) _lame.SetParameters(s);
        }

        public long ScanFileCount { get; private set; }

        public long VirusCount { get; private set; }

        public long ElapsedMilliseconds { get; private set; }

        public bool Load(VirusLib vdb)
        {
            if (vdb == null || !_lame.Load(vdb)) return false;

            return true;
        }

        public void Run()
        {
            _mannul_event.Set();
        }


        public void Wait()
        {
            _mannul_event.WaitOne();
        }

        public void Unload()
        {
            if (_lame == null) return;
            _lame.Unload();
            _lame = null;
        }

        private void scan()
        {
            _mannul_event.WaitOne();
            _mannul_event.Reset();

            var _Stopwatch = new Stopwatch();
            _Stopwatch.Start();

            while (true)
            {
                var file_path = _scan_objs.Dequeue();
                if (string.IsNullOrEmpty(file_path))
                {
                    Thread.Sleep(10);
                    continue;
                }

                if (file_path == "exit")
                {
                    _scan_objs.Enqueue("exit");
                    break;
                }

                ScanFileCount++;

                var res = _lame.ScanFile(file_path);
                if (res != null) VirusCount++;

                MtdUtility.ShowScanResult(file_path, res);
            }

            _Stopwatch.Stop();
            ElapsedMilliseconds = _Stopwatch.ElapsedMilliseconds;

            _mannul_event.Set();
        }
    }


    internal class Scanner
    {
        private DirTravelWorker _DirTravelWorker;
        private long _elapse_time = 0;
        private readonly List<string> _params;
        private readonly SFQueue _scan_obj_queue = new SFQueue();
        private readonly List<ScanWorker> _scanners = new List<ScanWorker>();
        private VirusLib _vlib = new VirusLib();

        public Scanner(List<string> param)
        {
            _params = param;
        }


        public bool Load(int sc)
        {
            if (!_vlib.lame_open_vdb(null)) return false;

            _DirTravelWorker = new DirTravelWorker(_scan_obj_queue);

            for (var i = 0; i < sc; i++)
            {
                var s = new ScanWorker(_params, _scan_obj_queue);
                if (!s.Load(_vlib)) continue;

                _scanners.Add(s);
            }

            if (_scanners.Count == 0)
            {
                _vlib.lame_close_vdb();
                _vlib = null;

                return false;
            }

            return true;
        }

        public void run()
        {
            _DirTravelWorker.Run();

            foreach (var s in _scanners) s.Run();
        }

        public void AppendScanPath(string path)
        {
            if (string.IsNullOrEmpty(path)) return;
            _DirTravelWorker.AppendScanPath(path);
        }

        public void Wait()
        {
            _DirTravelWorker.Wait();

            foreach (var s in _scanners) s.Wait();
        }

        public void Unload()
        {
            long _total_files = 0;
            long _total_virus = 0;
            long _elapse_minisecod = 0;

            foreach (var s in _scanners)
            {
                s.Unload();

                _total_files += s.ScanFileCount;
                _total_virus += s.VirusCount;

                if (s.ElapsedMilliseconds > _elapse_minisecod) _elapse_minisecod = s.ElapsedMilliseconds;
            }

            if (_vlib != null)
            {
                _vlib.lame_close_vdb();
                _vlib = null;
            }


            Console.WriteLine("");
            Console.WriteLine("Virus/Files: " + _total_virus + "/" + _total_files + " = {0:0.0000}" + "%",
                _total_virus / (double) _total_files);

            var second = _elapse_minisecod / 1000;
            var min = second / 60 % 60;
            var hour = second / 60 / 60;

            second = second % 60;

            Console.WriteLine("Elapse: " + hour + ":" + min + ":" + second + "." + _elapse_minisecod % 1000);
        }
    }
}