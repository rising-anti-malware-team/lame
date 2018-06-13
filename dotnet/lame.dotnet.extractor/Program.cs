using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using lame.dotnet;
using System.IO;

namespace lame.dotnet.extractor
{

    class PackageExtract
    {
        private VirusLib _lib = new VirusLib();
        private LameExtractor _lame = new LameExtractor();
        private string _dump_path = "";
        private bool _dump = false;
        private Stack<string> _file_names = new Stack<string>();
        public PackageExtract(bool dump) 
        {
            _dump = dump;
        }
        public bool Load() 
        {
            try
            {
                if (!_lib.lame_open_vdb(null)) return false;
                if (!_lame.Load(_lib)) 
                {
                    _lib.lame_close_vdb();
                    _lib = null;
                    _lame = null;
                    return false;
                }

                _lame.OnExtractEnterFileEvent = ExtractEnterFileEventHandle;
                _lame.OnExtractLeaveFileEvent = ExtractLeaveFileEventHandle;

                _dump_path = Path.Combine(AppDomain.CurrentDomain.BaseDirectory, "bin");
                Directory.CreateDirectory(_dump_path);

                return true;
            }
            catch (System.Exception ex)
            {
                Console.WriteLine(ex.Message);
            }

            return false;
        }
        public bool Extract(string file , string passwd) 
        {
            return _lame.Extract(file, passwd);
        }
        public void Unload() 
        {
            if (_lib != null) _lib.lame_close_vdb();
            if (_lame != null) _lame.Unload();
        }

        private LCT ExtractEnterFileEventHandle(LameFile file)
        {
            if (file == null) return LCT.Continue;

            Console.WriteLine("Name: " + file.FileName + "   Size:" + file.File.GetFileSize() + "    depth:" + file.Depth);

            if (_dump && file.Depth > 0)
            {
                string _dump_dir = Path.Combine(_dump_path, file.InnerPath);
                Directory.CreateDirectory(_dump_dir);

                string new_file_name = Path.Combine(_dump_dir, file.FileName);
                DumpFile(new_file_name, file.File);
            }


            return LCT.Extract;
        }
        private void ExtractLeaveFileEventHandle(LameFile file) 
        {
        }
        private void DumpFile(string fname , LameFile.ImmediateFile file) 
        {
            FileStream fs = null;
            BinaryWriter bw = null;
            try
            {
                 fs = new FileStream(fname, FileMode.OpenOrCreate);
                 bw = new BinaryWriter(fs);

                 byte[] bytes = new byte[0x1000];
                 while (true) 
                 {
                     int real_read_bytes = file.Read(bytes);
                     if (real_read_bytes == 0) break;

                     bw.Write(bytes);
                 }
            
            }
            catch (System.Exception ex)
            {
                Console.WriteLine(ex.Message);
            }
            finally 
            {
                if (bw != null)
                {
                    bw.Close();
                    bw = null;
                }

                if (fs != null)
                {
                    fs.Close();
                    fs = null;
                }
            }
        }
        private void Makedir(string fname) 
        {
            if (string.IsNullOrEmpty(fname)) return;

            string _path = Path.GetFullPath(fname);
            int idx = _path.IndexOf("::");
            if (idx != -1)
            {
                string s = _path.Substring(idx + 2).Replace("::" ,"\\");
            }
        }
    }



    class Program
    {
        static void Main(string[] args)
        {

            List<string> files = new List<string>();
            string password = "";
            bool dump = false;

            int idx = 0;
            while (idx < args.Length)
            {
                string s = args[idx++].ToLower().Trim();
                if (s == "-password")
                {
                    if (idx < args.Length)
                    {
                        password = args[idx++];
                    }
                }
                else if (s == "-dump") 
                {
                    dump = true;
                }
                else 
                {
                    files.Add(s);
                }
            }

            if (files.Count == 0)
            {
                Help();
                return;
            }

            PackageExtract _PackageExtract = null;
            try
            {
                _PackageExtract = new PackageExtract(dump);
                if (!_PackageExtract.Load()) return;

                foreach (string s in files)
                {
                    _PackageExtract.Extract(s, password);
                }
            }
            catch (System.Exception ex)
            {
                Console.WriteLine(ex.Message);
            }
            finally
            {
                if (_PackageExtract != null)
                {
                    _PackageExtract.Unload();
                }
            }
            
        }

        static void Help() 
        {
            Console.WriteLine("Usage:");
            Console.WriteLine("     lame.dotnet.extractor.exe [-password password] [-dump] file");
        }
    }
}
