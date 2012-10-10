#!/bin/bash

CHEMIN="/var/www/GnOmeDataCenter/log"
DATE=`date "+%y%m%d"`
FICHIER=$DATE"_vigicrue.csv"

COURBE1=644	#pontlieux
COURBE2=637	#ysoir

#hauteur
wget -O - "http://www.vigicrues.ecologie.gouv.fr/niveau3.php?idstation=$COURBE1&tr0=$COURBE2&idspc=9&typegraphe=h&AffProfondeur=600&AffRef=auto&AffPrevi=non&nbrstations=6&ong=2" | grep "Hauteurs en m" | sed "s#<td>##g" | sed "s#</td>##g" | sed "s#<tr>##g" | sed "s#</tr>##g" | sed "s/<tr  bgcolor='#CCCCFF'>/\n/g" | sed "s/<td align='right'>/,/g" | sed "s/<tr  bgcolor='#F2F2FF'>/\n/g"  | sed "s#</table><p>&nbsp;</p>##g" | sed "s#&nbsp;##g" | sed "s#<p class='titre_cadre'>##g" | sed "s#</p><p></p><table  class='liste'><th>#\n#g" | sed "s#</th><th>#;#g" | sed "s#</th>##g" > $CHEMIN/$FICHIER



