#! /bin/bash
INSTPATH=`dirname $0`
if [ $# -le 0 ]; then
	echo "Usage	: $0 [-r] filename.vtx"
	echo "-r	:Use vtxcheck.C at ROOT"
	echo "convert old vtxfile to trkfile (removed vtxdata) for track data analy(*2015~)"
	echo "先にconv_vtxを使ってから.trkファイルに変換します"
	echo "distortion.vtxや一部fin.vtxは.trkファイルと一緒"
	exit 1
fi
	rootflag=0
if [ $1 == "-r" ]; then
	rootflag=1
	shift;
fi

filename=`basename $1 .vtx`
awk '(NF==6) && ($1!="#"){print $1,$2,$3,$4,$5,$6}' ${1} 1>${filename}.trk

if [ $rootflag == 1 ]; then
root -l -x "${INSTPATH}/vtx_check.C(\"${filename}.trk\")"
fi
