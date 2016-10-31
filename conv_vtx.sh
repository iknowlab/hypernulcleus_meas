#! /bin/bash
if [ $# -le 0 ]; then
	echo "convert new vtxfile* to old vtxfile for XiIdification(*2015~)"
	echo "Usage: $0 filename.vtx"
	echo "newvtxfile rename (filename.vtx.ori)"
	exit 1
fi
mv $1 ${1}.ori
awk 'NF!=7{print }NF==7{print $1,$2,$3,$4,$5,$6/$7}' ${1}.ori 1>$1
chmod a-x ${1}.ori
