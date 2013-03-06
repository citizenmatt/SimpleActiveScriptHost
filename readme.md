# SimpleActiveScriptHost

SimpleActiveScriptHost is, surprisingly, a simple way to host Microsoft's ActiveScript scripting API in .net applications. 

It's key differentiation is the ability to debug your hosted scripts.

## What does it do?

It provides a simpler, .net friendly interface for interacting with the COM based ActiveScripting API. 

 For example:

	[Fact]
	public void Should_call_method_with_parameters_and_result()
	{
	    using(var host = ScriptHostFactory.CreateJavaScriptHost("application name"))
	    {
	    	host.Parse("function multiply(a, b) { return a * b; }", "stuff.js");
			var result = host.CallMethod("multiply", 12, 24);

			Assert.Equal(288, result);
	    }
	}

It allows you to pass objects that can be used from the script, as long as those objects are marked ``[ComVisible(true)]``. This allows you to add functionality to the scripting engines such as access to your object model, UI or files:

	[Fact]
	public void Should_interact_with_managed_objects()
	{
		using(var host = ScriptHostFactory.CreateJavaScriptHost("application name"))
		{
			host.AddNamedItem("managedObject", () => new ManagedObject());
			host.Parse("function multiply(a, b) { return managedObject.DoMultiply(a, b); }", "stuff.js");
			var result = host.CallMethod("multiply", 12, 24);

			Assert.Equal(288, result);
		}
	}

	[ComVisible(true)]
	public class ManagedObject
	{
		public int DoMultiply(int a, int b)
		{
			return a * b;
		}
	}

The return value of ``CallMethod`` is an ``object``, but you can use a ``dynamic`` type here if your application is .net 4+. Alternatively, there is a ``ScriptObject`` class provided to make it easier to get at more complicated types:

	[Fact]
	public void Should_handle_complex_script_objects()
	{
		using(var host = ScriptHostFactory.CreateJavaScriptHost("application name"))
		{
			host.Parse("function getComplexObject() { return { value: 'sausages' }; }", "stuff.js");
			var result = host.CallMethod("getComplexObject");

			dynamic dynamicObject = result;
			Assert.Equal("sausages", dynamicObject.value);

			var scriptObject = new ScriptObject(result);
			Assert.Equal("sausages", scriptObject.GetValue<string>("value"));
		}
	}

### Debugging

The key differentiator with this package is the ability to debug the scripts you host. This is why you must provide an application name and give each of your scripts a name.

Once your host has been created, you can attach to the host process using Visual Studio (make sure you have "Script" selected as the debug engine!) and Visual Studio will provide you with a list of running documents, one for each script you've added. Double clicking the document will allow you to set breakpoints, and from then on, it's just ordinary debugging.

## Where can I use it?

This package is a C++/CLI assembly. It is compiled against .net 2, so can be used in .net 2, 3.5 and 4.x applications. It requires the 

Since it is a C++/CLI assembly, it is x86 only! You will get an exception if you try to load this assembly in an x64 host process.

## Why C++/CLI?!

Well, it's not my first choice, I have to admit. However, debugging support just does not work with .net's COM interop. I don't know why, perhaps because the required COM object is passed from native to managed back to native, and something gets lost in translation. Doing the project in C++/CLI means being able to write native COM code, and still interop with C#.