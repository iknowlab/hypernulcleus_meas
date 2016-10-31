#! /usr/bin/awk -f
BEGIN{
	if(ARGC < 2){
		print "Usage:",ARGV[0],"beamfile(.vtk or .trk)" >> "/dev/stderr";
		exit(1);#ENDに進む
	}#if
#初期値設定コーナー
	count=0.;
	downsurface=0.;
	downbase=0.;
	upbase=0.;
	upsurface=0.;
}#begin

#read z
NF == 6 && $3 == 3.0{
	count+=1.0;
	downsurface+=$6;
}
#read z
NF == 6 && $3 == 2.0{
	downbase+=$6;
}#read z
NF == 6 && $3 == 1.0{
	upbase+=$6;
}#read z
NF == 6 && $3 == 0.0{
	upsurface+=$6;
}
END{
	if(ARGC < 2)exit(1);
	sfu=0.5/((upsurface-upbase)/count);
	sfd=0.5/((downbase-downsurface)/count);
	print sfu;
	print sfd;
	print sfu-sfd;
}#END
