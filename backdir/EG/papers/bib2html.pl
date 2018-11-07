#!/usr/imports/bin/perl


printf("<html>\n<body>\n<pre>\n");

while($_ = <STDIN>){
    if(/url/){
	$foo1 .= $_;
	($j1, $theurl, $j2) = split(/\"/, $foo1, 5);
	print('<a href="', $theurl, '">');
	print($_);
	print('</a>');
	print('<br>');
    }
    else{
	print($_);
	print('<br>');
    }
    reset 'ft';
}
printf("</pre>\n</body>\n</html>\n");
				
