Team:
Avi Amon (avraham.amon@mail.yu.edu)
Mordechai Schmutter (mordechai.schmutter@mail.yu.edu)
Marty Spiewak (martin.spiewak@mail.yu.edu)
We collaborated on all four parts of the project.

Design Overview:
In the server, if the scheduling algorithm is FIFO, we did a single ended queue that used mutex to simulate a FIFO queue. If the scheduling algorithm is HPIC or HPHC we used a double ended queue, and based on the specified scheduling algorithm, we arrive we pass the request into the appropriate side of our queue. HPIC will pull from the right and HTML will pull from the left.
In the client, we used a mutex ticket lock to simulate a FIFO queue for when FIFO is specified from the command line and pthread barriers to ensure that all threads execute at once for when CONCUR is specified.

Handling Ambiguities:
We handled ANY as FIFO.

Known Bugs:
We ran into issues getting the server to run, without changing the underlying boilerplate logic of how the server works, midway through our work on the server. This hindered out ability to effectively implement and test the the rest of the features that were still incomplete at the time that the server stopped running properly. The server began running only on certain ports, depending on the computer, and getting a binding error otherwise. Eventually it stopped running even on the few ports that worked when the issues began. Some statistics do not print.

Testing:
We added print statements to show what our code executed and which threads it was running in by printing the thread ID. We tried to make a bash script to test our server, but the issues we ran into made that option obsolete too.

Start Server:
./server 8008 [project directory] 1 10 FIFO

Start Client:
./client localhost 8008 5 CONCUR /index.html /nigel.jpg 
