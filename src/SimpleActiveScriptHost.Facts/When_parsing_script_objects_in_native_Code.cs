using System;
using System.Linq;
using System.Reflection;
using System.Runtime.InteropServices;
using Xunit;

namespace CitizenMatt.SimpleActiveScriptHost.Facts
{
    public class When_parsing_script_objects_in_native_code
    {
        [Fact]
        public void Should_handle_objects_with_arrays()
        {
            const string code =
@"function call_native() {
    var data = [];
    data.push({ value: 'cheese' });
    data.push({ value: 'bread' });
    data.push({ value: 'bananas' });
    native.blah({
        data: data
    });
}";
            using (var host = new ScriptHost("javascript", "test"))
            {
                var native = new Native();
                host.AddNamedItem("native", () => native);
                host.Parse(code, "code");
                host.CallMethod("call_native");

                var scriptObject = new ScriptObject(native.Value);

                Assert.NotNull(scriptObject);

                var d = scriptObject.GetValue<object>("data");
                var data = new ScriptObject(d);
                Assert.Equal(3, data.GetValue<int>("length"));

                // JS arrays are associative objects. The indices are just lookups
                // into object properties
                var o1 = data.GetValue<object>("0");
                var o2 = data.GetValue<object>("1");
                var o3 = data.GetValue<object>("2");

                Assert.NotNull(o1);
                Assert.NotNull(o2);
                Assert.NotNull(o3);

                Assert.Equal("cheese", new ScriptObject(o1).GetValue<string>("value"));
                Assert.Equal("bread", new ScriptObject(o2).GetValue<string>("value"));
                Assert.Equal("bananas", new ScriptObject(o3).GetValue<string>("value"));
            }
        }
    }

    [ComVisible(true)]
    public class Native
    {
        public void blah(object value)
        {
            Value = value;
        }

        public object Value { get; set; }
    }
}