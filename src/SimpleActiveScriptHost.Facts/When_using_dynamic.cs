using System.Runtime.InteropServices;
using Xunit;

namespace CitizenMatt.SimpleActiveScriptHost.Facts
{
    public class When_using_dynamic
    {
        [Fact]
        public void Should_work_with_simple_return_values()
        {
            using (var host = ScriptHostFactory.CreateJavaScriptHost("test"))
            {
                host.Parse(@"function get_string() { return 'sausages'; }", "get_string");
                dynamic result = host.CallMethod("get_string");

                Assert.Equal("sausages", result);
            }
        }

        [Fact]
        public void Should_work_with_complex_return_values()
        {
            using (var host = ScriptHostFactory.CreateJavaScriptHost("test"))
            {
                host.Parse(
@"function get_value() {
    return {
        value: 'sausages',
        data: [ { text: 'oink' }, { text: 'kapow' } ]
    };
}", "get_value");
                dynamic result = host.CallMethod("get_value");

                Assert.Equal("sausages", result.value);
                Assert.NotNull(result.data);
                Assert.Equal(2, result.data.length);

                // Hmm. Haven't figured out how to do this. Looks like dynamic is trying
                // to invoke an indexer as a method, but the JS object doesn't have an
                // indexer (data[0] is just getting the object called "0" from the JS object)
                // And we can't call data.0.text, because that's not valid C#
                //Assert.Equal("oink", result.data[0].text);
                //Assert.Equal("kapow", result.data["1"].text);
            }
        }

        [Fact]
        public void Should_work_with_complex_paramters()
        {
            using (var host = ScriptHostFactory.CreateJavaScriptHost("test"))
            {
                var thing = new Thing();
                host.Parse(@"function call_something(o) { o.Stuff({ value: 'sausages' }); }", "call_something");
                host.CallMethod("call_something", thing);
                Assert.Equal("sausages", thing.Parameter.value);
            }
        }

        [ComVisible(true)]
        public class Thing
        {
            public void Stuff(dynamic parameter)
            {
                Parameter = parameter;
            }

            public dynamic Parameter { get; private set; }
        }
    }
}