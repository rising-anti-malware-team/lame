using System.Collections.Generic;

namespace lame.dotnetcore.mt.scanner
{
    internal class Program
    {
        private static void Main(string[] args)
        {
            var _params = new List<string>();
            var _scan_path = new List<string>();

            var _workers = 3;

            var idx = 0;
            while (idx < args.Length)
            {
                var _param = args[idx++].ToLower().Trim();
                if (_param.StartsWith("-workers="))
                {
                    if (_param.Length <= 9) continue;

                    var s = _param.Substring(9);

                    var vl = 0;
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

            var _scanner = new Scanner(_params);
            if (!_scanner.Load(_workers)) return;


            foreach (var s in _scan_path)
            {
                _scanner.AppendScanPath(s);
            }

            _scanner.run();
            _scanner.Wait();
            _scanner.Unload();
        }
    }
}