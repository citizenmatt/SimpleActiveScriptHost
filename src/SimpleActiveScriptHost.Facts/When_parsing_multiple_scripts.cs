using System;
using Xunit;

namespace CitizenMatt.SimpleActiveScriptHost.Facts
{
    public class When_parsing_multiple_scripts : IDisposable
    {
        private const string ReturnsString = @"function return_string() { return 'sausages'; }";
        private const string ReturnsInt = @"function return_int() { return 42; }";

        private readonly ScriptHost host;

        public When_parsing_multiple_scripts()
        {
            host = new ScriptHost("javascript", "test");

            host.Parse(ReturnsString, "ReturnsString");
            host.Parse(ReturnsInt, "ReturnsInt");
        }

        public void Dispose()
        {
            host.Dispose();
        }

        [Fact]
        public void Should_return_simple_string()
        {
            var retval = host.CallMethod("return_string");
            var stringRetval = Assert.IsType<string>(retval);
            Assert.Equal("sausages", stringRetval);
        }

        [Fact]
        public void Should_return_simple_integer()
        {
            var retval = host.CallMethod("return_int");
            var intRetval = Assert.IsType<int>(retval);
            Assert.Equal(42, intRetval);
        }
    }
}