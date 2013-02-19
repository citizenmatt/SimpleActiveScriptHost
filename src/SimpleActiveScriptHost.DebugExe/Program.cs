using System;

namespace CitizenMatt.SimpleActiveScriptHost.DebugExe
{
    static class Program
    {
        private const string Code = "function return_string() {\r\n	var i = 42;\r\n	i = i + 1;\r\n	debugger;\r\n	i = i - 1;\r\n}; return_string();";

        private const string ReturnNumber =
@"function return_number() {

    var i = 42;
    i = i + 1;

    i = i - 1;
    return i;
}; ";

        private const string ReturnString =
@"function return_string() {
    return 'hello';
}";


        static void Main(string[] args)
        {
            using (var host = new ScriptHost("javascript", "debug_exe"))
            {
                host.Parse(ReturnNumber, "ReturnNumber.js");
                host.Parse(ReturnString, "ReturnString.js");

                Console.WriteLine("Press enter to continue...");
                Console.ReadLine();

                var o = host.CallMethod("return_number");

                Console.WriteLine(o);
            }
        }
    }
}
