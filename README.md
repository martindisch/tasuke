# tasuke [WIP]

<p align="center">
    <img alt="tasuke kanji" width="50%" src="media/kanji_regular.svg"/>
</p>

## Description
A simple task/todo application for the terminal.
A *no fuss, no muss* replacement for your trusty todo.txt, giving you a more
convenient way to access your tasks without all the cruft of more complex
todo applications that make you spend time overmanaging your todo list.
It supports multiple task lists and lets you manually order tasks, but
that's about it.
No history, no priorities, no dates and no reminders.
Your task lists are stored in plain text files in a location of your choice.

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
t -i "My task" 3                            # Insert into default list
t -i -n mylist "My task" 3                  # Insert into specific list
```

**Complete task(s)**
```
t -d 3 10 7                                 # Complete from default list
t -d -n mylist 3 10 7                       # Complete from specific list
```

**Move task** by inserting at given position, shifting other items downwards
```
t -m 3 5                                    # Move inside default list
t -m -n mylist 3 5                          # Move inside specific list
```

**Set task list directory**
```
t -a "New task" -s /path/to/dir             # Add to default list in directory
```

**Delete list(s)**
```
t -r mylist school                          # Delete specific lists
```

## License
[MIT License](LICENSE)
