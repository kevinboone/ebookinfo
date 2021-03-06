<h1>ebookinfo</h1>

Version 0.0.1, March 2017

<h2>What is this?</h2>

<code>ebookinfo</code> is a very simple utility for Linux, for extracting
meta-data (author, title, etc) from certain e-book files. At present, 
the utility supports EPUB, MOBI, and RTF, and extracts author,
title, year, genre/subject, and comment information. 
<p/>
<code>ebookinfo</code> is implemented as a library, that can be 
used by other applications that manage e-books. The utility really
serves as a demonstration how to use the library.

<h2>Usage examples</h2>

<pre class="codeblock">
# Display information, including description, formatted by text2html
ebookinfo -ch /path/to/my/book.epub
</pre>


<h2>Prerequisites</h2>

<code>ebookinfo</code> uses the PCRE library 
(<code>yum install pcre-devel</code>). 
<p/>
Some e-book authors format meta-data as HTML. 
<code>ebookinfo</code> can pipe its output through 
<code>html2text</code>, if it is installed.  

<h2>Building</h2>

The usual:

<pre class="codeblock">
$ make
$ sudo make install
</pre>

<code>ebookinfo</code> may build and run on systems other than Linux,
but this has not been tested. 


<h2>Notes</h2>

There is no standard for the kind of meta-data that different document
formats support, or how it should be interpreted.
This utility displays the five basic entries that 
most formats handle -- author, title, year, genre/subject, comment.
EPUB and MOBI formats allow multiple genre/subject tags, these are
displayed as a comma-separated list. The 'year' attribute is the
year of the last document revision, in formats that distinguish different
dates. Neither the library nor the utility make any attempt to 
modify the character encoding of the stored meta-data. With MOBI
and EPUB this is usually UTF-8; for RTF there are no guarantees.


<h2>Legal, etc</h2>

<code>ebookinfo</code> is maintained by Kevin Boone, and distributed under the
terms of the GNU Public Licence, version 3.0. Essentially, you may do
whatever you like with it, provided the original author is acknowledged, and
you accept the risks involved in doing so. <code>ebookinfo</code> contains
contributions from a number of other authors, whose details may be
found in the source code.
<p/>

