# terminal_emulator
Very simple terminal emulator written in C++ using std::filesystem.
 
Command syntax is similar to bash syntax.




## Implemented commands

``` bash
clear - clears the terminal window
pwd - prints working directory
cd (-fc) dir - change directory, with -fc changes directory to the last existing in path, without -fc stays in the original
ls (-all) (dir) - prints list of files in dir, with -all also prints size, permissions, date created
mkdir dir - creates new directory 
rmdir dir - removes existing directory
```

## To be implemented soon
```bash
rm - removes file or directory
cat 
mv
cp
```
