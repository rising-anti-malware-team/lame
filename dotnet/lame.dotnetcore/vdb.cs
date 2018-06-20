using System;
using System.Runtime.InteropServices;

namespace lame.dotnetcore
{
    public class VirusLib
    {
        public IntPtr vdb_handle { get; private set; } = IntPtr.Zero;

        [DllImport("./lame.dll", EntryPoint = "lame_open_vdb", CallingConvention = CallingConvention.Cdecl)]
        private static extern IntPtr lame_open_vdb_([In] [MarshalAs(UnmanagedType.LPStr)] string vlibf);

        [DllImport("./lame.dll", EntryPoint = "lame_close_vdb", CallingConvention = CallingConvention.Cdecl)]
        private static extern void lame_close_vdb_(IntPtr vdb);

        public bool lame_open_vdb(string vdbf)
        {
            try
            {
                vdb_handle = lame_open_vdb_(vdbf);
                if (vdb_handle == IntPtr.Zero) return false;

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
                if (vdb_handle == IntPtr.Zero) return;
                lame_close_vdb_(vdb_handle);
                vdb_handle = IntPtr.Zero;
            }
            catch (Exception)
            {
            }
        }
    }
}