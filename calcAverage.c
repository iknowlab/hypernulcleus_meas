#include<stdio.h>
#include<stdlib.h>
#include<string>
#include<math.h>
#include<ieeefp.h>
#include<ctype.h>

/* ������̍��E���g���~���O */
char* strTriming(char *input){
	
	/* �R�����g�F�����Ɩ߂�l�̐��� */
	/*           ARG1 - �Ώە����� */
	/*           RTN  - �g���~���O��̐擪�����̃|�C���^ */
	
	int pos;	/* �����ʒu */
	
	/* ������S�̂��猟�� */
	for(pos=0; pos<strlen(input); pos++){
		
		/* �擪����X�y�[�X�����ƃ^�u������ǂݔ�΂� */
		if(input[pos] == ' ' || input[pos] == '\t')
			continue;
		
		/* �����������������烋�[�v���I�� */
		break;
		
	}
	
	/* �����̕�����̍ŏI�������猟�� */
	for(int i=strlen(input)-1; i>pos; i--){
		
		/* �X�y�[�X�����A�^�u�����A���s�����łȂ������Ȃ�� */
		if(input[i] != ' ' && input[i] != '\t' && input[i] != '\n'){
			
			/* �P��̕�����NULL�����ɂ��� */
			input[i+1] = '\0';
			
			/* ���[�v�I�� */
			break;
		}
		
	}
	
	/* �߂�l�Z�b�g */
	return (input + pos);
	
}

/* ��������w�肳�ꂽ�����ŕ��������̃|�C���^��Ԃ� */
int strsplit(char *output[], char *input, char *schr){
	
	/* �R�����g�F�����Ɩ߂�l�̐��� */
	/*           ARG1 - ������̕����|�C���^ */
	/*           ARG2 - �����O�̕����� */
	/*           ARG3 - �������镶�� */
	/*           RTN  - 0�Ȃ琳��I���A1�ُ͈�I�� */
	
	/* �ϐ��錾 */
	int cnum;	/* ������ */
	char cp[2];	/* �Ώە��� */
	int index;	/* �z��ԍ� */
	
	/* �w�蕶�����P�����łȂ���΃G���[�I�� */
	if(strlen(schr) != 1)
		return 1;
	
	/* ����������̕����� */
	cnum = strlen(input);
	
	/* �z��ԍ��������� */
	index = 0;
	
	/* �擪�����Ƀ|�C���^�����蓖�Ă� */
	output[index] = input;
	
	/* �w�蕶�������� */
	for(int i=0; i<cnum; i++){
		
		/* ��r�Ώە������擾 */
		sprintf(cp, "%c", input[i]);
		
		/* �w�蕶���Ɣ�r */
		if(strcmp(cp, schr) == 0){
			
			/* �w�蕶����null�����ɒu�������� */
			input[i] = '\0';
			
			/* �z��ԍ����Z */
			index++;
			
			/* �w�蕶���̂P��̕����Ƀ|�C���^�𓖂Ă� */
			output[index] = input + i + 1;
			
			/* ���̕����� */
			continue;
		}
		
	}
	
	/* �߂�l���� */
	return 0;
	
}

/* �|�C���^�z��̐��� */
void pntArray(char *string[]){
	
	/* null�|�C���^�܂Ń��[�v */
	for(int i=0; string[i]!=NULL; i++){
		
		/* �󕶎��łȂ���΃��[�v���ċA */
		if(strlen(strTriming(string[i])) > 0)
			continue;
		
		/* �z��̒��g���V�t�g */
		for(int j=i; string[j]!=NULL; j++){
			if(string[j+1] !=NULL)
				string[j] = string[j+1];
			else
				string[j] = NULL;
		}
		
		/* �ĕ]������ */
		i--;
		
	}
	
}

/* ��P�����̕����񂪐��l�ɕϊ��\�ł��邩�`�F�b�N���� */
int CheckNumber(char *string){
	
	for(int i=0; i<strlen(string); i++){
		
		/* �����_�͋��� */
		if(string[i] == '.')
			continue;
		
		/* ���������� */
		if(string[i] == '+' || string[i] == '-' || string[i] == '/' || string[i] == '*')
			continue;
		
		/* E�܂���e������ */
		if(string[i] == 'E' || string[i] == 'e'){
			
			if(i > 0)
				continue;
			
		}
		
		if(isdigit(string[i]) == 0)
			return 1;
		
	}
	
	return 0;
	
}

int main(int argc, char *fname[]){
	int loop;	/* ���[�v�J�E���^ */
	int ent;	/* �f�[�^�� */
	int item;	/* ���ڐ� */
	double value;	/* �ǂݍ��݃f�[�^ */
	double ave[50];	/* ���ϒl */
	double sde[50];	/* �W���΍� */
	char *data[50];	/* ���ڃ|�C���^ */
	char linebuffer[2048];	/* �ǂݍ��ݍs */
	FILE *read;	/* File�|�C���^ */
	
	/* �ǂݍ���File���J�� */
	if((read = fopen(fname[1], "r"))==NULL){
		printf("Input File[%s] not open!!\n", fname[1]);
		exit(1);
	}
	
	/* �����N���A */
	for(loop=0; loop<50; loop++){
		ave[loop] = 0.;
		sde[loop] = 0.;
	}
	
	/* �|�C���^�̏����� */
	for(loop=0; loop<50; loop++)
		data[loop] = NULL;
	
	/* �f�[�^���̃N���A */
	ent = 0;
	
	/* �f�[�^�̓ǂݍ��� */
	while(fgets(linebuffer, sizeof(linebuffer), read)!=NULL){
		
		/* ���s�������폜 */
		strcpy(linebuffer, strTriming(linebuffer));
		
		/* �|�C���^�̏����� */
		for(loop=0; data[loop]!=NULL; loop++)
			data[loop] = NULL;
		
		/* ���ڕʂɃ|�C���^���擾 */
		strsplit(data, linebuffer, "\t");
		
		/* �|�C���^�̐��� */
		pntArray(data);
		
		/* ���l�ϊ����\�ł��邩�m�F */
		for(loop=0; data[loop]!=NULL; loop++){
			
			/* ���l�łȂ���Ε\����Nan Value */
			if(CheckNumber(data[loop]) != 0){
				ave[loop] = sqrt(-1.);
				continue;
			}
			
			/* �f�[�^��ۑ� */
			sscanf(data[loop], "%lf", &value);
			
			/* ���ς��v�Z���邽�ߒl�����Z */
			ave[loop] += value;
			
		}
		
		/* ���ڐ��̊m�F */
		if(ent == 0)
			item = loop;
		else if(item != loop){
			printf("[Exception] :  The keys are not equal to the previous keys!!\n");
			
			fclose(read);
			exit(1);
		}
		
		/* �f�[�^���̉��Z */
		ent++;
		
	}
	
	/* ���o���̕\�� */
	printf("Average :  entries=%d\n", ent);
	
	/* ���ϒl�̌v�Z�ƌ��ʂ̏o��  */
	for(loop=0; loop<item; loop++){
		
		/* �������܂ލ��ڂ͖��� */
		if(isnan(ave[loop]) != 1)
			ave[loop] = ave[loop] / ((double) (ent));
		
		/* �l�̕\�� */
		printf("%lf\t", ave[loop]);
		
	}
	
	/* ���s���� */
	printf("\n");
	
	/* File����� */
	fclose(read);
	
	/* �߂�l 0(����) */
	return 0;
	
}
