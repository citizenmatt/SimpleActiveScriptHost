using System.Runtime.InteropServices;
using Xunit;

namespace CitizenMatt.SimpleActiveScriptHost.Facts
{
    public class When_exposing_objects_to_scripts
    {
        [Fact]
        public void Should_not_create_named_item_before_use()
        {
            using (var host = new ScriptHost("javascript", "test"))
            {
                bool called = false;
                host.AddNamedItem("cool_object", () =>
                    {
                        called = true;
                        return new CoolObject("cheese");
                    });
                host.Parse("function do_nothing() { }", "do_nothing");
                var result = host.CallMethod("do_nothing");
                Assert.Null(result);

                Assert.False(called);
            }
        }

        [Fact]
        public void Should_be_able_to_read_property_from_script()
        {
            using (var host = new ScriptHost("javascript", "test"))
            {
                const string expected = "cheese";
                host.AddNamedItem("cool_object", () => new CoolObject(expected));
                host.Parse("function get_value() { return cool_object.value; }", "get_value");
                var result = host.CallMethod("get_value");
                Assert.NotNull(result);
                var stringResult = Assert.IsType<string>(result);
                Assert.Equal(expected, stringResult);
            }
        }

        [Fact]
        public void Should_be_able_to_set_property_from_script()
        {
            using (var host = new ScriptHost("javascript", "test"))
            {
                var coolObject = new CoolObject("cheese");
                host.AddNamedItem("cool_object", () => coolObject);
                host.Parse("function set_value() { cool_object.value = 'sausages'; }", "set_value");
                host.CallMethod("set_value");
                Assert.Equal("sausages", coolObject.Value);
            }
        }

        [Fact]
        public void Should_be_able_to_call_method_from_script()
        {
            using (var host = new ScriptHost("javascript", "test"))
            {
                var coolObject = new CoolObject("cheese");
                host.AddNamedItem("cool_object", () => coolObject);
                host.Parse("function set_value() { cool_object.SetValue('sausages'); }", "set_value");
                host.CallMethod("set_value");
                Assert.Equal("sausages", coolObject.Value);
            }
        }

        [Fact]
        public void Should_be_able_to_return_value_from_object_method()
        {
            using (var host = new ScriptHost("javascript", "test"))
            {
                const string expected = "cheese";
                host.AddNamedItem("cool_object", () => new CoolObject(expected));
                host.Parse("function get_value() { return cool_object.GetValue(); }", "get_value");
                var result = host.CallMethod("get_value");
                Assert.NotNull(result);
                var stringResult = Assert.IsType<string>(result);
                Assert.Equal(expected, stringResult);
            }
        }

        [Fact]
        public void Should_throw_if_object_is_not_com_visible()
        {
            using (var host = new ScriptHost("javascript", "test"))
            {
                host.AddNamedItem("rubbish_object", () => new RubbishObject("cheese"));
                host.Parse("function get_value() { return rubbish_object.value; }", "get_value");
                var exception = Assert.Throws<ScriptException>(() => host.CallMethod("get_value"));
                Assert.Equal("Class doesn't support Automation", exception.Description);
            }
        }

        [ComVisible(true)]
        public class CoolObject
        {
            public CoolObject(string value)
            {
                Value = value;
            }

            public string Value { get; set; }

            public void SetValue(string newValue)
            {
                Value = newValue;
            }

            public string GetValue()
            {
                return Value;
            }
        }

        public class RubbishObject
        {
            public RubbishObject(string value)
            {
                Value = value;
            }

            public string Value { get; set; }
        }
    }
}