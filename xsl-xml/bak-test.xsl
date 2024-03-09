<?xml version="1.0" encoding="UTF-8"?>
<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform">
    <!-- 定义一个模板，描述你想要如何转换你的XML数据 -->  
    <xsl:template match="/">  
            <!-- 应用模板到你的XML数据 -->  
            <xsl:apply-templates/>  
    </xsl:template>  
  
    <!-- 定义一个模板，处理你的XML数据中的每一个元素 -->  
    <xsl:template match="userinput">  
            <!-- 在这里处理你的元素，例如使用元素名作为包装的标题 -->  
            <xsl:value-of select="."/>  
            <!-- 你可能还希望处理元素的属性和子元素 -->  
    </xsl:template>  
</xsl:stylesheet>
