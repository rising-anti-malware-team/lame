using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace lame.dotnet.mt.scanner
{
    class Program
    {
        static void Main(string[] args)
        {
            List<string> _params = new List<string>();
            List<string> _scan_path = new List<string>();

            int _workers = 3;

            int idx = 0;
            while (idx < args.Length)
            {
                string _param = args[idx++].ToLower().Trim();
                if (_param.StartsWith("-workers="))
                {
                    if (_param.Length <= 9) continue;
                    
                    string s = _param.Substring(9);

                    int vl = 0;
                    if (int.TryParse(s, out vl) && vl > 0) 
                    {
                        if (vl > 10) _workers = 10;
                        else _workers = vl;
                        
                    } 
                }
                else if (_param.StartsWith("-"))
                {
                    if (_param.Length == 1) continue;

                    _params.Add(_param.Substring(1));
                }
                else 
                {
                    _scan_path.Add(_param);
                }

            }



            Scanner _scanner = new Scanner(_params);
            if (!_scanner.Load(_workers)) 
            {
                return;
            }


            foreach (string s in _scan_path)
            {
                _scanner.AppendScanPath(s);
            }


            _scanner.run();
            _scanner.Wait();
            _scanner.Unload();
            
        }
    }
}
