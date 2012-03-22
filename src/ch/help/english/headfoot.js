function header(text)
{
	document.write("<table width=\"100%\">");
	document.write("<tr><td>");
	document.write("<img src=\"ch.gif\" alt=\"CH Img\" border=\"0\" align=\"left\">");
	document.write("<div class=\"header\">Copy Handler help file</div>");
	document.write("<div class=\"headertitle\">");
	document.write( text );
	document.write("</div>");
	document.write("</td></tr>");
	document.write("<tr><td><hr></td></tr>");
	document.write("<tr><td>");
}

function footer()
{
	document.write("</td></tr>");
	document.write("<tr><td>");
	document.write("<hr>");
	document.write("<div class=\"footer\">Copyright © 2003-2004 Józef Starosczyk.</div>");
	document.write("<hr>");
	document.write("</td></tr>");
	document.write("</table>");
}