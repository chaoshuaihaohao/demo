<?xml version="1.0" encoding="ISO-8859-1"?>
<!--
chapter id
    title
    sect1
        title
        sect2
            title
        sect2
            title
	    screen userinput
        		
-->


<xsl:stylesheet
      xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
      xmlns:exsl="http://exslt.org/common"
      extension-element-prefixes="exsl"
      version="1.0">

    <xsl:output method="text" indent="yes"/>
  
    <xsl:template match="/chapter">  
	    <xsl:variable name="dirname" select="@id"/>
	    <xsl:text>#!/bin/bash&#xA;</xsl:text>
	    <xsl:apply-templates select="sect1 | sect2 | sect3 | sect4 | sect5"/>  
	    <xsl:text>&#xA;#End script&#xA;</xsl:text>
    </xsl:template>  

    <xsl:template match="sect1 | sect2 | sect3 | sect4 | sect5"/>  
    <xsl:for-each select="userinput">
	    <xsl:value-of select="." disable-output-escaping="yes"/>  
	    <xsl:text>&#xA;</xsl:text>
    </xsl:for-each>
    </xsl:template>  

    <xsl:template match="text()"/> 
</xsl:stylesheet>
