#!/bin/bash
PAGES=("introduction" "getting_started" "type_system" "contexts" "declaration" "functions" "addresses" "vectors" "tables") 
TITLES=("Introduction" "Getting Started" "Type System" "Contexts" "Declarations" "Functions" "Addresses" "Vectors" "Tables")
echo "<div id=\"sidebar\">" > guide/sidebar.temp
for i in {0..8}
do
    page=${PAGES[$i]}
    title=${TITLES[$i]}
    echo "<a href=\"${page}.html\"> <div class=\"navbutton\">${title}</div> </a><br>" >> guide/sidebar.temp
done
echo "</div>" >> guide/sidebar.temp

for i in {0..8}
do
    page=${PAGES[$i]}
    title=${TITLES[$i]}
    echo -n "Building $page . . . "
    pandoc -f markdown -t html "src/${page}.md" -o guide/content.temp 
    cat guide/header.html guide/sidebar.temp  > guide/temp1
    echo "<div id=\"content\">" >> guide/temp1
    cat guide/temp1 guide/content.temp > guide/temp2
    echo "</div>" >> guide/temp2
    cat guide/temp2 guide/footer.html > guide/${page}.html
    sed -i "s/HTML_PAGE_TITLE/Tabitha Programming Guide - ${title}/" guide/${page}.html 
    echo "Done."
done
