namespace CitizenMatt.SimpleActiveScriptHost.DebugExe
{
    static class Program
    {
        private const string Code = "function return_string() {\r\n	var i = 42;\r\n	i = i + 1;\r\n	debugger;\r\n	i = i - 1;\r\n}; return_string();";

        static void Main(string[] args)
        {
            using (var host = new ScriptHost("javascript", "debug_exe"))
            {
                host.Parse(Code, "code.js");

                // host.CallMethod();
            }
        }
    }
}
