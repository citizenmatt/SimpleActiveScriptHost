using Xunit;

namespace CitizenMatt.SimpleActiveScriptHost.Facts
{
    public class When_calling_method_with_parameters
    {
        [Fact]
        public void Should_return_passed_in_string()
        {
            const string code = @"function return_string(value) { return value; }";
            const string expectedValue = "sausages";

            using (var host = new ScriptHost("javascript", "test"))
            {
                host.Parse(code, "stuff");
                var retval = host.CallMethod("return_string", expectedValue);
                var stringRetval = Assert.IsType<string>(retval);
                Assert.Equal(expectedValue, stringRetval);
            }
        }

        [Fact]
        public void Should_return_passed_in_number()
        {
            const string code = @"function return_number(value) { return value; }";
            const int expectedValue = 33;

            using (var host = new ScriptHost("javascript", "test"))
            {
                host.Parse(code, "stuff");
                var retval = host.CallMethod("return_number", expectedValue);
                var intRetval = Assert.IsType<int>(retval);
                Assert.Equal(expectedValue, intRetval);
            }
        }

        [Fact]
        public void Should_handle_multiple_parameters()
        {
            const string code = @"function add(val1, val2, val3) { return val1 + val2 + val3; }";

            using (var host = new ScriptHost("javascript", "test"))
            {
                host.Parse(code, "stuff");
                var retval = host.CallMethod("add", 1, 2, 3);
                var intRetval = Assert.IsType<int>(retval);
                Assert.Equal(6, intRetval);
            }
        }
    }
}