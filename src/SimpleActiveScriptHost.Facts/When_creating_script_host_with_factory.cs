using System;
using System.IO;
using Xunit;

namespace CitizenMatt.SimpleActiveScriptHost.Facts
{
    public class When_creating_script_host_with_factory
    {
        const string GetScriptVersion = "function getScriptVersion() { return ScriptEngineMajorVersion() + '.' + ScriptEngineMinorVersion(); }";

        [Fact]
        public void Should_create_javascript_host()
        {
            using (var host = ScriptHostFactory.CreateJavaScriptHost("test"))
                Assert.NotNull(host);
        }

        [Fact]
        public void Should_create_vbscript_host()
        {
            using (var host = ScriptHostFactory.CreateVBScriptHost("test"))
                Assert.NotNull(host);
        }

        [Fact]
        public void Should_default_to_chakra_when_creating_javascript_host()
        {
            var jscript9dll = Path.Combine(Environment.SystemDirectory, "jscript9.dll");
            Assert.True(File.Exists(jscript9dll), "Test requires Chakra jscript to be installed");

            using (var host = ScriptHostFactory.CreateJavaScriptHost("test"))
            {
                host.Parse(GetScriptVersion, "scriptVersion");

                var version = (string)host.CallMethod("getScriptVersion");
                Assert.True(version == "9.0" || version == "10.0", string.Format("Version should be >= 9.0. Was {0}", version));
            }
        }

        [Fact]
        public void Should_fallback_to_jscript_if_chakra_not_available()
        {
            // This is tricky to do. If you have IE9/10 installed,
            // you'll always get it, unless I fudge the clsid
            var chakraClsid = ScriptHostFactory.ChakraClsid;
            ScriptHostFactory.ChakraClsid = "16d51579-XXXX-4c8b-a276-0ff4dc41e755";

            try
            {
                using (var host = ScriptHostFactory.CreateJavaScriptHost("test"))
                {
                    host.Parse(GetScriptVersion, "scriptVersion");

                    var version = (string)host.CallMethod("getScriptVersion");
                    Assert.Equal("5.8", version);
                }
            }
            finally
            {
                ScriptHostFactory.ChakraClsid = chakraClsid;
            }
        }
    }
}