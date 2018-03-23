using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Interop
{
    class Program
    {
        static void Main(string[] args)
        {
            ManagedObject myObj = new ManagedObject(11);

            Console.WriteLine(myObj.get_value());

            myObj.set_value(22);
            Console.WriteLine(myObj.get_value());

            //! delete object
            myObj.delete();

            //keep console window showing
            Console.ReadLine();
        }
    }
}
