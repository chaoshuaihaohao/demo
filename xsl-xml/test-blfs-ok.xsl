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

    <xsl:output method="xml" indent="yes"/>
  
    <xsl:template match="/">  

    <xsl:for-each select="//chapter">
	    <xsl:if test="sect1/sect2/screen/userinput">
	    <xsl:variable name="dirname" select="@id"/>
	    <xsl:for-each select="sect1">
		    <xsl:if test="sect2/screen/userinput">
		    <xsl:variable name="filename" select="title"/>
		    <exsl:document href="/home/hao/github/demo/xsl-xml/output/{$dirname}/{$filename}" method="text">
		    <xsl:for-each select="sect2">
			    <xsl:if test="position() = 1">
				    <xsl:text>#!/bin/bash&#xA;</xsl:text>
				    <xsl:text>&#xA;#Start scripts&#xA;</xsl:text>
			    </xsl:if>
			    <xsl:for-each select="screen/userinput">
				    <xsl:value-of select="." disable-output-escaping="yes"/>  
				    <xsl:text>&#xA;</xsl:text>
			    </xsl:for-each>  
			    <xsl:if test="position() = last()">
				    <xsl:text>&#xA;#End scripts&#xA;</xsl:text>
			    </xsl:if>
		    </xsl:for-each>  
		    </exsl:document>
		    <!--
		    -->
		    </xsl:if>
	    </xsl:for-each>  
	    </xsl:if>
    </xsl:for-each>  
    </xsl:template>  
</xsl:stylesheet>
