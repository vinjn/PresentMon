using Diagnostics.Tracing;
using Diagnostics.Tracing.Parsers;
using System;
using System.Diagnostics;
using System.Diagnostics.Tracing;
using System.Threading;
using System.Threading.Tasks;

// This program is a simple demo, it has both a EventSource and a EventGenerator which generate events for 10 seconds
// as well as a 'listener; that listens for these events (potentially in a differnet process, but we don't do that here)
// and does some rudimentary processing (calculates the difference between two particular events.    
namespace SimpleMonitor
{

    /// <summary>
    /// The main program is the 'listener' that listens and processes the events that come from EventGenerator
    /// </summary>
    class Program
    {
        /// <summary>
        /// This is a demo of using TraceEvent to activate a 'real time' provider that is listening to 
        /// the MyEventSource above.   Normally this event source would be in a differnet process,  but 
        /// it also works if this process generate the evnets and I do that here for simplicity.  
        /// </summary>
        static int Main(string[] args)
        {
            var sessionName = "My Session 2";
            ////int processId = 0;
            ////var providers = TraceEventSession.ProvidersInProcess(processId);
            using (var session = new TraceEventSession(sessionName, null))  // the null second parameter means 'real time session'
            {
                //session.EnableProvider(new Guid("9ba24471-5f63-5416-2ce1-edf7a97ef50c"));
                //session.EnableKernelProvider(KernelTraceEventParser.Keywords.All);
                session.EnableProvider(new Guid("5d8087dd-3a9b-4f56-90df-49196cdc4f11"));
                session.EnableProvider(new Guid("802ec45a-1e99-4b83-9920-87c98277ba9d"));

                // prepare to read from the session, connect the ETWTraceEventSource to the session
                using (var source = new ETWTraceEventSource(sessionName, TraceEventSourceType.Session))
            {
                // Hook up the parser that knows about EventSources
                var parser = new RegisteredTraceEventParser(source);
                parser.All += delegate (TraceEvent data)
                {
                    if (data.EventName == "HistoryBuffer")
                        Console.WriteLine("GOT EVENT: " + data.Dump());
                    //if (data.EventName == "RuntimeMarker")
                    //{
                    //    for(int i = 0; i < data.PayloadNames.Length; ++i)
                    //    {
                    //        var n = data.PayloadNames[i];
                    //        //   var o = data.PayloadString(i);
                    //        Console.Write(n + " ");

                    //    }
                    //    Console.WriteLine();
                    //    Console.WriteLine(data.Dump());
                    //}
                };
                ;
                //// Enable my provider, you can call many of these on the same session to get other events.  


                // Start another thread that Causes MyEventSource to create some events
                // Normally this code as well as the EventSource itself would be in a different process.  
                //EventGenerator.CreateEvents();

                Console.WriteLine("Staring Listing for events");
                // go into a loop processing events can calling the callbacks.  Because this is live data (not from a file)
                // processing never completes by itself, but only because someone called 'source.Close()'.  
                source.Process();

            }
                //             Console.ReadLine();
            }
            return 0;
        }
    }
}
