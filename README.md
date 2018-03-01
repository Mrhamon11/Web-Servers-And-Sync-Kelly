Team:
Avi Amon (avraham.amon@mail.yu.edu)
Mordechai Schmutter (mordechai.schmutter@mail.yu.edu)
Marty Spiewak (martin.spiewak@mail.yu.edu)

We collaborated on all four parts of the project. However, Avi and Marty did most of the work together for parts one and two, Mordechai did most of the work for part four, and all four of us worked on the statistics portion of part 3.

Design Overview:
In the server, we first implemented multithreading with mutexes and conditions for waiting threads. We also created a queue for requests. The operations on the queue change depending on the scheduling algorithm passed to it. If the scheduling algorithm is ANY or FIFO, we implemented a single ended queue and locked it to ensure that only one thread can access it at a time. Requests can only be pulled off in FIFO order in either cases. However, if the scheduling algorithm is HPIC or HPHC the queue acted like a double ended queue. Depending on the scheduling algorithm, Requests will be pulled off one side of the queue exclusively. All images are added on the right had side, and all HTML requests are added to the left. HPIC will pull from the right and HTML will pull from the left to ensure that the request corresponding to the scheduling algorthim is given priority.

In the client, we created a makeshift queue using mutexes and a struct called ticket_lock_t. This allowed us to handle which requests were processed in which order in the event of a FIFO scheduling in the client. In the event that a CONCUR is specified, pthread barries were used. If multiple files were passed into the client, the threads should alternate which file to send as a request.

Handling Ambiguities:
There were a few ambiguiteis we had to handle. Firstly, in ther server, ANY is treated as FIFO for simplicity. We also had to figure out how to implement requests, and which data to keep track of, so we created a few structs for that purpose, and the pointers of theses struct instances were passed around to many different parts of the program for execution.

Known Bugs:
While at first, our program was function fine, we ended up running into many programs that caused the execution of the program to fail every time. It functioned properly after implementing both parts 1 and 2, but randomly stopped working sometime in the middle of implementing the statistics. The issue was in the code that already existed as it was an issue with the bind function. After much research online, it seems that the server dies immediately after this function call, as for some reason, the machine believes that the port that was specified on the commandline was already in use.
This issue severly hindered our ability to effectively implement and test the the rest of the features that were still incomplete at the time of the failure. At first the server would only run on one port, 8008, and only on Avi's computer. On both of the other machines, the server failed immediately. Even when the program was working on 8008, although the client managed to get though, and the html of the passed in file was returned and displayed on screen, the logger was still logging error messages. Eventually it stopped running even on Avi's machine, especially when we implemented the client logic. We had absolutely no way of testing the multithreaded client because of this issue. We spent hours trying to debug this issue, and looking online for a solution, but we couldn't find one. Furthermore, printing stopped working for us, even when the <code>for</code> loop that closes down the printing functionality was commented out. Because of this, none of our statistics are ever printed out to the screen, and we couldn't do anything about it.

Testing:
When the program was functioning properly, we tested the functionality by starting the server, and executing many clients on it. Later, we had the idea to create a bash script to start many clients on the system to quickly start many clients, but when we had that idea, the program stopped functioning as we mentioned above. When the program stopped working, we had no way of testing our modifications, so we honestly don't know if it worked.

An example of our program execution looks as follows. Maybe it'll run on your machine, but as we've mentioned, it fails to run properly on ours:

Start Server:
./server 8008 [project directory] 5 10 FIFO

Start Client:
./client localhost 8008 5 FIFO /index.html /nigel.jpg 
