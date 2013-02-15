using System.Reflection;
using Xunit;

namespace CitizenMatt.SimpleActiveScriptHost.Facts
{
    public class When_calling_method
    {
        [Fact]
        public void Should_return_simple_string()
        {
            const string code = @"function return_string() { return 'sausages'; }";
            using (var host = new ScriptHost("javascript", "test"))
            {
                host.Parse(code, "stuff");
                var retval = host.CallMethod("return_string");
                var stringRetval = Assert.IsType<string>(retval);
                Assert.Equal("sausages", stringRetval);
            }
        }

        [Fact]
        public void Should_return_simple_integer()
        {
            const string code = @"function return_int() { return 42; }";
            using (var host = new ScriptHost("javascript", "test"))
            {
                host.Parse(code, "stuff");
                var retval = host.CallMethod("return_int");
                var intRetval = Assert.IsType<int>(retval);
                Assert.Equal(42, intRetval);
            }
        }

        [Fact]
        public void Should_return_object()
        {
            const string code = @"function return_object() { return { text: 'hello', number: 42 }; }";
            using (var host = new ScriptHost("javascript", "test"))
            {
                host.Parse(code, "stuff");
                var retval = host.CallMethod("return_object");
                Assert.NotNull(retval);

                var type = retval.GetType();

                var result = type.InvokeMember("text", BindingFlags.GetProperty, null, retval, null);
                var stringResult = Assert.IsType<string>(result);
                Assert.Equal("hello", stringResult);

                result = type.InvokeMember("number", BindingFlags.GetProperty, null, retval, null);
                var intResult = Assert.IsType<int>(result);
                Assert.Equal(42, intResult);
            }
        }

        [Fact]
        public void Should_throw_exception_if_script_errors()
        {
            const string code = @"function fails() { return doesnt_exist.value; }";

            using (var host = new ScriptHost("javascript", "test"))
            {
                host.Parse(code, "stuff");

                var exception = Assert.Throws<ScriptException>(() => host.CallMethod("fails"));
                Assert.Equal("Microsoft JScript runtime error", exception.Source);
                Assert.Equal("'doesnt_exist' is undefined", exception.Description);
                Assert.Equal(1, exception.LineNumber);
                Assert.Equal(20, exception.CharacterPosition);
                Assert.Null(exception.LineText);
            }
        }
    }
}