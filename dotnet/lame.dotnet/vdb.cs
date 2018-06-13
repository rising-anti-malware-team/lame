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
        [System.Runtime.InteropServices.DllImportAttribute("lame.dll", EntryPoint = "lame_open_vdb", CallingConvention = CallingConvention.Cdecl)]
        private static extern System.IntPtr lame_open_vdb_([System.Runtime.InteropServices.InAttribute()] [System.Runtime.InteropServices.MarshalAsAttribute(System.Runtime.InteropServices.UnmanagedType.LPStr)] string vlibf);

        [System.Runtime.InteropServices.DllImportAttribute("lame.dll", EntryPoint = "lame_close_vdb" , CallingConvention = CallingConvention.Cdecl)]
        private static extern void lame_close_vdb_(System.IntPtr vdb);

        private IntPtr _vdb_hanle = IntPtr.Zero;

        public bool lame_open_vdb(string vdbf) 
        {
            try
            {
                _vdb_hanle = lame_open_vdb_(vdbf);
                if (_vdb_hanle == IntPtr.Zero) return false;
                
                return true;
                
            }
            catch (System.Exception e)
            {
                Console.WriteLine(e.Message);
            }
            return false;
        }
        public void lame_close_vdb() 
        {
            try
            {
                if (_vdb_hanle == IntPtr.Zero) return;
                lame_close_vdb_(_vdb_hanle);
                _vdb_hanle = IntPtr.Zero;
                
            }
            catch (System.Exception)
            {

            }
        }

        public IntPtr vdb_handle
        {
            get 
            {
                return _vdb_hanle;
            }
        }

    }
}
