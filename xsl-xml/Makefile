BOOK=/mnt/build_dir/jhalfs/book-source/lfs-book
BLFS-BOOK=/home/hao/github/haoos-lfs/blfs-book

all:
	  xsltproc --nonet --output ./out test.xsl blfs-prbook.xml

lfs:
	  xsltproc --nonet                                      \
           --xinclude                                   \
           --stringparam profile.revision systemd    \
           --stringparam profile.arch     default   \
           --output prbook.xml                          \
           ${BOOK}/stylesheets/lfs-xsl/profile.xsl        \
           ${BOOK}/index.xml

prbook-blfs:
	  xsltproc --nonet                                      \
           --xinclude                                   \
           --stringparam profile.revision systemd    \
           --stringparam profile.arch     default   \
           --output blfs-prbook.xml                          \
           ${BLFS-BOOK}/stylesheets/lfs-xsl/profile.xsl        \
           ${BLFS-BOOK}/index.xml

blfs:
	#make -C ${BLFS-BOOK} REV=systemd wget-list
	#make -C ${BLFS-BOOK} BASEDIR=${BLFS-BOOK}/ REV=systemd wget-list
	xsltproc --nonet                                      \
           --xinclude                                   \
           --stringparam profile.revision systemd    \
           --stringparam profile.arch     default   \
           --output blf-prbook.xml                          \
           ${BLFS-BOOK}/stylesheets/lfs-xsl/profile.xsl        \
           ${BLFS-BOOK}/index.xml

