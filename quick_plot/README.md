# QuickPlot
Plots a file with one click/command, automatically determining some properties.

## Usage
If the file is called QuickPlot, you can use it to plot a file using command:
```sh
./QuickPlot file.csv
```
In a GUI environment, assigning the program to opening the data files needed is enough.

## Operation
It is assumed that the file is human readable and contains a table with some separators separating columns and newlines separating lines. The first column must contain the x values. The first row may contain data series titles that will be used in the legend.

Some statistical analysis is used to determine if the scale of x is supposed to be linear or logarithmic. Depending on the number of points and number of points relative to other data series, it decides to plot using points or lines.

## Feedback
If you think its intuition sucks in some cases or it is greatly impractical in some common case, please open an issue.

Known limitations:

* The data may contain no column for x