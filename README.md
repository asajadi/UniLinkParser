# Unification Link Grammar Parser Machine Translation System

This directory is a distribution of  Unification Link Grammar Parser and a machine translation system based on the idea.
The system is made on top of Link Grammar Parser 4.1b. The original license is still in-effect with a few more restrictions (LICENSE file included with this software).

The  following source files are added to the main package:

* api-example.c: 
  * A simple translation example based on the translation api functions, To test this module you should rename “MakeFile-api” to “MakeFile” and remake
* trans-api-example.c
* feature-sets.c
* hash-set.c
* mapping_unit.c
* morhpo_api_example.c
* morpho-analyzer.c
* morpho-synth-fa.c
* read-feature.c
* trans.c: 
  * The main interface of the system
  * To test this module you should rename “MakeFile-trans” to “MakeFile” and  remake

# Building the binaries
Simply type make, it will create a trans file. 

## Running the System
`./parse [options]`
This results in reading all the sentences in in.txt and get parsed (or translated)

### options: 
`-t:`	Translate, if not specified, it will just parse the sentence
`-du:`	Unification will be in effect, if not specified, the feature structures will simply be ignored
`-a:`	For a sentence, all possible linkages are reported ( or translated)
`-o:`	The results will be written into different files, and renamed. The index
            the out files names would be something like :
	If in Parse mode:
The files are indexed as out_x_y, where x is the index of the sentence in the array and y 
is the index of the linkage
	If in translate mode:
            src-x-y:    Contains a linkage for the source sentence, x is the index
                             of sentence in the array and y is the index of linkage
            trg-x-y-z: Contains a linklage for the target sentence, x is the index
                             of sentence in the array, y is the index of linkage and z
                             is the zth translation of that linkage x-y(a linkage in the
                             source language may have more than one correspondent target
                             linkage)


# A brief introduction to the knowledge base

## 4.0.dict: 
A dictionary similar to link grammar dictionary, the only difference is the possibility of adding features to the words, based on the methods described in [1].  The structure of the dictionary is the same, but adding feature structures in Attribute-Value Matrices is also possible and gives more flexibility, the technical explanations can be found in [1], but some highlights of the differences are as follows:

* A word (with the same subscripts) can be defined with two different feature structures and hence two linking requirements, but there should be a feature named "sub" with different values, like:

`blah [sub:x, feature1: value1]: X+;`
`blah [sub:y, feature1: value2]: Y+;`

* Coreferencing is also possible, in the following example "l" is the label of the value of "f" and "g:(l)" says that the value of g is the same as f, they point to the same DAG.

`a[f:t(l), g:(l)]:  X+;`
`b[f:?]: X-;`


* Disjunctive values are allowed in the leaves, like 
the[agr:sing pl]
It means that agr can be unified with either sing or pl. 
* A ? value can be unified with anything
* Adding a feature structure to a set of words is possible as follows (using || was the easiest way to avoid a confusion with link costs which uses square braces as well as feature structures:

 `a b c: |[sub:x, feature1: value1]|  X+;`

This way, the feature structure is concatenated with the feature structure of each word if they have any , 

`a[feature1: value1] b c: |[feature2: value2]|  X+;`

In  the previous example, if we replace feature1 with feature2 the parser generates an error.

* The same trick can be used to attach a feature to the all words in a word file. Suppose that all words in "words.n" have their own features (they usually don't), by adding the following line in the dictionary, all words will have <feature1:value1> feature along with they's own features:

/words.n: |[feature1:value1| X+;


** 4.0.features.dict

_Note: if the name of the dictionary is “name.dict”, the name of the feature file should be “name.features.dict”_
 
This is a file containing pre-defined feature structures, called templates. The idea is similar to use macros in linking definitions. For example, you can define a feature structure like:

`<noun-s>: [cat:n, agr:[person:3, agr:s]];`

And then use it in the dictionary:
`blah[<noun-s>, other features...]: X+;`

You can use templates wherever features are applicable.


## 4.0.uni.dict
It contains unification rules exactly the way it is explained in [1]
Note: If the name of the dictionary is “name.dict”, the name of the “Unification Rules” file should be “name.uni.dict” 

## Morphology Directory
This directory is used by the morphological analyzer, it works exactly like the normal parser, so it has a dictionary “morphemes.dict” which contains morphemes as words, a feature template file “morphemes.features.dict” and a unification rules file: “morphemes.uni.dict”.
The exact same word files used for the main dictionary can be used here, but with different rules, for example we have the following rule for /words/words.v.6.1 files (present stems)


`
/words/words.v.6.1 : |[<M-VPRS>]| G+; and <M-VPRS> is defined as follows
<M-VPRS> :
 [stem:+ 
  ,cat: v
  ,subcat:?
  ,head : [agr : ?
           ,vform : [tense: ?
                     ,aspect: [perfect:?
                               ,gerund:?
                               ,sub_imp:?
                              ]
                   ]
          ]
];`

And also we have this rule in the morphemes.uni.dict file, 

`G : <head vform aspect gerund> = +, <subcat> = ING-WORDS;`

Now, when the parser gets a word like “hunting” which is not found in the dictionary, it decomposes it to hunt+ing, the morphological anyzer apply these rules resulting in a G link between “hunt” and “ing”, this links sets the gerund feature of the word to + and its subcat to ING-WORDS, and returns it to the parser. Then the parser finds the definition for ING-WORDS, unify this feature structure to whatever features ING-WORDS has in the dictionary and uses it for parsing. This process is illustrated in [1]

# Translation Directory

This directory contains three files, 
1. `lexicon.txt`: its the bilingual dictionary, subscripts can be used to have multiple entries for the same word.
2. `links list`: This is used to map connector names, for example if Pgfb is going to be treated as a P link while mapping the linkages, we can have this rule: Pgfb : P;
After translation, P link is recovered to Pgfb. If this recovery is not going to happen (most of the time its the case as its safer), we can add a %, like Pgfb: P%;

3. `mapfile.txt`: The heart of the translator, it contains the transfer rules, please refer to [1] to review the details

4.`target directory`

trg.dict: It has the feature structures of the words in the target language, although no linking requirement is used but there should be something there as the parser uses the original functions to read the dictionary, that’s why all of them has been given a dummy link (D+). So practically it can use the trg.features.dict to store the templates the same way the original dictionary uses the templayes. The features listed in the file are used by the morphological modifier, for example if the translation is into Farsi, something like it’s plural afix, or past affix can be stored.
** There are two other files in this directory which are empty but they shouldn't be removed **



