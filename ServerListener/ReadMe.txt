Good day. First of all, thank you for giving me an opportunity to have this assessment.
I was truly benefited from this assessment.

For this assessement, it mostly a new things for me, I started from scratch from researched, study and develop.
For example, which database system to use, how to setup and configure and how to implement
into the programming language I use. Other than this, I had research and study the docker too.

Unfortunately, I feel sorry that unable to fulfilled all the requirements mentioned in the assesstment but only some of it.
Which I already try my best within the given time line.

******* Project Details ******
Project name		: ServerListener
Programming language	: C++
Database system		: MySQL

Files include in	: Project solution, todolist.sql (Extracted schema from MySQL)

Result screenshots	: ServerListener\ResultScreenshots


******* How to run the application ******
***Prerequisite, make sure MySQL database was imported into MySQL and running.
database login credential:
server		: tcp://localhost:3306
username	: root
Password	: Foo1234

1.) Go to directory, ServerListener\prj\x64\release.
2.) Copy, ServerListener.exe and mysqlcppconn-9-vs14.dll into any directory of your choice.
3.) Double click on ServerListener.exe to run the application.
4.) Or, perfrom Step 3 directly in directory mentioned in step 1.

******* How to test the application ******
1.) Open any internet browser.
2.) use http request below to perform the task:-
	Insert record - http://127.0.0.1:54000/insertrecord/{"descriptions":"Create a new database","status":"ongoing"}

	Update record - http://127.0.0.1:54000/updaterecord/{"id":"10001","status":"completed"}

	view record   - view by status, http://127.0.0.1:54000/viewrecord/{"id":"","status":"ongoing"}
		      - view by id, http://127.0.0.1:54000/viewrecord/{"id":"10001","status":""}
		      - view all, http://127.0.0.1:54000/viewrecord/{"id":"","status":""}

	delete record - delete by id, http://127.0.0.1:54000/deleterecord/{"id":"10001","status":""}

3.) server will reply and display message on the internet browser according.

******* How to build the application ******
***Prerequisite, make sure you have Microsoft Visual Studio installed.

1.) Go to directory, ServerListener\prj
2.) Open solution file, ServerListener.sln.
3.) Go to top pane, Build->Rebuild solution.
