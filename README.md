# tasuke

<p align="center">
    <img alt="tasuke kanji" width="50%" src="kanji.svg"/>
</p>

## Description
A simple task/todo application for the terminal.
A *no fuss, no muss* replacement for your trusty todo.txt, giving you a more
convenient way to access your tasks without all the cruft of more complex
todo applications that make you spend time overmanaging your todo list.
It supports multiple task lists and lets you manually order tasks, but
that's about it.
No history, no priorities, no dates and no reminders.
Your task lists are stored as plain text in a location of your choice.

## Usage

Most commands follow the pattern
```
t [command] [list selection] [content...]
```
For example, to add two new tasks to a list called *school*, you would do
```
t -a -n school "Study for exam" "Prepare presentation"
```
To be as portable as possible, tasuke uses the POSIX utility argument syntax.
This lets you combine options, subject to certain limitations of course.
```
t -an school "Study for exam" "Prepare presentation"
```

If you don't select a specific list, your command will be run against the
default list, which is called *todo*.

Every list has a corresponding plain text file `listname.txt` which is stored
in the `.tasuke` directory of your user home.
If you want tasuke to keep these files somewhere else, pass this path along
with the `-s` option.
Since you'll most likely be assigning an alias to the tasuke executable anyway,
you can make the alias use this argument by default.
If you want to see a list after modification, use the `-v` flag.

**List tasks**
```
t                                           # List default list
t mylist school                             # List specific lists
```

**Add task(s)** by appending to list
```
t -a "My first task" "Second task"          # Add to default list
t -a -n mylist "First task" "Second task"   # Add to specific list
```

**Insert task** by inserting at given position, shifting other items downwards
```
t -i 3 "My task"                            # Insert into default list
t -i -n mylist 3 "My task"                  # Insert into specific list
```

**Complete task(s)**
```
t -d 3 10 7                                 # Complete from default list
t -d -n mylist 3 10 7                       # Complete from specific list
```
**Move task** from one position to another, by bubbling it up or down
```
t -m 3 5                                    # Move inside default list
t -m -n mylist 3 5                          # Move inside specific list
```

**Delete list(s)**
```
t -r                                        # Delete default task list
t -r mylist school                          # Delete specific lists
```

**Show lists**
```
t -l                                        # Show all list names
```

**Set task list directory**
```
t -a "New task" -s /path/to/dir             # Add to default list in directory
```

**Show list after modification**
```
t -i -v 5 "Another task"                    # Insert into default list, showing
                                            # it after the modification
```

## Installation
Since tasuke uses only POSIX system interfaces, you should be able to compile
it on almost every platform.
Just run `make` and you're good to go.
You'll probably want to add the executable to your `PATH` variable, or better
yet, create an alias in your `.bashrc` or equivalent.
An alias is very convenient if you want tasuke to store your lists in some
other place than the default `.tasuke` directory, since you can add the `-s`
option to the command. For example, you could use
```
alias t='/opt/tasuke/tasuke -s /home/user/Dropbox/tasuke'
```

## Language, standards & platforms
By default, tasuke is compiled by GCC according to strict ISO C11, with some
POSIX functions.
C11 is used because it was the newest standard at the time of development.
However, tasuke doesn't use any features that are not already in C99, so it's
possible to compile it with that as well.
The project doesn't conform to ANSI C89/ISO C90, because there are several
annoying restrictions (no mixing of declarations and code, short minimum
supported string length by compiler, no variable declarations in for loop
statements, etc.) and there are no tangible benefits of doing that for
userspace programs.
Some POSIX functions that are not part of standard C are used (`getopt`,
`strcasecmp`, etc.), and feature test macros are defined at the top of source
files to communicate the required POSIX version to the standard library.
If your glibc is older than 2.12, you may need to use different macros, because
some functions were not part of POSIX before that.

## License
[MIT License](LICENSE)
