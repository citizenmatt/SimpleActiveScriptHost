using Xunit;

namespace CitizenMatt.SimpleActiveScriptHost.Facts
{
    public class When_creating_script_host
    {
        [Fact]
        public void Should_throw_exception_when_language_is_not_found()
        {
            Assert.Throws<UnknownScriptLanguageException>(() => new ScriptHost("sausages", "test"));
        }
    }
}