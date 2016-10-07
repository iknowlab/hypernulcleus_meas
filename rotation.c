#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

int main()
{
	int ans;	/* ���[�U�[����̕ԓ� */
	int line;	/* �s�ԍ� */
	int loop;	/* ���[�v�J�E���^ */
	int pljoint[1000];	/* plate�ڑ����� */
	double angle;	/* �p�x */
	double R[1000];	/* Range */
	double X[1000];	/* X���W */
	double Y[1000];	/* Y���W */
	double Z[1000];	/* Z���W */
	double X_out[1000];	/* �o��X���W */
	double Y_out[1000];	/* �o��Y���W */
	double Z_out[1000];	/* �o��Z���W */
	char id[64];	/* particle ID */
	char linebuffer[2048];	/* �ǂݎ��s�i�[ */
	FILE *read,*out,*ang;	/* File�|�C���^ */
	
	/* �����J�n */
	printf("\n------------------------------------------------\n");
	printf("\t\trotation.exe starts...\n");
	printf("\n------------------------------------------------\n\n");
	
	/* �����菇��I������ */
	printf("> Sellect 1, 2 or 3:\n");
	printf("> \t1) XY-plane,\t2) YZ-plane,\t3) assign an angle.\n");
	printf("> [Answer] :  ");
	scanf("%d",&ans);
	
	/* �����N���A */
	for(loop=0;loop<1000;loop++)
		pljoint[loop] = 0;
	
	/* XY���ʂł̉�] */
	if (ans == 1){
		
		/* �ǂݎ��File���J�� */
		if ((read = fopen("dataTR.stp", "r")) == NULL) {
			printf("Input_File[dataTR.stp] not open\n");
			exit(1);
		}
		
		/* �o��File���J�� */
		if ((out = fopen("dataTR.stpr", "w")) == NULL) {
			printf("Output_file[dataTR.stpr] not open\n");
			exit(1);
		}
		
		/* �p�x�o��File���J�� */
		if((ang = fopen("anglex.log", "w")) == NULL){
			printf("Log_file[anglex.log] not open\n");
			exit(1);
		}
		
		/* ���W�̓ǂݎ��J�n */
		while(fgets(linebuffer,sizeof(linebuffer),read) != NULL){
			line = 0;
			sscanf(linebuffer,"%s %lf %lf %lf %lf %d",id,&R[line],&X[line],&Y[line],&Z[line],&pljoint[line]);
			line++;
			
			/* ���������܂Łi�P���q���j�ǂݎ�� */
			while(strncmp(linebuffer,"--------------------------------------------------",40) != 0){
				fgets(linebuffer,sizeof(linebuffer),read);
				sscanf(linebuffer,"%*s %lf %lf %lf %lf %d",&R[line],&X[line],&Y[line],&Z[line],&pljoint[line]);
				line++;
			}
			
			/* �p�x�v�Z */
			angle = atan((X[line-2]-X[0])/(Y[line-2]-Y[0]));
			
			/* ��]�p��\�� */
			printf("angle of trackID[%s] :\t%lf\n",id,angle*(180.0/acos(-1)));
			fprintf(ang, "%s\t%lf\n", id, angle*(180.0/acos(-1)));
			
			/* ���W�n�̉�] */
			for(loop=0;loop<line-1;loop++){
				X_out[loop] = (X[loop]-X[0])*(cos(angle)) - (Y[loop]-Y[0])*(sin(angle));
				Y_out[loop] = (X[loop]-X[0])*(sin(angle)) + (Y[loop]-Y[0])*(cos(angle));
				
				/* �ڑ������ɋ�ʂ����� */
				if(pljoint[loop] == 111)
					fprintf(out,"%s\t%lf\t%lf\t%lf\t%lf\t111\n",id,R[loop],X_out[loop],Y_out[loop],Z[loop]);
				
				/* plate�����̏ꍇ */
				else
					fprintf(out,"%s\t%lf\t%lf\t%lf\t%lf\t000\n",id,R[loop],X_out[loop],Y_out[loop],Z[loop]);
			}
			
			/* ���������̒ǋL */
			fprintf(out,"----------------------------------------\n");
			
		}	/* while [���W�̓ǂݎ��J�n] end */
		
		/* File����� */
		fclose(read);
		fclose(out);
		fclose(ang);
		
	}	/* if [XY���ʂł̉�]] end */
	
	/* YZ���ʂł̉�] */
	else if (ans == 2){
		
		/* �ǂݎ��File���J�� */
		if ((read = fopen("dataTR.stpr", "r")) == NULL) {
			printf("Input_File[dataTR.stpr] not open\n");
			exit(1);
		}
		
		/* �o��File���J�� */
		if ((out = fopen("dataTR.stprr", "w")) == NULL) {
			printf("Output_file[dataTR.stprr] not open\n");
			exit(1);
		}
		
		/* �p�x�o��File���J�� */
		if((ang = fopen("angley.log", "w")) == NULL){
			printf("Log_file[angley.log] not open\n");
			exit(1);
		}
		
		/* ���W�̓ǂݎ��J�n */
		while(fgets(linebuffer,sizeof(linebuffer),read) != NULL){
			line = 0;
			sscanf(linebuffer,"%s %lf %lf %lf %lf %d",id,&R[line],&X[line],&Y[line],&Z[line],&pljoint[line]);
			line++;
			
			/* ���������܂Łi�P���q���j�ǂݎ�� */
			while(strncmp(linebuffer,"--------------------------------------------------",40) != 0){
				fgets(linebuffer,sizeof(linebuffer),read);
				sscanf(linebuffer,"%*s %lf %lf %lf %lf %d",&R[line],&X[line],&Y[line],&Z[line],&pljoint[line]);
				line++;
			}
			
			/* �p�x�v�Z */
			angle = atan((Y[line-2]-Y[0])/(Z[line-2]-Z[0]));
			
			/* ��]�p��\�� */
			printf("angle of trackID[%s] :\t%lf\n",id,angle*(180.0/acos(-1)));
			fprintf(ang, "%s\t%lf\n", id, angle*(180.0/acos(-1)));
			
			/* ���W�n�̉�] */
			for(loop=0;loop<line-1;loop++){
				Y_out[loop] = (Y[loop]-Y[0])*(cos(angle)) - (Z[loop]-Z[0])*(sin(angle));
				Z_out[loop] = (Y[loop]-Y[0])*(sin(angle)) + (Z[loop]-Z[0])*(cos(angle));
				
				/* �ڑ������ɋ�ʂ����� */
				if(pljoint[loop] == 111)
					fprintf(out,"%s\t%lf\t%lf\t%lf\t%lf\t111\n",id,R[loop],X[loop],Y_out[loop],Z_out[loop]);
				
				/* plate�����̏ꍇ */
				else
					fprintf(out,"%s\t%lf\t%lf\t%lf\t%lf\t000\n",id,R[loop],X[loop],Y_out[loop],Z_out[loop]);
			}
			/* ���������̒ǋL */
			fprintf(out,"----------------------------------------\n");
			
		}	/* while [���W�̓ǂݎ��J�n] end */
		
		/* File����� */
		fclose(read);
		fclose(out);
		fclose(ang);
		
	}	/* if [YZ���ʂł̉�]] end */
	
	/* ��]�p���w�肵���ꍇ */
	else if(ans == 3){
		
		/* XY��]�p�̓��� */
		printf("XY angle (deg):  ");
		scanf("%lf",&angle);
		
		/* 0���Ȃ牽�����Ȃ� */
		if(angle != 0){
			
			/* �p�x��radian�ɕϊ� */
			angle = angle/(180.0/acos(-1));
			
			/* �ǂݎ��File���J�� */
			if ((read = fopen("dataTR.stp", "r")) == NULL) {
				printf("Input_File[dataTR.stp] not open\n");
				exit(1);
			}
			
			/* ���ԏo��File���J�� */
			if ((out = fopen("dataTR.stpr", "w")) == NULL) {
				printf("Output_file[dataTR.stpr] not open\n");
				exit(1);
			}
			
			/* �p�x�o��File���J�� */
			if((ang = fopen("anglex.log", "w")) == NULL){
				printf("Log_file[anglex.log] not open\n");
				exit(1);
			}
			
			/* ���W�̓ǂݎ��J�n */
			while(fgets(linebuffer,sizeof(linebuffer),read) != NULL){
				line = 0;
				sscanf(linebuffer,"%s %lf %lf %lf %lf %d",id,&R[line],&X[line],&Y[line],&Z[line],&pljoint[line]);
				line++;
				
				/* ���������܂Łi�P���q���j�ǂݎ�� */
				while(strncmp(linebuffer,"--------------------------------------------------",40) != 0){
					fgets(linebuffer,sizeof(linebuffer),read);
					sscanf(linebuffer,"%*s %lf %lf %lf %lf %d",&R[line],&X[line],&Y[line],&Z[line],&pljoint[line]);
					line++;
				}
				
				/* ��]�p�x��\�� */
				printf("angle of trackID[%s] :\t%lf\n",id,angle*(180.0/acos(-1)));
				fprintf(ang, "%s\t%lf\n", id, angle*(180.0/acos(-1)));
				
				/* ���W�n�̉�] */
				for(loop=0;loop<line-1;loop++){
					X_out[loop] = (X[loop]-X[0])*(cos(angle)) - (Y[loop]-Y[0])*(sin(angle));
					Y_out[loop] = (X[loop]-X[0])*(sin(angle)) + (Y[loop]-Y[0])*(cos(angle));
					
					/* �ڑ������ɋ�ʂ����� */
					if(pljoint[loop] == 111)
						fprintf(out,"%s\t%lf\t%lf\t%lf\t%lf\t111\n",id,R[loop],X_out[loop],Y_out[loop],Z[loop]);
					
					/* plate�����̏ꍇ */
					else
						fprintf(out,"%s\t%lf\t%lf\t%lf\t%lf\t000\n",id,R[loop],X_out[loop],Y_out[loop],Z[loop]);
				}
				/* ���������̒ǋL */
				fprintf(out,"----------------------------------------\n");
				
			}	/* while [���W�̓ǂݎ��J�n] end */
			
			/* File����� */
			fclose(read);
			fclose(out);
			fclose(ang);
			
		}	/* if [0���Ȃ牽�����Ȃ�] end */
		
		/* ������YZ��]�p�̓��� */
		printf("YZ angle (deg):  ");
		scanf("%lf",&angle);
		
		/* 0���Ȃ牽�����Ȃ� */
		if(angle != 0){
			
			/* �p�x��radian�ɕϊ� */
			angle = angle/(180.0/acos(-1));
			
			/* ����File���J�� */
			if ((read = fopen("dataTR.stpr", "r")) == NULL) {
				printf("Input_File[dataTR.stpr] not open\n");
				exit(1);
			}
			
			/* �o��File���J�� */
			if ((out = fopen("dataTR.stprr", "w")) == NULL) {
				printf("Output_file[dataTR.stprr] not open\n");
				exit(1);
			}
			
			/* �p�x�o��File���J�� */
			if((ang = fopen("angley.log", "w")) == NULL){
				printf("Log_file[angley.log] not open\n");
				exit(1);
			}
			
			/* ���W�̓ǂݎ��J�n */
			while(fgets(linebuffer,sizeof(linebuffer),read) != NULL){
				line = 0;
				sscanf(linebuffer,"%s %lf %lf %lf %lf %d",id,&R[line],&X[line],&Y[line],&Z[line],&pljoint[line]);
				line++;
				
				/* ���������܂Łi�P���q���j�ǂݎ�� */
				while(strncmp(linebuffer,"--------------------------------------------------",40) != 0){
					fgets(linebuffer,sizeof(linebuffer),read);
					sscanf(linebuffer,"%*s %lf %lf %lf %lf %d",&R[line],&X[line],&Y[line],&Z[line],&pljoint[line]);
					line++;
				}
				
				/* ��]�p�x��\�� */
				printf("angle of trackID[%s] :\t%lf\n",id,angle*(180.0/acos(-1)));
				fprintf(ang, "%s\t%lf\n", id, angle*(180.0/acos(-1)));
				
				/* ���W�n�̉�] */
				for(loop=0;loop<line-1;loop++){
					Y_out[loop] = (Y[loop]-Y[0])*(cos(angle)) - (Z[loop]-Z[0])*(sin(angle));
					Z_out[loop] = (Y[loop]-Y[0])*(sin(angle)) + (Z[loop]-Z[0])*(cos(angle));
					
					/* �ڑ������ɋ�ʂ����� */
					if(pljoint[loop] == 111)
						fprintf(out,"%s\t%lf\t%lf\t%lf\t%lf\t111\n",id,R[loop],X[loop],Y_out[loop],Z_out[loop]);
					
					/* plate�����̏ꍇ */
					else
						fprintf(out,"%s\t%lf\t%lf\t%lf\t%lf\t000\n",id,R[loop],X[loop],Y_out[loop],Z_out[loop]);
				}
				/* ���������̒ǋL */
				fprintf(out,"----------------------------------------\n");
				
			}	/* while [���W�̓ǂݎ��J�n] end */
			
			/* File����� */
			fclose(read);
			fclose(out);
			fclose(ang);
			
		}	/* if [0���Ȃ牽�����Ȃ�] end */
		
	}	/* if [��]�p���w�肵���ꍇ] end */
	
	/* 1, 2, 3�ȊO�̐������͂��ꂽ�� */
	else {
		printf("> You should answer by 1, 2 or 3!!\n");
		printf("> Exit soon ...\n");
		
		exit(1);
	}
	
	/* main() �߂�l 0�i����I���j */
	return 0;
	
}

