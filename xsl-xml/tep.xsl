<?xml version="1.0" encoding="ISO-8859-1"?>  
<xsl:stylesheet xmlns:xsl="http://www.w3.org/1999/XSL/Transform" version="1.0">  
    <xsl:param name="lfs-full" select="'./lfs-xml/lfs-full.xml'"/>  
    <xsl:param name="installed-packages" select="'../lib/instpkg.xml'"/>  
  
    <!-- 定义一个模板，描述你想要如何转换你的XML数据 -->  
    <xsl:template match="/">  
        <packages>  
            <!-- 应用模板到你的XML数据 -->  
            <xsl:apply-templates/>  
        </packages>  
    </xsl:template>  
  
    <!-- 定义一个模板，处理你的XML数据中的每一个元素 -->  
    <xsl:template match="*">  
        <package>  
            <!-- 在这里处理你的元素，例如使用元素名作为包装的标题 -->  
            <title><xsl:value-of select="name()"/></title>  
            <!-- 你可能还希望处理元素的属性和子元素 -->  
        </package>  
    </xsl:template>  
</xsl:stylesheet>
