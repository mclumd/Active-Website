# Active Logic Website
## Creating pages
To create a new web page linked from the home ALMECOM page
(or any other of our pages), do this:

1. create your_page_name.shtml as a blank file

2. insert at the TOP of that file the line (*exactly this way*): `<!--#include virtual="include.shtml"-->`

If your page is not in the same directory as the include file, you will need to prepend the appropriate path. 
Example for a page in the research directory:
`<!--#include virtual="../include.shtml"-->`


3. Insert *after* that line whatever you want to appear on your
page.  The line above (from 2.) will automatically read the
standard ALMECOM banner and sidebar into your page when it is
viewed.

4. Repeat steps 2 and 3 at the bottom of the file with the line: `<!--#include virtual="footer.shtml"-->`

5. For convenience, there is a TEMPLATE file that you can copy
to your_page_name.shtml, that has the special "include" line
already in it, plus some other hopefully-helpful info.  The
file template.shtml is also identical to TEMPLATE.


## TODO

* Fix the redirect issues
* Empty image at bottom of seminars page (metacogseminar/index.html line 101)
* Broken link near the end of home page (index.shtml line 41)
* Research Page links to empty contradiction page (research/contra.html linked from research/index.shtml line 26)
* Bad A. V. Williams link (metacogseminar/index.html line 15)
* Bad Seminar Links (metacogseminar.index.html lines 93-96)
* Update the People page (people/index.shtml)
* Broken People Links (people/index.shtml)
* Papers list on publication by year page link to mclumd.github.io (publications/papers-search.shtml)
* Broken activelogic.org links
    * Conversational Adequacy (research/conv.shtml lines 7, 10)
    * Uncertainty Primer (primers/uncertain.shtml lines 58-61)
    * Technical Primer (primers/technical.shtml lines 5, 54-59)
* Clean up unused or unlinked files
