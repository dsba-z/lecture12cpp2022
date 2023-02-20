# Simple CSV reader

Features:

1. Filename -> Vector of vectors of strings.
2. **Support for quotation marks**.
3. Handling incorrectly formatted files (wrong number of columns).
4. No type conversion (handled outside of this reader).
5. No newlines escaping.

Adapted from https://stackoverflow.com/a/30338543

# Steps

## Reading the file line by line

A CSV reader needs to read *the whole file* at once. Which means reading until the input stream says the file is over.

They ways it can say this:

1. `inStream.eof()`
   1. Becomes true when you stop reading at the end-of-line character.
2. `inStream.fail()`
   1. Becomes true when you cannot read characters from the input stream.

https://en.cppreference.com/w/cpp/io/ios_base/iostate

In this example let's read the file with `std::getline`. If we don't escape newline characters, we can read a single line with a single `std::getline` call. In a CSV the last line may end with the newline character. In this case `std::getline` will stop at the newline character when it reads every line, and it will never stop at the end-of-line character before failing.

So, we use `inStream.fail()` to determine if the stream is over.

The reading steps are:

1. Read line.
2. Check if the stream has failed.
3. Parse line.

It can be achieved with the code

```cpp
std::string line;
while (std::getline(inStream, line))
{
    result = parseLine(line);
}
```

## Parsing one line

After reading one line from a file you have its contents in a string variable.

The problems to solve are

1. Separating the string by delimiters (assuming them to be `,` at first).
2. Saving the result in a vector of strings.
3. **Not** considering delimiters in quotation marks.

The first two problems are similar to workshop and contest examples. For example, they could be solved with stringstreams.

The third problem requires a more complex approach. To solve it, let's work *character-by-character*.

### Parsing examples

#### **No commas**

Consider the following line of a CSV file:

`1,2,Bread; Butter,91.89`

When parsed without type conversion, it should be split into 4 cells:

* `1`
* `2`
* `Bread; Butter`
* `91.89`

If you use simple separation by the delimiter `,`, that's exactly what would happen.

#### **Commans and quotation marks**

Now, let's consider the next example:

`1,2,"Bread, Butter",91.89`

If you split this line by commas, you will get the following tokens:

* `1`
* `2`
* `"Bread`
* ` Butter"`
* `91.89`

This is incorrect! If a comma appears inside quotation marks, we need to not condider it a delimiter. The correct tokens are:

* `1`
* `2`
* `Bread, Butter`
* `91.89`

### Character by character

To solve this problem, instead of splitting a line by a single delimiter, we need two special characters:

1. Delimiter (`,`)
2. Quotation character (`"`)

The previous algorithm without double quotes would be:

1. Read the next character.
2. If the character is not `,`
   1. add it to a buffer.
3. If the character is `,`
   1. add the buffer to the result vector
   2. clear the buffer


The new algorithm with quote handling:

1. Read the next character.
2. If *you are not inside quotation marks* and the next character is `,`
   1. add the buffer to the result vector
   2. clear the buffer
3. If *you are not inside quotation marks* and the next character is `"`
   1. you are now inside quotation marks
   2. don't add anything to the buffer
4. If you are inside quotation marks and the next character is `"`
   1. you are not outside quotation marks
5. In other cases (any normal character or `,` in quotes)
   1. add the character to the buffer


This is hard to read, so let's write it in code as well, to have two versions:

```cpp
    std::vector<std::string> lineData;
    std::string token;
    bool inQuotes = false;
    for (char c: line)
    {
        if (c == ',')
        {
            if (inQuotes)
            {
                token.push_back(c);
            }
            else
            {
                lineData.push_back(token);
                token.clear();
            }
        }
        else if (c == '"')
        {
            if (inQuotes)
            {
                inQuotes = false;
            }
            else
            {
                inQuotes = true;
            }
        }
        else
        {
            token.push_back(c);
        }
    }
```

Some parts may be simplified or changed, like writing `inQuotes = !inQuotes` or using `switch`.

|state                  |,  (comma)               |" (double quote)  |anything else|
|-|-|-|-|
|in a quoted line       |read character             |stop a quoted line| read character
|not in a quoted line   |save token, start new token|start a quoted line| read character


### Escaping double quotes

You may notice from the table that the action "read character" is never performed for double quotes themselves. So, the code about cannot read a line that contains them.

`1,2,"Bread "Baker's Good Bread Brand" and some Butter",91.89` will be read as

* `1`
* `2`
* `Bread Baker's Good Bread Brand and some Butter`
* `91.89`

To read these characters, we need to *escape* them, just like we're doing with commas in the previous example. The way to escape them is simpler: if you need to place a literal `"` in a quoted field, use two double quotes, `""`.

`1,2,"Bread ""Baker's Good Bread Brand"" and some Butter",91.89` should produce

* `1`
* `2`
* `Bread "Baker's Good Bread Brand" and some Butter`
* `91.89`

**New algorithm**

Now, if you encounter a double quote character inside a quoted field, it might be the end of the field or it might be the start of `""`, which would continue the field.

|state                       |,  (comma)                 |" (double quote)      |anything else|
|-|-|-|-|
|not in a quoted line        |save token, start new token|start a quoted line   | read character
|in a quoted line            |read character             |change state          | read character
|in a quoted line after **"**|save token, start new token|add " to token        | **undefined**

One case is undefined here. It refers to examples like this:

`1,2,"Partially Quoted" Field,91.89`

You can see that the field here is only partially quoted. What happens here is not strictly defined and depends on the implementation. Potential options:

1. Try to parse such cases anyway, adding new characters to the buffer.
2. Throw an exception or return an error, refusing to parse these cases.
3. Add an option for the user to control this behavior.

In this example implementation let's consider the easiest solution - continuing to parse the line.

## Considering the number of columns

CSV files don't hold metadata, so you can't compare the number with some specific number.

To ensure the number of columns in your data is correct, you may want to keep track of this manually.

1. Read the first line
2. Set the number of columns in the first line as "The number of columns in this file"
3. As you read all other lines:
   1. Check if the number of columns is the same as in the first line
   2. If it's not, stop reading with an error message about column count.

