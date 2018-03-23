using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Interop
{
    class ManagedObject
    {
        private IntPtr native_Instance;

        public ManagedObject(int value)
        {
            native_Instance = ManagedWrapper.new_NativeObject(value);
        }

        public void delete()
        {
            ManagedWrapper.delete_NativeObject(native_Instance);
        }

        public void set_value(int value)
        {
            ManagedWrapper.set_value(native_Instance, value);
        }

        public int get_value()
        {
            return ManagedWrapper.get_value(native_Instance);
        }
    }
}
