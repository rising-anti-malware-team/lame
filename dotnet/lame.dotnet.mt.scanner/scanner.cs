using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Text;
using System.Threading;
using System.Threading.Tasks;
using System.Diagnostics;

namespace lame.dotnet.mt.scanner
{
    class MtdUtility 
    {
       private static Object _locker = new Object();

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

    class SFQueue 
    {
        private Object _locker = new Object();
        private Queue<string> _queue = new Queue<string>();
        private uint _max_queue_size = 100;

        public bool Enqueue(string s, bool force = true) 
        {
            if (force)
            {
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

            }
            else 
            {
                lock (_locker)
                {
                    if (_queue.Count >= _max_queue_size) return false;
                    _queue.Enqueue(s);
                    return true;
                }
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


    class DirTravelWorker 
    {
        private SFQueue _scan_obj_queue = null;
        private SFQueue _scan_path = new SFQueue();
        private Thread _travel_worker = null;
        private ManualResetEvent _manual_event = new ManualResetEvent(false);

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
                string path = _scan_path.Dequeue();
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

            string[] files = Directory.GetFiles(s);
            foreach (string f in files)
            {
                _scan_obj_queue.Enqueue(f);
            }

            string[] dirs = Directory.GetDirectories(s);
            foreach (string d in dirs)
            {
                travel(d);
            }
        }

    }


    class ScanWorker
    {
        private SFQueue _scan_objs = null;
        private ManualResetEvent _mannul_event = new ManualResetEvent(false);
        private Thread _thread = null;
        private Lame _lame = new Lame();
        private long _scan_file_count = 0;
        private long _virus_file_count = 0;
        private long _elapse_time = 0;

        public ScanWorker(List<string> param , SFQueue scan_path) 
        {
            _scan_objs = scan_path;
            _thread = new Thread(scan);
            _thread.Start();

            if (param == null) return;

            foreach (string s in param)
            {
                _lame.SetParameters(s);
            }

        }

        public bool Load(VirusLib vdb)
        {
          
            if (vdb == null || !_lame.Load(vdb)) 
            {
                return false;
            }
            
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

            Stopwatch _Stopwatch = new Stopwatch();
            _Stopwatch.Start();

            while (true) 
            {
                string file_path = _scan_objs.Dequeue();
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

                _scan_file_count++;

                LameScanResult res = _lame.ScanFile(file_path);
                if (res != null) _virus_file_count++;
                
                MtdUtility.ShowScanResult(file_path , res);
            }

            _Stopwatch.Stop();
            _elapse_time = _Stopwatch.ElapsedMilliseconds;

            _mannul_event.Set();
        }

        public long ScanFileCount
        {
            get 
            {
                return _scan_file_count;
            }
        }

        public long VirusCount
        {
            get 
            {
                return _virus_file_count;
            }
        }

        public long ElapsedMilliseconds
        {
            get 
            {
                return _elapse_time;
            }
        }
    }


    class Scanner 
    {
        private SFQueue _scan_obj_queue = new SFQueue();
        private DirTravelWorker _DirTravelWorker = null;
        private List<ScanWorker> _scanners = new List<ScanWorker>();
        private VirusLib _vlib = new VirusLib();
        private List<string> _params = null;
        private long _elapse_time = 0;

        public Scanner(List<string> param) 
        {
            _params = param;
        }


        public bool Load(int sc) 
        {
            if (!_vlib.lame_open_vdb(null)) return false;

            _DirTravelWorker = new DirTravelWorker(_scan_obj_queue);

            for (int i = 0; i < sc; i++)
            {
                ScanWorker s = new ScanWorker(_params, _scan_obj_queue);
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

            foreach (ScanWorker s in _scanners)
            {
                s.Run();
            }
        }
        public void AppendScanPath(string path)
        {
            if (string.IsNullOrEmpty(path)) return;
            _DirTravelWorker.AppendScanPath(path);
        }

        public void Wait() 
        {
            _DirTravelWorker.Wait();

            foreach (ScanWorker s in _scanners)
            {
                s.Wait();
            }
        }

        public void Unload()
        {
            long _total_files = 0;
            long _total_virus = 0;
            long _elapse_minisecod = 0;

            foreach (ScanWorker s in _scanners)
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
            Console.WriteLine("Virus/Files: " + _total_virus +  "/"+_total_files+ " = {0:0.0000}" +"%", _total_virus / (double)_total_files);

            long second = _elapse_minisecod / 1000;
            long min = (second / 60) % 60;
            long hour = second / 60 / 60;

            second = second % 60;
            
            Console.WriteLine("Elapse: " + hour + ":" + min + ":" + second + "." + _elapse_minisecod % 1000);


         
        }
    }
}
