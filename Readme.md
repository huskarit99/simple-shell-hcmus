---------------------------------------- Simple Shell -------------------------------------------
This is a simple shell implemented by C language

- Run simple-shell.c:
+ Build:
➜  1712045 gcc -o simple_shell simple_shell.c
+ Run:
➜  1712045 ./simple_shell

.................................................................................................

The shell provides these features as follows:

- The shell parses the input command by spaces. It can tolerate spaces at the beginning or ending of the command, and more than one spaces between two parameters in the command. But it may not work when spaces appear in the quotation marks.
- When you want to quit, you can use the command "exit".
- You can use Ctrl+C to exit the shell as well.
- Use the system call execvp, most commands that can run in a practical shell also work in this simple shell.
- If you want to run the command in the background, you can append an ampersand & as the last parameter. For example, "cat out.txt &". In this case, the parent and child processes will run concurrently.
- The shell implements the command history, to show history commands. Moreover, you can use command "!!" to display the newest command.
- In addition, the shell also realizes the command cd, and supports some special types of calling like cd ~, cd /, cd ~/Documents, cd ../...
- The shell allows commands with some syntax error and do not crash. It will return you messages like Command not found, No such file or directory, No such command in history and some others when you input illegal commands like lss, etc.
- My shell can support the ‘>’ and ‘<’ redirection operators, where ‘>’ redirects the output of a command to a file and ‘<’ redirects the input to a command from a file.
- The final modification to my shell is to allow the output of one command to serve as input to another using a pipe. (Communication via a Pipe)

...................................................................................................

Here is a demo you can refer to:

➜  1712045 ./simple_shell
osh> ls
Description_file.pdf  out.txt  Readme.md  simple_shell	simple_shell.c
osh> pwd
/home/dell/1712045
osh> history
3 history
2 pwd
1 ls
osh> ls -l
total 112
-rw-rw-r-- 1 dell dell 68594 Thg 10 10 21:53 Description_file.pdf
-rw------- 1 dell dell    19 Thg 10 10 20:03 out.txt
-rw-rw-r-- 1 dell dell  1770 Thg 10 10 22:24 Readme.md
-rwxr-xr-x 1 dell dell 21880 Thg 10 10 22:10 simple_shell
-rw-rw-r-- 1 dell dell  8364 Thg 10 10 20:02 simple_shell.c
osh> !!
ls -l
total 112
-rw-rw-r-- 1 dell dell 68594 Thg 10 10 21:53 Description_file.pdf
-rw------- 1 dell dell    19 Thg 10 10 20:03 out.txt
-rw-rw-r-- 1 dell dell  1770 Thg 10 10 22:24 Readme.md
-rwxr-xr-x 1 dell dell 21880 Thg 10 10 22:10 simple_shell
-rw-rw-r-- 1 dell dell  8364 Thg 10 10 20:02 simple_shell.c
osh> ls -l > out.txt
osh> cat out.txt
total 112
-rw-rw-r-- 1 dell dell 68594 Thg 10 10 21:53 Description_file.pdf
-rw------- 1 dell dell     0 Thg 10 10 22:28 out.txt
-rw-rw-r-- 1 dell dell  1770 Thg 10 10 22:24 Readme.md
-rwxr-xr-x 1 dell dell 21880 Thg 10 10 22:10 simple_shell
-rw-rw-r-- 1 dell dell  8364 Thg 10 10 20:02 simple_shell.c
osh> sort < out.txt
-rw------- 1 dell dell     0 Thg 10 10 22:39 out.txt
-rw-rw-r-- 1 dell dell  1770 Thg 10 10 22:24 Readme.md
-rw-rw-r-- 1 dell dell 68594 Thg 10 10 21:53 Description_file.pdf
-rw-rw-r-- 1 dell dell  8399 Thg 10 10 22:38 simple_shell.c
-rwxr-xr-x 1 dell dell 21880 Thg 10 10 22:38 simple_shell
total 112
osh> ls -l | wc
      6      52     302

...................................................................................................

--- Nguyen Thai Hoc --- 1712045 ---
10/10/2019.



