# Context-free grammar generator
This is a program for generating texts using a [context-free grammar](https://en.wikipedia.org/wiki/Context-free_grammar). It is available both as a C++ library and as a command line utility.

It interprets a grammar file or string (C++ library version only) to generate text. It has some syntax sugar compared to plain context-free grammar.

It is more powerful and controllable than a Markov chain-based generator, but its rules need to be set manually. When used to generate text, the result is perfectly grammatically correct if the grammar is set properly. When used to generate words (like names), it will not generate overly long or unpronunciable ones (again, if the grammar is set properly).

## The grammar file
Here is an example of a grammar:
```
main(name)={hello}, here is {name}.\n\n{some_text}\n\n{name}
hello=Hi|Hello|Wazzup
name=John|Tom|Harry|Bob
some_text=I would like to invite you to {place}.|I don't have time for the visit of {place}.
place=café|bar|restaurant|concert
```

You may need to read [something about context-free grammars](https://en.wikipedia.org/wiki/Context-free_grammar) to understand the following. The starting nonterminal is called `main`. The derivation starts with it, but otherwise it's the same as any other nonterminal. Nonterminals can be derived to one of several groups of terminals and nonterminals, separated by the `|` symbol. They are written as plain text, with nonterminals marked with curly brackets. Everything not in curly brackets are terminals. Nonterminal derivation descriptions are separated by new lines.

If you are lazy to read the article, read at least this: Anything on the left of the equals sign is called _nonterminal_ and breaks into one of those possibilities separated by `|` if wrapped in curly brackets. The process starts with the part called `main`.

Because newline is used as a separator, you need to write `\n` to write a new line. Tabs can be written as `\t`.

There is also syntax sugar allowing choices to develop in the same way:

To have a nonterminal produce the same output in everything that derives from some nonterminal, use this:

```
speaking_of_name(name)={sentence_name} {sentence_name} {sentence_name} | {sentence_name} {sentence_name}
```

It will cause `{name}` to produce the same result in all breakups of `{sentence_name}` in the derivation of a `{speaking_of_name}` nonterminal. Other `{speaking_of_name}` nonterminals not produce the same result of derivation of `{name}`. If you want a nonterminal to always produce the same result, fixate its value in the `{main}` nonterminal.

To have some nonterminals choose the same option in everything that derives from some nonterminal, use this:

```
story(pronoun:sex,name:sex)=So I met {name}. It seems {pronoun} has returned from holiday.
pronoun=he|she
name={male_name}|{female_name}
```

The same text behind the `:` symbol for more possibilities forces the choice of these nonterminals to be the same. In this case, it will make the program pick the first possibility of `{name}` if it has chosen the first possibility of `{pronoun}` and the second possibility of `{name}` if it has chosen the second possibility of `{pronoun}`. It can also be used to have a nonterminal always pick one possibility without having it develop completely identically.
