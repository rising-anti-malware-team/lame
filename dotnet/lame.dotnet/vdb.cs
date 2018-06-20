using System;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.InteropServices;
using System.Text;
using System.Threading.Tasks;

namespace lame.dotnet
{
    public class VirusLib
    {
        [DllImport("lame.dll", EntryPoint = "lame_open_vdb", CallingConvention = CallingConvention.Cdecl)]
        private static extern IntPtr lame_open_vdb_([In] [MarshalAs(UnmanagedType.LPStr)] string vlibf);

        [DllImport("lame.dll", EntryPoint = "lame_close_vdb", CallingConvention = CallingConvention.Cdecl)]
        private static extern void lame_close_vdb_(IntPtr vdb);

        private IntPtr _vdb_handle = IntPtr.Zero;

        public bool lame_open_vdb(string vdbf)
        {
            try
            {
                _vdb_handle = lame_open_vdb_(vdbf);
                if (_vdb_handle == IntPtr.Zero) return false;

                return true;

            }
            catch (Exception e)
            {
                Console.WriteLine(e.Message);
            }
            return false;
        }
        public void lame_close_vdb()
        {
            try
            {
                if (_vdb_handle == IntPtr.Zero) return;
                lame_close_vdb_(_vdb_handle);
                _vdb_handle = IntPtr.Zero;

            }
            catch (Exception)
            {

            }
        }

        public IntPtr vdb_handle
        {
            get
            {
                return _vdb_handle;
            }
        }
    }
}
