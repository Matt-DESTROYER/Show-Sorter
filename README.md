# Show-Sorter

A simple script written to enable easily renaming a series of TV shows and sorting them into directories.

This script uses a format file (or alternatively you can create one in the script). The format file `name-scheme.format` is very simple to use; it has two lines, the former being the input format and the latter being the output format. You can add named variables by using the '`<`' and '`>`' to enclose your variable (and you can escape these characters with the classic '`\`'). Here's an example
```txt
<SHOW_NAME> - <EPISODE_TITLE> (<SEASON_NUMBER>x<EPISODE_NUMBER>)
<SHOW_NAME>/<SEASON_NUMBER>/<EPISODE_NUMBER> - <EPISODE_TITLE>
```

> Note: The file extension is automatically handled and does not need to be included in the patter.

> Note: Any folders you add are automatically created.

This tool was written to assist bulk renaming files already in a standard format, removing the hastle of manually renaming them.
