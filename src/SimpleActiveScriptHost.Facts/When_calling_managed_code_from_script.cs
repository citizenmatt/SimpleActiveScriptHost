﻿using System;
using System.Runtime.InteropServices;
using Xunit;

namespace CitizenMatt.SimpleActiveScriptHost.Facts
{
    public class When_calling_managed_code_from_script
    {
        [Fact]
        public void Should_not_create_named_item_before_use()
        {
            using (var host = new ScriptHost("javascript", "test"))
            {
                var called = false;
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

        [Fact]
        public void Should_dispose_any_objects_when_finished()
        {
            var disposed = false;
            using (var host = new ScriptHost("javascript", "test"))
            {
                host.AddNamedItem("disposable_object", () => new DisposableObject(() => disposed = true));
                host.Parse("function get_value() { return disposable_object; }", "get_value");
                var result = host.CallMethod("get_value");
                Assert.NotNull(result);
            }

            Assert.True(disposed);
        }

        [Fact]
        public void Should_pass_script_objects_to_managed_code()
        {
            const string code =
@"function call_managed() {
    managed.DoSomething({
        text: 'cheese',
        number: 37
    });
}";

            using (var host = new ScriptHost("javascript", "test"))
            {
                var managed = new ManagedObject();
                host.AddNamedItem("managed", () => managed);
                host.Parse(code, "test");
                host.CallMethod("call_managed");

                var scriptObject = new ScriptObject(managed.Value);

                Assert.Equal("cheese", scriptObject.GetValue<string>("text"));
                Assert.Equal(37, scriptObject.GetValue<int>("number"));
            }
        }

        [Fact]
        public void Should_handle_script_arrays_in_managed_code()
        {
            const string code =
@"function call_managed() {
    var data = [];
    data.push({ value: 'cheese' });
    data.push({ value: 'bread' });
    data.push({ value: 'bananas' });
    managed.DoSomething({
        data: data
    });
}";
            using (var host = new ScriptHost("javascript", "test"))
            {
                var managed = new ManagedObject();
                host.AddNamedItem("managed", () => managed);
                host.Parse(code, "code");
                host.CallMethod("call_managed");

                var scriptObject = new ScriptObject(managed.Value);

                Assert.NotNull(scriptObject);

                var data = scriptObject.GetObject("data");
                Assert.Equal(3, data.GetValue<int>("length"));

                // JS arrays are associative objects. The indices are just lookups
                // into object properties
                var o1 = data[0];
                var o2 = data["1"];
                var o3 = data.GetObject("2");

                Assert.NotNull(o1);
                Assert.NotNull(o2);
                Assert.NotNull(o3);

                Assert.Equal("cheese", o1.GetValue<string>("value"));
                Assert.Equal("bread", o2.GetValue<string>("value"));
                Assert.Equal("bananas", o3.GetValue<string>("value"));
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

        [ComVisible(true)]
        public class DisposableObject : IDisposable
        {
            private readonly Action onDispose;

            public DisposableObject(Action action)
            {
                onDispose = action;
            }

            public void Dispose()
            {
                onDispose();
            }
        }

        [ComVisible(true)]
        public class ManagedObject
        {
            public void DoSomething(object value)
            {
                Value = value;
            }

            public object Value { get; set; }
        }
    }
}