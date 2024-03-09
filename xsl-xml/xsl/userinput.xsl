<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform">  
    <xsl:output method="xml" indent="yes"/>
  
    <xsl:template match="/">  
            <xsl:for-each select="//screen/userinput">  
                    <xsl:value-of select="." disable-output-escaping="yes"/>  
		    <xsl:text>&#xA;</xsl:text>
		    <xsl:text>&#xA;</xsl:text>
            </xsl:for-each>  
    </xsl:template>  
</xsl:stylesheet>
