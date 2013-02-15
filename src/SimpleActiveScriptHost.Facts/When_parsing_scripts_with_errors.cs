using Xunit;

namespace CitizenMatt.SimpleActiveScriptHost.Facts
{
    public class When_parsing_scripts_with_errors
    {
        [Fact]
        public void Should_raise_exception()
        {
            using (var host = new ScriptHost("javascript", "test"))
            {
                const string scriptText = "funciodjny() { wibble; }";
                var exception = Assert.Throws<ScriptException>(() => host.Parse(scriptText, "broken"));
                Assert.Equal("Microsoft JScript compilation error", exception.Source);
                Assert.Equal("Expected ';'", exception.Description);
                Assert.Equal(1, exception.LineNumber);
                Assert.Equal(14, exception.CharacterPosition);
                Assert.Equal(scriptText, exception.LineText);
            }
        }

        [Fact]
        public void Should_not_raise_exception_when_parsing_next_script()
        {
            using (var host = new ScriptHost("javascript", "test"))
            {
                Assert.Throws<ScriptException>(() => host.Parse("funciodjny() { wibble; }", "broken"));
                Assert.DoesNotThrow(() => host.Parse("function() { }", "working"));
            }
        }

        [Fact]
        public void Should_include_file_name_in_error_text()
        {
            const string returnsString = @"function return_string() { return 'sausages'; }";
            const string causesError = @"function causes_error() { return doesnt_exist.value; }";

            using (var host = new ScriptHost("javascript", "test"))
            {
                host.Parse(returnsString, "ReturnsString");
                host.Parse(causesError, "CausesError");

                var exception = Assert.Throws<ScriptException>(() => host.CallMethod("causes_error"));
                Assert.Equal("CausesError", exception.ScriptName);
            }
        }
    }
}